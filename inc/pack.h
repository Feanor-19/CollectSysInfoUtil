#pragma once

#include <stdlib.h>
#include <stdio.h>

//! @brief Makes an archive with name `out` from files `filenames`.
//! @param[in] out          Name of the arhive to be created.
//! @param[in] filenames    NULL-termintated array of C strings with 
//!                         filenames. Don't forget to make last entry
//!                         NULL!
//! @param[out] error_code_p Optional, changed only if `false` is returned.
//! @param[in] warn_stream  Optional. If some warnings occur inside `libarchive`,
//!                         corresponding error messages will be printed here.
//! @return `true` if everything is okay, `false` otherwise 
//!         and *error_code_p is set.
bool archive_files(const char *out, const char * const *filenames, 
                   int *error_code_p = NULL, FILE *warn_stream = NULL);