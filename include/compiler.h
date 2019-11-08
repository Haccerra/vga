#if !defined __COMPILER__
#define __COMPILER__

#include "../include/dictionary.h"
#include "../include/platform.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define __BUFFER__SIZE__ 256

extern uint8 number_of_commands;

struct Commands* parse(FILE*);

#endif /* __COMPILER__ */
