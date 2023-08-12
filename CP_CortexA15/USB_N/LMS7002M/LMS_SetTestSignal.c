/*
 *  ======= LMS_SetTestSignal ========
 *
 *  Created on: 15 дек. 2021 г.
 *  Author:     OlegO
 */
#include <xdc/runtime/System.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "LMS7002M_impl.h"
#include <USB_N/LMS7002M/LMS7002M_logger.h>
#include <USB_N/LMS7002M/LMS7002M.h>
#include "delay/delay.h"
extern int Modify_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit, unsigned short new_bits_data);

/*
 * ======== main ========
 */

int LoadDC_REG_IQ(LMS7002M_t *self, bool tx, int16_t I, int16_t Q)
{
    if(tx)
    {
        Modify_SPI_Reg_bits(self, 0x020C, 15, 0, I);
        Modify_SPI_Reg_bits(self, 0x0200, 5, 5, 0);
        Modify_SPI_Reg_bits(self, 0x0200, 5, 5, 1);
        Modify_SPI_Reg_bits(self, 0x0200, 5, 5, 0);
        Modify_SPI_Reg_bits(self, 0x020C, 15, 0, Q);
        Modify_SPI_Reg_bits(self, 0x0200, 6, 6, 0);
        Modify_SPI_Reg_bits(self, 0x0200, 6, 6, 1);
        Modify_SPI_Reg_bits(self, 0x0200, 6, 6, 0);
    }
    else
    {
        Modify_SPI_Reg_bits(self, 0x040B, 15, 0, I);
        Modify_SPI_Reg_bits(self, 0x0400, 5, 5, 0);
        Modify_SPI_Reg_bits(self, 0x0400, 5, 5, 1);
        Modify_SPI_Reg_bits(self, 0x0400, 5, 5, 0);
        Modify_SPI_Reg_bits(self, 0x040B, 15, 0, Q);
        Modify_SPI_Reg_bits(self, 0x0400, 6, 6, 0);
        Modify_SPI_Reg_bits(self, 0x0400, 6, 6, 1);
        Modify_SPI_Reg_bits(self, 0x0400, 6, 6, 0);
    }
    return 0;
}
int SetTestSignal(LMS7002M_t *self, bool dir_tx, unsigned chan, lms_testsig_t sig, int16_t dc_i, int16_t dc_q)
{

    if (dir_tx == false)
    {
        if (Modify_SPI_Reg_bits(self, 0x0400, 2, 2, sig != LMS_TESTSIG_NONE) != 0)
            return -1;

        if (sig == LMS_TESTSIG_NCODIV8 || sig == LMS_TESTSIG_NCODIV8F)
            Modify_SPI_Reg_bits(self, 0x0400, 8, 7, 1);
        else if (sig == LMS_TESTSIG_NCODIV4 || sig == LMS_TESTSIG_NCODIV4F)
            Modify_SPI_Reg_bits(self, 0x0400, 8, 7, 2);

        if (sig == LMS_TESTSIG_NCODIV8 || sig == LMS_TESTSIG_NCODIV4)
            Modify_SPI_Reg_bits(self, 0x0400, 9, 9, 0);
        else if (sig == LMS_TESTSIG_NCODIV8F || sig == LMS_TESTSIG_NCODIV4F)
            Modify_SPI_Reg_bits(self, 0x0400, 9, 9, 1);

        Modify_SPI_Reg_bits(self, 0x0400, 3, 3, sig == LMS_TESTSIG_DC);
    }
    else
    {
        if (Modify_SPI_Reg_bits(self, 0x0200, 2, 2, sig != LMS_TESTSIG_NONE) != 0)
            return -1;

        if (sig == LMS_TESTSIG_NCODIV8 || sig == LMS_TESTSIG_NCODIV8F)
            Modify_SPI_Reg_bits(self, 0x0200, 8, 7, 1);
        else if (sig == LMS_TESTSIG_NCODIV4 || sig == LMS_TESTSIG_NCODIV4F)
            Modify_SPI_Reg_bits(self, 0x0200, 8, 7, 2);

        if (sig == LMS_TESTSIG_NCODIV8 || sig == LMS_TESTSIG_NCODIV4)
            Modify_SPI_Reg_bits(self, 0x0200, 9, 9, 0);
        else if (sig == LMS_TESTSIG_NCODIV8F || sig == LMS_TESTSIG_NCODIV4F)
            Modify_SPI_Reg_bits(self, 0x0200, 9, 9, 1);

        Modify_SPI_Reg_bits(self, 0x0200, 3, 3, sig == LMS_TESTSIG_DC);

    }

    if (sig == LMS_TESTSIG_DC)
        return LoadDC_REG_IQ(self, dir_tx, dc_i, dc_q);

    return 0;
}
