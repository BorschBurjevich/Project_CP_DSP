/*
 * Copyright (C) 2016-2018 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <stdio.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/Error.h>
#include <ti/board/board.h>
#include <ti/csl/soc.h>
#include "ti/drv/usb/example/common/hardware.h"
#include <ti/fs/fatfs/diskio.h>
#include <ti/fs/fatfs/FATFS.h>
#include "timer.h"
#include "types.h"
#include "fatfs_port_usbmsc.h"
#include "fs_shell_app_utils.h"
#include <ti/drv/usb/usb_drv.h>
#include <ti/drv/usb/usb_osal.h>
#include "usblib.h"
#include "usbhost.h"
#include "usbhmsc.h"
#include "UsbDefs.h"
#include <ti/drv/uart/UART_stdio.h>
#include "C:\ti\pdk_am57xx_1_0_19\packages\ti\drv\usb\src\xhci\usb_hcd_xhci.h"
#include "delay/delay.h"
#include "Zdnglobdef.h"
#include "Lorandit_functions/Cod_com.h"
#include <xdc/runtime/Timestamp.h>
/*****************************************************************************
*
* The instance data for the MSC driver.
*
*****************************************************************************/
unsigned int g_ulMSCInstance = 0;


/*****************************************************************************
*
* Hold the current state for the application.
*
****************************************************************************/
typedef enum
{
    /*
    * No device is present.
    */
    STATE_NO_DEVICE,

    /*
    * Mass storage device is being enumerated.
    */
    STATE_DEVICE_ENUM,

    /*
    * Mass storage device is ready.
    */
    STATE_DEVICE_READY,

    /*
    * An unsupported device has been attached.
    */
    STATE_UNKNOWN_DEVICE,

    /*
    * A power fault has occurred.
    */
    STATE_POWER_FAULT
} tState;

volatile tState g_eState;

uint32_t     g_fsHasOpened = 0;


/* ========================================================================== */
/*                                Prototypes                                  */
/* ========================================================================== */
void usbHostIntrConfig(USB_Params* usbParams);
void MSCCallback(uint32_t ulInstance, uint32_t ulEvent, void *pvData);
void usbCoreIntrHandler(uint32_t* pUsbParam);
void MPU_Inter(UArg a0, UArg a1);
////////////
void test_read_write_MPU();
void test_R_W_FIFO();
uint8_t set_SI5340A(uint32_t  data_in,uint8_t flag_read);


////////////
/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */


                                        
extern void consolePrintf(const char *pcString, ...);
extern uint32_t USBHostWrite(uint32_t, uint8_t, uint8_t*, uint32_t);
extern uint32_t USBHostRead (uint32_t, uint8_t, uint8_t*, uint32_t);
extern uint32_t compCode1;
extern void LMS_7002m_set();
extern uint32_t USBHCDControlTransfer(uint32_t ulIndex, tUSBRequest *pSetupPacket,
                          uint32_t ulDevAddress, unsigned char *pData,
                          uint32_t ulSize, uint32_t ulMaxPacketSize);


extern uint32_t USBHostControlTransfer(uint32_t instanceNumber,
                       usbSetupPkt_t *pSetupPkt, uint8_t* pData, uint32_t uSize);


/////////////////////////////////

void MPU_Inter(UArg a0, UArg a1)
{
    USB_Params  usb_host_params;
    USB_Handle  usb_handle;
    usbSetupPkt_t setupPacket;
    int         rc;

    memset(&Buf_Read_USB[0],0x0,sizeof(Buf_Read_USB[0])*size_buf_read);
    memset(&Buf_Write_USB[0],0x0,sizeof(Buf_Write_USB[0])*size_buf_wright);
    uint8_t Mas_setup[20];

    //osalTimerTest();

    usb_host_params.usbMode      = USB_HOST_MSC_MODE;
    usb_host_params.instanceNo   = USB_INSTANCE;
    usb_handle = USB_open(usb_host_params.instanceNo, &usb_host_params);

    if (usb_handle == 0) 
    {
        printf("\n Failed to open the USB driver.\n");
        while(1);
    }

    /*
    *Setup the INT Controller
    */
	usbHostIntrConfig (&usb_host_params);

	g_ulMSCInstance = USBHMSCDriveOpen(usb_host_params.instanceNo, 0, MSCCallback);
    //
    //Initialize the file system.
    //

    printf("\n USB driver in progress.\n");


    rc = USBHCDMain(USB_INSTANCE, g_ulMSCInstance);
    if (rc != 0)
    {
        printf("USBHCDMain failed with this error: 0x%x\n", rc);
    }

    // включение синхронного режима FTDI
   /* setupPacket.bmRequestType = 0x40;
    setupPacket.bRequest      = 0x0B;
    setupPacket.wValue        = 0x40ff;
    setupPacket.wIndex        = 0x00;
    setupPacket.wLength       = 0x00;

    USBHostControlTransfer(USB_INSTANCE, &setupPacket, Mas_setup, MAX_PACKET_SIZE_EP0);*/
    memset(&Buf_Read_USB[0],0x0,sizeof(Buf_Read_USB[0])*size_buf_read);
    memset(&Buf_Write_USB[0],0x0,sizeof(Buf_Write_USB[0])*size_buf_wright);
    LMS_7002m_set();

}

