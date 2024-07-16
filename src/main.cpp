#include "collecters.h"
#include "utils.h"

#include <stdio.h>


int main()
{
    if (!is_sudo())
    {
        printf("This program must be run using sudo. Please, try again.\n");
        return -1;
    }

    
}
