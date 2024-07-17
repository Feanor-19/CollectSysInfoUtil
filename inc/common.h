#pragma once

#include <stdlib.h>

struct OSInfo
{
    char *os      = NULL; // e.g. #41~22.04.2-Ubuntu SMP PREEMPT_DYNAMIC Mon Jun  3 11:32:55 UTC 2
    char *kernel  = NULL; // e.g. 6.5.0-41-generic
    char *machine = NULL; // e.g. x86_64
};

#define OUT_DIR     "CollectSysInfoData"
#define OUT_ARR     "sys_info.gz"

//! @brief All files which are to be added to an archive.
//! @attention THIS LIST MUST BE NULL-TERMINATED.
const char * const FILES_TO_AR[] = 
{
    OUT_DIR,
    "/etc/hosts",
    "/etc/resolv.conf",
    "/etc/ntp.conf",
    "/etc/krb5.conf",
    "/etc/samba/smb.conf",
    "/etc/nsswitch.conf",
    "/etc/NetworkManager/NetworkManager.conf",
    "/opt/cyberprotect/protego/etc/config.json",
    "/opt/cyberprotect/protego/logs",
    NULL
};