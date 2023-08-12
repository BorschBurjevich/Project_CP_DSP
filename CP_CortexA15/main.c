/*
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SPI/SPI_header.h>
#include <xdc/std.h>
#include <ti/sysbios/utils/Load.h>

#include <ti/board/board.h>
#include <xdc/cfg/global.h>

#include <ti/sysbios/BIOS.h>
#include <ti/osal/osal.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/family/arm/a15/Mmu.h>
#include <ti/sysbios/family/arm/a15/Cache.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/sysbios/knl/Clock.h>
#include <ti/drv/spi/SPI.h>
#include <ti/drv/spi/soc/SPI_soc.h>

#include "Lorandit_functions/Cod_com.h"
#include "Lorandit_functions/reciever/reciever.h"
#include "SPI/SPI_header.h"
#include "I2C/I2C.h"
#include "gpio/gpio.h"
#include "uart/uart.h"
#include <vip/Func_Loopback.h>

#include <ti/drv/i2c/soc/I2C_soc.h>
#include <Lorandit_functions/modes/modes.h>
#include <Lorandit_functions/modes/mode_Calibration/Calibration.h>
#include "delay/delay.h"
#include <Shared_mem_communication/Shared.h>

// Timer includes //
#include <sys/time.h>
#include <ti\sysbios\hal\Timer.h>


//////////////////////////
/* XDCtools Header files */

#include <ti/csl/soc.h>
#include "ti/drv/usb/example/common/hardware.h"
#include <ti/fs/fatfs/diskio.h>
#include <ti/fs/fatfs/FATFS.h>
#include "timer.h"
#include "types.h"
#include <fatfs_port_usbmsc.h>
#include <fs_shell_app_utils.h>
#include <ti/drv/usb/usb_drv.h>
#include <ti/drv/usb/usb_osal.h>
#include <usblib.h>
#include "usbhost.h"
#include "usbhmsc.h"
#include <ti/drv/uart/UART_stdio.h>
#include "C:\ti\pdk_am57xx_1_0_19\packages\ti\drv\usb\src\xhci\usb_hcd_xhci.h"

///////////////////////////


//об€ъвление глобальных переменных
Semaphore_Handle         semen;
extern  SOCKET              Us;
DATA_ALIGN(32) DATA_SECTION(".bss:taskStackSection")
UInt8             gLpbkAppTskStackMain[LPBK_APP_TSK_STACK_MAIN];
DATA_ALIGN(32) DATA_SECTION(".bss:taskStackSection")
UInt8             gLpbkAppTskStackEthernet[LPBK_APP_TSK_STACK_ETHERNET];
LpbkApp_Obj        gLpbkAppObj;
Task_Struct        gTskStruct;
uint8_t count_Start_data = 0;
uint8_t data_flow = 0;
uint8_t first_strt_ADC = 0x0;
Timer_Handle timerHandle;

//объ€вление функций
void            peripheral_dialog();
extern void     ethernet();
extern void     LpbkApp_tskMain(UArg arg0, UArg arg1);
extern void     MCSPI_Board_crossbarInit(void);
extern void     send_eth(SOCKET s, uint8_t *tx_buf, int size);
extern void     send_eth_ticket(SOCKET s, uint16_t com);
extern int      Board_crossbarInit(void);
extern void     taskFxn(UArg arg0, UArg arg1);
extern void     MPU_Inter(UArg a0, UArg a1);
int main(void)
{
    Board_initCfg boardCfg;
    Task_Params tskPrms;

    Mmu_DescriptorAttrs attrs3;
    extern char ti_sysbios_family_arm_a15_Mmu_Module_State_0_secondLevelTableBuf_0__A;
    extern char ti_sysbios_family_arm_a15_Mmu_Module_State_0_secondLevelTableBuf_1__A;
    uint32_t addr0 = (uint32_t)&ti_sysbios_family_arm_a15_Mmu_Module_State_0_secondLevelTableBuf_0__A;
    uint32_t addr1 = (uint32_t)&ti_sysbios_family_arm_a15_Mmu_Module_State_0_secondLevelTableBuf_1__A;

    Mmu_initDescAttrs(&attrs3);
    attrs3.type = Mmu_DescriptorType_TABLE;
    attrs3.shareable = 0;            // non-shareable
    attrs3.accPerm = 1;              // read/write at any privelege level
    attrs3.attrIndx = 0;             // Use MAIR0 Register Byte 3 for
                                      // determining the memory attributes
                                      // for each MMU entry
      // Update the first level table's MMU entry for 0x00000000 with the
      // new attributes.
    Mmu_setFirstLevelDesc((Ptr)0x00000000, (UInt64)addr0, &attrs3);
      // Update the first level table's MMU entry for 0x40000000 with the
      // new attributes.
    Mmu_setFirstLevelDesc((Ptr)0x40000000, (UInt64)addr1, &attrs3);
    boardCfg = BOARD_INIT_UNLOCK_MMR | BOARD_INIT_PINMUX_CONFIG | BOARD_INIT_MODULE_CLOCK | BOARD_INIT_UART_STDIO;
    Board_init(boardCfg);
    delayTimerSetup();

    semen = Semaphore_create(1, NULL, NULL);
    MCSPI_Board_crossbarInit();
    I2Cx_mux();
    UARTx_mux();
    Board_crossbarInit();

    /* USB 1 (1 base) */
    CSL_xbarIrqConfigure(CSL_XBAR_IRQ_CPU_ID_MPU, CSL_XBAR_INST_MPU_IRQ_76,  CSL_XBAR_USB1_IRQ_INTR0);  /* main irq */
    CSL_xbarIrqConfigure(CSL_XBAR_IRQ_CPU_ID_MPU, CSL_XBAR_INST_MPU_IRQ_77,  CSL_XBAR_USB1_IRQ_INTR1);  /* misc irq */

    /* USB 2 (1 base) */
    CSL_xbarIrqConfigure(CSL_XBAR_IRQ_CPU_ID_MPU, CSL_XBAR_INST_MPU_IRQ_78,  CSL_XBAR_USB2_IRQ_INTR0);  /* main irq */
    CSL_xbarIrqConfigure(CSL_XBAR_IRQ_CPU_ID_MPU, CSL_XBAR_INST_MPU_IRQ_92,  CSL_XBAR_USB2_IRQ_INTR1);  /* misc irq */

    Task_Params_init(&tskPrms);
    tskPrms.priority      = 4;
    tskPrms.stack         = gLpbkAppTskStackEthernet;
    tskPrms.stackSize     = sizeof (gLpbkAppTskStackEthernet);
    tskPrms.arg0          = (UArg) & gLpbkAppObj;
    //tskPrms.affinity      = 1;
    Task_create(ethernet, &tskPrms, NULL);

    Task_Params_init(&tskPrms);
    tskPrms.priority  = 5;
    tskPrms.stack     = gLpbkAppTskStackMain;
    tskPrms.stackSize = sizeof (gLpbkAppTskStackMain);
    tskPrms.arg0      = (UArg) & gLpbkAppObj;
    Task_construct(&gTskStruct, LpbkApp_tskMain, &tskPrms, NULL);
    gLpbkAppObj.tskHandle = Task_handle(&gTskStruct);

    Task_Params_init(&tskPrms);
    tskPrms.priority     = 3;
    tskPrms.stackSize    = 8000;
    //tskPrms.affinity     = Task_AFFINITY_NONE;
    Task_create (MPU_Inter, &tskPrms, NULL);






    BIOS_start();

    return (0);
}

