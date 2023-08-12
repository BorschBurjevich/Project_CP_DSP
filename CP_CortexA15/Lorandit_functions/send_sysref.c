#include "SPI/SPI_header.h"
extern bool spi_LMK_write(SPI_Handle spiii, uint16_t addr, uint8_t data);

void send_sysref(SPI_Handle h)
{
    bool ff;

   // uint8_t data[4] = {0x01,0x3e,0x00};
   // uint8_t data[4] = {0x01,0x3e,0x03};

   ff =  spi_LMK_write(h,0x013e,0x03);   // было 0x00

}
