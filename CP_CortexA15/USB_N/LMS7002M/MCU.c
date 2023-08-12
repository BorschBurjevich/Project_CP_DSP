/*
 *  ======= MCU ========
 *
 *  Created on: 19 џэт. 2021 у.
 *  Author:     OLEG
 */
#include <xdc/runtime/System.h>
#include "mcu_dc_iq_calibration.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "USB_N/LMS7002M/LMS7002M_impl.h"
#include "USB_N/LMS7002M/LMS7002M_logger.h"
#include <ti/board/src/flash/nor/spi/nor_spi.h>
#include "USB_N/LMS7002M/LMS7002M.h"
#include "delay/delay.h"
/*
 * ======== main ========
 */
#define PROGRAM_BUFF_MASK 1<<0
#define PROGRAM_COMP_MASK 1<<6
#define SPI_CONTROL_MCU 1<<0

void MCU(LMS7002M_t *self)
{
    LMS7002M_spi_write(self, 0x0002, 0x0000);//reset the MCU

    udelay(300);

    LMS7002M_spi_write(self, 0x0002, 0x0001);//select EEPROM/SRAM Mode

    int value=LMS7002M_spi_read(self, 0x0003);//read programming buffer register

    uint8_t *fp=(uint8_t *)mcu_program_lms7_dc_iq_calibration_bin;

    //TODO:ADD timeout here
    int tmr = value & PROGRAM_BUFF_MASK;

    while( (value & PROGRAM_BUFF_MASK)==  0 )

    {//loop if program buffer is not empty
        value=LMS7002M_spi_read(self, 0x0003);
        printf("Program buffer not empty\n");
    }
    //printf("Out of while loop");
    printf("================Ready to flash the image======================\n");

    int size=16384/16;
    //write the binary into the MCU
    //NOTE:The size of binary is 16384,so total writes in steps of 32 are 16384/32=4096
    //uint16_t MSBr;
    //uint16_t LSBr;
    uint16_t data = 0;
/*
    for(int i=0;i<size;i++)
    {
        MSBr = (fp[i]&0xFF00)>>8;
        LSBr = (fp[i]&0x00FF)<<8;
        data = MSBr|LSBr;
        LMS7002M_spi_write(self, 0x0004, data);

        value=LMS7002M_spi_read(self, 0x0003);
         //TODO:ADD timeout here
        while( (value & PROGRAM_BUFF_MASK)==  1 )

        {//loop if program buffer is not empty
            value=LMS7002M_spi_read(self, 0x0003);//read programming buffer register
            printf("Waiting for the Program buffer\n");//PROGRAM STUCK HERE
        }
    }

    value=LMS7002M_spi_read(self, 0x0003);
    //TODO:Add timeout here
    while( (value & PROGRAM_COMP_MASK)== 0 )
    {
        value=LMS7002M_spi_read(self, 0x0003);
        printf("Programming not complete\n");
    }

    //spidev_interface_close(handle);//close the handle
*/
    int IMAGE_SIZE = 16384;
    for (int i = 0; i < IMAGE_SIZE; i += 4)
    {
            data = (0x00 << 8) | fp[i];
            //printf("%x\t",data);
            LMS7002M_spi_write(self, 0x0004, data);
            udelay(300);

            data = (0x00 << 8) | fp[i + 1];
            //printf("%x\t",data);
            LMS7002M_spi_write(self, 0x0004, data);
            udelay(300);

            data = (0x00 << 8) | fp[i + 2];
            //printf("%x\t",data);
            LMS7002M_spi_write(self, 0x0004, data);
            udelay(300);

            data = (0x00 << 8) | fp[i + 3];
            //printf("%x\t",data);
            LMS7002M_spi_write(self, 0x0004, data);
            value = LMS7002M_spi_read(self, 0x0003);
            udelay(300);
            //TODO:ADD timeout here
            //printf("%d",i);
            while ( (value & PROGRAM_BUFF_MASK) ==  1 )
            { //loop if program buffer is not empty
                value = LMS7002M_spi_read(self, 0x0003); //read programming buffer register
                //printf("Waiting for the Program buffer\n");
            }
        }
        value = LMS7002M_spi_read(self, 0x0003);
        udelay(300);
        //TODO:Add timeout here
        while ( (value & PROGRAM_COMP_MASK) == 0 )
        {
            value = LMS7002M_spi_read(self, 0x0003);
            printf("Programming not complete\n");
            udelay(300);
        }


}

void run_calib_procedure(LMS7002M_t *self, int calib)
{
    int value = LMS7002M_spi_read(self, 0x0006);
    value |= SPI_CONTROL_MCU;
    udelay(300);
    LMS7002M_spi_write(self, 0x0006, value); //switch SPI control to MCU

    udelay(300);
    LMS7002M_spi_write(self, 0x0000, 0x00); //run TX chain calibration

    udelay(300);
    value = LMS7002M_spi_read(self, 0x0002);
    value &= ~(1 << 3);

    udelay(300);
    LMS7002M_spi_write(self, 0x0002, value); //reset the 3rd bit of 0x0002
    value |= (1 << 3);

    udelay(300);
    LMS7002M_spi_write(self, 0x0002, value); //set the 3rd bit of 0x0002
    value &= ~(1 << 3);

    udelay(300);
    LMS7002M_spi_write(self, 0x0002, value); //reset the 3rd bit of 0x0002

    udelay(300);
    uint8_t status = LMS7002M_spi_read(self, 0x0001) & 0xff;
    udelay(300);//TODO:Add timeout here
    while (status == 0xff)
    {
        status = LMS7002M_spi_read(self, 0x0001) & 0xff;
    };//wait till calibration is in progress
    if (status == 0x00)
        printf("Calibration complete successful\n");
    else
        printf("Calibration failed.\n");
    value = LMS7002M_spi_read(self, 0x0006);
    value &= ~SPI_CONTROL_MCU;

    LMS7002M_spi_write(self, 0x0006, value); //give back control to the BB processor
}

void update_BW_or_clock(LMS7002M_t *self, uint8_t kHz_LSB, uint8_t kHz_MSB, uint8_t MHz)
{
    LMS7002M_spi_write(self, 0x0000, kHz_LSB);
    int value = LMS7002M_spi_read(self, 0x0002);
    value |= (1 << 2); //set the second bit

    LMS7002M_spi_write(self, 0x0002, value);
    value = LMS7002M_spi_read(self, 0x0002);
    value &= ~(1 << 2);

    LMS7002M_spi_write(self, 0x0002, value); //reset the second bit
    LMS7002M_spi_write(self, 0x0000, kHz_MSB);

    value = LMS7002M_spi_read(self, 0x0002);
    value |= (1 << 2); //set the second bit

    LMS7002M_spi_write(self, 0x0002, value);
    value = LMS7002M_spi_read(self, 0x0002);
    value &= ~(1 << 2);

    LMS7002M_spi_write(self, 0x0002, value); //reset the second bit
    LMS7002M_spi_write(self, 0x0000, MHz);

    value = LMS7002M_spi_read(self, 0x0002);
    value |= (1 << 2); //set the second bit

    LMS7002M_spi_write(self, 0x0002, value);
    value = LMS7002M_spi_read(self, 0x0002);
    value &= ~(1 << 2);

    LMS7002M_spi_write(self, 0x0002, value); //reset the second bit
}
