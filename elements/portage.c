#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

char *portage(void)
{
        char *buf = safe_malloc(sizeof(char) * 256);

        size_t c = 0;
        struct dirent *ent;
        DIR *dir = opendir("/var/db/pkg");
        if (!dir) {
		free(buf);
                return NULL;
	}

        while ((ent = readdir(dir)) != NULL) {
                char *n = ent->d_name;
                if (strcmp(n, ".") == 0 || strcmp(n, "..") == 0)
                        continue;

                char *p = safe_malloc(sizeof(char) * PATH_MAX);
                snprintf(p, PATH_MAX, "/var/db/pkg/%s", n);

                struct dirent *e2;
                DIR *d2 = opendir(p);
                if (!d2) {
                        free(p);
                        break;
                }

                while ((e2 = readdir(d2)) != NULL) {
                        char *n2 = e2->d_name;
                        if (strcmp(n2, ".") == 0 || strcmp(n2, "..") == 0)
                                continue;

                        c++;
                }

                closedir(d2);
                free(p);
        }
        closedir(dir);

        snprintf(buf, 256, "%zu", c);

        return buf;
}
