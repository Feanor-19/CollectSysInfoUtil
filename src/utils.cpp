#include "utils.h"

#include <unistd.h>

bool is_sudo()
{
    return (getuid() == 0);
}