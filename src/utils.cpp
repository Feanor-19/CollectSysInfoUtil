#include "utils.h"

#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

bool is_sudo()
{
    return (getuid() == 0);
}

bool prep_out_dir(const char *dirname, mode_t mode, int *error_code_p)
{
    assert(dirname);

    struct stat st = {};

    if (stat(dirname, &st) != -1)
        return true; // dir already exists

    if (mkdir(dirname, mode) == -1)
    {
        if (error_code_p) *error_code_p = errno;
        return false;
    }

    return true;
}

bool write_os_info_into_file(OSInfo info, const char *filename, int *error_code_p)
{
    assert(filename);

    int err = 0;
    int printf_res = 0;

    FILE *file = fopen(filename, "w");
    if (!file)
    {
        err = errno;
        goto CleanUp;
    }

    printf_res = fprintf(file, "OS: %s\nKernel: %s\nMachine: %s\n", info.os, 
                                                                    info.kernel, 
                                                                    info.machine);
    if (printf_res < 0)
    {
        err = errno;
        goto CleanUp;
    }

CleanUp:
    if (file) fclose(file);
    bool res = true;
    if (err) res = false;
    if (error_code_p)  *error_code_p  = err;
    return res;
}

void free_OSInfo(OSInfo *ptr)
{
    if (ptr)
    {
        free(ptr->kernel);
        free(ptr->machine);
        free(ptr->os);

        ptr->kernel = NULL;
        ptr->machine = NULL;
        ptr->os = NULL;
    }
}

void print_exec_cmd_with_output_err(FILE *stream, int err, int exit_status)
{
    if      (err == ENOSYS)
        fprintf(stream, "/bin/sh is somehow not available.\n");
    else if (err == ECHILD && exit_status == 0)
        fprintf(stream, "Executed command didn't exit normally.\n");
    else if (err == ECHILD && exit_status != 0)
        fprintf(stream, "Executed command exited with non-zero code: %d\n", exit_status);
    else
        fprintf(stream, "%s\n", strerror(err));
}