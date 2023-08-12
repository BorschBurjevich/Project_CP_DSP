/*
 *  ======== MainHost.c ========
 *  главное ядро в IPC обмене
 */

// xdctools header files
#include <xdc/std.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Timestamp.h>

// package header files
#include <ti/ipc/Ipc.h>
#include <ti/ipc/MultiProc.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

// local header files
#include "App.h"
#include "shared/AppCommon.h"

// TI-RTOS Header files
#include <ti/drv/spi/SPI.h>
#include <ti/drv/spi/soc/SPI_v1.h>
#include <ti/drv/spi/soc/SPI_soc.h>
#include <ti/drv/spi/test/src/SPI_log.h>
#include <ti/drv/spi/test/src/SPI_test.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

Void IPC(UArg arg0, UArg arg1);

extern void delay(unsigned int delayValue);
extern Semaphore_Handle semen;

UInt32  txBuf_DSP0[N_DOTS_IPC] = {0};
UInt32  txBuf_DSP1[N_DOTS_IPC] = {0};
UInt32  rx_buff[N_DOTS_IPC] = {0};
uint8_t bearing_flag = 0x1;


Void IPC(UArg arg0, UArg arg1)
{

    Int         i;
    Int         status = 0;
    UInt16      remoteProcId[2] = {0};
    UInt16      App_MsgHeap[2] = {0};
    UInt32      slaveQue[2] = {0};
    App_Module  DSP[2] = {0};
    //Int         cnt;
    //UInt32      t1, t2, delta;

    Ipc_start();
    remoteProcId[0] = MultiProc_getId("DSP1");
    remoteProcId[1] = MultiProc_getId("DSP2");


/*
   //этот кусочек нужен если включена парная синхронизация
    Int         status[2] = {0};
    do {
        status[0] = Ipc_attach(remoteProcId[0]);
        status[1] = Ipc_attach(remoteProcId[1]);

    } while ((status[0] < 0) ||(status[1] < 0));
*/


    App_MsgHeap[0] = 1;
    App_MsgHeap[1] = 2;
    slaveQue[0] = 0x00010080;
    slaveQue[1] = 0x00020080;

    App_create(remoteProcId[0], App_MsgHeap[0], slaveQue[0], &DSP[0]);
    App_create(remoteProcId[1], App_MsgHeap[1], slaveQue[1], &DSP[1]);

    //тестовый обмен сообщениями
    do
    {
        status = test_msg(&DSP[0], &DSP[1]);
    }while(status != 0x1);
    printf("\n ipc connect completed \n");


    int cc = 0;
    int j = 0;
    while(1)
    {
        Semaphore_pend(semen, BIOS_WAIT_FOREVER);

        if(bearing_flag == 0x1)
        {
            for (i = 0; i < N_DOTS_IPC; ++i)
            {
                txBuf_DSP0[i] = 0xbabeface;
                txBuf_DSP1[i] = 0xbabec0de;
            }

            //t1 = Timestamp_get32();
            //t2 = Timestamp_get32();
            //delta = t2 - t1;

            send_mess(txBuf_DSP0, &DSP[0]);
            send_mess(txBuf_DSP1, &DSP[1]);

            receive_mess(rx_buff, &DSP[0]);
            receive_mess(rx_buff, &DSP[1]);

            //тут производятся считывания с вип порта и тд
            if ((cc == 0) || (cc == 10000))
            {
                printf("\n ipc cnt = %d\n", j);
                j++;
                cc = 0;
            }
            cc++;
        }

        Semaphore_post(semen);
        Task_sleep(5);
    }

    App_delete(App_MsgHeap[0], &DSP[0]);
    App_delete(App_MsgHeap[1], &DSP[1]);
    Ipc_stop();
}


