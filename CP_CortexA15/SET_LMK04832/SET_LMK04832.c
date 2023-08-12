/*
 * SET_LMK04832.c
 *
 *  Created on: 10 дек. 2020 г.
 *      Author: NK
 */

#include "delay/delay.h"
#include "SET_LMK04832/SET_LMK04832.h"


//static SPI_Transaction transaction;     /* SPI transaction structure */
extern void Set_LMK04832(SPI_Handle spi4lmk)
{
    bool answer;
    int count;
    answer = spi_LMK_write(spi4lmk,LMK_REG_TABLE[0]);  // init LMK04x
    if (answer == 0) printf("Error spi write to lmk!\n");
    udelay(50);


       //// Настройка конфигурации регистров для LMK 04x ///

    for (count = 1; count < 126; count ++)
    {
        spi_LMK_write(spi4lmk,LMK_REG_TABLE[count]);
        udelay(50);
        //spi_LMK_read(spi4lmk,Mas_LMK_Addr[count]);
        //udelay(50);
    }


}

bool spi_LMK_write(SPI_Handle spiii, uint32_t data)
{
    SPI_Transaction transaction;

    bool            retVal;
    uint8_t         txBuf[3] = {0};
    // Initiate transfer //

        txBuf[2] = (uint8_t)((data & 0x00ff0000) >> 16);
        txBuf[1] = (uint8_t)((data & 0x0000ff00)>>8);
        txBuf[0] = (uint8_t)(data  & 0x000000ff);

    transaction.count = 1;
    transaction.txBuf = &txBuf[0];
    transaction.rxBuf = NULL;

    retVal = SPI_transfer(spiii, &transaction);
    return (retVal);
}

uint8_t spi_LMK_read(void  *spiii, uint16_t addr)
{
    static SPI_Transaction transaction;

    uint8_t         txBuf[3] = {0};
    uint8_t         rxBuf[3] = {0};
    // Initiate transfer //
    addr = 0x8000 | addr;

    txBuf[2] = (uint8_t)((addr & 0xff00) >> 8);
    txBuf[1] = (uint8_t)(addr & 0x00ff);
    txBuf[0] = 0x00;


    transaction.count = 1;
    transaction.txBuf = &txBuf[0];
    transaction.rxBuf = &rxBuf[0];

    SPI_transfer((SPI_Handle)spiii, &transaction);
    return (rxBuf[0]);
}


