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

bool prep_out_dir(const char *dirname, int *error_code_p)
{
    assert(dirname);

    struct stat st = {};

    if (stat(dirname, &st) != -1)
        return true; // dir already exists

    if (mkdir(dirname, 0644) == -1)
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

bool exec_cmd_with_output(const char *cmd_fmt, const char *out_filename,
                          int *error_code_p, int *exit_status_p)
{
    assert(cmd_fmt);
    assert(out_filename);

    int err = 0;
    int sys_res = 0;
    int exit_status = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
    int cmd_size = snprintf(NULL, 0, cmd_fmt);
#pragma GCC diagnostic pop

    char *cmd = (char*) calloc((size_t) cmd_size + 1, sizeof(char));
    if (!cmd)
    {
        err = ENOMEM;
        goto CleanUp;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    snprintf(cmd, (size_t) cmd_size + 1, cmd_fmt, out_filename);
#pragma GCC diagnostic pop

    //DEBUG
    printf("cmd: %s\n", cmd);

    if (system(NULL) == 0)
    {
        err = ENOSYS; // somehow `/bin/sh` isn't available
        goto CleanUp;
    }

    sys_res = system(cmd);
    if (sys_res == -1)
    {
        err = errno;
        goto CleanUp;
    }
    else if (WIFEXITED(sys_res) && WEXITSTATUS(sys_res) != 0)
    {
        err = ECHILD;
        exit_status = WEXITSTATUS(sys_res);
        goto CleanUp; 
    }
    else if(!WIFEXITED(sys_res))
    {
        err = ECHILD;
        exit_status = 0;
    }

CleanUp:
    free(cmd);
    bool res = true;
    if (err) res = false;
    if (error_code_p)  *error_code_p  = err;
    if (exit_status_p) *exit_status_p = exit_status;
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