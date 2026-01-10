#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <math.h>

void die(char *fmt, ...)
{
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        exit(1);
}

void warning(char *fmt, ...)
{
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
}

void checkmem(void *ptr)
{
        if (ptr == NULL)
                die("error allocating memory.\n");
}

void *safe_malloc(size_t s)
{
        void *m = malloc(s);
        checkmem(m);
        return m;
}

void *safe_calloc(size_t n, size_t s)
{
        void *m = calloc(n, s);
        checkmem(m);
        return m;
}

void *safe_realloc(void *ptr, size_t s)
{
        void *m = realloc(ptr, s);
        checkmem(m);
        return m;
}

void format_secs(int secs, char *dest, size_t dest_size)
{
        double minr = (double)secs / 60;
        double secr = minr - floor(minr);
        secr *= 60;
        snprintf(dest, dest_size, "%02d:%02d",
                        (int)floor(minr), (int)round(secr));
}

void get_basename(char *path, char *dest, size_t dest_size)
{
        size_t len = strlen(path);
        long i;
        int leading_slash = 1;
        for (i = (long)(len - 1); i >= 0; i--) {
                char c = path[i];
                if (c != '/')
                        leading_slash = 0;

                if (!leading_slash && c == '/')
                        break;
        }

        snprintf(dest, dest_size, "%s", path + i);
}

int sleep_ms(long ms)
{
        struct timespec ts;
        int res;

        if (ms <= 0) {
                errno = EINVAL;
                return 1;
        }

        ts.tv_sec = ms / 1000;
        ts.tv_nsec = (ms % 1000) * 1000000;

        do {
                res = nanosleep(&ts, &ts);
        } while (res && errno == EINTR);

        return res;
}
