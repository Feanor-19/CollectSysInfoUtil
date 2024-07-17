#pragma once

#include "common.h"

#include <stdlib.h>

//! @brief Fills `struct OSInfo`.
//! @param[out] info OSInfo struct to be filled.
//! @param[out] error_code_p Optional, changed only if `false` is returned.
//! @return `true` if everything is okay, `false` otherwise 
//!         and *error_code_p is set.
bool collect_OSInfo(OSInfo *info, int *error_code_p = NULL);

//! @brief Executes given cmd, which must have redirection of stdout.
//! @param[in] cmd_fmt              Command to execute with obligatory part "> %s".
//!                             `%s` will be replaced with given filename.
//!                             E.g. "dpkg -l > %s".
//! @param[in] out_filename     File to write result into.
//! @param[out] error_code_p    Optional, changed only if `false` is returned.
//! @param[out] exit_status_p   Optional, changed only if `false` is returned.
//!                             See `@note` for details.
//! @return `true` if everything is okay, `false` otherwise 
//!         and *error_code_p is filled.
//! @note if somehow `/bin/sh` isn't available, error_code = ENOSYS.
//! @note if executed command returned non-zero, error_code = ECHILD and
//!       exit_status is set. If executed command didn't exit normally,
//!       error_code = ECHILD and exit_status_p = 0.
bool exec_cmd_with_output(const char *cmd_fmt, const char *out_filename,
                          int *error_code_p = NULL, int *exit_status_p = NULL);
