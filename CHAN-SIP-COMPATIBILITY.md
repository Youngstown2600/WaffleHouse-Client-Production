# Asterisk chan_sip compatibility

WaffleHouse does not need a separate `chan_sip` client library. `chan_sip` is an Asterisk server-side SIP channel driver; WaffleHouse speaks SIP using PJSIP 2.17.

For a legacy Asterisk account, select **Server compatibility: `asterisk-chan_sip`**.

The compatibility profile applies these interoperability choices:

- Digest credentials follow the registrar's challenge realm.
- RFC 5626 SIP-Outbound is disabled for the account.
- Registration Contact rewriting uses PJSUA's legacy unregister/re-register method.
- Via/source-port rewriting remains enabled for NAT-friendly classic SIP behavior.
- The account's Caller-ID/AOR domain is independent of the registrar domain.

Use `standard` if the registrar requires newer PJSIP defaults. `auto` remains the default general-purpose mode.
