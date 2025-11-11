#ifndef DEPS_H
#define DEPS_H

#include "dynamic.h"

// Dependency loading functions

// Load all dependencies specified by DT_NEEDED entries
// Parameters:
//   info: DynInfo structure with needed[] array populated
// Returns: 0 on success, -1 on error
// Side effects: Populates info->dep_handles with dlopen handles
int load_dependencies(DynInfo *info);

// Close all loaded dependencies
void close_dependencies(DynInfo *info);

// Check if a library is already loaded
// This can help avoid loading the same library twice
int is_library_loaded(const char *libname);

#endif /* DEPS_H */
