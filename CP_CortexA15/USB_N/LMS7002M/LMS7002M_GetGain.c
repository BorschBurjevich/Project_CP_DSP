/*
 *  ======= LMS7002M_GetGain ========
 *
 *  Created on: 10 но€б. 2021 г.
 *  Author:     OlegO
 */
#include <xdc/runtime/System.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <USB_N/LMS7002M/LMS7002M_logger.h>
#include <USB_N/LMS7002M/LMS7002M.h>
extern unsigned short Get_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit);
extern void Modify_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit, unsigned short new_bits_data);
/*
 * ======== main ========
 */

float GetRFELNA_dB(LMS7002M_t *self)
{
    const double gmax = 30;
    short g_lna_rfe = Get_SPI_Reg_bits(self, 0x0113, 9, 6);
    switch (g_lna_rfe)
    {
    case 15: return gmax-0;
    case 14: return gmax-1;
    case 13: return gmax-2;
    case 12: return gmax-3;
    case 11: return gmax-4;
    case 10: return gmax-5;
    case 9: return gmax-6;
    case 8: return gmax-9;
    case 7: return gmax-12;
    case 6: return gmax-15;
    case 5: return gmax-18;
    case 4: return gmax-21;
    case 3: return gmax-24;
    case 2: return gmax-27;
    case 1: return gmax-30;
    }
    return 0.0;
}

float GetRFELoopbackLNA_dB(LMS7002M_t *self)
{
    const double gmax = 40;
    short g_rxloopb_rfe = Get_SPI_Reg_bits(self, 0x0113, 5, 2);
    switch (g_rxloopb_rfe)
    {
    case 15: return gmax-0;
    case 14: return gmax-0.5;
    case 13: return gmax-1;
    case 12: return gmax-1.6;
    case 11: return gmax-2.4;
    case 10: return gmax-3;
    case 9: return gmax-4;
    case 8: return gmax-5;
    case 7: return gmax-6.2;
    case 6: return gmax-7.5;
    case 5: return gmax-9;
    case 4: return gmax-11;
    case 3: return gmax-14;
    case 2: return gmax-17;
    case 1: return gmax-24;
    }
    return 0.0;
}

float GetRFETIA_dB(LMS7002M_t *self)
{
    const double gmax = 12;
    short g_tia_rfe = Get_SPI_Reg_bits(self, 0x0113, 1, 0);
    switch (g_tia_rfe)
    {
    case 3: return gmax-0;
    case 2: return gmax-3;
    case 1: return gmax-12;
    }
    return 0.0;
}

float GetRBBPGA_dB(LMS7002M_t *self)
{
    short g_pga_rbb = Get_SPI_Reg_bits(self, 0x0119, 4, 0);
    return g_pga_rbb - 12;
}

float GetTRFPAD_dB(LMS7002M_t *self)
{
    const double pmax = 52;
    short loss_int = Get_SPI_Reg_bits(self, 0x0101, 10, 6);
    if (loss_int > 10) return pmax-10-2*(loss_int-10);
    return pmax-loss_int;
}
