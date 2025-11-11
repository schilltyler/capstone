#include "deps.h"
#include "utils.h"
#include <dlfcn.h>

// Problem 4: Load Dependencies

// Load all dependencies specified by DT_NEEDED entries
int load_dependencies(DynInfo *info) {
    // Your solution here!
}

// Close all loaded dependencies
void close_dependencies(DynInfo *info) {
    // Your solution here!
}

// Check if a library is already loaded
int is_library_loaded(const char *libname) {
    // Your solution here!
}
