#pragma once
#include "types.hpp"

#include <net/ethernet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstring>
#include <filesystem>
#include <optional>
#include <sdbusplus/bus.hpp>
#include <stdplus/zstring.hpp>
#include <string>
#include <string_view>
#include <unordered_set>
#include <xyz/openbmc_project/Network/EthernetInterface/server.hpp>

namespace phosphor
{
namespace network
{
namespace config
{
class Parser;
}

using EthernetInterfaceIntf =
    sdbusplus::xyz::openbmc_project::Network::server::EthernetInterface;

namespace mac_address
{

/** @brief gets the MAC address from the Inventory.
 *  @param[in] bus - DBUS Bus Object.
 *  @param[in] intfName - Interface name
 */
ether_addr getfromInventory(sdbusplus::bus_t& bus, const std::string& intfName);

/** @brief Determines if the mac address is empty
 *  @param[in] mac - The mac address
 *  @return True if 00:00:00:00:00:00
 */
bool isEmpty(const ether_addr& mac);

/** @brief Determines if the mac address is a multicast address
 *  @param[in] mac - The mac address
 *  @return True if multicast bit is set
 */
bool isMulticast(const ether_addr& mac);

/** @brief Determines if the mac address is a unicast address
 *  @param[in] mac - The mac address
 *  @return True if not multicast or empty
 */
bool isUnicast(const ether_addr& mac);

} // namespace mac_address

template <int family>
struct FamilyTraits
{
};

template <>
struct FamilyTraits<AF_INET>
{
    using addr = in_addr;
};

template <>
struct FamilyTraits<AF_INET6>
{
    using addr = in6_addr;
};

/* @brief converts a sockaddr for the specified address family into
 *        a type_safe InAddrAny.
 * @param[in] family - The address family of the buf
 * @param[in] buf - The network byte order address
 */
template <int family>
typename FamilyTraits<family>::addr addrFromBuf(std::string_view buf);
InAddrAny addrFromBuf(int family, std::string_view buf);

/* @brief checks that the given ip address valid or not.
 * @param[in] family - IP address family(AF_INET/AF_INET6).
 * @param[in] address - IP address.
 * @returns true if it is valid otherwise false.
 */
bool isValidIP(int family, stdplus::const_zstring address) noexcept;
bool isValidIP(stdplus::const_zstring address) noexcept;

/** @brief Delete the given interface.
 *  @param[in] intf - interface name.
 */
void deleteInterface(stdplus::const_zstring intf);

/** @brief Converts the interface name into a u-boot environment
 *         variable that would hold its ethernet address.
 *
 *  @param[in] intf - interface name
 *  @return The name of th environment key
 */
std::optional<std::string> interfaceToUbootEthAddr(std::string_view intf);

/** @brief read the IPv6AcceptRA value from the configuration file
 *  @param[in] config - The parsed configuration.
 */
bool getIPv6AcceptRA(const config::Parser& config);

/** @brief read the DHCP value from the configuration file
 *  @param[in] config - The parsed configuration.
 */
struct DHCPVal
{
    bool v4, v6;
};
DHCPVal getDHCPValue(const config::Parser& config);

/** @brief Read a boolean DHCP property from a conf file
 *  @param[in] config - The parsed configuration.
 *  @param[in] key - The property name.
 */
bool getDHCPProp(const config::Parser& config, std::string_view key);

namespace internal
{

/* @brief runs the given command in child process.
 * @param[in] path - path of the binary file which needs to be execeuted.
 * @param[in] args - arguments of the command.
 */
void executeCommandinChildProcess(stdplus::const_zstring path, char** args);

/** @brief Get ignored interfaces from environment */
std::string_view getIgnoredInterfacesEnv();

/** @brief Parse the comma separated interface names */
std::unordered_set<std::string_view>
    parseInterfaces(std::string_view interfaces);

/** @brief Get the ignored interfaces */
const std::unordered_set<std::string_view>& getIgnoredInterfaces();

} // namespace internal

/* @brief runs the given command in child process.
 * @param[in] path -path of the binary file which needs to be execeuted.
 * @param[in] tArgs - arguments of the command.
 */
template <typename... ArgTypes>
void execute(stdplus::const_zstring path, ArgTypes&&... tArgs)
{
    using expandType = char*[];

    expandType args = {const_cast<char*>(tArgs)..., nullptr};

    internal::executeCommandinChildProcess(path, args);
}

} // namespace network

} // namespace phosphor
