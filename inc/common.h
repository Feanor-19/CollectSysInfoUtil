#pragma once

#include <stdlib.h>

struct OSInfo
{
    char *os      = NULL; // e.g. #41~22.04.2-Ubuntu SMP PREEMPT_DYNAMIC Mon Jun  3 11:32:55 UTC 2
    char *kernel  = NULL; // e.g. 6.5.0-41-generic
    char *machine = NULL; // e.g. x86_64
};