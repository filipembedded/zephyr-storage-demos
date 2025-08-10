#include <math.h>
#include <stdio.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

static struct fs_littlefs main_fs;

struct fs_mount_t main_fs_mount = {
    .type = FS_LITTLEFS,
    .fs_data = &main_fs,
#ifdef CONFIG_STORAGE_MEDIUM_SD_CARD
    .flags = FS_MOUNT_FLAG_USE_DISK_ACCESS,
    .storage_dev = "SD_SPI",
#elif CONFIG_STORAGE_MEDIUM_NOR_FLASH
    .storage_dev = (void *)FIXED_PARTITION_ID(lfs_partition),
#endif
    .mnt_point = "/DEMO_MOUNT_POINT:",
};

int32_t boot_count = -1;

static int update_boot_count(void) {
    struct fs_file_t boot_count_file;
    fs_file_t_init(&boot_count_file);

    char filename[LFS_NAME_MAX];
    snprintf(filename, sizeof(filename), "%s/boot_count",
             main_fs_mount.mnt_point);
    int ret = fs_open(&boot_count_file, filename, FS_O_RDWR | FS_O_CREATE);
    if (ret < 0) {
        LOG_ERR("Could not open file!\n");
        return ret;
    }

    ret = fs_read(&boot_count_file, &boot_count, sizeof(boot_count));
    if (ret < 0) {
        LOG_ERR("Could not read from file!\n");
        return ret;
    }

    LOG_INF("Boot count: %d\n", (int)boot_count);

    ret = fs_seek(&boot_count_file, 0, FS_SEEK_SET);
    if (ret < 0) {
        LOG_ERR("Could not seek to beggining of the file!\n");
        return ret;
    }

    boot_count += 1;
    ret = fs_write(&boot_count_file, &boot_count, sizeof(boot_count));
    if (ret < 0) {
        LOG_ERR("Could not write to file!\n");
        return ret;
    }

    ret = fs_close(&boot_count_file);
    if (ret < 0) {
        LOG_ERR("Could not close file!\n");
        return ret;
    }

    return 0;
}

#if defined(CONFIG_USB_DEVICE_STACK_NEXT)
static struct usbd_context *sample_usbd;

static int enable_usb_device_next(void) {
    int err;

    sample_usbd = sample_usbd_init_device(NULL);
    if (sample_usbd == NULL) {
        return -ENODEV;
    }

    err = usbd_enable(sample_usbd);
    if (err) {
        return err;
    }

    return 0;
}
#endif /* defined(CONFIG_USB_DEVICE_STACK_NEXT) */

int main(void) {
    if (DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_console), zephyr_cdc_acm_uart)) {
#if defined(CONFIG_USB_DEVICE_STACK_NEXT)
        if (enable_usb_device_next()) {
            return 0;
        }
#else
        if (usb_enable(NULL)) {
            return 0;
        }
#endif
    }

#ifdef CONFIG_STORAGE_MEDIUM_SD_CARD
    const struct device *storage_device = DEVICE_DT_GET(DT_NODELABEL(sdhc0));
#elif CONFIG_STORAGE_MEDIUM_NOR_FLASH
    const struct device *storage_device = DEVICE_DT_GET(DT_NODELABEL(spiflash));
#endif

    if (!device_is_ready(storage_device)) {
        LOG_ERR("Storage device not ready");
        return;
    }

    int ret = fs_mount(&main_fs_mount);
    if (ret < 0) {
        LOG_ERR("Could not mount filesystem!\n");
    }

    ret = update_boot_count();
    if (ret < 0) {
        LOG_ERR("Could not update boot count!");
    }
}
