#include "appbranding.h"
#include "platforminfo.h"
#include "terminalui.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QHash>
#include <QFileInfo>
#include <QList>
#include <QProcess>
#include <QStandardPaths>
#include <QPair>
#include <QSet>
#include <QSettings>
#include <QVariant>

#include <cstdio>

#include <cstdlib>

namespace {
void configureApplicationIdentity()
{
    QCoreApplication::setApplicationName(appId());
    QCoreApplication::setApplicationVersion(appVersionString());
    QCoreApplication::setOrganizationName(appId());
    QCoreApplication::setOrganizationDomain(QStringLiteral("local.wafflehouse-client"));
}

void runStartupAudioPreflight()
{
    // The call stack can degrade gracefully to speaker-only/null audio, so a
    // failed Android permission probe must never prevent AIM/IRC/BBS/SIP from
    // starting. The helper is silent when capture already works; when Android
    // permission is missing, termux-microphone-record triggers the normal
    // Termux:API permission request and the helper prints recovery guidance.
    if (qEnvironmentVariableIsSet("WAFFLEHOUSE_SKIP_AUDIO_PREFLIGHT")) return;

    QString helper = QStandardPaths::findExecutable(QStringLiteral("wafflehouse-audio-preflight"));
    if (helper.isEmpty()) {
        const QString prefix = qEnvironmentVariable("PREFIX", QStringLiteral("/data/data/com.termux/files/usr"));
        const QString candidate = prefix + QStringLiteral("/bin/wafflehouse-audio-preflight");
        if (QFileInfo::exists(candidate)) helper = candidate;
    }
    if (helper.isEmpty()) {
        std::fprintf(stderr,
            "[audio] Startup microphone preflight helper is unavailable; continuing without blocking startup.\n");
        return;
    }

    QProcess probe;
    probe.setProgram(helper);
    probe.setArguments({QStringLiteral("--startup")});
    probe.setProcessChannelMode(QProcess::MergedChannels);
    probe.start();
    if (!probe.waitForStarted(5000)) {
        std::fprintf(stderr,
            "[audio] Could not start microphone permission preflight; continuing without blocking startup.\n");
        return;
    }

    // Deliberately wait without a short timeout: Android may be displaying its
    // runtime permission dialog and the user should be allowed to answer it.
    probe.waitForFinished(-1);
    if (probe.exitStatus() == QProcess::NormalExit && probe.exitCode() == 0) return;

    const QByteArray details = probe.readAll();
    if (!details.isEmpty()) {
        std::fwrite(details.constData(), 1, static_cast<size_t>(details.size()), stderr);
        if (!details.endsWith('\n')) std::fputc('\n', stderr);
    }
}

struct StoredProfile {
    QHash<QString, QVariant> values;
    QString dedupeKey;
};

QList<StoredProfile> readProfiles(QSettings &settings)
{
    static const QStringList keys = {
        QStringLiteral("id"), QStringLiteral("protocol"), QStringLiteral("server"),
        QStringLiteral("port"), QStringLiteral("username"), QStringLiteral("redirectHost"),
        QStringLiteral("redirectPort"), QStringLiteral("realName"), QStringLiteral("tls"),
        QStringLiteral("ircBuddies"), QStringLiteral("sipContacts"),
        QStringLiteral("telnetTerminalType"), QStringLiteral("sipProfileName"),
        QStringLiteral("sipDomain"), QStringLiteral("sipRegistrar"),
        QStringLiteral("sipAuthUsername"), QStringLiteral("sipDisplayName"),
        QStringLiteral("sipOutboundProxy"), QStringLiteral("sipCallerIdDomain"),
        QStringLiteral("sipDialPrefix"), QStringLiteral("sipStunServer"),
        QStringLiteral("sipTransport"), QStringLiteral("sipIdentityMode"),
        QStringLiteral("sipLocalPort"), QStringLiteral("sipRegistrationExpires"),
        QStringLiteral("sipUseIce"), QStringLiteral("sipEnableSrtp"), QStringLiteral("sipCompatibility"),
        QStringLiteral("debug"), QStringLiteral("secretRequired"),
        QStringLiteral("savePassword"), QStringLiteral("password")
    };

    QList<StoredProfile> out;
    const int count = settings.beginReadArray(QStringLiteral("connections"));
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        StoredProfile profile;
        for (const QString &key : keys) {
            if (settings.contains(key)) profile.values.insert(key, settings.value(key));
        }
        const QString protocol = profile.values.value(QStringLiteral("protocol")).toString();
        const QString server = profile.values.value(QStringLiteral("server")).toString().trimmed().toCaseFolded();
        const QString port = profile.values.value(QStringLiteral("port")).toString();
        const QString username = profile.values.value(QStringLiteral("username")).toString().trimmed().toCaseFolded();
        profile.dedupeKey = protocol + QLatin1Char('|') + server + QLatin1Char('|') + port + QLatin1Char('|') + username;
        if (!server.isEmpty() || !username.isEmpty()) out.append(profile);
    }
    settings.endArray();
    return out;
}

