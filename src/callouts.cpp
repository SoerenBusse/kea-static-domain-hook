#include <hooks/hooks.h>
#include <dhcpsrv/lease.h>
#include "library_common.h"
#include <log/macros.h>
#include <dhcp/pkt4.h>
#include <dhcp/option4_client_fqdn.h>

using namespace isc::hooks;
using namespace isc::dhcp;
using namespace isc::log;
using namespace isc::data;
using namespace std;

extern "C" {
// Initialize logger
isc::log::Logger staticDomainHookLogger("static-domain-hook");

// Define static domain
string staticDomain{""};

/*
* This method parses the FQDN supplied by the Client in the DHCPDISCOVER or DHCPREQUEST
* and replaces the domain part of the FQDN with a static domain supplied as argument for the hook
*/
string processDomainName(std::string &hostname) {
    // Try to get the first label of our domain
    string firstLabel{hostname.substr(0, hostname.find('.'))};

    // Append the first label to our static domain
    stringstream hostnameStringStream;
    hostnameStringStream << firstLabel << "." << staticDomain;

    return hostnameStringStream.str();
}

/**
 * This method is a wrapper method to process all lease related callouts
 * @param handle
 * @return 0 if the processing was successful otherwise 1
 */
int processLeaseHook(CalloutHandle &handle) {
    Lease4Ptr leasePtr;
    handle.getArgument("lease4", leasePtr);

    // If the domainName is empty we have nothing to do
    // Return immediately
    if (leasePtr->hostname_.empty()) {
        return 0;
    }

    string domainName{processDomainName(leasePtr->hostname_)};

    // Apply the new domainName
    leasePtr->hostname_ = domainName;

    // We save the hostname in the context
    handle.setContext("replacedHostname", domainName);

    return 0;
}

int load(LibraryHandle &handle) {
    // Parse arguments
    ConstElementPtr staticDomainElementPtr = handle.getParameter("static_domain");
    if (!staticDomainElementPtr) {
        LOG_ERROR(staticDomainHookLogger, STATIC_DOMAIN_HOOK_ARGUMENT_ERROR)
                    .arg("static_domain")
                    .arg("The argument is missing");

        return 1;
    }

    if (staticDomainElementPtr->getType() != Element::string) {
        LOG_ERROR(staticDomainHookLogger, STATIC_DOMAIN_HOOK_ARGUMENT_ERROR)
                    .arg("static_domain")
                    .arg("The argument is not a string");
        return 1;
    }

    if (staticDomainElementPtr->stringValue().back() != '.') {
        LOG_ERROR(staticDomainHookLogger, STATIC_DOMAIN_HOOK_ARGUMENT_ERROR)
                    .arg("static_domain")
                    .arg("The domain has to end with a dot");
        return 1;
    }

    // Assign value of parameter to the global value
    staticDomain = staticDomainElementPtr->stringValue();

    // Load was successful
    return 0;
}

int unload() {
    // Unload was successful
    return 0;
}

/**
 * Executed by the KEA DHCP-Server when the server selects a lease
 */
int lease4_select(CalloutHandle &handle) {
    return processLeaseHook(handle);
}

/**
 * Executed by the KEA DHCP-Server when the client requests a renew of a lease
 */
int lease4_renew(CalloutHandle &handle) {
    return processLeaseHook(handle);
}

/**
 * Executed by the KEA DHCP-Server when the server is sending a response
 * This callout is needed to modify the FQDN in the DHCP-Response as stated in RFC4702 Section 4.
 * So even if the client sends a FQDN option with test.domain.tld we send back our replaced FQDN with test.domain-replaced.tld
 */
int pkt4_send(CalloutHandle &handle) {
    // Retrieve the response packet
    Pkt4Ptr responsePktPtr;
    handle.getArgument("response4", responsePktPtr);

    // Get the FQDN option
    Option4ClientFqdnPtr fqdn = boost::dynamic_pointer_cast<Option4ClientFqdn>
            (responsePktPtr->getOption(DHO_FQDN));

    // Skip the processing if the option doesn't exist
    // If this option is missing kea hasn't done any FQDN processing, so there was also no hostname in the lease
    if (!fqdn) {
        return 0;
    }

    // Normally this should never fail, because if the response contains an FQDN there should also be a call
    // to the lease callouts, which sets the context key
    try {
        string domainName;
        handle.getContext("replacedHostname", domainName);

        fqdn->setDomainName(domainName, Option4ClientFqdn::DomainNameType::FULL);
    } catch (const NoSuchCalloutContext &) {
        // In this case we cannot do anything :(
    }

    return 0;
}
}