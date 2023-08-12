#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "LMS7002M_impl.h"
#include <USB_N/LMS7002M/LMS7002M_logger.h>
#include <USB_N/LMS7002M/LMS7002M.h>
#include "delay/delay.h"

extern void Modify_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit, unsigned short new_bits_data);
extern unsigned short Get_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit);

int CalibrateInternalADC(LMS7002M_t *self, int clkDiv)
{

    const uint16_t biasMux = Get_SPI_Reg_bits(self, 0x0084, 12, 11);
    Modify_SPI_Reg_bits(self, 0x0084, 12, 11, 1);

    LMS7002M_spi_write(self, 0x0600, 0x0F01);
    LMS7002M_spi_write(self, 0x0602, 0x2000);
    LMS7002M_spi_write(self, 0x0603, 0x0000);

    Modify_SPI_Reg_bits(self, 0x0600, 0, 0, 0);
    Modify_SPI_Reg_bits(self, 0x0600, 1, 1, 1);
    Modify_SPI_Reg_bits(self, 0x0600, 15, 8, clkDiv);
    Modify_SPI_Reg_bits(self, 0x0602, 13, 9, 8);
    Modify_SPI_Reg_bits(self, 0x0603, 7, 0, 170);

    uint8_t bias = Get_SPI_Reg_bits(self, 0x0602, 13, 9);
    uint16_t regValue = LMS7002M_spi_read(self, 0x0601);
    while( ((regValue >> 5) & 0x1) != 1)
    {
        if(bias > 31)
        {
            printf("Temperature internal ADC calibration failed");
            return -2;
        }
        ++bias;
        Modify_SPI_Reg_bits(self,0x0602, 13, 9, bias);
        regValue = LMS7002M_spi_read(self, 0x0601);
    }
    Modify_SPI_Reg_bits(self, 0x0602, 13, 9, 0);
    Modify_SPI_Reg_bits(self, 0x0084, 12, 11, biasMux);
    Modify_SPI_Reg_bits(self, 0x0600, 1, 1, 0);
    return 0;
}

int CalibrateRP_BIAS(LMS7002M_t *self)
{
    CalibrateInternalADC(self, 32);
    Modify_SPI_Reg_bits(self, 0x0600, 0, 0, 0);
    Modify_SPI_Reg_bits(self, 0x0600, 1, 1, 0);

    const uint16_t biasMux = Get_SPI_Reg_bits(self, 0x0084, 12, 11);
    Modify_SPI_Reg_bits(self, 0x0084, 12, 11, 1);
    udelay(250);
    uint16_t reg606 = LMS7002M_spi_read(self, 0x0606);
    uint16_t Vref = (reg606 >> 8) & 0xFF;
    uint16_t Vptat = reg606 & 0xFF;

    if(Vref > Vptat)
    {
        uint16_t rpCalib = Get_SPI_Reg_bits(self, 0x0084, 10, 6);
        while(Vref > Vptat)
        {
            --rpCalib;
            Modify_SPI_Reg_bits(self, 0x0084, 10, 6, rpCalib);
            reg606 = LMS7002M_spi_read(self, 0x0606);
            Vref = (reg606 >> 8) & 0xFF;
            Vptat = reg606 & 0xFF;
        }
    }
    if(Vref < Vptat)
    {
        uint16_t rpCalib = Get_SPI_Reg_bits(self, 0x0084, 10, 6);
        while(Vref < Vptat)
        {
            ++rpCalib;
            Modify_SPI_Reg_bits(self, 0x0084, 10, 6, rpCalib);
            reg606 = LMS7002M_spi_read(self, 0x0606);
            Vref = (reg606 >> 8) & 0xFF;
            Vptat = reg606 & 0xFF;
        }
    }
    Modify_SPI_Reg_bits(self, 0x0084, 12, 11, biasMux );
    return 0;
}
