#include "config.h"

#include "network_config.hpp"

#include <fstream>
#include <string>

namespace phosphor
{
namespace network
{

namespace bmc
{
void writeDHCPDefault(const std::string& filename, const std::string& interface)
{
    std::ofstream filestream;

    filestream.open(filename);
    // Add the following line to your phosphor-network bbappend file
    // to control IPV6_ACCEPT_RA
    //   EXTRA_OECONF_append = " --enable-ipv6-accept-ra=yes"
    // If this switch is not present or set to 'no'
    // ENABLE_IPV6_ACCEPT_RA will be undefined.
    // The new value is only assigned on first boot, when the default
    // file is not present, or after the default file has been
    // manually removed.
    if (interface == "eth1")
    {
        filestream << "[Match]\nName=" << interface <<
                "\n[Network]\nDHCP=true\n"
                "LinkLocalAddressing=no\n"
#ifdef ENABLE_IPV6_ACCEPT_RA
                "IPv6AcceptRA=true\n"
#else
                "IPv6AcceptRA=false\n"

#endif
                "[DHCP]\nClientIdentifier=mac\nUseDNS=true\nUseDomains=true\nUseNTP=true\nUseHostname=true\nSendHostname=true\n";
    }
    else
    {
        filestream << "[Match]\nName=" << interface <<
                "\n[Network]\nDHCP=true\n"
#ifdef LINK_LOCAL_AUTOCONFIGURATION
                "LinkLocalAddressing=yes\n"
#else
                "LinkLocalAddressing=no\n"
#endif
#ifdef ENABLE_IPV6_ACCEPT_RA
                "IPv6AcceptRA=true\n"
#else
                "IPv6AcceptRA=false\n"

#endif
                "[DHCP]\nClientIdentifier=mac\nUseDNS=true\nUseDomains=true\nUseNTP=true\nUseHostname=true\nSendHostname=true\n";
    }
    filestream.close();
}
} // namespace bmc

} // namespace network
} // namespace phosphor