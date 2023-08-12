/*
 *  ======= LMS7002M_get_rate ========
 *
 *  Created on: 9 но€б. 2021 г.
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
extern float GetReferenceClk_TSP(LMS7002M_t *self, bool tx);
double GetRate (LMS7002M_t *self, bool tx, double rf_rate_Hz);
/*
 * ======== main ========
 */
double GetRate (LMS7002M_t *self, bool tx, double rf_rate_Hz)
{
    double interface_Hz;
    int ratio;

    if (tx)
    {
        ratio = Get_SPI_Reg_bits(self, 0x0203, 14, 12);
        interface_Hz = GetReferenceClk_TSP(self, tx);
    }
    else
    {
        ratio = Get_SPI_Reg_bits(self, 0x0403, 14, 12);
        interface_Hz = GetReferenceClk_TSP(self, tx);
    }

    if (rf_rate_Hz)
        rf_rate_Hz = interface_Hz;

    if (ratio != 7)
        interface_Hz /= 2*pow(2.0, ratio);

    return interface_Hz;
}