void usbHostIntrConfig(USB_Params* usbParams)
{
    int32_t mainIntNo = 0;
    HwiP_Params hwiInputParams;

    usb_osalHwiParamsInit(&hwiInputParams);

    /* Construct Hwi object for this USB interrupts. */
    hwiInputParams.name = NULL;
    hwiInputParams.arg  = (uintptr_t)usbParams;
    hwiInputParams.priority = 0x10U;
    hwiInputParams.evtId = 0;

    if (usbParams->instanceNo == 0) 
    {
        mainIntNo = SYS_INT_USB0;
    } 

    else 
    {
        mainIntNo = SYS_INT_USB1;
    }

    hwiInputParams.triggerSensitivity = 0x3; /* interrupt edge triggered */
    usb_osalRegisterInterrupt(mainIntNo,
                              (HwiP_Fxn)usbCoreIntrHandler,
							  &hwiInputParams);

    USB_irqConfig(usbParams->usbHandle, usbParams);   
}



/*****************************************************************************
*
* This is the callback from the MSC driver.
*
* \param ulInstance is the driver instance which is needed when communicating
* with the driver.
* \param ulEvent is one of the events defined by the driver.
* \param pvData is a pointer to data passed into the initial call to register
* the callback.
*
* This function handles callback events from the MSC driver.  The only events
* currently handled are the MSC_EVENT_OPEN and MSC_EVENT_CLOSE.  This allows
* the main routine to know when an MSC device has been detected and
* enumerated and when an MSC device has been removed from the system.
*
* \return Returns \e true on success or \e false on failure.
*
*****************************************************************************/
void MSCCallback(uint32_t ulInstance, uint32_t ulEvent, void *pvData)
{
    /*
    * Determine the event.
    */
    switch(ulEvent)
    {
        /*
        * Called when the device driver has successfully enumerated an MSC
        * device.
        */
        case MSC_EVENT_OPEN:
        {
            g_eState = STATE_DEVICE_ENUM;
            break;
        }

        /*
        * Called when the device driver has been unloaded due to error or
        * the device is no longer present.
        */
        case MSC_EVENT_CLOSE:
        {
            /*
            * Go back to the "no device" state and wait for a new connection.
            */
            g_eState = STATE_NO_DEVICE;

            g_fsHasOpened = 0;

            break;
        }

        default:
        {
            break;
        }
    }
}



/* main entry point for USB host core interrupt handler with USB Wrapper setup
* Matching interrupt call-back function API */
void usbCoreIntrHandler(uint32_t* pUsbParam)
{
    USB_coreIrqHandler(((USB_Params*)pUsbParam)->usbHandle, (USB_Params*)pUsbParam);
}

/*****************************************************************************
* main 
*
*****************************************************************************/

void   test_read_write_MPU()
{
    uint32_t mass_data[225] = {
                                #include "data_for_write.txt"
                               };
    uint8_t value = 0;
    uint8_t data_for_write = 0;
    uint32_t count = 0;
    int count_errors = 0;

    test_R_W_FIFO();
    ///////////////////////////////////////////////////////////////////
    // инит и настройка Si5340A //

    for (count = 0; count < 225; count ++)
    {
        value = set_SI5340A(mass_data[count],0);
        value = set_SI5340A(mass_data[count],1);
        /////////// проверка на ошибки  ////////////
        data_for_write = (uint8_t)(mass_data[count]&0x000000ff);
        if (value != data_for_write)
        {
            count_errors++;
        }
        ///////////////////////////////////////////
        udelay(300);
    }

    printf("Set SI5340A is finished \n");
    udelay(300);

}

