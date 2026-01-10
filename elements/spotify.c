#include "util.h"

#include <stdio.h>
#include <stdlib.h>

char *command(char *command);

typedef struct {
        int maxwidth;
        int padding;
        const char *current_text;
        int update_ms;
} Scroller;
