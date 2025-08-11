#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int meminfo_lookup(char *key, char *contents)
{
        char *buf = strdup(contents);

        char *tok = strtok(buf, "\n");
        while (tok != NULL) {
                if (strncmp(tok, key, strlen(key)) == 0) {
                        int val = 0;
                        char fmt[1024];
                        snprintf(fmt, 1024, "%s:\t%%d kB", key);

                        sscanf(tok, fmt, &val);
                        free(buf);
                        return val;
                }

                tok = strtok(NULL, "\n");
        }

        free(buf);

        return 0;
}

char *memory(void)
{
        char *buf = safe_malloc(sizeof(char) * 512);
        strncpy(buf, "NULL", 512);

        FILE *fp = fopen("/proc/meminfo", "rb");
        if (!fp)
                return buf;

        char *contents = safe_malloc(sizeof(char) * 4096);
        bzero(contents, 4096);

        char c;
        for (size_t i = 0; i < 4095; i++) {
                c = fgetc(fp);
                if (c == EOF)
                        break;
                contents[i] = c;
                contents[i + 1] = c;
        }
        fclose(fp);

#define lookup(v) meminfo_lookup(v, contents)

        int mtotal = lookup("MemTotal");
        int stotal = lookup("SwapTotal");
        int mfree = lookup("MemFree");
        int sfree = lookup("SwapFree");
        int mbuffers = lookup("Buffers");
        int mcache = lookup("Cached");
        int msrecl = lookup("SReclaimable");

        free(contents);

        int used = mtotal + stotal
                - mfree - sfree - mbuffers - mcache - msrecl;

        used = round((double)used / 1024);

        snprintf(buf, 512, "%d", used);

        return buf;
}