void migrateLegacyProfiles()
{
    QSettings current;
    if (current.value(QStringLiteral("connections/size"), 0).toInt() > 0) return;

    QList<StoredProfile> merged;
    QSet<QString> seen;
    const QList<QPair<QString, QString>> stores = {
        {QStringLiteral("WaffleHouseGUI"), QStringLiteral("WaffleHouseGUI")},
        {QStringLiteral("WaffleHouse-CLI"), QStringLiteral("WaffleHouse-CLI")}
    };

    for (const auto &store : stores) {
        QSettings legacy(store.first, store.second);
        for (StoredProfile profile : readProfiles(legacy)) {
            if (seen.contains(profile.dedupeKey)) continue;
            seen.insert(profile.dedupeKey);
            if (profile.values.value(QStringLiteral("id")).toString().trimmed().isEmpty()) {
                profile.values.insert(QStringLiteral("id"),
                    QStringLiteral("migrated-%1").arg(QString::fromLatin1(
                        QCryptographicHash::hash(profile.dedupeKey.toUtf8(), QCryptographicHash::Sha256)
                            .left(16).toHex())));
            }
            merged.append(profile);
        }
    }

    if (merged.isEmpty()) return;
    current.remove(QStringLiteral("connections"));
    current.beginWriteArray(QStringLiteral("connections"));
    for (int i = 0; i < merged.size(); ++i) {
        current.setArrayIndex(i);
        for (auto it = merged.at(i).values.constBegin(); it != merged.at(i).values.constEnd(); ++it)
            current.setValue(it.key(), it.value());
    }
    current.endArray();
    current.setValue(QStringLiteral("migration/legacyWaffleHouseProfiles"), true);
    current.sync();
}
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    configureApplicationIdentity();

    QCommandLineParser parser;
    parser.setApplicationDescription(
        QStringLiteral("WaffleHouse-Client %1 for Termux — AIM/OSCAR, IRC, Telnet/BBS, SIP/VoIP, secure rooms, file transfer and media")
            .arg(appVersionString()));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(QCommandLineOption(QStringList{QStringLiteral("cli")}, QStringLiteral("Compatibility option; Termux is CLI-only.")));
    parser.addOption(QCommandLineOption(QStringList{QStringLiteral("gui")}, QStringLiteral("Unsupported on the Termux build.")));
    parser.process(app);

    if (parser.isSet(QStringLiteral("gui"))) {
        std::fprintf(stderr, "WaffleHouse-Client-Termux Build 1.1 is a native terminal build; --gui is not available.\n");
        return 2;
    }

    runStartupAudioPreflight();
    migrateLegacyProfiles();
    const RuntimeEnvironment runtime = RuntimeEnvironment::detect();
    TerminalUi ui;
    QObject::connect(&ui, &TerminalUi::finished, &app, &QCoreApplication::quit);
    if (!ui.start()) {
        std::fprintf(stderr, "WaffleHouse-Client %s: terminal frontend could not start.\nDetected: %s\n",
                     appVersionString().toUtf8().constData(), runtime.summary().toUtf8().constData());
        return 1;
    }
    return app.exec();
}
