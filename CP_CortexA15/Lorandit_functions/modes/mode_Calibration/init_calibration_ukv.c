#include "Lorandit_functions/modes/modes.h"
#include "Lorandit_functions/modes/mode_Calibration/Calibration.h"
#include "Lorandit_functions/reciever/reciever.h"
#include "Lorandit_functions/Structs.h"
#include "AD9208_spi/AD9208.h"
#include "AD9208_spi/ad9208_reg.h"

void init_calibration_ukv()
{
    uint32_t k = 0;
    uint32_t tmp,q;
    uint16_t fft_points;

    fft_points = 512;   // калибруемс€ по 512 точек жЄстко
    memcpy(&callibration.step, &pBuf[k], sizeof(callibration.step));
    k += sizeof(callibration.step);

    memcpy(&callibration.auto_att, &pBuf[k], sizeof(callibration.auto_att));
    k += sizeof(callibration.auto_att);

    memcpy(&callibration.bkl_noise, &pBuf[k], sizeof(callibration.bkl_noise));
    k += sizeof(callibration.bkl_noise);

    memcpy(&callibration.num_bands, &pBuf[k], sizeof(callibration.num_bands));
    tmp = callibration.num_bands;
    k += sizeof(callibration.num_bands);

    while(tmp != 0)
    {
        memcpy(&callibration.band_idx[callibration.num_bands - tmp], &pBuf[k],  sizeof(callibration.band_idx[0])); // кладЄм индекс полосы по циклу с количеством полос,
                                                                                                                   // в pBuf[] уже лежат интерпретируемые индексы полос соответсвующим частотам
        k += sizeof(callibration.band_idx[0]);

        memcpy(&callibration.att_prm[callibration.num_bands - tmp], &pBuf[k], sizeof(callibration.att_prm[0]));
        k += sizeof(callibration.att_prm[0]);

        memcpy(&callibration.att_bk[callibration.num_bands - tmp], &pBuf[k], sizeof(callibration.att_bk[0]));
        k += sizeof(callibration.att_bk[0]);
        tmp--;
    }

    memset(&callibration.att_bk[callibration.num_bands - tmp],0, sizeof(callibration.att_bk[0])*393);
    freq_mhz = (mass_bands[callibration.band_idx[0]] >> 16); // считал нулевую полосу
    //настройка €чейки коммутатора на режим калибровки//

    //// настройка антенны на нулевую полосу, (€чейки коммутации) ////

/*    if  (freq_mhz < 100)                            // 1 литера
    {
        Set_Cell_Switchboard(RGM_IN_CAL_ANT1,0x0,0x0);  // настройка €чейки коммутатора
        flag_aerial         = 1;
    }

    else if ((freq_mhz >=100) && (freq_mhz < 800))  // 2 литера
    {
        Set_Cell_Switchboard(RGM_IN_CAL_ANT2,0x0,0x0);
        flag_aerial         = 2;
    }

    else if (freq_mhz >= 800)                       // 3 литера
    {
        Set_Cell_Switchboard(RGM_IN_CAL_ANT3,0x0,0x0);
        flag_aerial         = 3;
    }*/


    //настройка приЄмника//
    callibration.mas_freq_range[0] = pres_freq_range(freq_mhz*1000000);                       // индекс используемого частотного диапазона
    set_rec_mode(set_cmd_1,MCSPI2_LVDS_CS3, 0x5, callibration.mas_freq_range[0],callibration.att_prm[0],0,0); // настройка приЄмника


    for (q = 0; q <callibration.num_bands; q++)
    {
        callibration.left_freq[q]   =  mass_bands[callibration.band_idx[q]] & 0x0000ffff;
        callibration.n_ddc[q] =(( mass_bands[callibration.band_idx[q]] >> 16) - (mass_bands[callibration.band_idx[q]] & 0x0000ffff))/2;

    }

    //настрйока ѕЋ»— и ј÷ѕ//
    config_ADC(callibration.n_ddc[0], callibration.left_freq[0], fft_points, 0x55);

    // со всех каналов

    AD9208_adc_set_channel_select(AD9208_MCSPI1_CS0, AD9208_ADC_CH_ALL); // AD1
    AD9208_adc_set_channel_select(AD9208_MCSPI1_CS1, AD9208_ADC_CH_ALL); // AD2
    AD9208_adc_set_channel_select(AD9208_MCSPI1_CS2, AD9208_ADC_CH_A);   // AD3, channel A

}
