
/*
 * receiver.c
 *
 *  Created on: 3 июн. 2020 г.
 *
 */

#include "SPI/SPI_header.h"
#include <Lorandit_functions/reciever/reciever.h>
#include "Lorandit_functions/Structs.h"
#include "Lorandit_functions/Cod_com.h"
uint16_t Write_Cmd;
//расчет контрольной суммы
uint8_t crc8(uint8_t *pData, uint8_t nbytes)
{
    uint8_t crc = 0xff;
    uint8_t i = 0;

    while(nbytes--)
    {
        crc ^= *pData++;
        for(i = 0; i < 8; i++)
        {
            if(crc & 0x80)
            {
                crc = (crc << 1) ^ 0x31;
            }
            else
            {
                crc = crc << 1;
            }
        }
    }
    return(crc);
}

/*
 * num_ch  - выбор приемного канала
 * freq_rage - используемый частотный диапазон
 * att - ослабление аттенюатора,
 * lock - блокировка входа: 1-вкл, 0-выкл
 */

// status_receiver == -1 - ошибка в отправке кодограммы
// status_receiver == 1  - правильна€ запись
// status_receiver == 2  - при переполнении нужен макс атт 31 дЅ
// status_receiver == 3  - нужно включить доп ослабление в 20 дЅ
// status_receiver == 4  - включено доп ослабление в 20 дЅ
int8_t set_rec_mode(uint8_t set_cmd,MCSPI_Handle mcspi2_cs3,  uint8_t num_ch, uint8_t freq_rage, uint8_t att, uint8_t lock, uint8_t flag20db_on)
{
    // set_cmd - номер команды, дефайны задаютс€ в receiver.h
    // lock нужно учесть
    // нужно учитыввать режим при работе с PRSU - L, включение нужныx €чеек
    // ќтправка осуществл€етс€ по одному фрейму //


    uint8_t n20db =0;
    ////////////////////////////////////////////////////////////////


    uint8_t  txdata[4] = {0};
    bool     attr;

    if (set_cmd == set_cmd_1_PRSUL)
    {
        num_ch = 0;
    }

    if (num_ch == 0x8 )         // спектр со всех каналов
    {
        num_ch = 0x3F;
    }

    if (num_ch == 0x5 )         // пеленг
    {
        num_ch = 0x1F;
    }


    if (flag20db_on == 1)
    {
        n20db = 1;
    }

    Write_Cmd     = 0;
    txdata[0] = (set_cmd << 3)|(num_ch >> 5);
    txdata[1] = (num_ch << 5)|(freq_rage);
    txdata[2] = (att << 3) |(n20db << 2) | (lock << 1) ;
    txdata[3] = crc8(&txdata[0],3);

    /////////////////////////////////////////
    Write_Cmd = (uint16_t) txdata[0];
    Write_Cmd = Write_Cmd << 8;
    Write_Cmd = Write_Cmd | ((uint16_t)txdata[1]);
    spi2_LVDS_write(mcspi2_cs3,Write_Cmd, 1);
    Write_Cmd = (uint16_t) txdata[2];
    Write_Cmd = Write_Cmd << 8;
    Write_Cmd = Write_Cmd | ((uint16_t)txdata[3]);
    spi2_LVDS_write(mcspi2_cs3,Write_Cmd, 1);

    /////////////////////////////////////////////
    //отладить чтение, запись работает правильно//
    attr = spi2_LVDS_read(mcspi2_cs3,2);
    if (attr == 0) printf("Error spi2_LVDS_read!\n");
    //проверка правильности квитанции
    if((( rxSPI_Buf[0] >> 5) != set_cmd) && ( (( rxSPI_Buf[0] >> 3) & 0x01) != 0x01) )
    {
      status_receiver = -1;
      goto error;
    }

    else
    {
      status_receiver = 1;

      if (att == 30)
      {
          status_receiver = 2;
          return (status_receiver);
      }

      if (att == 31)
      {
          status_receiver = 3;
          return (status_receiver);
      }
      if (n20db == 1)
      {
          status_receiver = 4;
          return (status_receiver);
      }

    }

    error:
    return(status_receiver);
    /////////////////////////////////////////

    /*
        Write_Cmd     = 0;
        txdata[0] = (set_cmd << 3)|(num_ch >> 5);
        txdata[1] = (num_ch << 5)| (freq_rage);
        txdata[2] = (att << 3) | (lock << 1);
        txdata[3] = crc8(&txdata[0],3);
        Write_Cmd = (uint16_t) txdata[0];
        Write_Cmd = Write_Cmd << 8;
        Write_Cmd = Write_Cmd | ((uint16_t)txdata[1]);
        spi2_LVDS_write(mcspi2_cs3,Write_Cmd, 1);
        Write_Cmd = (uint16_t) txdata[2];
        Write_Cmd = Write_Cmd << 8;
        Write_Cmd = Write_Cmd | ((uint16_t)txdata[3]);
        spi2_LVDS_write(mcspi2_cs3,Write_Cmd, 1);
        /////////////////////////////////////////////
        //отладить чтение, запись работает правильно//
        attr = spi2_LVDS_read(mcspi2_cs3,2);

        //проверка правильности квитанции
        if((( rxSPI_Buf[0] >> 5) != set_cmd) && ( (( rxSPI_Buf[0] >> 3) & 0x01) != 0x01) )
        {
           status = -1;
           goto error;
        }

        else
        {
           status = 1;
        }

        error:
        return(status);
        */

}

