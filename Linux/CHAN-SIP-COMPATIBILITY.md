# Asterisk chan_sip compatibility

WaffleHouse-Client uses PJSIP 2.17 internally as its SIP user-agent stack. The remote SIP server does **not** need to use PJSIP. WaffleHouse can register to standard SIP servers including Asterisk systems using the legacy `chan_sip` channel driver.

SIP accounts expose a **Server compatibility** setting:

- `auto` — normal WaffleHouse/PJSIP defaults.
- `standard` — standards-oriented normal SIP behavior.
- `asterisk-chan_sip` — conservative registration/NAT behavior intended for legacy Asterisk `chan_sip` peers and registrars.

In `asterisk-chan_sip` mode WaffleHouse disables RFC 5626 SIP-Outbound use for the account, uses legacy-safe Contact/Via rewriting, follows the registrar's Digest realm, and keeps the configured Caller-ID/AOR domain independent from the registrar domain.

This mode does not load or embed Asterisk `chan_sip`; it is interoperability behavior on the WaffleHouse SIP client side.
