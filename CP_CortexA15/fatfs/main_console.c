/* TI-RTOS Header files */
#include <ti/csl/cslr_device.h>
#include "FATFS_log.h"
#include <ti/fs/fatfs/diskio.h>
#include "FATFS.h"
#include <ti/drv/mmcsd/MMCSD.h>
#include "fs_shell_app_utils.h"
#include "gpio/gpio.h"
#include <ti/drv/gpio/soc/GPIO_soc.h>


/************************** Internal functions ************************/


/************************** Global Variables **************************/
/* MMCSD function table for MMCSD implementation */
FATFS_DrvFxnTable FATFS_drvFxnTable = {
    MMCSD_close,
    MMCSD_control,
    MMCSD_init,
    MMCSD_open,
    MMCSD_write,
    MMCSD_read
};

/* FATFS configuration structure */
FATFS_HwAttrs FATFS_initCfg[_VOLUMES] =
{
#if defined (SOC_AM65XX) || defined (SOC_J721E) || defined (SOC_J7200)
    {  /* MMC1 is SD card  for AM65xx GP EVM */
        1U
    },
#else
    {
        0U
    },
#endif
    {
        1U
    },
    {
        2U
    },
    {
        3U
    }
};

/* FATFS objects */
FATFS_Object FATFS_objects[_VOLUMES];

/* FATFS configuration structure */
const FATFSConfigList FATFS_config = {
    {
        &FATFS_drvFxnTable,
        &FATFS_objects[0],
        &FATFS_initCfg[0]
    },

    {
         &FATFS_drvFxnTable,
         &FATFS_objects[1],
         &FATFS_initCfg[1]
    },

    {
         &FATFS_drvFxnTable,
         &FATFS_objects[2],
         &FATFS_initCfg[2]
    },

    {NULL, NULL, NULL},

    {NULL, NULL, NULL}
};

FATFS_Handle fatfsHandle = NULL;
Uint32 fatfsShellProcessFlag = 0;
Uint32 fs_media_needs_initialization=0;
Uint32 fs_media_needs_close=0;

/*
 *  ======== test function ========
 */

/*
 * 0 - ls
 * 1 - read app sd
 * 2 - read mlo sd
 * 3 - write app emmc
 * 4 - write mlo emmc
 */
void emmc_ls(void)
{
    FATFS_init();
    FATFS_open(1, NULL, &fatfsHandle);
    fatfsShellProcessFlag = 1;
    FSShellAppUtilsProcess(0);
    FATFS_close(fatfsHandle);
}

void sd_ls(void)
{
    FATFS_init();
    FATFS_open(0, NULL, &fatfsHandle);
    fatfsShellProcessFlag = 1;
    FSShellAppUtilsProcess(0);
    FATFS_close(fatfsHandle);
}

void read_app_from_sd(void)
{
    FATFS_init();
    FATFS_open(0, NULL, &fatfsHandle);
    fatfsShellProcessFlag = 1;
    FSShellAppUtilsProcess(1);
    FATFS_close(fatfsHandle);

}

void read_mlo_from_sd(void)
{
    FATFS_init();
    FATFS_open(0, NULL, &fatfsHandle);
    fatfsShellProcessFlag = 1;
    FSShellAppUtilsProcess(2);
    FATFS_close(fatfsHandle);

}

void write_app_to_emmc(void)
{
    FATFS_init();
    FATFS_open(1, NULL, &fatfsHandle);
    fatfsShellProcessFlag = 1;
    FSShellAppUtilsProcess(3);
    FATFS_close(fatfsHandle);
}

void write_mlo_to_emmc(void)
{
    FATFS_init();
    FATFS_open(1, NULL, &fatfsHandle);
    fatfsShellProcessFlag = 1;
    FSShellAppUtilsProcess(4);
    FATFS_close(fatfsHandle);
}

void delete_app_in_emmc(void)
{
    FATFS_init();
    FATFS_open(1, NULL, &fatfsHandle);
    fatfsShellProcessFlag = 1;
    FSShellAppUtilsProcess(5);
    FATFS_close(fatfsHandle);
}

void delete_mlo_in_emmc(void)
{
    FATFS_init();
    FATFS_open(1, NULL, &fatfsHandle);
    fatfsShellProcessFlag = 1;
    FSShellAppUtilsProcess(6);
    FATFS_close(fatfsHandle);
}

