#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *command(char *command)
{
        FILE *fp = popen(command, "r");
        if (!fp)
                return NULL;

        size_t size = 1024;
        size_t idx = 0;
        char *buf = safe_calloc(size, sizeof(char));

        char c;
        while ((c = fgetc(fp)) != EOF) {
                if (idx + 2 >= size)
                        buf = safe_realloc(buf, sizeof(char) * (size += 512));

                buf[idx++] = c;
                buf[idx] = '\0';
        }

        int rc = pclose(fp);
        if (rc != 0) {
                free(buf);
                return NULL;
        }

        if (buf[strlen(buf) - 1] == '\n')
                buf[strlen(buf) - 1] = '\0';

        return buf;
}
