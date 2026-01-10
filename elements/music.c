#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *command(char *command);

void parse_cmus_attr(char *line, char **attr, char **val)
{
        char *buf = strdup(line);

        size_t len = strlen(line);
        *attr = safe_malloc(sizeof(char) * (len + 1));
        *val = safe_malloc(sizeof(char) * (len + 1));

        bzero(*attr, len + 1);
        bzero(*val, len + 1);

        /* skip the word "tag" or "set" at the beginning of cmus-remote -Q */
        int offset = 0;
        if (strncmp(line, "tag ", 4) == 0 || strncmp(line, "set ", 4) == 0)
                offset = 4;

        /* get the first word, which is the key */
        size_t i;
        for (i = offset; i < len; i++) {
                if (isspace(line[i]))
                        break;
        }

        /*
         * no value, e.g. "tag comment". "tag" will be
         * skipped, leaving "comment", which can't be separated.
         */
        if (i == len - 1) {
                /*
                 * we don't free *attr and *val because they should be freed
                 * by the caller.
                 */
                goto end;
        }

        strncpy(*attr, line + offset, MIN(i - offset, len));
        strncpy(*val, line + i + 1, len);  /* +1 for leading space */

end:
        free(buf);
}

char *music_cmus(void)
{
        char *cmus_data = command("cmus-remote -Q 2> /dev/null");
        if (cmus_data == NULL)
                return NULL;

        char *buf = safe_malloc(sizeof(char) * 1024);
        strcpy(buf, "?");

        char *tok = strtok(cmus_data, "\n");

        char *status   = NULL;
        char *artist   = NULL;
        char *title    = NULL;
        char *date     = NULL;
        char *vol_l    = NULL;
        char *vol_r    = NULL;
        char *duration = NULL;
        char *position = NULL;
        char *album    = NULL;
        char *path     = NULL;  /* fallback if no metadata */

        while (tok != NULL) {
                char *attr = NULL;
                char *val = NULL;
                int used = 0;
                parse_cmus_attr(tok, &attr, &val);

#define get(key, dest) {\
        if (val != NULL && attr != NULL && strcmp(attr, key) == 0) {\
                dest = val;\
                used = 1;\
        }\
}
                get("status", status);
                get("duration", duration);
                get("position", position);
                get("artist", artist);
                get("title", title);
                get("date", date);
                get("vol_left", vol_l);
                get("vol_right", vol_r);
                get("album", album);
                get("file", path);

                free(attr);
                if (!used)
                        free(val);

                tok = strtok(NULL, "\n");
        }

        free(cmus_data);

        if (status == NULL || strcmp(status, "stopped") == 0) {
                free(buf);
                buf = NULL;
                goto end;
        }

        char play_pause[32];
        char position_p[128];
        char duration_p[128];
        char vol_p[128];
        char path_basename[256];

        if (strcmp(status, "playing") == 0)
                strcpy(play_pause, ">");
        else
                strcpy(play_pause, "|");

        format_secs(atoi(position), position_p, 128);
        format_secs(atoi(duration), duration_p, 128);

        if (strcmp(vol_l, vol_r) == 0)
                strncpy(vol_p, vol_l, 128);
        else
                snprintf(vol_p, 128, "%s,%s", vol_l, vol_r);

        if (path != NULL)
                get_basename(path, path_basename, 256);

        /* full metadata */
        if (artist != NULL && album != NULL && title != NULL) {
                snprintf(buf, 1024, "%s %s / %s vol: %s%% - %s - %s - %s",
                                play_pause, position_p, duration_p, vol_p,
                                artist, album, title);

        /* fallback using path basename */
        } else if (path != NULL) {
                snprintf(buf, 1024, "%s %s / %s vol: %s%% - %s",
                                play_pause, position_p, duration_p, vol_p,
                                path_basename);
        }

        /* if the above don't work, then a simple "?" will be returned. */

end:
        free(status);
        free(duration);
        free(position);
        free(artist);
        free(title);
        free(date);
        free(vol_l);
        free(vol_r);
        free(album);
        free(path);

        return buf;
}

char *music_tidal(void)
{
        char *tidal_data = command("faststatus-tidal.sh");
        return tidal_data;
}

char *music_spotify(void)
{
        char *spotify_data = command("faststatus-spotify.sh");
        if (spotify_data == NULL)
                printf("no spotify\n");
        return spotify_data;
}
