#pragma once

#include <stdlib.h>

struct OSInfo
{
    const char *sys_version = NULL; // #41~22.04.2-Ubuntu SMP PREEMPT_DYNAMIC Mon Jun  3 11:32:55 UTC 2
    const char *kernel      = NULL; // 6.5.0-41-generic
    const char *machine     = NULL; // x86_64
};

//! @brief Fills `struct OSInfo`.
//! @param[out] info OSInfo struct to be filled.
//! @param[out] error_code_p Optional, changed only if `false`
//!             is returned.
//! @return `true` if everything is okay, `false` otherwise 
//!         and *error_code_p is filled.
bool collect_OSInfo(OSInfo *info, int *error_code_p = NULL);