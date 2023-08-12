/*
 *  ======= ConfigureGFIR ========
 *
 *  Created on: 1 дек. 2021 г.
 *  Author:     OlegO
 */
#include <xdc/runtime/System.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <USB_N/LMS7002M/LMS7002M_logger.h>
#include <USB_N/LMS7002M/LMS7002M.h>
#include "delay/delay.h"

extern void Modify_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit, unsigned short new_bits_data);
extern unsigned short Get_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit);
extern float GetReferenceClk_TSP(LMS7002M_t *self, bool tx);
extern void GenerateFilter(int n, double w1, double w2, double a1, double a2, double *coefs);
extern int SetGFIRCoefficients(LMS7002M_t *self, bool tx, uint8_t GFIR_index, const int16_t *coef, uint8_t coefCount);
/*
 * ======== main ========
 */
void ConfigureGFIR(LMS7002M_t *self)
{
    double coef[120];
    double coef2[40];
    short gfir1[120];
    short gfir2[40];
    double w, w2;
    int L;
    int div = 1;
    double bandwidth ;
    double interface_MHz;
    int ratio;
    bool enabled = 1;

    bandwidth = 0.15625e6; //0.15625
    bandwidth /= 1e6;

    ratio = Get_SPI_Reg_bits(self, 0x0403, 14, 12);
    interface_MHz = GetReferenceClk_TSP(self, 0/*for Rx*/) / 1e6;

    if (ratio != 7)
    {
        div = (2<<(ratio));
    }

    w = (bandwidth/2)/(interface_MHz/div);
    L = div > 8 ? 8 : div;
    div -= 1;

    w2 = w*1.1; //w2 = w*1.1;
    if (w2 > 0.495)
    {
        w2 = w*1.05;//w2 = w*1.05;
        if (w2 > 0.495)
        {
            enabled = false; //Filter disabled
        }
    }
//    Modify_SPI_Reg_bits(self, 0x040C, 3, 3, 0);
//    Modify_SPI_Reg_bits(self, 0x040C, 4, 4, 0);
//    Modify_SPI_Reg_bits(self, 0x040C, 5, 5, 0);
//    bool sisoDDR = Get_SPI_Reg_bits(self, 0x0022, 14, 14);
//    Modify_SPI_Reg_bits(self, 0x00AD, 2, 2, !(enabled|sisoDDR));
//    Modify_SPI_Reg_bits(self, 0x00AE, 1, 0, enabled? 0 : 0); //3:0
//    Modify_SPI_Reg_bits(self, 0x00AD, 15, 14, enabled? 0 : 0);

    GenerateFilter(L*15, w, w2, 1.0, 0, coef);
    GenerateFilter(L*5, w, w2, 1.0, 0, coef2);

    int sample = 0;
    for(int i=0; i<15; i++)
    {
    for(int j=0; j<8; j++)
        {
            if( (j < L) && (sample < L*15) )
            {
                gfir1[i*8+j] = (coef[sample]*32767.0);//32767.0
                sample++;
            }
            else
            {
                gfir1[i*8+j] = 0;
            }
    }
    }

    sample = 0;
    for(int i=0; i<5; i++)
    {
    for(int j=0; j<8; j++)
        {
            if( (j < L) && (sample < L*5) )
            {
                gfir2[i*8+j] = (coef2[sample]*32767.0);
                sample++;
            }
            else
            {
                gfir2[i*8+j] = 0;
            }
    }
    }

    L-=1;

    Modify_SPI_Reg_bits(self, 0x0405, 10, 8, L);
    Modify_SPI_Reg_bits(self, 0x0405, 7, 0, div);
    Modify_SPI_Reg_bits(self, 0x0406, 10, 8, L);
    Modify_SPI_Reg_bits(self, 0x0406, 7, 0, div);
    Modify_SPI_Reg_bits(self, 0x0407, 10, 8, L);
    Modify_SPI_Reg_bits(self, 0x0407, 7, 0, div);

    SetGFIRCoefficients(self, 0/*for Rx*/, 0, gfir2, 40);
    SetGFIRCoefficients(self, 0/*for Rx*/, 1, gfir2, 40);
   SetGFIRCoefficients(self, 0/*for Rx*/, 2, gfir1, 120);
}
