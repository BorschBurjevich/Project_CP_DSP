///
/// \file LMS7002M_impl.c
///
/// Implementation details for the LMS7002M C driver.
/// This file defines the LMS7002M_t structure,
/// and the SPI transaction helper functions.
///
/// \copyright
/// Copyright (c) 2016-2017 Skylark Wireless
/// Copyright (c) 2014-2015 Fairwaves, Inc.
/// Copyright (c) 2014-2015 Rice University
/// SPDX-License-Identifier: Apache-2.0
/// http://www.apache.org/licenses/LICENSE-2.0
///

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "LMS7002M_impl.h"
#include "USB_N/LMS7002M/LMS7002M_logger.h"
#include <ti/board/src/flash/nor/spi/nor_spi.h>
#include "USB_N/LMS7002M/LMS7002M.h"
//////////////////////////////////////////////
#include "USB_N/LMS7002M/Common_inf_LMS/Com_USB_Func.h"
#include "delay/delay.h"
#include "Zdnglobdef.h"
#include "Lorandit_functions/Cod_com.h"
//////////////////////////////////////////////
extern uint32_t USBHostWrite(uint32_t, uint8_t, uint8_t*, uint32_t);
extern uint32_t USBHostRead (uint32_t, uint8_t, uint8_t*, uint32_t);
extern uint32_t compCode1;
//extern int Mas_Global[1600];
extern int Global_counter;
LMS_Info spiInfo =
{
    0,                          /* hwHandle */
    0,                          /* deviceId */
    0                           /* busWidth */
};

static SPI_Transaction transaction;     /* SPI transaction structure */
LMS7002M_STATUS LMS_spiCmdRead(SPI_Handle handle, uint8_t *cmdBuf, uint32_t cmdLen, uint8_t *rxBuf, uint32_t rxLen);

LMS7002M_STATUS LMS_spiCmdWrite(SPI_Handle handle, uint8_t *cmd, uint32_t cmdLen, uint8_t *txBuf, uint32_t rxLen);
//extern uint64_t mdelay(uint32_t ms);
/***********************************************************************
 * Create/destroy implementations
 **********************************************************************/
LMS7002M_t *LMS7002M_create( uint32_t *handle)
{
    LMS7002M_t *self = (LMS7002M_t *)malloc(sizeof(LMS7002M_t));    // выделяем память под указатель

    if (self == NULL)
    {
        return NULL;
    }

    LMS7002M_regs_init(&self->_regs[0]);
    LMS7002M_regs_init(&self->_regs[1]);
    self->regs = self->_regs;
    self->cgen_freq = 0.0;
    self->sxr_freq = 0.0;
    self->sxt_freq = 0.0;
    self->cgen_fref = 0.0;
    self->sxr_fref = 0.0;
    self->sxt_fref = 0.0;
    return self;
}
void LMS7002M_destroy(LMS7002M_t *self)
{
    free(self);
}

/***********************************************************************
 * Helper calls to format SPI transactions
 **********************************************************************/
void LMS7002M_spi_write(LMS7002M_t *self, const int addr, const uint32_t value)
{
    uint32_t data = (((uint32_t)1) << 31) | (((uint32_t)addr) << 16) | (value & 0xffff);
    self->spi_transact(self->spi_transact_handle, data, /*(uint8_t *)&rxBuf,*/ false/*no readback*/);
}

void LMS7002M_usb_write(const int addr, const uint32_t value)
{

    uint32_t data = 0;
    uint32_t incr = 0;

    data = (((uint32_t)1) << 31) | (((uint32_t)addr) << 16) | (value & 0xffff); // запись по протколу в LMS
    ////////////////
 //   Mas_Global[Global_counter] = data;
    Global_counter++;
    ///////////////


    Buf_Write_USB[0] = 0xAD;    // шапка сообщения в FTDI для записи в LMS
    incr++;
    Buf_Write_USB[1] = 0x04; // длина сообщения c учётом формата SPI для LMS
    incr++;
    Buf_Write_USB[2] = (uint8_t)((data & 0xFF000000)>>24);
    incr++;
    Buf_Write_USB[3] = (uint8_t)((data & 0x00FF0000)>>16);
    incr++;
    Buf_Write_USB[4] = (uint8_t)((data & 0x0000FF00)>>8);
    incr++;
    Buf_Write_USB[5] = (uint8_t)(data & 0x000000FF);
    incr++;
    USBHostWrite(USB_INSTANCE,0, &Buf_Write_USB[0],incr);
    incr = 0;
    udelay(300);

}

