#include <hooks/hooks.h>

extern "C" {
int version() {
    // Return the kea version the library is build against
    return KEA_HOOKS_VERSION;
}
}