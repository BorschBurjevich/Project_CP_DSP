#include "GPS_Kompas.h"
#include <math.h>
#include "delay/delay.h"
#include "Lorandit_functions/Cod_com.h"
int Kompas_BMR_3000(UART_Handle uartHnd)
{

    uint8_t      return_data = 0;

    memset(&rx_Kompas,0,sizeof(rx_Kompas)*14);
    memset(&tx_Kompas,0,sizeof(tx_Kompas)*6);

    pitch  = 0.0; roll = 0;
    heading = 0.0;

    return_data = read_the_Magnetic(uartHnd);

    /////////////////////////////
    return_data = get_Pitch_Angle(uartHnd);
    return_data = get_Roll_Angle(uartHnd);
    return_data = get_Heading_Angle(uartHnd);
    return_data = get_All_Angles(uartHnd);
    return_data = set_the_Magnetic(uartHnd,-20.5);
    return_data = read_the_Magnetic(uartHnd);
    return_data = set_baud_rate_BMR(uartHnd,RATE_115200_);
    return_data = set_address_BMR(uartHnd,0x01);
    return_data = set_Query_address(uartHnd);
    return_data = set_output_mode(uartHnd,MODE_100Hz_);
    return_data = save_settings(uartHnd);
    return_data = switch_calib_output(uartHnd);

    return return_data;
}
// 1 - данные не пришли, 2 - не сошлась контрольная сумма
uint8_t get_Pitch_Angle(UART_Handle Kompas_Hnd)      // тангаж
{
    uint32_t fractional_part;
    uint8_t  mask;
    uint64_t cmd;
    uint32_t data;
    uint8_t  word_read_bcd[6];
    uint32_t ck_sum = 0;
    uint16_t counter = 0;

    mask = 0x80;
    //data:77 04 00 01 05//Read PITCH angle
    cmd = 0x0501000477;
    memcpy(&tx_Kompas[0],&cmd,sizeof(tx_Kompas)*5);
    UART_write(Kompas_Hnd, (void *)&tx_Kompas[0], 5);
    UART_read (Kompas_Hnd, (void *)&rx_Kompas[0], 8);

    if (rx_Kompas[0] == 0)
    {
        printf("\n Nothing to get from BMR!\n");
        return 1; // данные не пришли
    }
    for (counter = 1; counter < 8; counter ++)
    {
        ck_sum += rx_Kompas[counter];
    }
    ck_sum = ck_sum & 0x000000FF;
    if (ck_sum != rx_Kompas[7])
    {
        printf("\n Error in getting checksum!\n");
        return 2; // не сошлась контрольная сумма
    }
  /*  ////////////example_data////////

    rx_Kompas[4] = 0x83;
    rx_Kompas[5] = 0x13;
    rx_Kompas[6] = 0x71;
    ///////////////////////////////*/
    // read_half_bite//
    word_read_bcd[0]   = (rx_Kompas[4] & 0xF0)>>4;
    word_read_bcd[1]   =  rx_Kompas[4] & 0x0F;
    word_read_bcd[2]   = (rx_Kompas[5] & 0xF0)>>4;
    word_read_bcd[3]   =  rx_Kompas[5] & 0x0F;
    word_read_bcd[4]   = (rx_Kompas[6] & 0xF0)>>4;
    word_read_bcd[5]   =  rx_Kompas[6] & 0x0F;
    ///////////////////

    data               = (word_read_bcd[1]*100) + (word_read_bcd[2]*10) + word_read_bcd[3];
    fractional_part    = word_read_bcd[4]*10 + word_read_bcd[5];
    pitch              = (float)(fractional_part * 0.01);
    pitch              = (float)(data + pitch);

    mask               = (mask & rx_Kompas[4])>>7;

    if (mask == 1)
    {
        pitch = pitch * -1.0;
    }
    ck_sum = 0;
    return 0;
}
// 1 - данные не пришли, 2 - не сошлась контрольная сумма
uint8_t get_Roll_Angle(UART_Handle Kompas_Hnd)  // крен
{
    uint32_t fractional_part;
    uint8_t  mask;
    uint64_t cmd;
    uint32_t data;
    uint8_t  word_read_bcd[6];
    uint32_t ck_sum = 0;
    uint16_t counter = 0;

    mask = 0x80;
    //data:77 04 00 02 06//Read PITCH angle
    cmd = 0x0602000477;
    memcpy(&tx_Kompas[0],&cmd,sizeof(tx_Kompas)*5);

    UART_write(Kompas_Hnd, (void *)&tx_Kompas[0], 5);
    UART_read (Kompas_Hnd, (void *)&rx_Kompas[0], 8);
    if (rx_Kompas[0] == 0)
    {
        printf("\n Nothing to get from BMR!\n");
        return 1; // данные не пришли
    }

    for (counter = 1; counter < 8; counter ++)
    {
        ck_sum += rx_Kompas[counter];
    }
    ck_sum = ck_sum & 0x000000FF;
    if (ck_sum != rx_Kompas[7])
    {
       printf("\n Error in getting checksum!\n");
       return 2; // не сошлась контрольная сумма
    }

    // read_half_bite//
    word_read_bcd[0]   = (rx_Kompas[4] & 0xF0)>>4;
    word_read_bcd[1]   =  rx_Kompas[4] & 0x0F;
    word_read_bcd[2]   = (rx_Kompas[5] & 0xF0)>>4;
    word_read_bcd[3]   =  rx_Kompas[5] & 0x0F;
    word_read_bcd[4]   = (rx_Kompas[6] & 0xF0)>>4;
    word_read_bcd[5]   =  rx_Kompas[6] & 0x0F;
    ///////////////////

    data               = (word_read_bcd[1]*100) + (word_read_bcd[2]*10) + word_read_bcd[3];
    fractional_part    = word_read_bcd[4]*10 + word_read_bcd[5];
    roll              = (float)(fractional_part * 0.01);
    roll              = (float)(data + roll);

    mask               = (mask & rx_Kompas[4])>>7;

    if (mask == 1)
    {
        roll = roll * -1.0;
    }
    ck_sum = 0;
    return 0;
}
// 1 - данные не пришли, 2 - не сошлась контрольная сумма
uint8_t get_Heading_Angle(UART_Handle Kompas_Hnd)  // курс
{
    uint32_t fractional_part;
    uint8_t  mask;
    uint64_t cmd;
    uint32_t data;
    uint8_t  word_read_bcd[6];
    uint32_t ck_sum = 0;
    uint16_t counter = 0;

    mask = 0x80;
    //data:77 04 00 03 07 // Read Heading azimuth angle

    cmd = 0x0703000477;
    memcpy(&tx_Kompas[0],&cmd,sizeof(tx_Kompas)*5);

    UART_write(Kompas_Hnd, (void *)&tx_Kompas[0], 5);
    UART_read (Kompas_Hnd, (void *)&rx_Kompas[0], 8);

    if (rx_Kompas[0] == 0)
    {
        printf("\n Nothing to get from BMR!\n");
        return 1; // данные не пришли
    }
    for (counter = 1; counter < 8; counter ++)
    {
       ck_sum += rx_Kompas[counter];

    }
    ck_sum = ck_sum & 0x000000FF;
    if (ck_sum != rx_Kompas[7])
    {

       printf("\n Error in getting checksum!\n");
       return 2; // не сошлась контрольная сумма
    }
    // read_half_bite//
    word_read_bcd[0]   = (rx_Kompas[4] & 0xF0)>>4;
    word_read_bcd[1]   =  rx_Kompas[4] & 0x0F;
    word_read_bcd[2]   = (rx_Kompas[5] & 0xF0)>>4;
    word_read_bcd[3]   =  rx_Kompas[5] & 0x0F;
    word_read_bcd[4]   = (rx_Kompas[6] & 0xF0)>>4;
    word_read_bcd[5]   =  rx_Kompas[6] & 0x0F;
    ///////////////////

    data               = (word_read_bcd[1]*100) + (word_read_bcd[2]*10) + word_read_bcd[3];
    fractional_part    = word_read_bcd[4]*10 + word_read_bcd[5];
    heading            = (float)(fractional_part * 0.01);
    heading            = (float)(data + heading);

    mask               = (mask & rx_Kompas[4])>>7;

    if (mask == 1)
    {
        heading = heading * -1.0;
    }
    ck_sum = 0;
    return 0;
}
// 1 - данные не пришли, 2 - не сошлась контрольная сумма
uint8_t get_All_Angles(UART_Handle Kompas_Hnd)
{
    uint32_t fractional_part;
    uint8_t  mask[3];
    uint64_t cmd;
    uint32_t data;
    uint8_t  word_read_bcd[6];
    uint32_t ck_sum = 0;
    uint16_t counter = 0;
    mask[0] = 0x80;
    mask[1] = 0x80;
    mask[2] = 0x80;

    //data 77 04 00 04 08
    cmd     = 0x0804000477;
    memcpy(&tx_Kompas[0],&cmd,sizeof(tx_Kompas)*5);

    UART_write(Kompas_Hnd, (void *)&tx_Kompas[0], 5);
    UART_read (Kompas_Hnd, (void *)&rx_Kompas[0], 14);
    if (rx_Kompas[0] == 0)
    {
       printf("\n Nothing to get from BMR!\n");
       return 1; // данные не пришли
    }
    for (counter = 1; counter < 13; counter ++)
    {
       ck_sum += rx_Kompas[counter];
    }
    ck_sum = ck_sum & 0x000000FF;
    if (ck_sum != rx_Kompas[12])
    {
       printf("\n Error in getting checksum!\n");
       return 2; // не сошлась контрольная сумма
    }

    // read_half_bite//
    word_read_bcd[0]   = (rx_Kompas[4] & 0xF0)>>4;
    word_read_bcd[1]   =  rx_Kompas[4] & 0x0F;
    word_read_bcd[2]   = (rx_Kompas[5] & 0xF0)>>4;
    word_read_bcd[3]   =  rx_Kompas[5] & 0x0F;
    word_read_bcd[4]   = (rx_Kompas[6] & 0xF0)>>4;
    word_read_bcd[5]   =  rx_Kompas[6] & 0x0F;
    ///////////////////
    data               = (word_read_bcd[1]*100) + (word_read_bcd[2]*10) + word_read_bcd[3];
    fractional_part    = word_read_bcd[4]*10 + word_read_bcd[5];
    pitch              = (float)(fractional_part * 0.01);
    pitch              = (float)(data + pitch);               // Pitch angle

    mask[0]               = (mask[0] & rx_Kompas[4])>>7;

    if (mask[0] == 1)
    {
        pitch = pitch * -1.0;
    }

    // read_half_bite//
    word_read_bcd[0]   = (rx_Kompas[7] & 0xF0)>>4;
    word_read_bcd[1]   =  rx_Kompas[7] & 0x0F;
    word_read_bcd[2]   = (rx_Kompas[8] & 0xF0)>>4;
    word_read_bcd[3]   =  rx_Kompas[8] & 0x0F;
    word_read_bcd[4]   = (rx_Kompas[9] & 0xF0)>>4;
    word_read_bcd[5]   =  rx_Kompas[9] & 0x0F;
    ///////////////////
    data               = (word_read_bcd[1]*100) + (word_read_bcd[2]*10) + word_read_bcd[3];
    fractional_part    = word_read_bcd[4]*10 + word_read_bcd[5];
    roll               = (float)(fractional_part * 0.01);
    roll               = (float)(data + roll);               // Roll angle

    mask[1]            = (mask[1] & rx_Kompas[7])>>7;

    if (mask[1] == 1)
    {
        roll = roll * -1.0;
    }

    // read_half_bite//
    word_read_bcd[0]   = (rx_Kompas[10] & 0xF0)>>4;
    word_read_bcd[1]   =  rx_Kompas[10] & 0x0F;
    word_read_bcd[2]   = (rx_Kompas[11] & 0xF0)>>4;
    word_read_bcd[3]   =  rx_Kompas[11] & 0x0F;
    word_read_bcd[4]   = (rx_Kompas[12] & 0xF0)>>4;
    word_read_bcd[5]   =  rx_Kompas[12] & 0x0F;
    ///////////////////
    data               = (word_read_bcd[1]*100) + (word_read_bcd[2]*10) + word_read_bcd[3];
    fractional_part    = word_read_bcd[4]*10 + word_read_bcd[5];
    heading            = (float)(fractional_part * 0.01);
    heading            = (float)(data + heading);                  // Heading angle

    mask[2]            = (mask[2] & rx_Kompas[10])>>7;

    if (mask[2] == 1)
    {
        heading = heading * -1.0;
    }

    ck_sum = 0;
    return 0;
}
// 1 - данные не пришли, 2 - не сошлась контрольная сумма, 3 - status failure
uint8_t set_the_Magnetic(UART_Handle Kompas_Hnd, float Magnetic)
{
    uint32_t ck_sum = 0;
    uint16_t counter = 0;
    unsigned int tmpi = 0; // градусы
    unsigned int tmpf = 0.0; // минуты(дробная часть)

    //data:77 06 00 06 02 08 16 // Set the magnetic declination
    tx_Kompas[0] = 0x77;
    tx_Kompas[1] = 0x06;
    tx_Kompas[2] = 0x00;
    tx_Kompas[3] = 0x06;
    tx_Kompas[4] = 0x00;
    if (Magnetic < 0)
    {
        tx_Kompas[4] = tx_Kompas[4] | 0x80;
        Magnetic = Magnetic * (-1.0);
    }
    tmpi = (int)Magnetic;
    tmpf = (float)(Magnetic - tmpi) * 10.0;
    ///перекладка целой части склонения
    tx_Kompas[4] |= (uint8_t)(tmpi / 10.0);
    tx_Kompas[5] = ((uint8_t)(tmpi % 10))<<4;
    tx_Kompas[5] |= ((uint8_t)tmpf) & 0x0000000F;

    for (counter = 1; counter < 6; counter++)
    {
        ck_sum += tx_Kompas[counter];
    }

    tx_Kompas[6] = ck_sum & 0x000000FF;

    UART_write(Kompas_Hnd, (void *)&tx_Kompas[0], 7);
    UART_read (Kompas_Hnd, (void *)&rx_Kompas[0], 6);

    if (rx_Kompas[0] == 0)
    {
       printf("\n Nothing to get from BMR!\n");
       return 1; // данные не пришли
    }
    for (counter = 1; counter < 7; counter ++)
    {
        ck_sum += rx_Kompas[counter];
    }
    ck_sum = ck_sum & 0x000000FF;
    if (ck_sum != rx_Kompas[7])
    {
        printf("\n Error in getting checksum!\n");
        return 2; // не сошлась контрольная сумма
    }

    if (rx_Kompas[4] == 0xFF)
    {

        return 3;
    }
    ck_sum = 0;
    return 0;
}
// чтение магнитного склонения; 1 - данные не пришли, 2 - не сошлась контрольная сумма
uint8_t read_the_Magnetic(UART_Handle Kompas_Hnd)
{

    uint32_t fractional_part;
    uint8_t  mask;
    uint32_t data;
    uint8_t  word_read_bcd[6] = {0};
    uint32_t ck_sum;
    uint16_t counter = 0;

    mask = 0x80;
    //data:77 04 00 07 00 0B // Read magnetic declination

    tx_Kompas[0] = 0x77;
    tx_Kompas[1] = 0x04;
    tx_Kompas[2] = 0x00;
    tx_Kompas[3] = 0x07;
    tx_Kompas[4] = 0x00;
    tx_Kompas[5] = 0x0B;


    UART_write(Kompas_Hnd, (void *)&tx_Kompas[0], 6);
    UART_read (Kompas_Hnd, (void *)&rx_Kompas[0], 7);

    if (rx_Kompas[0] == 0)
    {
       printf("\n Nothing to get from BMR!\n");
       return 1; // данные не пришли
    }

    for (counter = 1; counter < 8; counter ++)
    {
        ck_sum += rx_Kompas[counter];
    }
    ck_sum = ck_sum & 0x000000FF;

    if (ck_sum != rx_Kompas[6])
    {
       printf("\n Error in getting checksum!\n");
       return 2;
    }
    ///Test Data//
  /*////////////example_data////////
    rx_Kompas[0] = 0x77;
    rx_Kompas[1] = 0x05;
    rx_Kompas[2] = 0x00;
    rx_Kompas[3] = 0x87;
    rx_Kompas[4] = 0x82;
    rx_Kompas[5] = 0x15;
    rx_Kompas[6] = 0x23;
    ///////////////////////////////*/

    // read_half_bite//
    word_read_bcd[0]   = (rx_Kompas[4] & 0xF0)>>4;
    word_read_bcd[1]   =  rx_Kompas[4] & 0x0F;
    word_read_bcd[2]   = (rx_Kompas[5] & 0xF0)>>4;
    word_read_bcd[3]   =  rx_Kompas[5] & 0x0F;
    ///////////////////

    data               = (word_read_bcd[1]*10) + word_read_bcd[2];
    fractional_part    = word_read_bcd[3];
    magnetic_declination              = (float)(fractional_part * 0.1);
    magnetic_declination              = (float)(data + magnetic_declination);

    mask               = (mask & rx_Kompas[4])>>7;

    if (mask == 1)
    {
        magnetic_declination = magnetic_declination * -1.0;
    }
    ck_sum = 0;
    return 0;
}
/*
 *  выставить частоту в бодах
 *  RATE_2400_      (0x00U)
 *  RATE_4800_      (0x01U)
 *  RATE_9600_      (0x02U)
 *  RATE_19200_     (0x03U)
 *  RATE_115200_    (0x04U)
 *  RATE_38400_     (0x05U)
 *  RATE_57600_     (0x06U)*/