uint32_t LMS7002M_usb_read(LMS7002M_t *self, const int addr)
{
///0xAD04002F0000
    uint32_t data = 0;
    uint32_t value = 0;
    uint32_t incr = 0;
    data = (uint32_t)((addr & 0x00007FFF)<<16);  // чтение по протоколу в LMS
    Buf_Write_USB[0] = 0xAD;    // шапка сообщения в FTDI для чтения с LMS
    incr++;
    Buf_Write_USB[1] = 0x4;     // длина сообщения c учётом формата SPI для LMS
    incr++;
    Buf_Write_USB[2] = (uint8_t)((data & 0xFF000000)>>24);
    incr++;
    Buf_Write_USB[3] = (uint8_t)((data & 0x00FF0000)>>16);
    incr++;
    Buf_Write_USB[4] = 0x00;
    incr++;
    Buf_Write_USB[5] = 0x00;
    incr++;

    USBHostWrite(USB_INSTANCE, 0, &Buf_Write_USB[0], incr);
    USBHostRead(USB_INSTANCE, 0, &Buf_Read_USB[0],4);
    USBHostRead(USB_INSTANCE, 0, &Buf_Read_USB[0],4);
    USBHostRead(USB_INSTANCE, 0, &Buf_Read_USB[0],4);

   /* while (compCode1 != 1)
    {
         USBHostRead(USB_INSTANCE, 0, &Buf_Read_USB[0],4);

    }*/

   // if ((Buf_Read_USB[2] != 0x41) && (Buf_Read_USB[3] != 0x38))
   // {
   //     return 0;
   // }
    value = (uint32_t)(Buf_Read_USB[3] &0x000000FF)<<8;
    value = value | Buf_Read_USB[2];
    memset(&Buf_Read_USB[0],0x0,sizeof(&Buf_Read_USB[0])*4);
    return value;


}

uint32_t LMS7002M_spi_read(LMS7002M_t *self, const int addr)
{
   // uint32_t data = (((uint32_t)0) << 16) | (((uint32_t)addr) << 16);
    uint32_t returned_data;
            //(((uint32_t)addr) << 16); //if doesn't work return back
    returned_data = LMS7002M_usb_read(self, addr);
    //return self->spi_transact(self->spi_transact_handle, data, /*(uint8_t *)&rxBuf,*/ true/*readback*/) & 0xffff;
    return returned_data;
}

void LMS7002M_regs_spi_write(LMS7002M_t *self, const int addr)
{
    int value = LMS7002M_regs_get(self->regs, addr);

    LMS7002M_usb_write(addr,value);
   // LMS7002M_spi_write(self, addr, value);

    //for CHAB mode: duplicate to the CHB register shadow
    if (self->regs->reg_0x0020_mac == REG_0X0020_MAC_CHAB)
    {
        LMS7002M_regs_set(&self->_regs[1], addr, value);
    }
}

uint32_t LMS7002M_regs_spi_read(LMS7002M_t *self, const int addr)
{
    uint32_t returned_data;
   // LMS7002M_regs_set(self->regs, addr, LMS7002M_spi_read(self, addr));
    returned_data = LMS7002M_usb_read(self, addr);
    LMS7002M_regs_set(self->regs, addr, returned_data);
    return returned_data;
}

LMS7002M_regs_t *LMS7002M_regs(LMS7002M_t *self)
{
    return self->regs;
}

void LMS7002M_regs_to_rfic(LMS7002M_t *self)
{
    LMS7002M_set_mac_ch(self, LMS_CHA);
    for (const int *addrp = LMS7002M_regs_addrs(); *addrp != 0; addrp++)
    {
        LMS7002M_regs_spi_write(self, *addrp);
    }

    LMS7002M_set_mac_ch(self, LMS_CHB);
    for (const int *addrp = LMS7002M_regs_addrs(); *addrp != 0; addrp++)
    {
        //ignore registers that do not depend on MAC
        if (*addrp < 0x0100) continue;

        LMS7002M_regs_spi_write(self, *addrp);
    }

    //ensure that we cache the chips actual version register
    //used in the codebase to handle hardware rev differences
    LMS7002M_regs_spi_read(self, 0x002f);
}

