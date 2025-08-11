#include "config.h"
#include "elements.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <X11/Xlib.h>

#define N LEN(funcs)
#define BUFSZ 128

typedef struct {
        Func func;
        int idx;
} Block;

/* from extern in elements.h */
Display *dpy;

static pthread_t threads[N];
static char thread_buffers[N][BUFSZ];
static char status[STATUS_MAXLEN];
static int done = 0;

static void setdwmstatus(char *str)
{
        if (XStoreName(dpy, DefaultRootWindow(dpy), str) < 0)
                die("XStoreName\n");
        XFlush(dpy);
}

void *start_func_thread(void *blockfull_voidptr)
{
        Block *block = (Block *)blockfull_voidptr;
        int idx = block->idx;

        if (block == NULL)
                return NULL;
        printf("started thread %d\n", idx);

        for (;;) {
                if (done)
                        break;

                clock_t start = clock();

                char *func_output = NULL;
                if (block->func.arg == NULL)
                        func_output = block->func.func();
                else
                        func_output = block->func.func(block->func.arg);

                if (func_output == NULL) {
                        bzero(thread_buffers[idx], BUFSZ);
                        sleep_ms(block->func.interval_ms);
                        continue;
                }

                strncpy(thread_buffers[idx], func_output, BUFSZ - 1);
                thread_buffers[idx][BUFSZ - 1] = 0;
                free(func_output);

                clock_t end = clock();
                double generation_time_ms = (((double)(end - start)) / CLOCKS_PER_SEC) * 1000;
                long time_to_sleep = (block->func.interval_ms - (long)generation_time_ms);

                if (time_to_sleep < 0)
                        time_to_sleep = 0;

                sleep_ms(time_to_sleep);
        }

        return NULL;
}

int main(void)
{
        if (!(dpy = XOpenDisplay(NULL)))
                die("XOpenDisplay");

        Block *blocks[N];
        // memset(blocks, 0, (sizeof(Block *) * N));
        for (int i = 0; i < N; i++) {
                Block *b = calloc(1, sizeof(Block));

                b->idx = i;
                b->func = funcs[i];
                blocks[i] = b;
                bzero(thread_buffers[i], BUFSZ);

                pthread_create(&(threads[i]), NULL, start_func_thread, (void *)b);
        }

        sleep_ms(UPDATE_INTERVAL_MS);

        for (;;) {
                bzero(status, STATUS_MAXLEN);
                size_t status_curr_len = 0;
                for (int i = 0; i < N; i++) {
                        if (thread_buffers[i][0] == 0)
                                continue;

                        char buf[BUFSZ] = {0};
                        size_t tried_to_write = snprintf(
                                        buf, BUFSZ,
                                        funcs[i].fmt, thread_buffers[i]);

                        if (tried_to_write >= BUFSZ)
                                fprintf(stderr, "warning: output truncated\n");

                        size_t len = strlen(buf);
                        if (status_curr_len + len > STATUS_MAXLEN - 4) {
                                strcat(status, "...");
                                break;
                        }

                        strncat(status, buf, BUFSZ - 1);
                        status_curr_len += len;
                }

                setdwmstatus(status);
                //printf("%s\n", status);

                sleep_ms(UPDATE_INTERVAL_MS);
        }

        done = 1;

        for (int i = 0; i < N; i++) {
                pthread_join(threads[i], NULL);
                free(blocks[i]);
        }

        XCloseDisplay(dpy);

}
