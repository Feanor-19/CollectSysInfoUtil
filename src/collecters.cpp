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

bool collect_installed_packages(const char *out_filename, int *error_code_p,
                                int *exit_status_p)
{
    assert(out_filename);

    int err = 0;
    int exit_status = 0;
    if (!exec_cmd_with_output("dpkg -l > %s", out_filename, &err, &exit_status))
    {
        if (error_code_p) *error_code_p = err;
        if (exit_status) *exit_status_p = exit_status;
        return false;
    }

    return true;
}

bool collect_journalctl_last24h(const char *out_filename, int *error_code_p,
                                int *exit_status_p)
{
    assert(out_filename);

    int err = 0;
    int exit_status = 0;
    if (!exec_cmd_with_output("journalctl --since \"1 day ago\" > %s", out_filename, &err, &exit_status))
    {
        if (error_code_p) *error_code_p = err;
        if (exit_status) *exit_status_p = exit_status;
        return false;
    }

    return true;
}