/*
 * result_flag - флаг необходимости приема результатов диагностики
 */
int8_t rec_diagnostic(uint8_t set_cmd,uint8_t result_flag,MCSPI_Handle mcspi2_cs3)
{
    //set_cmd - номер команды
    uint8_t  status;
    uint8_t  txdata[4];
    Write_Cmd     = 0;

    //если рузультаты не нужны, просто принимаем квитанцию
    if(result_flag == 0)
    {

        txdata[0] = (set_cmd << 5);
        txdata[1] = crc8(&txdata[0],1);
        Write_Cmd = (uint16_t) txdata[0];
        Write_Cmd = Write_Cmd << 8;
        Write_Cmd = Write_Cmd | ((uint16_t)txdata[1]);

        spi2_LVDS_write(mcspi2_cs3,Write_Cmd,1);
        spi2_LVDS_read(mcspi2_cs3,2);

        //проверка правильности квитанции
        if(((rxSPI_Buf[0] >> 5) != set_cmd) && ( ((rxSPI_Buf[0] >> 3) & 0x01) != 0x01) )
        {
            status = -1;
            goto error;
        }
        else
        {
            status = 1;
        }
    }

    //если надо, то работаем по протоколу
    if(result_flag == 1)
    {
        set_cmd   = 0x0;
        txdata[0] = (set_cmd << 5);
        txdata[1] = crc8(&txdata[0],2);
        Write_Cmd = (uint16_t) txdata[0];
        Write_Cmd = Write_Cmd << 8;
        Write_Cmd = Write_Cmd | ((uint16_t)txdata[1]);

        spi2_LVDS_write(mcspi2_cs3,Write_Cmd,1);
        spi2_LVDS_read(mcspi2_cs3,2);

        //проверка правильности ответа
        if(((rxSPI_Buf[0] >> 5) != set_cmd) && ( ((rxSPI_Buf[0] >> 3) & 0x01) != 0x01) )
        {
            status = -1;
            goto error;
        }

        else
        {
            status = 1;
        }
    }

error:
    return(status);
}


/*
 * ¬ключение/выключение питани€ каналов
 * num_ch  - выбор приемного канала          0b00000000 - ни одного канала
 *                                           0b00000001 - 1 канал
 *                                           0b00000011 - 1 и 2 канал
 */
int8_t on_off_rec_ch(uint8_t set_cmd,uint8_t num_ch,MCSPI_Handle mcspi2_cs3)
{
    //0x04 - номер команды

    bool     status;
    bool     attrs;
    uint8_t  txdata[4] = {0};


    Write_Cmd = 0;
    txdata[0] = (set_cmd << 3)|(num_ch >> 5);
    txdata[1] = (num_ch << 5)| 0x0;
    txdata[2] = 0;
    txdata[3] = crc8(&txdata[0],3);
    Write_Cmd = (uint16_t) txdata[0];
    Write_Cmd = Write_Cmd << 8;
    Write_Cmd = Write_Cmd | ((uint16_t)txdata[1]);
    spi2_LVDS_write(mcspi2_cs3,Write_Cmd,1);           // Frame 0
    Write_Cmd = (uint16_t) txdata[2];
    Write_Cmd = Write_Cmd << 8;
    Write_Cmd = Write_Cmd | ((uint16_t)txdata[3]);
    attrs     = spi2_LVDS_write(mcspi2_cs3,Write_Cmd,1);           // Frame 1

    if (attrs != 1) printf("Invalid receiver transact!\n");



   /*проверить чтение при стыковке*/
    spi2_LVDS_read(mcspi2_cs3,4);


    //проверка правильности ответа
    if(((rxSPI_Buf[0] >> 5) != set_cmd) && ( ((rxSPI_Buf[0] >> 3) & 0x01) != 0x01) )
    {
        status = -1;
        goto error;
    }

   else
    {
        status = 1;
    }

error:
    return(status);
}


