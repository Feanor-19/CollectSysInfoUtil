#include "collecters.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

#define OUT_DIR "collect_sys_info_data"
#define OS_INFO OUT_DIR "/" "os_info.txt"

enum MainRetCodes
{
    RET_OK = 0,    // no errors occured
    RET_PARTLY_OK, // some parts had errors
    RET_NOT_SUDO,  // run not as sudo
    RET_OUT_DIR,   // can't prepare out dir
};

int main()
{
    if (!is_sudo())
    {
        printf("This program must be run using sudo. Please, try again.\n");
        return RET_NOT_SUDO;
    }

    int err = 0;
    if (!prep_out_dir(OUT_DIR, &err))
    {
        printf("Can't prepare out dir: %s", strerror(err));
        return RET_OUT_DIR;
    }

    OSInfo info;
    if (collect_OSInfo(&info, &err))
    {
        if (!write_os_info_into_file(info, OS_INFO))
            printf("Can't write OS info into file \"%s\", error: %s", OS_INFO, strerror(err));

        free_OSInfo(&info);
    }
    else
        printf("Can't collect OS info: %s\nSkipping this part...\n\n", strerror(err));

    

    if (err)
        return RET_PARTLY_OK;
    
    return RET_OK;
}
