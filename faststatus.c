#include "config.h"
#include "handler.h"
#include "elements.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <execinfo.h>
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
pthread_mutex_t dpy_mutex;

Block *blocks[N];
static pthread_t threads[N];
static char thread_buffers[N][BUFSZ];
static pthread_mutex_t buffer_mutexes[N];
static char status[STATUS_MAXLEN];
static atomic_int done = 0;

static void setdwmstatus(char *str)
{
        pthread_mutex_lock(&dpy_mutex);
        if (XStoreName(dpy, DefaultRootWindow(dpy), str) < 0)
                die("XStoreName\n");
        XFlush(dpy);
        pthread_mutex_unlock(&dpy_mutex);
}

void *start_func_thread(void *blockfull_voidptr)
{
        Block *block = (Block *)blockfull_voidptr;
        if (block == NULL)
                return NULL;
        int idx = block->idx;

        printf("started thread %d\n", idx);

        for (;;) {
                if (atomic_load(&done))
                        break;

                clock_t start = clock();

                char *func_output = NULL;
                if (block->func.arg == NULL)
                        func_output = block->func.func(NULL);
                else
                        func_output = block->func.func(block->func.arg);

                if (func_output == NULL) {
                        printf("NULL output for block %d\n", idx);
                        pthread_mutex_lock(&buffer_mutexes[idx]);
                        bzero(thread_buffers[idx], BUFSZ);
                        pthread_mutex_unlock(&buffer_mutexes[idx]);

                        sleep_ms(block->func.interval_ms);
                        continue;
                }

                pthread_mutex_lock(&buffer_mutexes[idx]);

                strncpy(thread_buffers[idx], func_output, BUFSZ - 1);
                thread_buffers[idx][BUFSZ - 1] = 0;
                free(func_output);

                pthread_mutex_unlock(&buffer_mutexes[idx]);

                clock_t end = clock();
                double generation_time_ms = 0;
                long time_to_sleep = 0;

                if (end < start) {
                       time_to_sleep = block->func.interval_ms;
                       generation_time_ms = 0;
                } else {
                        generation_time_ms = (((double)(end - start)) / CLOCKS_PER_SEC) * 1000;
                        time_to_sleep = (block->func.interval_ms - (long)generation_time_ms);
                }

                sleep_ms(time_to_sleep);
        }

        return NULL;
}

void cleanup(void)
{
        for (int i = 0; i < N; i++) {
                pthread_mutex_destroy(&buffer_mutexes[i]);
                pthread_join(threads[i], NULL);
                free(blocks[i]);
        }

        pthread_mutex_lock(&dpy_mutex);
        XCloseDisplay(dpy);
        pthread_mutex_unlock(&dpy_mutex);
        pthread_mutex_destroy(&dpy_mutex);
}

void set_done()
{
        atomic_store(&done, 1);
}

int main(void)
{
        /* multithreading - unsure if this is needed */
        XInitThreads();

        signal(SIGINT, set_done);
        signal(SIGTERM, set_done);

        signal(SIGSEGV, segfault_handler);
        setup_dump_file();

        pthread_mutex_init(&dpy_mutex, NULL);

        pthread_mutex_lock(&dpy_mutex);
        if (!(dpy = XOpenDisplay(NULL)))
                die("XOpenDisplay");
        pthread_mutex_unlock(&dpy_mutex);

        // memset(blocks, 0, (sizeof(Block *) * N));
        for (int i = 0; i < N; i++) {
                pthread_mutex_init(&buffer_mutexes[i], NULL);
                Block *b = calloc(1, sizeof(Block));

                b->idx = i;
                b->func = funcs[i];
                blocks[i] = b;
                bzero(thread_buffers[i], BUFSZ);

                if (pthread_create(&(threads[i]), NULL, start_func_thread, (void *)b) != 0) {
                        free(b);
                        fprintf(stderr, "error: failed to start thread %d\n", i);
                        done = 1;
                }
        }

        if (!atomic_load(&done))  /* wait for status to initialize */
                sleep_ms(UPDATE_INTERVAL_MS);

        unsigned long iter_n = 1;
        for (;;) {
                if (atomic_load(&done))
                        break;

                if (iter_n == 0) {
                        /* cause a segfault to test handler - change val above */
                        int *ptr = NULL;
                        int val = *ptr;
                }

                bzero(status, STATUS_MAXLEN);
                size_t status_curr_len = 0;
                for (int i = 0; i < N; i++) {
                        pthread_mutex_lock(&buffer_mutexes[i]);
                        if (thread_buffers[i][0] == 0) {
                                pthread_mutex_unlock(&buffer_mutexes[i]);
                                continue;
                        }

                        char buf[BUFSZ];
                        long written = snprintf(
                                        buf, BUFSZ,
                                        funcs[i].fmt, thread_buffers[i]);

                        pthread_mutex_unlock(&buffer_mutexes[i]);

                        size_t buf_len = (written < 0) ? 0 :
                                (size_t)MIN(written, BUFSZ - 1);

                        if (status_curr_len + buf_len + 4 >= STATUS_MAXLEN) {
                                strcat(status, "...");
                                break;
                        }

                        strcat(status, buf);
                        status_curr_len += buf_len;

                }

                setdwmstatus(status);
                iter_n++;

                sleep_ms(UPDATE_INTERVAL_MS);
        }

        cleanup();

        return 0;
}
