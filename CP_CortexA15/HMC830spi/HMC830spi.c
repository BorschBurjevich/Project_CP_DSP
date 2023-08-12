#include "SPI/SPI_header.h"
void  FUNC_SET_HMC830(void *spi_cs0,void *spi_cs1)
{

    spi3_cs0 = spi3_cs0 & 0xFFFFFFBF; // SS в активной единице
    spi3_cs1 = spi3_cs1 & 0xFFFFFFBF; // SS в активной единице

    /////// Config VCO 450MHz //////

    spi3_write(spi_cs0,0x02000004); // Enables Register
    spi3_write(spi_cs0,0x04000005); // REFDIV Register , R = 5
    spi3_write(spi_cs0,0x0A002C50); // Reg 5.5
    spi3_write(spi_cs0,0x0A00C140); // Reg 5.4
    spi3_write(spi_cs0,0x0A000310); // Reg 5.0
    spi3_write(spi_cs0,0x0A000000); // Reg 5.0
    spi3_write(spi_cs0,0x0A000000); // Reg 5.0
    spi3_write(spi_cs0,0x0C061E94); // Reg 6
    spi3_write(spi_cs0,0x0E00029A); // Reg 7
    spi3_write(spi_cs0,0x11837DFE); // Reg 8
    spi3_write(spi_cs0,0x12A8FFFE); // Reg 9
    spi3_write(spi_cs0,0x1400408C); // Reg A
    spi3_write(spi_cs0,0x160F8042); // Reg B
    spi3_write(spi_cs0,0x18000000); // Reg C
    spi3_write(spi_cs0,0x1A000000); // Reg D
    spi3_write(spi_cs0,0x1C000000); // Reg E
    spi3_write(spi_cs0,0x1E000002); // Reg F
    spi3_write(spi_cs0,0x0A005530); // Reg 5.0
    spi3_write(spi_cs0,0x00000000); // Reg 4

    ///////Генерация частоты //////////
    /////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
    spi3_write(spi_cs0,0x0A000310);    // Reg 5.0
    spi3_write(spi_cs0,0x06000048);    // Reg 3 , установка Nint = 36d
    spi3_write(spi_cs0,0x1400408C);    // Reg A
    spi3_write(spi_cs0,0x0A01C420);    // Reg 5.2 установка делителя K = 4d микросхемы
    spi3_write(spi_cs0,0x0A004600);    // Reg 5.0
    spi3_write(spi_cs0,0x18000000);    // Reg C
    spi3_write(spi_cs0,0x00000000);    // Reg 4   установка Nfrac
    spi3_write(spi_cs0,0x0A000000);    // Reg 5.0
    // ждать 100мкс //
    udelay(100);
    spi3_write(spi_cs0,0x0A005130);


    /////////////////////////////////////////////////////////////

    /////// Config VCO 800MHz //////

    spi3_write(spi_cs0,0x02000004); // Enables Register
      spi3_write(spi_cs0,0x04000005); // REFDIV Register , R = 5
      spi3_write(spi_cs0,0x0A002C50); // Reg 5.5
      spi3_write(spi_cs0,0x0A00C140); // Reg 5.4
      spi3_write(spi_cs0,0x0A000310); // Reg 5.0
      spi3_write(spi_cs0,0x0A000000); // Reg 5.0
      spi3_write(spi_cs0,0x0A000000); // Reg 5.0
      spi3_write(spi_cs0,0x0C061E94); // Reg 6
      spi3_write(spi_cs0,0x0E00029A); // Reg 7
      spi3_write(spi_cs0,0x11837DFE); // Reg 8
      spi3_write(spi_cs0,0x12A8FFFE); // Reg 9
      spi3_write(spi_cs0,0x1400408C); // Reg A
      spi3_write(spi_cs0,0x160F8042); // Reg B
      spi3_write(spi_cs0,0x18000000); // Reg C
      spi3_write(spi_cs0,0x1A000000); // Reg D
      spi3_write(spi_cs0,0x1C000000); // Reg E
      spi3_write(spi_cs0,0x1E000002); // Reg F
      spi3_write(spi_cs0,0x0A005530); // Reg 5.0
      spi3_write(spi_cs0,0x00000000); // Reg 4



    ///////Генерация частоты //////////
       /////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//

    spi3_write(spi_cs1,0x0A000310);    // Reg 5.0
    spi3_write(spi_cs1,0x06000040);    // Reg 3 , uстановка Nint(32d)
    spi3_write(spi_cs1,0x1400408C);    // Reg A
    spi3_write(spi_cs1,0x0A01C220);    // Reg 5.2 установка делителя K(2d)  микросхемы
    spi3_write(spi_cs1,0x0A004600);    // Reg 5.0
    spi3_write(spi_cs1,0x18000000);    // Reg C
    spi3_write(spi_cs1,0x00000000);    // Reg 4   установка Nfrac
    spi3_write(spi_cs1,0x0A000000);    // Reg 5.0
    // ждать 100мкс //
    udelay(100); // udelay - микросекунды
    spi3_write(spi_cs0,0x0A005130);

}
