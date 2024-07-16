#include "collecters.h"

#include <sys/utsname.h>
#include <errno.h>
#include <assert.h>

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

    info->sys_version = sys_info.release;
    info->kernel      = sys_info.version;
    info->machine     = sys_info.machine;

    return true;

Failed:
    if (error_code_p) *error_code_p = err;
    return false;
}