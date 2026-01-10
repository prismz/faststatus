#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *loadavg(void *arg)
{
        (void)arg;

        double avgs[3];
        getloadavg(avgs, 3);

        char *buf = safe_malloc(sizeof(char) * 512);

        if (strcmp(arg, "1") == 0) {
                snprintf(buf, 512, "%.2f",
                                avgs[0]);
        } else if (strcmp(arg, "5") == 0) {
                snprintf(buf, 512, "%.2f",
                                avgs[1]);
        } else if (strcmp(arg, "15") == 0) {
                snprintf(buf, 512, "%.2f",
                                avgs[2]);
        } else if (strcmp(arg, "all") == 0) {
                snprintf(buf, 512, "%.2f %.2f %.2f",
                                avgs[0], avgs[1], avgs[2]);
        } else {
                snprintf(buf, 512, "invalid format");
        }

        return buf;
}