void LMS7002M_rfic_to_regs(LMS7002M_t *self)
{
    LMS7002M_set_mac_ch(self, LMS_CHA);
    for (const int *addrp = LMS7002M_regs_addrs(); *addrp != 0; addrp++)
    {
        LMS7002M_regs_spi_read(self, *addrp);
    }

    LMS7002M_set_mac_ch(self, LMS_CHB);
    for (const int *addrp = LMS7002M_regs_addrs(); *addrp != 0; addrp++)
    {
        LMS7002M_regs_spi_read(self, *addrp);
    }
}

int LMS7002M_dump_ini(LMS7002M_t *self, const char *path)
{
    FILE *p = fopen(path, "w");
    if (p == NULL) return -1;

    fprintf(p, "[FILE INFO]\n");
    fprintf(p, "type=LMS7002 configuration\n");
    fprintf(p, "version=1\n");

    fprintf(p, "[Frequencies]\n");
    fprintf(p, "CGEN frequency MHz=%f\n", self->cgen_freq/1e6);
    fprintf(p, "SXR frequency MHz=%f\n", self->sxr_freq/1e6);
    fprintf(p, "SXT frequency MHz=%f\n", self->sxt_freq/1e6);

    size_t i = 0;
    const int *addrs = LMS7002M_regs_addrs();

    fprintf(p, "[LMS7002 registers ch.A]\n");
    LMS7002M_set_mac_ch(self, LMS_CHA);
    i = 0;
    while (addrs[i] != 0x0000)
    {
        fprintf(p, "0x%04x=0x%04x\n", addrs[i], LMS7002M_spi_read(self, addrs[i]));
        i++;
    }

    fprintf(p, "[LMS7002 registers ch.B]\n");
    LMS7002M_set_mac_ch(self, LMS_CHB);
    i = 0;
    while (addrs[i] != 0x0000)
    {
        fprintf(p, "0x%04x=0x%04x\n", addrs[i], LMS7002M_spi_read(self, addrs[i]));
        i++;
    }

    fprintf(p, "[Reference clocks]\n");
    fprintf(p, "CGEN reference frequency MHz=%f\n", self->cgen_fref/1e6);
    fprintf(p, "SXR reference frequency MHz=%f\n", self->sxr_fref/1e6);
    fprintf(p, "SXT reference frequency MHz=%f\n", self->sxt_fref/1e6);

    return fclose(p);
}

LMS7002mHandle LMS7002M_Open(uint32_t  deviceId, uint32_t  portNum, void *params)
{
        SPI_Params          spiParams;  /* SPI params structure */
        SPI_Handle          spiHandle;  /* SPI handle */
        LMS7002mHandle      LMSHandle = 0;

        /* Init SPI driver */
        SPI_init();

        if (params)
        {
            memcpy(&spiParams, params, sizeof(SPI_Params));
        }
        else
        {
          // Use default SPI config params if no params provided
            SPI_Params_init(&spiParams);
        }

        spiHandle = (SPI_Handle)SPI_open(portNum, &spiParams);

        if (spiHandle)
            {
                    spiInfo.hwHandle = (uint32_t)spiHandle;
                    LMSHandle = (LMS7002mHandle)(&spiInfo);
            }

        return (LMSHandle);

}

void LMS7002M_Close(LMS7002mHandle handle)
{
    LMS_Info    *LMSSpiInfo;
    SPI_Handle   spiHandle;
//    LMS7002m_STATUS lmsSTATUS = 0;
    if (handle)
    {
        LMSSpiInfo = (LMS_Info  *)handle;
        spiHandle = (SPI_Handle)LMSSpiInfo->hwHandle;

        if (spiHandle)
        {
            SPI_close(spiHandle);
        }
    }
}
LMS7002M_STATUS spidev_interface_transact(void *handle, const uint32_t data, /*uint8_t *rxBuf,*/ const bool readback)
{
    LMS_Info            *LMSSpiInfo;
    uint8_t             cmd[4];
    SPI_Handle          spiHandle;
    unsigned char       rxBuf[4];
    memset (rxBuf,0,sizeof(rxBuf));
    if (!handle)
    {
       return NOR_FAIL;
    }

    LMSSpiInfo = (LMS_Info *)handle;

    if (!LMSSpiInfo->hwHandle)
    {
       return NOR_FAIL;
    }

    spiHandle = (SPI_Handle)LMSSpiInfo->hwHandle;


    cmd[1] = (data >> 24);
    cmd[0] = (data >> 16);
    cmd[3] = (data >> 8);
    cmd[2] = (data);

//    mdelay(30);
    if (readback)
    {
        LMS_spiCmdRead(spiHandle, cmd, 1, rxBuf, 1);
    }
    else
    {
        LMS_spiCmdWrite(spiHandle, cmd, 2, rxBuf, 1);
    }

//    LMSHandle = (LMS7002mHandle)(&spiInfo);
//    return (LMSHandle);
    uint8_t val0 = rxBuf[0];
    uint8_t val1 = rxBuf[1];
    uint8_t val2 = rxBuf[2];
    uint8_t val3 = rxBuf[3];

    uint32_t value = val0 | val1 << 8 | val2 << 16 | val3 << 24  ;
    //uint32_t value = val3 | val2 << 8 | val1 << 16 | val0 << 24  ;

    return value ;
}

