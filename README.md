# Kea Static Domain Hook
This hook overrides the domain part of a FQDN send back to a client or to the DDNS server with a static domain, which
becomes handy when doing DDNS updates. In this case the Kea DHCP server is connected to an authoritative dns server
which is responsible for a domain like `dhcp.domain.tld`. However in some cases the client sends a different domain in
the FQDN option (81) as the DNS server is responsible for. This is the case when a Windows client is joined to an Active
Directory domain, which uses a different domain then configured in the authoritative DNS server
like `active-directory.domain.tld`

When Kea receives a `DHCPDiscover` or `DHCPOffer` with a FQDN option containing the domain `active-directory.domain.tld`
the update requests to the authoritative DNS server contains the wrong domain, because Kea forwards the FQDN 1:1 to the
authoritative server. When using the option `ddns-qualifying-suffix` set to `dhcp.domain.tld` and the client sends a
FQDN for `active-directory.domain.tld` Kea would send an update request
for `hostname.active-directory.domain.tld.dhcp.domain.tld` to the authoritative server.

To prevent this issue you can use this hook which ensures that the domain part of the FQDN is static. When configuring
the static domain `dhcp.domain.tld` as an argument and the client sends the FQDN `hostname.active-directory.domain.tld`
Kea would send the FQDN `hostname.dhcp.domain.tld` to the authoritative dns server and back to the client.

This override of the FQDN send by the client is allowed as specified in RFC4702 section 4: https://datatracker.ietf.org/doc/html/rfc4702#section-4

**The hook is currently only implemented for IPv4 yet. An update for IPv6 will follow**

## Build
You need to install the following requirements. The `isc-kea-dev` package can be installed from the Kea Repository as described here: https://kea.readthedocs.io/en/latest/arm/install.html 
```
apt-get install cmake make g++ libboost-dev isc-kea-dev
```
Exec the following command to build the project:
```shell
cmake .
make
```

Copy the `libkea_static_domain_hook.so` to `/usr/lib/x86_64-linux-gnu/kea/`
```shell
cp libkea_static_domain_hook.so /usr/lib/x86_64-linux-gnu/kea/
```

## Configuration
Add the following section to your Kea configuration file and specify the location of the library file.
```json
"hooks-libraries": [
  {
    "library": "/usr/lib/x86_64-linux-gnu/kea/hooks/libkea_static_domain_hook.so",
    "parameters": {
      "static_domain": "dhcp.domain.tld."
    }
  }
],
```