uint8_t set_SI5340A(uint32_t data_in, uint8_t flag_read)
{
    uint32_t incr = 0;
    uint32_t i = 0;
    uint8_t return_value = 0;
    if (flag_read == 0)
    {
        Buf_Write_USB[0] = 0x4D; // шапка сообщения в FTDI
        incr++;
        Buf_Write_USB[1] = 0x08; // длина сообщения с учетом протокола SPI
        incr++;
        Buf_Write_USB[2] = 0x00; // команда уст-ки адресса в Si5340A
        incr++;
        Buf_Write_USB[3] = 0x01; // обращ-ие к регистру 0x01
        incr++;
        Buf_Write_USB[4] = 0x40; // команда на запись в регистр
        incr++;
        Buf_Write_USB[5] = (uint8_t)((data_in & 0x00FF0000)>>16); // переключаемся на заданную страницу
        incr++;
        Buf_Write_USB[6] = 0x00;
        incr++;
        Buf_Write_USB[7] = (uint8_t)((data_in & 0x0000FF00)>>8);  // установка адресса регистра для выбранной страницы
        incr++;
        Buf_Write_USB[8] = 0x40;
        incr++;
        Buf_Write_USB[9] = (uint8_t)(data_in & 0x000000FF);       // запись данных в  регистр выбранной страницы
        incr++;
        USBHostWrite(USB_INSTANCE, 0, &Buf_Write_USB[0], incr);

    }

    else
    {
        Buf_Write_USB[0] = 0x4D; // шапка сообщения в FTDI
        incr++;
        Buf_Write_USB[1] = 0x08; // длина сообщения с учетом протокола SPI
        incr++;
        Buf_Write_USB[2] = 0x00; // команда уст-ки адресса в Si5340A
        incr++;
        Buf_Write_USB[3] = 0x01; // обращ-ие к регистру 0x01
        incr++;
        Buf_Write_USB[4] = 0x40; // команда на запись в регистр
        incr++;
        Buf_Write_USB[5] = (uint8_t)((data_in & 0x00FF0000)>>16); // переключаемся на заданную страницу
        incr++;
        Buf_Write_USB[6] = 0x00;
        incr++;
        Buf_Write_USB[7] = (uint8_t)((data_in & 0x0000FF00)>>8);  // установка адресса регистра для выбранной страницы
        incr++;
        Buf_Write_USB[8] = 0x80;       // команда на чтение
        incr++;
        Buf_Write_USB[9] = 0x00;       // дата добиваю нулями
        incr++;
        USBHostWrite(USB_INSTANCE, 0, &Buf_Write_USB[0], incr);

        USBHostRead(USB_INSTANCE, 0, &Buf_Read_USB[0],3);
        USBHostRead(USB_INSTANCE, 0, &Buf_Read_USB[0],3);
        USBHostRead(USB_INSTANCE, 0, &Buf_Read_USB[0],3);
        return_value = Buf_Read_USB[2];
        return return_value;



    }
    return 1;

}

void  test_R_W_FIFO()
{
    uint32_t count = 0;
    int count_errors = 0;
    int return_value;
    int write_value = 0x56C512;

      Buf_Write_USB[0] = 0x0D;
      Buf_Write_USB[1] = 0x03;
      Buf_Write_USB[2] = 0x12;
      Buf_Write_USB[3] = 0xC5;
      Buf_Write_USB[4] = 0x56;

      for (count = 0; count < 50; count++)
      {
          USBHostWrite(USB_INSTANCE, 0, &Buf_Write_USB[0],5);
          USBHostRead(USB_INSTANCE, 0, &Buf_Read_USB[0],5);
          USBHostRead(USB_INSTANCE, 0, &Buf_Read_USB[0],5);
          USBHostRead(USB_INSTANCE, 0, &Buf_Read_USB[0],5);
          return_value = Buf_Read_USB[2];
          return_value |= Buf_Read_USB[3] << 8;
          return_value |= Buf_Read_USB[4] << 16;
          if (return_value != write_value)
          {
              count_errors ++;
              printf("Test FIFO read/write failed! /n");
          }
      }

}
