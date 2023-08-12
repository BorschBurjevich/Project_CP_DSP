/*
 *  ======= LMS7002M_GetFrequencySX ========
 *
 *  Created on: 17 но€б. 2021 г.
 *  Author:     OlegO
 */
#include <xdc/runtime/System.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <USB_N/LMS7002M/LMS7002M_logger.h>
#include <USB_N/LMS7002M/LMS7002M.h>
#include "delay/delay.h"
extern unsigned short Get_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit);
/*
 * ======== main ========
 */
float GetFrequencySX(LMS7002M_t *self, bool tx)
{
    int ch = Get_SPI_Reg_bits(self, 0x0020, 1, 0); //remember previously used channel
    float dMul;

    uint16_t gINT = Get_SPI_Reg_bits(self, 0x011E, 13, 0);    // read whole register to reduce SPI transfers
    uint32_t gFRAC = ((gINT&0xF) * 65536) | Get_SPI_Reg_bits(self, 0x011D, 15, 0);

    const float refClk_Hz = 50.00e6;    // 20.00e6
    dMul = (float)refClk_Hz / (1 << (Get_SPI_Reg_bits(self, 0x011F, 8, 6) + 1));
    //Calculate real frequency according to the calculated parameters
    dMul = dMul * ((gINT >> 4) + 4 + (float)gFRAC / 1048576.0) * (Get_SPI_Reg_bits(self, 0x011C, 10, 10) + 1);
    //this->SetActiveChannel(ch); //restore used channel
    return dMul;
}
