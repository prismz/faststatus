#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define LEN(a) ((int)(sizeof(a) / sizeof(a[0])))
#define MIN(a, b) ((a < b) ? a : b)

/*
 * Documentation explaining /proc/diskstats:
 * https://www.kernel.org/doc/Documentation/ABI/testing/procfs-diskstats
 */
struct io_stats {
        int s_read;
        int s_written;
        time_t last_checked;
        bool initialized;
        FILE *ds_fp;
};

void die(char *fmt, ...);
void warning(char *fmt, ...);
void checkmem(void *ptr);
void *safe_malloc(size_t s);
void *safe_calloc(size_t n, size_t s);
void *safe_realloc(void *ptr, size_t s);
void format_secs(int secs, char *dest, size_t dest_size);
void get_basename(char *path, char *dest, size_t dest_size);
int sleep_ms(long ms);

#endif
