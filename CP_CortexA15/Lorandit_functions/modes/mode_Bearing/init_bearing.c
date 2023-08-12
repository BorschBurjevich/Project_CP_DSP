#include "Lorandit_functions/modes/modes.h"
#include "Lorandit_functions/Structs.h"
#include "Lorandit_functions/modes/mode_Calibration/Calibration.h"
#include "Lorandit_functions/reciever/reciever.h"
#include "AD9208_spi/AD9208.h"
#include "AD9208_spi/ad9208_reg.h"

void init_bearing()
{
    uint32_t k = 0;
    uint32_t tmp;
    uint32_t left_frequency;

    memcpy(&init_bear.priznak_bearing, &pBuf[k], sizeof(init_bear.priznak_bearing));
    k += sizeof(init_bear.priznak_bearing);

   /* memcpy(&slsr.num_words_slsr, &pBuf[k], sizeof(slsr.num_words_slsr));      // разбор для слайсера
    k += sizeof(slsr.num_words_slsr);

    memcpy(&slsr.slsr_cmd[0],&pBuf[k],sizeof(slsr.slsr_cmd[0])*slsr.num_words_slsr);
    k += sizeof(slsr.num_words_slsr);*/


    /*работаем по слайсеру*/
    //save_idx_band = &sliser_.slser_cmd[0]
    //sliser(&save_idx_band, slsr.band_next, &sliser_.slser_cmd[0]);
    //slsr.band_now = slsr.band_next;
    //sliser(&save_idx_band, slsr.band_next, &sliser_.slser_cmd[0]);
    ////////////////////////
    memcpy(&init_bear.num_bands, &pBuf[k], sizeof(init_bear.num_bands)); // количество полос пеленгования
    tmp = init_bear.num_bands;
    k += sizeof(init_bear.num_bands);

    while(tmp != 0)                                                                 // цикл по полосам
    {
        memcpy(&init_bear.band_idxs[init_bear.num_bands - tmp], &pBuf[k], sizeof(init_bear.band_idxs[0]));
        k += sizeof(init_bear.band_idxs[0]);
        tmp--;
    }

    memcpy(&init_bear.num_sectors, &pBuf[k], sizeof(init_bear.num_sectors));
    tmp = init_bear.num_sectors;
    k +=  sizeof(init_bear.num_sectors);

    while (tmp != 0)                                                                 // цикл по секторам пеленгования
    {
        memcpy(&init_bear.left_border[init_bear.num_sectors - tmp], &pBuf[k], sizeof(init_bear.left_border[0]));
        k += sizeof(init_bear.left_border[0]);

        memcpy(&init_bear.right_border[init_bear.num_sectors - tmp], &pBuf[k], sizeof(init_bear.right_border[0]));
        k += sizeof(init_bear.right_border[0]);
        tmp--;
    }

    ///////////////////////
    for (tmp = 0; tmp < init_bear.num_bands; tmp ++)                          // заполняем для каждой полосы требуемое количество DDC
    {
       init_bear.nddc[tmp] = ((mass_bands[init_bear.band_idxs[tmp]] >> 16) - (mass_bands[init_bear.band_idxs[tmp]] & 0x0000ffff))/2;
    }

    //проставляем аттенюацию с в соответсвии с порогом

    if (tresholds_item.att_uhf != 0)
    {
        int i;
        for (i = 0; i < 16; i++)
        {
           init_bear.att_prm[i] = tresholds_item.att_uhf;    // счтывание из порогов знач - ие атт для каждого преселектора
        }

    }

    else
    {
        memset(&init_bear.att_prm[0], 0, 16*sizeof(init_bear.att_prm[0]));
    }


    /// перестройка антенны ///
    freq_mhz = (mass_bands[init_bear.band_idxs[0]] >> 16);
    init_bear.mas_freq_range[0] = pres_freq_range(freq_mhz*1000000);                       // индекс используемого частотного диапазона
    flag_aerial = selection_litera(freq_mhz,flag_aerial,0);
    left_frequency = mass_bands[init_bear.band_idxs[0]] & 0x0000ffff;

    if (init_bear.mas_freq_range[0] == 11) // приёмник сносит частоту
    {
       freq_mhz        = 2650 - freq_mhz;
       left_frequency  = freq_mhz - (init_bear.nddc[0] << 1);
    }
    else if (init_bear.mas_freq_range[0] == 15)
    {
       freq_mhz        = 5000  - freq_mhz;
       left_frequency  = freq_mhz - (init_bear.nddc[0] << 1);
    }
    set_rec_mode(set_cmd_1,MCSPI2_LVDS_CS3, 0x5, init_bear.mas_freq_range[0],init_bear.att_prm[0], 0,0); // настройка приёмника

    // со всех каналов
    AD9208_adc_set_channel_select(AD9208_MCSPI1_CS0,AD9208_ADC_CH_ALL); // AD1
    AD9208_adc_set_channel_select(AD9208_MCSPI1_CS1,AD9208_ADC_CH_ALL); // AD2
    AD9208_adc_set_channel_select(AD9208_MCSPI1_CS2,AD9208_ADC_CH_A);   // AD3, channel A

       ////////////// настройка ПЛИС на приём нулевой полосы
    config_ADC(init_bear.nddc[0],left_frequency,useful_struct.fft_points,0x5);

}
