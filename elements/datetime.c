#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *datetime(void *fmt)
{
        time_t t = time(NULL);
        char *buf = safe_malloc(sizeof(char) * 512);

        if (!strftime(buf, 512, (char *)fmt,
                                localtime(&t))) {
                strncpy(buf, "ERR", 512);
        }

        return buf;
}