int8_t pres_freq_range(uint32_t freq)
{
    int8_t freq_rage; // диапазон частот приЄмника

         if((PRES_FREQ_BORDER_HZ_1  < freq) && (freq <= PRES_FREQ_BORDER_HZ_2))  { freq_rage = 0;}
    else if((PRES_FREQ_BORDER_HZ_2  < freq) && (freq <= PRES_FREQ_BORDER_HZ_3))  { freq_rage = 1;}
    else if((PRES_FREQ_BORDER_HZ_3  < freq) && (freq <= PRES_FREQ_BORDER_HZ_4))  { freq_rage = 2;}
    else if((PRES_FREQ_BORDER_HZ_4  < freq) && (freq <= PRES_FREQ_BORDER_HZ_5))  { freq_rage = 3;}
    else if((PRES_FREQ_BORDER_HZ_5  < freq) && (freq <= PRES_FREQ_BORDER_HZ_6))  { freq_rage = 4;}
    else if((PRES_FREQ_BORDER_HZ_6  < freq) && (freq <= PRES_FREQ_BORDER_HZ_7))  { freq_rage = 5;}
    else if((PRES_FREQ_BORDER_HZ_7  < freq) && (freq <= PRES_FREQ_BORDER_HZ_8))  { freq_rage = 6;}
    else if((PRES_FREQ_BORDER_HZ_8  < freq) && (freq <= PRES_FREQ_BORDER_HZ_9))  { freq_rage = 7;}
    else if((PRES_FREQ_BORDER_HZ_9  < freq) && (freq <= PRES_FREQ_BORDER_HZ_10)) { freq_rage = 8;}
    else if((PRES_FREQ_BORDER_HZ_10 < freq) && (freq <= PRES_FREQ_BORDER_HZ_11)) { freq_rage = 9;}
    else if((PRES_FREQ_BORDER_HZ_11 < freq) && (freq <= PRES_FREQ_BORDER_HZ_12)) { freq_rage = 10;}
    else if((PRES_FREQ_BORDER_HZ_12 < freq) && (freq <= PRES_FREQ_BORDER_HZ_13)) { freq_rage = 11;}  // приЄмник сносит на частоту 1000-1200 (freq_rage = 9)
    else if((PRES_FREQ_BORDER_HZ_13 < freq) && (freq <= PRES_FREQ_BORDER_HZ_14)) { freq_rage = 12;}
    else if((PRES_FREQ_BORDER_HZ_14 < freq) && (freq <= PRES_FREQ_BORDER_HZ_15)) { freq_rage = 13;}
    else if((PRES_FREQ_BORDER_HZ_15 < freq) && (freq <= PRES_FREQ_BORDER_HZ_16)) { freq_rage = 14;}
    else if((PRES_FREQ_BORDER_HZ_16 < freq) && (freq <= PRES_FREQ_BORDER_HZ_17)) { freq_rage = 15;}  // сносит на частоту 2000-2200 (freq_rage = 13)
    else                                                                         { freq_rage = -1;}//ошибка

    return(freq_rage);
}

int8_t pres_freq_range_PRSUL(uint32_t freq)
{
    int8_t freq_rage; // диапазон частот приЄмника

         if((PRSUL_FREQ_BORDER_HZ_1  < freq) && (freq <= PRSUL_FREQ_BORDER_HZ_2))  { freq_rage = 0;}
    else if((PRSUL_FREQ_BORDER_HZ_2  < freq) && (freq <= PRSUL_FREQ_BORDER_HZ_3))  { freq_rage = 1;}
    else if((PRSUL_FREQ_BORDER_HZ_3  < freq) && (freq <= PRSUL_FREQ_BORDER_HZ_4))  { freq_rage = 2;}
    else if((PRSUL_FREQ_BORDER_HZ_4  < freq) && (freq <= PRSUL_FREQ_BORDER_HZ_5))  { freq_rage = 3;}
    else if((PRSUL_FREQ_BORDER_HZ_5  < freq) && (freq <= PRSUL_FREQ_BORDER_HZ_6))  { freq_rage = 4;}
    else if((PRSUL_FREQ_BORDER_HZ_6  < freq) && (freq <= PRSUL_FREQ_BORDER_HZ_7))  { freq_rage = 5;}
    else if((PRSUL_FREQ_BORDER_HZ_7  < freq) && (freq <= PRSUL_FREQ_BORDER_HZ_8))  { freq_rage = 6;}
    else if((PRSUL_FREQ_BORDER_HZ_8  < freq) && (freq <= PRSUL_FREQ_BORDER_HZ_9))  { freq_rage = 7;}

    else                                                                         { freq_rage = -1;}//ошибка

    return(freq_rage);
}
