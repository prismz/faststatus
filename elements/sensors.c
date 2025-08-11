#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* e.x. /sys/class/hwmon/hwmon1/temp1_input */
char *temp_sensor(const char *hwmon_path)
{
        FILE *fp = fopen(hwmon_path, "rb");
        if (!fp)
                return 0;

        char buf[1024];
        if (fgets(buf, 1024, fp) == NULL) {
                fclose(fp);
                return 0;
        }

        fclose(fp);

        size_t buf_len = strlen(buf);
        if (buf[buf_len - 1] == '\n')
                buf[buf_len - 1] = 0;

        char *temp;
        float sensor_value_raw = (float)strtol(buf, &temp, 10);
        long sensor_value = round(sensor_value_raw / 1000);

        char *result = malloc(9);
        snprintf(result, 9, "%ld°C", sensor_value);

        return result;

}

char *temp_sensor_f(const char *hwmon_path)
{
        FILE *fp = fopen(hwmon_path, "rb");
        if (!fp)
                return 0;

        char buf[1024];
        if (fgets(buf, 1024, fp) == NULL) {
                fclose(fp);
                return 0;
        }

        fclose(fp);

        size_t buf_len = strlen(buf);
        if (buf[buf_len - 1] == '\n')
                buf[buf_len - 1] = 0;

        char *temp;
        float sensor_value_raw = (float)strtol(buf, &temp, 10);
        long sensor_value = round(sensor_value_raw / 1000 * 9 / 5 + 32);

        char *result = malloc(9);
        snprintf(result, 9, "%ld°F", sensor_value);

        return result;


}
