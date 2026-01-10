#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "elements/util.h"

#include <pthread.h>
#include <X11/Xlib.h>

extern pthread_mutex_t dpy_mutex;
extern Display *dpy;

char *datetime(void *fmt);
char *memory(void *);
char *loadavg(void *fmt);
char *portage(void *);
char *uptime(void *);
char *keymap(void *);
char *music_cmus(void *);
char *music_tidal(void *);
char *music_spotify(void *);
char *command(void *command);
char *separator(void *s);
char *get_disk_rw(void *dev);
char *temp_sensor(void *hwmon_path);
char *temp_sensor_f(void *hwmon_path);

#endif
