#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char *uptime(void)
{
        char *buf = safe_malloc(sizeof(char) * 512);
        strncpy(buf, "NULL", 512);

        FILE *fp = fopen("/proc/uptime", "rb");
        if (!fp)
                return buf;

        float up, idle;
        fscanf(fp, "%f %f", &up, &idle);

        fclose(fp);

        int secs_in_hour = 60 * 60;
        int total_hours = floor(up / secs_in_hour);
        int days = floor((double)total_hours / 24);
        int rsec = up - (total_hours * secs_in_hour);
        int mins = floor((double)rsec / 60);
        int hours = total_hours - (24 * days);

        if (days > 1) {
                snprintf(buf, 512,
                                "up %d days, %02d:%02d", days, hours, mins);
        } else if (days == 1) {
                snprintf(buf, 512,
                                "up %d day, %02d:%02d", days, hours, mins);
        } else {
                snprintf(buf, 512,
                                "up %02d:%02d", hours, mins);
        }

        return buf;
}