LMS7002M_STATUS LMS_spiCmdRead(SPI_Handle handle, uint8_t *cmdBuf, uint32_t cmdLen, uint8_t *rxBuf, uint32_t rxLen)
{
    uint32_t        xferEnable;
    uint32_t        terminateXfer = 0;
    bool            retVal;

    /* Enable transfer */
   xferEnable = 1;
 //   SPI_control(handle, SPI_V0_CMD_XFER_ACTIVATE, (void *)&xferEnable);

    /* If no read data, release CS at the end of write */
    if (rxLen == 0)
    {
        terminateXfer = 1;
    }

    /* Write Command */
    transaction.txBuf = cmdBuf;
    transaction.rxBuf = NULL;
    transaction.count = cmdLen;

    terminateXfer = 0;//if doesm't work delete
    transaction.arg = (void *)&terminateXfer;

    retVal = SPI_transfer(handle, &transaction);

    if (retVal == false)
    {
        return LMS7002M_FAIL;
    }


    if (rxLen)
    {
        // Read Device ID
        transaction.txBuf = NULL;
        transaction.rxBuf = rxBuf;
        transaction.count = rxLen;
        terminateXfer = 1;
        transaction.arg = (void *)&terminateXfer;

        retVal = SPI_transfer(handle, &transaction);
    }



    xferEnable = 0;
  //  SPI_control(handle, SPI_V0_CMD_XFER_ACTIVATE, (void *)&xferEnable);

    if (retVal == true)
    {
        return rxBuf[0];
    }
    else
    {
        return LMS7002M_FAIL;
    }

}

LMS7002M_STATUS LMS_spiCmdWrite(SPI_Handle handle, uint8_t *cmd, uint32_t cmdLen, uint8_t *rxBuf, uint32_t rxlen)
{
    uint32_t        xferEnable;
    uint32_t        terminateXfer = 0;
    bool            retVal;

    /* Enable transfer */
    xferEnable = 1;
   // SPI_control(handle, SPI_V0_CMD_XFER_ACTIVATE, (void *)&xferEnable);

    /* If no read data, release CS at the end of write */
    if (rxlen == 0)
    {
        terminateXfer = 1;
    }

    /* Write Command */
    transaction.txBuf = cmd;
    transaction.rxBuf = NULL;
    transaction.count = cmdLen;
    terminateXfer = 1;//if doesm't work delete
    transaction.arg = (void *)&terminateXfer;

    retVal = SPI_transfer(handle, &transaction);

    if (retVal == false)
    {
        return LMS7002M_FAIL;
    }

    /* Write Command */
 /*   transaction.txBuf = NULL;
    transaction.rxBuf = rxBuf;
    transaction.count = rxlen;
    terminateXfer = 1;//if doesm't work delete
    transaction.arg = (void *)&terminateXfer;

    retVal = SPI_transfer(handle, &transaction);

    if (retVal == false)
    {
        return LMS7002M_FAIL;
    }
*/
    /* write data */
/*    if (txlen)
    {
        transaction.txBuf = txBuf;
        transaction.rxBuf = NULL;
        transaction.count = txlen;
        terminateXfer = 1;
        transaction.arg = (void *)&terminateXfer;

        retVal = SPI_transfer(handle, &transaction);
    }
*/
    /* Disable transfer */
    xferEnable = 0;
   // SPI_control(handle, SPI_V0_CMD_XFER_ACTIVATE, (void *)&xferEnable);
    if (retVal == true)
    {
        return LMS7002M_PASS;
    }
    else
    {
        return LMS7002M_FAIL;
    }
}

