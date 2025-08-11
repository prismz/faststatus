#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "elements/util.h"
#include <X11/Xlib.h>

extern Display *dpy;

char *datetime(char *fmt);
char *memory(void);
char *loadavg(char *fmt);
char *portage(void);
char *uptime(void);
char *keymap(void);
char *music_cmus(void);
char *music_tidal(void);
char *music_spotify(void);
char *command(char *command);
char *separator(char *s);
char *get_disk_rw(char *dev);
char *temp_sensor(const char *hwmon_path);
char *temp_sensor_f(const char *hwmon_path);

#endif
