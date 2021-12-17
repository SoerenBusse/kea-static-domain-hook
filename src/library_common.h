#ifndef KEA_STATIC_DOMAIN_HOOK_LIBRARY_COMMON_H
#define KEA_STATIC_DOMAIN_HOOK_LIBRARY_COMMON_H

#include <log/message_initializer.h>
#include <log/macros.h>
#include <iostream>
#include "messages.h"

extern "C" {
extern isc::log::Logger staticDomainHookLogger;
extern std::string staticDomain;
}
#endif
