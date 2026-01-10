#include "handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <execinfo.h>

const char *dump_path = "/home/hasan/faststatus_trace.txt";
FILE *dump_fp = NULL;
int dump_fileno = -1;

int setup_dump_file(void)
{
        dump_fp = fopen(dump_path, "wb+");
        if (!dump_fp)
                return 1;

        dump_fileno = fileno(dump_fp);
        if (dump_fileno < 0) {
                fclose(dump_fp);
                return 2;
        }

        return 0;

}

static void full_write(int fd, const char *buf, size_t len)
{
        while (len > 0) {
                ssize_t ret = write(fd, buf, len);

                if ((ret == -1) && (errno != EINTR))
                        break;

                buf += (size_t) ret;
                len -= (size_t) ret;
        }
}

void segfault_handler()
{
        char *start = "SEGFAULT OCCURRED - BACKTRACE:\n";
        full_write(dump_fileno, start, strlen(start));

        void *array[10];
        size_t n = backtrace(array, 10);
        char **backtrace_syms = backtrace_symbols(array, n);

        for (size_t i = 1; i < n; i++) {
                size_t len = strlen(backtrace_syms[i]);
                full_write(dump_fileno, backtrace_syms[i], len);
                full_write(dump_fileno, "\n", 1);
        }

        fclose(dump_fp);

        exit(1);
}

/*
static int fake_main(void)
{
        signal(SIGSEGV, segfault_handler);
        setup_dump_file();

        int *number = NULL;
        printf ("number: %d\n", *number);

        return 0;
}
*/