// 1 - данные не пришли, 2 - не сошлась контрольная сумма, 3 - status failure
uint8_t  set_baud_rate_BMR(UART_Handle Kompas_Hnd, uint8_t rate)
{
    uint32_t ck_sum = 0;
    uint16_t counter = 0;

    tx_Kompas[0] = 0x77;
    tx_Kompas[1] = 0x05;
    tx_Kompas[2] = 0x00;
    tx_Kompas[3] = 0x0B;
    tx_Kompas[4] = rate;    //baud rate
    tx_Kompas[5] = tx_Kompas[1] + tx_Kompas[3] + tx_Kompas[4];


    UART_write(Kompas_Hnd, (void *)&tx_Kompas[0], 6);
    UART_read (Kompas_Hnd, (void *)&rx_Kompas[0], 6);

    if (rx_Kompas[0] == 0)
    {
       printf("\n Nothing to get from BMR!\n");
       return 1; // данные не пришли
    }

    for (counter = 1; counter < 5; counter++)
    {
       ck_sum += rx_Kompas[counter];
    }

    ck_sum = ck_sum & 0x000000FF;

    if (ck_sum != rx_Kompas[5])
    {
       printf("\n Error in getting checksum!\n");
       return 2;
    }

    if (rx_Kompas[4] == 0xFF)
    {
        return 3;
    }

    ck_sum = 0;
    return 0;
}
// 1 - данные не пришли, 2 - не сошлась контрольная сумма, 3 - status failure
uint8_t set_address_BMR(UART_Handle Kompas_Hnd, uint8_t addr)
{
    uint32_t ck_sum = 0;
    uint16_t counter = 0;

    // data : 77 05 00 0F 01 15 // Set address command

    tx_Kompas[0] = 0x77;
    tx_Kompas[1] = 0x05;
    tx_Kompas[2] = 0x00;
    tx_Kompas[3] = 0x0F;
    tx_Kompas[4] = addr;
    tx_Kompas[5] = tx_Kompas[1] + tx_Kompas[3] + tx_Kompas[4]; // crc

    UART_write(Kompas_Hnd, (void *)&tx_Kompas[0], 6);
    UART_read (Kompas_Hnd, (void *)&rx_Kompas[0], 6);

    if (rx_Kompas[0] == 0)
    {
       printf("\n Nothing to get from BMR!\n");
       return 1; // данные не пришли
    }

    for (counter = 1; counter < 5; counter++)
    {
        ck_sum += rx_Kompas[counter];
    }
    ck_sum = ck_sum & 0x000000FF;
    if (ck_sum != rx_Kompas[5])
    {
        printf("\n Error in getting checksum!\n");
        return 2;
    }

    if (rx_Kompas[4] == 0xFF)
    {
        return 3;
    }
    ck_sum = 0;
    return 0;

}
// 1 - данные не пришли, 2 - не сошлась контрольная сумма, 3 - status failure
uint8_t set_Query_address(UART_Handle Kompas_Hnd)
{
    uint32_t ck_sum = 0;
    uint16_t counter = 0;

    // data : 77 04 00 1F 00 23 // Query address command
    // the return data is the device address

    tx_Kompas[0] = 0x77;
    tx_Kompas[1] = 0x04;
    tx_Kompas[2] = 0x00;
    tx_Kompas[3] = 0x1F;
    tx_Kompas[4] = 0x00;
    tx_Kompas[5] = tx_Kompas[1] + tx_Kompas[3] + tx_Kompas[4];


    UART_write(Kompas_Hnd, (void *)&tx_Kompas[0], 6);
    UART_read (Kompas_Hnd, (void *)&rx_Kompas[0], 6);
    if (rx_Kompas[0] == 0)
    {
       printf("\n Nothing to get from BMR!\n");
       return 1; // данные не пришли
    }

    for (counter = 1; counter < 5; counter++)
    {
       ck_sum += rx_Kompas[counter];
    }
    ck_sum = ck_sum & 0x000000FF;
    if (ck_sum != rx_Kompas[5])
    {
        printf("\n Error in getting checksum!\n");
        return 2;
    }

    if (rx_Kompas[4] == 0xFF)
    {
        return 3;
    }
    ck_sum = 0;
    return 0;
}
/*Выставить частоту для ответа
 * MODE_Default_    (0x00U) режим запроса/ответа
 * MODE_5Hz_        (0x01U)
 * MODE_10Hz_       (0x02U)
 * MODE_20Hz_       (0x03U)
 * MODE_25Hz_       (0x04U)
 * MODE_50Hz_       (0x05U)
 * MODE_100Hz_      (0x06U) // baud rate must be set to 115200
 * */
