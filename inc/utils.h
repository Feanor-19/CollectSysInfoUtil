#pragma once

#include "common.h"

#include <stdlib.h>
#include <stdio.h>


#define FCLOSE(_file) do{   \
    fclose((_file));          \
    (_file) = NULL;           \
}while(0)                   


//! @brief Checks is prog run as sudo. 
bool is_sudo();

//! @brief Creates if needed `dirname`, setting it's permissions to '644'.
//! @param[in] dirname          Directory to prepare.
//! @param[out] error_code_p    Optional, changed only if `false` is returned.
//! @return `true` if everything is okay, `false` otherwise 
//!         and *error_code_p is filled.
bool prep_out_dir(const char *dirname, int *error_code_p = NULL);

//! @brief Writes given `OSInfo` into file with given `filename`.
//! @param[in]  info            Info to write into file.
//! @param[in]  filename        Filename to write into.
//! @param[out] error_code_p    Optional, changed only if `false` is returned.
//! @return `true` if everything is okay, `false` otherwise 
//!         and *error_code_p is filled.
bool write_os_info_into_file(OSInfo info, const char *filename, int *error_code_p = NULL);

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

void free_OSInfo(OSInfo *ptr);

//! @brief Prints to given `stream` message with error, returned by `exec_cmd_with_output`.
void print_exec_cmd_with_output_err(FILE *stream, int err, int exit_status);