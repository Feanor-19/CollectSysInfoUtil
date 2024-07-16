#pragma once

#include <stdlib.h>

struct OSInfo
{
    char *os      = NULL; // e.g. #41~22.04.2-Ubuntu SMP PREEMPT_DYNAMIC Mon Jun  3 11:32:55 UTC 2
    char *kernel  = NULL; // e.g. 6.5.0-41-generic
    char *machine = NULL; // e.g. x86_64
};

#define OUT_DIR     "CollectSysInfoData"
#define OS_INFO     OUT_DIR "/" "os_info.txt"
#define PACKAGES    OUT_DIR "/" "packages.txt"
#define JOURNALCTL  OUT_DIR "/" "journalctl.txt"
#define DMESG       OUT_DIR "/" "dmesg.txt"
#define OUT_ARR     "sys_info.gzip"

//! @brief All files which are to be added to an archive.
const char * const ALL_FILES[] = 
{
    OS_INFO,
    PACKAGES,
    JOURNALCTL,
    DMESG,
    "/etc/hosts",
    "/etc/resolv.conf",
    "/etc/ntp.conf",
    "/etc/krb5.conf",
    "/etc/samba/smb.conf",
    "/etc/nsswitch.conf",
    "/etc/NetworkManager/NetworkManager.conf",
    NULL
};