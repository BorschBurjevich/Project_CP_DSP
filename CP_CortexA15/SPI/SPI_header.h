#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <stdio.h>
#include <ti/drv/spi/SPI.h>
#include <ti/drv/spi/soc/SPI_v1.h>
#include <ti/drv/spi/SPI.h>
#include <ti/drv/spi/soc/SPI_v1.h>
#include <ti/drv/spi/soc/SPI_soc.h>
#include <ti/drv/spi/test/src/SPI_log.h>
#include <ti/drv/spi/test/src/SPI_test.h>
#include <stdlib.h>
#include <ti/drv/spi/soc/SPI_soc.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/Timestamp.h>
#include <ti/sdo/edma3/drv/edma3_drv.h>
#include <ti/sdo/edma3/rm/edma3_rm.h>
#include <ti/sdo/edma3/rm/sample/bios6_edma3_rm_sample.h>
#include "delay/delay.h"

#define CSL_MPU_CORE_PAD_IO_SPI_REGISTERS_REGS          (0x4a003700U)
#define CTRL_CORE_PAD_SPI2_SCLK                     (0xC0U)     //SPI2_CLK
#define CTRL_CORE_PAD_SPI2_D1                       (0xC4U)     //SPI2_D1
#define CTRL_CORE_PAD_SPI2_D0                       (0xC8U)     //SPI2_D0
#define CTRL_CORE_PAD_SPI2_CS0                      (0xCCU)     //SPI2_CS0
#define CTRL_CORE_PAD_SPI2_MODE_SCLK                     0x0U//(0x50000U)
#define CTRL_CORE_PAD_SPI2_MODE_D1                       0x0U//(0xD0000U)
#define CTRL_CORE_PAD_SPI2_MODE_D0                       0x0U//(0xD0000U)
#define CTRL_CORE_PAD_SPI2_MODE_CS0                      0x0U//(0xD0000U)

#define NUM_CS_0 0
#define NUM_CS_1 1
//////////////////////////////
#define CS_0               (0U)
#define CS_1               (1U)
#define CS_2               (2U)
#define CS_3               (3U)
//////////////////////////////////////
#define multi   1
#define Single  0

#define mode_4 1
#define mode_3 0

#define SPI1_PORT        0U
#define SPI2_PORT        1U
#define SPI3_PORT        2U
#define SPI4_PORT        3U

#define spi1_cs0   (*((unsigned int *) 0x4809812C))
#define spi1_cs1   (*((unsigned int *) 0x48098140))
#define spi1_cs2   (*((unsigned int *) 0x48098154))

#define spi2_cs0   (*((unsigned int *) 0x4809A12C))
#define spi2_cs3   (*((unsigned int *) 0x4809A168))

#define spi3_cs0   (*((unsigned int *) 0x480B812C))
#define spi3_cs1   (*((unsigned int *) 0x480B8140))
#define spi3_cs2   (*((unsigned int *) 0x480B8154))
#define spi3_cs3   (*((unsigned int *) 0x480B8168))

#define spi4       (*((unsigned int *) 0x480BA12C))


//////////////////////////////////////////////////
//#define spi1   (*((unsigned int *) 0x4809812C))
//#define spi2   (*((unsigned int *) 0x4809A12C))
//#define spi3   (*((unsigned int *) 0x480B812C))
//#define spi4   (*((unsigned int *) 0x480BA12C))
/////////////////////////////////////////////////
SPI_Handle           SPI11_Handle;
MCSPI_Handle      AD9208_MCSPI1_CS0,AD9208_MCSPI1_CS1,AD9208_MCSPI1_CS2,MCSPI3_CS0,MCSPI3_CS1,MCSPI3_CS2,MCSPI3_CS3,MCSPI2_LVDS_CS0,MCSPI2_LVDS_CS3;

SPI_Handle   SPI_Master_Open(uint32_t portNum, uint32_t bit_rate,uint8_t frame_size);
MCSPI_Handle MCSPI_Master_Open(uint32_t portNum, uint8_t frm_size, uint32_t bit_rate,uint8_t num_spi);

void SPI_Close(SPI_Handle handle);
void SPI_callback(SPI_Handle handle, SPI_Transaction *transaction);
uint32_t spi_master_xfer(void *spi, uint8_t *data,uint32_t n);
bool spi1_write(void *spi, uint16_t addr, uint8_t data);
uint8_t spi1_read(void *spi, uint16_t addr);
bool spi3_write(void *spi,uint32_t cmd_in);
uint8_t spi3_read(void *spi,uint32_t cmd_in);
void  FUNC_SET_HMC830(void *spi_cs0,void *spi_cs1);
bool spi3FPGA_write(MCSPI_Handle pointer_spi,uint8_t* buf_trans ,int count_tr);
bool spi2_LVDS_write(MCSPI_Handle pointer_spi,uint16_t data,int count_tr);
uint8_t spi2_LVDS_read(MCSPI_Handle pointer_spi,int count_tr);
uint8_t rxSPI_Buf[4];
