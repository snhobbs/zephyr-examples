
/*
 * Generic Zephyr settings module
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/printk.h>
#include <errno.h>
#include <string.h>

#if defined(CONFIG_SETTINGS_FILE)
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>
#include <zephyr/storage/flash_map.h>
#endif

/* =========================
 * Configuration
 * ========================= */

#define SETTINGS_ROOT_NAME "app"

/* =========================
 * Stored values (defaults)
 * ========================= */

uint32_t app_counter = 0;
int32_t  app_offset  = 0;
char     app_name[16] = "default";

/* =========================
 * Settings handler
 * ========================= */

static int app_settings_set(const char *name, size_t len,
			    settings_read_cb read_cb, void *cb_arg)
{
	const char *next;
	int rc;

	if (settings_name_steq(name, "counter", &next) && !next) {
		if (len != sizeof(app_counter)) {
			return -EINVAL;
		}
		return read_cb(cb_arg, &app_counter, sizeof(app_counter));
	}

	if (settings_name_steq(name, "offset", &next) && !next) {
		if (len != sizeof(app_offset)) {
			return -EINVAL;
		}
		return read_cb(cb_arg, &app_offset, sizeof(app_offset));
	}

	if (settings_name_steq(name, "name", &next) && !next) {
		if (len >= sizeof(app_name)) {
			return -EINVAL;
		}
		rc = read_cb(cb_arg, app_name, sizeof(app_name));
		if (rc > 0) {
			app_name[rc - 1] = '\0';
		}
		return rc;
	}

	return -ENOENT;
}

static int app_settings_commit(void)
{
	/* Called after all settings are loaded */
	printk("Settings loaded: counter=%u offset=%d name=%s\n",
	       app_counter, app_offset, app_name);
	return 0;
}

static int app_settings_export(int (*cb)(const char *name,
					 const void *value, size_t len))
{
	cb("app/counter", &app_counter, sizeof(app_counter));
	cb("app/offset",  &app_offset,  sizeof(app_offset));
	cb("app/name",    app_name,     strlen(app_name) + 1);
	return 0;
}

static struct settings_handler app_settings = {
	.name = SETTINGS_ROOT_NAME,
	.h_set = app_settings_set,
	.h_commit = app_settings_commit,
	.h_export = app_settings_export,
};

/* =========================
 * Initialization
 * ========================= */

int app_settings_init(void)
{
	int rc;

#if defined(CONFIG_SETTINGS_FILE)
	static struct fs_mount_t fs_mnt;
	static struct fs_littlefs fs;

	fs_mnt.type = FS_LITTLEFS;
	fs_mnt.fs_data = &fs;
	fs_mnt.storage_dev =
		(void *)FIXED_PARTITION_ID(storage_partition);
	fs_mnt.mnt_point = "/settings";

	rc = fs_mount(&fs_mnt);
	if (rc < 0) {
		printk("Failed to mount settings FS (%d)\n", rc);
		return rc;
	}
#endif

	rc = settings_subsys_init();
	if (rc) {
		printk("settings init failed (%d)\n", rc);
		return rc;
	}

	rc = settings_register(&app_settings);
	if (rc) {
		printk("settings register failed (%d)\n", rc);
		return rc;
	}

	settings_load();
	return 0;
}
