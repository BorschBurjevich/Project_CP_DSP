/*
 *  ======= LMS7002M_SetInterfaceFrequency ========
 *
 *  Created on: 19 но€б. 2021 г.
 *  Author:     OlegO
 */
#include <xdc/runtime/System.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <USB_N/LMS7002M/LMS7002M_logger.h>
#include <USB_N/LMS7002M/LMS7002M.h>
#include "delay/delay.h"

extern uint8_t SetFrequencyCGEN(LMS7002M_t *self, float freq);
extern void Modify_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit, unsigned short new_bits_data);
extern unsigned short Get_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit);
/*
 * ======== main ========
 */
int SetInterfaceFrequency(LMS7002M_t *self, float cgen_freq_Hz, const uint8_t interpolation, const uint8_t decimation)
{
    int status = 0;
    Modify_SPI_Reg_bits(self, 0x0403, 14, 12, decimation);

    Modify_SPI_Reg_bits(self, 0x0203, 14, 12, interpolation);

    //clock rate already set because the readback frequency is pretty-close,
    //dont set the cgen frequency again to save time due to VCO selection
    //const auto freqDiff = std::abs(this->GetFrequencyCGEN() - cgen_freq_Hz);
    //if (not this->GetCGENLocked() or freqDiff > 10.0)
    {
        status = SetFrequencyCGEN(self, cgen_freq_Hz);
        if (status != 0) return status;
    }
    int /*auto*/ siso =  Get_SPI_Reg_bits(self, 0x0022, 14, 14);
    int mclk2src = Get_SPI_Reg_bits(self, 0x002B, 5, 4);
    if (decimation == 7 || (decimation == 0 && siso == 0)) //bypass
    {
        Modify_SPI_Reg_bits(self, 0x002C, 7, 0, 0);
        Modify_SPI_Reg_bits(self, 0x002B, 0, 0, false);
        Modify_SPI_Reg_bits(self, 0x002B, 5, 4, (mclk2src & 1) | 0x2);
    }
    else
    {
        uint8_t divider = (uint8_t)pow(2.0, decimation+siso);
        if (divider > 1)
            Modify_SPI_Reg_bits(self, 0x002C, 7, 0, (divider / 2) - 1);
        else
            Modify_SPI_Reg_bits(self, 0x002C, 7, 0, 0);//
        Modify_SPI_Reg_bits(self, 0x002B, 0, 0, true);
        Modify_SPI_Reg_bits(self, 0x002B, 5, 4, mclk2src & 1);
    }

    if (Get_SPI_Reg_bits(self, 0x002A, 11, 10) == 0)
    {
        bool mimoBypass = (decimation == 7) && (siso == 0);
        Modify_SPI_Reg_bits(self, 0x002A, 3, 2, mimoBypass ? 3 : 1);
        Modify_SPI_Reg_bits(self, 0x002A, 1, 0,mimoBypass ? 1 : 2);
    }

    siso =  Get_SPI_Reg_bits(self, 0x0022, 12, 12);
    int mclk1src = Get_SPI_Reg_bits(self, 0x002B, 3, 2);
    if (interpolation == 7 || (interpolation == 0 && siso == 0)) //bypass
    {
        Modify_SPI_Reg_bits(self, 0x002C, 15, 8, 0);
        Modify_SPI_Reg_bits(self, 0x002B, 1, 1, false);
        Modify_SPI_Reg_bits(self, 0x002B, 3, 2, (mclk1src & 1) | 0x2);
    }
    else
    {
        uint8_t divider = (uint8_t)pow(2.0, interpolation+siso);
        if (divider > 1)
            Modify_SPI_Reg_bits(self, 0x002C, 15, 8, (divider / 2) - 1);
        else
            Modify_SPI_Reg_bits(self, 0x002C, 15, 8, 0);
        Modify_SPI_Reg_bits(self, 0x002B, 1, 1, true);
        Modify_SPI_Reg_bits(self, 0x002B, 3, 2, mclk1src & 1);
    }

    if (Get_SPI_Reg_bits(self, 0x002A, 9, 8) == 0)
    {
        bool mimoBypass = (interpolation == 7) && (siso == 0);
        Modify_SPI_Reg_bits(self, 0x002A, 7, 6,mimoBypass ? 0 : 2);
        Modify_SPI_Reg_bits(self, 0x002A, 5, 4, 0);
    }

    return 0;
}
