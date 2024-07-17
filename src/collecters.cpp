#include "collecters.h"
#include "utils.h"

#include <sys/utsname.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

bool collect_OSInfo(OSInfo *info, int *error_code_p)
{
    assert(info);

    int err = 0;

    struct utsname sys_info = {};
    if (uname(&sys_info) != 0)
    {
        err = errno;
        goto Failed;
    }

    info->os      = strdup(sys_info.version);
    info->kernel  = strdup(sys_info.release);
    info->machine = strdup(sys_info.machine);

    if (!info->os || !info->kernel || !info->machine)
    {
        err = ENOMEM;
        goto Failed;
    }

    return true;

Failed:
    if (error_code_p) *error_code_p = err;
    return false;
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
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    int cmd_size = snprintf(NULL, 0, cmd_fmt, out_filename);
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
    //printf("cmd: %s\n", cmd);

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
