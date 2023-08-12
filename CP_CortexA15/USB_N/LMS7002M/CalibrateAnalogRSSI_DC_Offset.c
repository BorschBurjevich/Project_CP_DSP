/*
 *  ======= CalibrateAnalogRSSI_DC_Offset ========
 *
 *  Created on: 17 дек. 2021 г.
 *  Author:     OlegO
 */
#include <xdc/runtime/System.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>


#include <USB_N/LMS7002M/LMS7002M_logger.h>
#include <USB_N/LMS7002M/LMS7002M.h>
#include "delay/delay.h"

extern void Modify_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit, unsigned short new_bits_data);
extern unsigned short Get_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit);
extern int CalibrateInternalADC(LMS7002M_t *self, int clkDiv);
extern double  *vector();
/*double  *vector(nl,nh)
int    nl,nh;
{
    double *v;

    v = (double *) malloc((unsigned) (nh-nl+1)*sizeof(double));
    if (!v) printf("Allocation failure in vector().\n");
    return (v-nl);
}*/
/*
 * ======== main ========
 */
int CalibrateAnalogRSSI_DC_Offset(LMS7002M_t *self)
{
    Modify_SPI_Reg_bits(self, 0x010D, 1, 1, 1);
    CalibrateInternalADC(self, 32);
    Modify_SPI_Reg_bits(self, 0x010C, 2, 2, 0);
    Modify_SPI_Reg_bits(self, 0x010C, 1, 1, 0);

    /*Modify_SPI_Reg_bits(LMS7param(RSSIDC_RSEL), 22);
    Modify_SPI_Reg_bits(LMS7param(RSSIDC_HYSCMP), 0);
    Modify_SPI_Reg_bits(LMS7param(RSSIDC_PD), 0);*/
    LMS7002M_spi_write(self, 0x0640, 22 << 4);

    Modify_SPI_Reg_bits(self, 0x0641, 13, 7, 0);
//
//    int value = -63;
//    uint8_t wrValue = abs(value);
//    if(value < 0)
//        wrValue |= 0x40;
//    Modify_SPI_Reg_bits(self, 0x0641, 6, 0, wrValue);
//    uint8_t cmp = Get_SPI_Reg_bits(self, 0x0640, 15, 15);
//    uint8_t cmpPrev = cmp;
//    //vector <int8_t> edges;
//    double *v;
//    int row1 = 0, row2 = 0;
//    v = vector(row1, row2);
//    for(value = -63; value < 64; ++value)
//    {
//        wrValue = abs(value);
//        if(value < 0)
//            wrValue |= 0x40;
//        Modify_SPI_Reg_bits(self, 0x0641, 6, 0, wrValue);
//
//        mdelay(5);
//
//        cmp = Get_SPI_Reg_bits(self, 0x0640, 15, 15);
//        if(cmp != cmpPrev)
//        {
////            edges.push_back(value);
//            v[value] = (double)value;
//            cmpPrev = cmp;
//        }
//        if(value > 1)
//            break;
//    }
//    if(value != 2)
//    {
//        printf("Not found\n");
//        return -1;
//    }
//    int8_t found = (v[0]+v[1])/2;
//    wrValue = abs(found);
//    if(found < 0)
//        wrValue |= 0x40;
//    Modify_SPI_Reg_bits(self, 0x0641, 6, 0, wrValue);
//    printf("Found %i\n", found);
//    Modify_SPI_Reg_bits(self, 0x010D, 1, 1, 0);
    return 0;
}