// 1 - данные не пришли, 2 - не сошлась контрольная сумма, 3 - status failure
uint8_t set_output_mode(UART_Handle Kompas_Hnd, uint8_t cmd)
{
    uint32_t ck_sum = 0;
    uint16_t counter = 0;

    // data : 77 05 00 0C 00 11 //Set output mode command


    tx_Kompas[0] = 0x77;
    tx_Kompas[1] = 0x05;
    tx_Kompas[2] = 0x00;
    tx_Kompas[3] = 0x0C;
    tx_Kompas[4] = cmd;
    tx_Kompas[5] = 0x11;

    UART_write(Kompas_Hnd, (void *)&tx_Kompas[0], 6);
    UART_read (Kompas_Hnd, (void *)&rx_Kompas[0], 6);

    if (rx_Kompas[0] == 0)
    {
       printf("\n Nothing to get from BMR!\n");
       return 1; // данные не пришли
    }
    for (counter = 1; counter < 5; counter++)
    {
        ck_sum += rx_Kompas[counter];
    }
    ck_sum = ck_sum & 0x000000FF;
    if (ck_sum != rx_Kompas[5])
    {
        printf("\n Error in getting checksum!\n");
        return 2;
    }

    else if (rx_Kompas[4] == 0xFF)
    {
       return 3;
    }

    ck_sum = 0;
    return 0;

}
// 1 - данные не пришли, 2 - не сошлась контрольная сумма, 3 - status failure
uint8_t save_settings(UART_Handle Kompas_Hnd)
{
    //note нужно выплнить сохранение настроек, иначе по выключению питания не сохранится конфигурация
    uint8_t status;
    uint32_t ck_sum = 0;
    uint16_t counter = 0;

    // data : 77 04 00 0A 0E// Save setting command

    tx_Kompas[0] = 0x77;
    tx_Kompas[1] = 0x04;
    tx_Kompas[2] = 0x00;
    tx_Kompas[3] = 0x0A;
    tx_Kompas[4] = 0x00;
    tx_Kompas[5] = 0x0E;


    UART_write(Kompas_Hnd, (void *)&tx_Kompas[0], 5);
    UART_read (Kompas_Hnd, (void *)&rx_Kompas[0], 6);

    if (rx_Kompas[0] == 0)
    {
       printf("\n Nothing to get from BMR!\n");
       return 1; // данные не пришли
    }
    for (counter = 1; counter < 5; counter++)
    {
        ck_sum += rx_Kompas[counter];
    }
    ck_sum = ck_sum & 0x000000FF;
    if (ck_sum != rx_Kompas[5])
    {
       printf("\n Error in getting checksum!\n");
    }

    if (rx_Kompas[4] == 0xFF)
    {
       return 3;
    }
    ck_sum = 0;
    return 0;

}
// 1 - данные не пришли, 2 - ошибка в контрольной сумме, 3 - калибровочный выход закрыт, 4 - калибровочный выход закрыт
uint8_t switch_calib_output(UART_Handle Kompas_Hnd)
{
    uint32_t ck_sum = 0;
    uint16_t counter = 0;

   // data : 77 04 00 A3 A7// Switch calibration output

   tx_Kompas[0] = 0x77;
   tx_Kompas[1] = 0x04;
   tx_Kompas[2] = 0x00;
   tx_Kompas[3] = 0xA3;
   tx_Kompas[4] = 0x00;
   tx_Kompas[5] = 0xA7;


   UART_write(Kompas_Hnd, (void *)&tx_Kompas[0], 6);
   UART_read (Kompas_Hnd, (void *)&rx_Kompas[0], 6);

    if (rx_Kompas[0] == 0)
    {
       printf("\n Nothing to get from BMR!\n");
       return 1; // данные не пришли
    }

    for (counter = 1; counter < 5; counter++)
    {
       ck_sum += rx_Kompas[counter];
    }

    ck_sum = ck_sum & 0x000000FF;

    if (ck_sum != rx_Kompas[5])
    {
       printf("\n Error in getting checksum!\n");
       return 2;
    }

    if (rx_Kompas[4] == 0x01)
    {
       return  4;   // open
       printf("\n Calibration output is opened!\n");
    }

    else if (rx_Kompas[4] == 0x00)
    {
       return 3;   // close
       printf("\n Calibration output is closed!\n");
    }

   return 0;


}
