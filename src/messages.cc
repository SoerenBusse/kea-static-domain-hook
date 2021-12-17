// File created from /root/kea-static-domain-hook/src/messages.mes

#include <cstddef>
#include <log/message_types.h>
#include <log/message_initializer.h>

extern const isc::log::MessageID STATIC_DOMAIN_HOOK_ARGUMENT_ERROR = "STATIC_DOMAIN_HOOK_ARGUMENT_ERROR";
extern const isc::log::MessageID STATIC_DOMAIN_HOOK_LOAD_WITH_ARGUMENT = "STATIC_DOMAIN_HOOK_LOAD_WITH_ARGUMENT";

namespace {

const char* values[] = {
    "STATIC_DOMAIN_HOOK_ARGUMENT_ERROR", "Error while parsing argument \"%1\". Reason \"%2\"",
    "STATIC_DOMAIN_HOOK_LOAD_WITH_ARGUMENT", "Hook is loaded with static domain \"%1\"",
    NULL
};

const isc::log::MessageInitializer initializer(values);

} // Anonymous namespace

