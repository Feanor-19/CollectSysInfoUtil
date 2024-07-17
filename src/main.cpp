#include "collecters.h"
#include "utils.h"
#include "pack.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define OS_INFO     OUT_DIR "/" "os_info.txt"
#define PACKAGES    OUT_DIR "/" "packages.txt"
#define JOURNALCTL  OUT_DIR "/" "journalctl.txt"
#define DMESG       OUT_DIR "/" "dmesg.txt"
#define LSOF_XORG   OUT_DIR "/" "lsof_xorg.txt"

enum MainRetCodes
{
    RET_OK = 0,    // no errors occured
    RET_PARTLY_OK, // some parts had errors
    RET_NOT_SUDO,  // run not as sudo
    RET_OUT_DIR,   // can't prepare out dir
    RET_ARCHIVE,   // didn't manage to create an archive
};

int main()
{
    if (!is_sudo())
    {
        printf("This program must be run using sudo. Please, try again.\n");
        return RET_NOT_SUDO;
    }

    bool is_fully_ok = true;

    int err = 0;
    if (!prep_out_dir(OUT_DIR, 0644, &err))
    {
        printf("Can't prepare out dir: %s\n", strerror(err));
        return RET_OUT_DIR;
    }

    OSInfo info;
    if (collect_OSInfo(&info, &err))
    {
        if (!write_os_info_into_file(info, OS_INFO, &err))
        {
            printf("Can't write OS info into file \"%s\", error: %s\n", OS_INFO, strerror(err));
            is_fully_ok = false;
        }
        free_OSInfo(&info);
    }
    else
    {
        printf("Can't collect OS info: %s\nSkipping this part...\n\n", strerror(err));
        is_fully_ok = false;
    }

    err = 0;
    int exit_status = 0;
    if (!exec_cmd_with_output("dpkg -l > %s", PACKAGES, &err, &exit_status))
    {
        printf("Can't collect installed packages: ");
        print_exec_cmd_with_output_err(stdout, err, exit_status);
        printf("Skipping this part...\n\n");
        is_fully_ok = false;
    }

    err = 0;
    exit_status = 0;
    if (!exec_cmd_with_output("journalctl --since \"1 day ago\" > %s", JOURNALCTL, &err, &exit_status))
    {
        printf("Can't collect info from journalctl: ");
        print_exec_cmd_with_output_err(stdout, err, exit_status);
        printf("Skipping this part...\n\n");
        is_fully_ok = false;
    }

    err = 0;
    exit_status = 0;
    if (!exec_cmd_with_output("dmesg --reltime --color=never > %s", DMESG, &err, &exit_status))
    {
        printf("Can't collect info from dmesg: ");
        print_exec_cmd_with_output_err(stdout, err, exit_status);
        printf("Skipping this part...\n\n");
        is_fully_ok = false;
    }

    err = 0;
    exit_status = 0;
    if (!exec_cmd_with_output("lsof -c Xorg > %s", LSOF_XORG, &err, &exit_status))
    {
        printf("Can't collect info from lsof about Xorg: ");
        print_exec_cmd_with_output_err(stdout, err, exit_status);
        printf("Skipping this part...\n\n");
        is_fully_ok = false;
    }

    err = 0;
    if (!archive_files(OUT_ARR, FILES_TO_AR, &err, stderr))
    {
        printf("Can't make an archive: %s\n", strerror(err));
        return RET_ARCHIVE;
    }

    if (!is_fully_ok)
        return RET_PARTLY_OK;
    
    return RET_OK;
}
