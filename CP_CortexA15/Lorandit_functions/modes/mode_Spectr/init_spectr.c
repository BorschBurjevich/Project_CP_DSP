#include "Lorandit_functions/modes/modes.h"
#include "AD9208_spi/AD9208.h"
#include "AD9208_spi/ad9208_reg.h"
#include "Lorandit_functions/modes/mode_Calibration/Calibration.h"
#include "Lorandit_functions/reciever/reciever.h"
#include "Lorandit_functions/Structs.h"


void init_spectr()
{
    uint8_t cnt;
    uint16_t i, j, left_band, right_band, tmp;
    uint32_t k = 0;
    uint32_t left_frequency;

    //////////////// разбор квитанции //////////

    memcpy(&spectrum.ch_num, &pBuf[k], sizeof(spectrum.ch_num));
    k += sizeof(spectrum.ch_num);
    memcpy(&cnt, &pBuf[k], sizeof(cnt));        // колич-во частотных диапазонов
    k += sizeof(cnt);

    spectrum.num_bands = 0x0;

    for(i = 0; i < cnt; i++)
    {
       memcpy(&left_band, &pBuf[k], sizeof(left_band));     // номер левой крайней полосы
       k += sizeof(left_band);
       memcpy(&right_band, &pBuf[k], sizeof(right_band));   // номер правой крайней полосы
       k += sizeof(right_band);

       tmp = right_band - left_band;

       for(j = 0; j <= tmp; j++)
       {
           spectrum.band_idxs[spectrum.num_bands++] = left_band + j;  // заполнение массива с индексами дл€ полос
       }

    }
    ///////////////////////
    for (i = 0; i < spectrum.num_bands; i ++)                          // заполн€ем дл€ каждой полосы требуемое количество DDC
    {
        spectrum.nddc[i] = ((mass_bands[spectrum.band_idxs[i]] >> 16) - (mass_bands[spectrum.band_idxs[i]] & 0x0000ffff))/2;
    }

    left_frequency = mass_bands[spectrum.band_idxs[0]] & 0x0000ffff;
    freq_mhz = (mass_bands[spectrum.band_idxs[0]] >> 16); // считал нулевую полосу

   //// настройка антенны на нулевую полосу, (€чейки коммутации) ////


/*    if  (freq_mhz >= 25 && freq_mhz < 100)      // 1 литера
    {
       Set_Cell_Switchboard(RGM_IN_ANT1,0x0,0x0);  // настройка €чейки коммутатора
       flag_aerial         = 1;
    }

    else if (freq_mhz >=100 && freq_mhz < 800)  // 2 литера
    {
       Set_Cell_Switchboard(RGM_IN_ANT2,0x0,0x0);
       flag_aerial         = 2;
    }

    else if (freq_mhz >= 800)                       // 3 литера
    {
       Set_Cell_Switchboard(RGM_IN_ANT3,0x0,0x0);
       flag_aerial         = 3;
    }*/

    flag_aerial = selection_litera(freq_mhz,flag_aerial,0);


   ///////////////////////////////////////////////
    if (tresholds_item.att_uhf != 0)
    {
        for (i = 0; i < 16; i++)
        {
            spectrum.att_prm[i] = tresholds_item.att_uhf;    // счтывание из порогов знач - ие атт дл€ каждого преселектора
        }
    }


    spectrum.mas_freq_range[0] = pres_freq_range(freq_mhz*1000000);                       // индекс используемого частотного диапазона

    if (spectrum.mas_freq_range[0] == 11)       // приЄмник сносит частоту
    {
        freq_mhz       = 2650 - freq_mhz;
        left_frequency  = freq_mhz - (spectrum.nddc[0] << 1);
    }
    else if (spectrum.mas_freq_range[0] == 15)
    {
        freq_mhz       = 5000  - freq_mhz;
        left_frequency  = freq_mhz - (spectrum.nddc[0] << 1);
    }
    set_rec_mode(set_cmd_1,MCSPI2_LVDS_CS3,spectrum.ch_num,spectrum.mas_freq_range[0],spectrum.att_prm[0],0,0); // настройка приЄмника

    ////////////// настройка ѕЋ»— на приЄм нулевой полосы
    config_ADC(spectrum.nddc[0],left_frequency,useful_struct.fft_points,spectrum.ch_num);

    if  (spectrum.ch_num != 8)   // условие на спектр с одного канала
    {
        (spectrum.ch_num & 0x01) ? (tmp = 1) : (tmp = 0);  // определ€ем четность канала

        switch(tmp)
        {
           case 0 :
           {
               if (spectrum.ch_num < 2)
               {
                   AD9208_adc_set_channel_select(AD9208_MCSPI1_CS0,AD9208_ADC_CH_A);  // AD1 ,channel A
                   break;
               }
               else if (spectrum.ch_num == 2)
               {
                   AD9208_adc_set_channel_select(AD9208_MCSPI1_CS1,AD9208_ADC_CH_A);  // AD2, channel A
                   break;
               }
               else if (spectrum.ch_num > 2)
               {
                   AD9208_adc_set_channel_select(AD9208_MCSPI1_CS2,AD9208_ADC_CH_A);  // AD3, channel A
                   break;
               }

               break;
           }
           case 1 :
           {

               if (spectrum.ch_num == 1)
               {
                   AD9208_adc_set_channel_select(AD9208_MCSPI1_CS0,AD9208_ADC_CH_B); // AD1 ,channel B
                   break;
               }
               else
               {
                   AD9208_adc_set_channel_select(AD9208_MCSPI1_CS1,AD9208_ADC_CH_B); // AD2 ,channel B
                   break;
               }

               break;
           }
        }
    }
    else                            // со всех каналов
    {
        AD9208_adc_set_channel_select(AD9208_MCSPI1_CS0,AD9208_ADC_CH_ALL); // AD1
        AD9208_adc_set_channel_select(AD9208_MCSPI1_CS1,AD9208_ADC_CH_ALL); // AD2
        AD9208_adc_set_channel_select(AD9208_MCSPI1_CS2,AD9208_ADC_CH_A);   // AD3, channel A
    }

}
