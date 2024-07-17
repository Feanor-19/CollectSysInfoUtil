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

//! @brief Creates if needed `dirname`, setting it's permissions to `mode`.
//! @param[in] dirname          Directory to prepare.
//! @param[out] error_code_p    Optional, changed only if `false` is returned.
//! @return `true` if everything is okay, `false` otherwise 
//!         and *error_code_p is filled.
bool prep_out_dir(const char *dirname, mode_t mode = 0644, int *error_code_p = NULL);

//! @brief Writes given `OSInfo` into file with given `filename`.
//! @param[in]  info            Info to write into file.
//! @param[in]  filename        Filename to write into.
//! @param[out] error_code_p    Optional, changed only if `false` is returned.
//! @return `true` if everything is okay, `false` otherwise 
//!         and *error_code_p is filled.
bool write_os_info_into_file(OSInfo info, const char *filename, int *error_code_p = NULL);

void free_OSInfo(OSInfo *ptr);

//! @brief Prints to given `stream` message with error, returned by `exec_cmd_with_output`.
void print_exec_cmd_with_output_err(FILE *stream, int err, int exit_status);