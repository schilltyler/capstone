#ifndef INIT_H
#define INIT_H

#include "dynamic.h"

// Initialization function handlers

// Execute the .init_array functions
// These are constructor functions marked with __attribute__((constructor))
// They must be called in order, before jumping to the entry point
void call_init_array(DynInfo *info);

// Execute the DT_INIT function (legacy initialization)
// This is an older mechanism for initialization
// Should be called before .init_array
void call_init_function(DynInfo *info);

// Execute all initialization functions in the correct order:
// 1. DT_INIT (if present)
// 2. .init_array (if present)
void execute_initializers(DynInfo *info);

#endif /* INIT_H */
