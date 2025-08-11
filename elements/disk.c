#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

int update_diskstat_cache_file(char *path, time_t info_timestamp, int r, int w)
{
        FILE *fp = fopen(path, "wb+");
        if (!fp)
                return 1;

        fprintf(fp, "%ld %d %d", info_timestamp, r, w);
        fclose(fp);

        return 0;
}

int read_diskstat_cache_file(char *path, time_t *timestamp, int *r, int *w)
{
        FILE *fp = fopen(path, "rb");
        if (!fp)
                return 1;

        fscanf(fp, "%ld %d %d", timestamp, r, w);
        fclose(fp);

        return 0;
}

int get_current_disk_rw(char *dev, int *o_r, int *o_w)
{
	char buf[2048];

        FILE *ds_fp = fopen("/proc/diskstats", "rb");
        if (!ds_fp)
                return 1;

	while ((fgets(buf, 2048, ds_fp)) != NULL) {
		char name[2048];
		time_t current_time;
		int maj_num, min_num, r_completed, r_merged, s_read, r_ms,
		    w_completed, w_merged, s_written, w_ms, n_ios, io_ms,
		    io_wms;

		int n_read = sscanf(buf,
				"%d %d %s %d %d %d %d %d %d %d %d %d %d %d\n",
				&maj_num, &min_num, name, &r_completed,
				&r_merged, &s_read, &r_ms, &w_completed,
				&w_merged, &s_written, &w_ms, &n_ios, &io_ms,
				&io_wms
		);

		time(&current_time);

		if (n_read != 14)
			continue;

		if (strcmp(name, dev) != 0)
			continue;

                *o_r = s_read;
                *o_w = s_written;
        }

        fclose(ds_fp);

        return 0;
}

char *get_disk_rw(char *dev)
{
        char *cache_path = "/tmp/.diskstat-cache";
        time_t last_checked = 0;
        int last_read = 0;
        int last_wrote = 0;

        /* read saved data */
        read_diskstat_cache_file(cache_path, &last_checked, &last_read,
                        &last_wrote);

        /* get current data */
        int s_read, s_written;
        get_current_disk_rw(dev, &s_read, &s_written);

        /* update cache file */
        time_t current_time = time(NULL);
        update_diskstat_cache_file(cache_path, current_time, s_read, s_written);

        time_t interval = current_time - last_checked;

        int mbr = (s_read - last_read) / interval / 2 / 1024;
        int mbw = (s_written - last_wrote) / interval / 2 / 1024;

        char *usage_str = safe_malloc(sizeof(char) * 512);
        snprintf(usage_str, 512, "R:%dMB/s W:%dMB/s", mbr, mbw);

        return usage_str;
}
