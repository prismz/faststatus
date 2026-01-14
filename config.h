#ifndef CONFIG_H
#define CONFIG_H

#include "elements.h"
#include "handler.h"

typedef struct {
        char *(*func)(void *);
        char *arg;
        char *fmt;
        long interval_ms;
} Func;

/*
 * (playerctl required for these)
 * music_tidal   tidal-hifi track info         arg: NULL
 * music_spotify spotify track info            arg: NULL
 * music_cmus    cmus track info               arg: NULL
 *
 * command       runs a command                arg: command
 *
 * get_disk_rw   disk r/w speed in MB/s        arg: device name (dmcrypt => dm-0,1,2,etc)
 * keymap        X11 keymap                    arg: NULL
 * uptime        system uptime                 arg: NULL
 * portage       number of gentoo packages     arg: NULL
 * loadavg       CPU load average              arg: 1, 5, 15 or all (man 3 getloadavg)
 * memory        memory usage in MiB           arg: NULL
 * datetime      date and time                 arg: time format (man 3 strftime)
 * temp_sensor   temp sensor reading           arg: hwmon path (/sys/class/hwmon/hwmon1/temp1_input)
 * temp_sensor_f  (same as above, just in farenheit)
 */

/* global update interval in ms */
#define UPDATE_INTERVAL_MS 100

/* max status length */
#define STATUS_MAXLEN 256

/* from extern in handler.h */
//const char *dump_path = "/home/hasan/faststatus_trace.txt";

static const Func funcs[] = {
//      { music_tidal,   NULL,                                       "[%s]",            1000 },
//      { music_cmus,    NULL,                                       "[%s]",            1000 },
        { command,       "/usr/local/bin/current_merge",             "[%s]",            5000 },
        { music_spotify, NULL,                                       "[%s]",            300 },
//      { get_disk_rw,   "dm-0",                                     "[%s]",            3000 },
        { keymap,        NULL,                                       "[%s",            600 },
//      { uptime,        NULL,                                       "[%s]",            50000 },
//      { command,       "qlop -r",                                  "[%s]",            30000 },
        { command,       "pipewire-volume",                          "|🔊 %s",          400 },
//      { portage,       NULL,                                       "[portage: %s]",   45000 },

/* hard-coded hwmon paths are unreliable,
 * write a script (/sys/class/drm/ for gpu, /sys/devices/pci* for cpu)
 */
//        { temp_sensor,   "/tmp/gpu_temp",                            "|G:%s",       1000 },
        { temp_sensor,   "/tmp/cpu_temp",                            "|%s",       800 },
        { loadavg,       "1",                                        "|%s",            600 },
        { memory,        NULL,                                       "|%sMiB]",         200 },
        { datetime,      "%a %b %d %I:%M:%S %p",                  "[%s]",            100 },
};

#endif
