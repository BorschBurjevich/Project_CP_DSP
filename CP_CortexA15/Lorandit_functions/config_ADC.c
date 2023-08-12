#include "Lorandit_functions/modes/modes.h"
#include "AD9208_spi/AD9208.h"
#include "AD9208_spi/ad9208_reg.h"



void config_ADC(uint8_t num_DDC, uint32_t left_fr, uint16_t n_fft, uint8_t channel)
{
    AD9208_t AD;
    int tmp;
    uint32_t central_freq_for_each_DDC[4];
    uint8_t buf[20];

    memset(&buf[0],0,sizeof(buf[0])*20);
    memset(&central_freq_for_each_DDC[0],0,sizeof(central_freq_for_each_DDC[0])*4);
    /////////////////////////////////////////////////////////////////////

     /// Настройка ПЛИС Славы на центральную частоту для каждой полосы DDC ////
      // подумать над универсальностью функции, используется в калибровке

    for (tmp = 0; tmp < num_DDC; tmp++)
    {

        central_freq_for_each_DDC[tmp] =  left_fr + left_fr + 2;                          // left_freq + right_freq
        left_fr                        =  central_freq_for_each_DDC[tmp];                 // left_freq = left_freq + right_freq  (для следующей полосы DDC)
        central_freq_for_each_DDC[tmp] =  central_freq_for_each_DDC[tmp] * 500000;        // в Гц


    }

    buf[0] = num_DDC;
    buf[1] = channel;
    buf[2] = (uint8_t)((n_fft & 0xff00) >> 8);  // MSB for n_fft
    buf[3] = (uint8_t)n_fft & 0x00ff;           // LSB for n_fft
    ////// buf[4] - buf[20] for mas central_freq_for_each_DDC///

    spi3FPGA_write(MCSPI3_CS2,&buf[0],5);

                                              // определиться с SPI, который будет писать на плис
 //////////////////////////////////////////////////////////////////////////////////

    /////      Настраиваю AD9208 на центральную часоту        ////

        for (tmp = 0; tmp < 4; tmp ++)
        {

            AD9208_adc_set_ddc_gain(AD9208_MCSPI1_CS0, tmp, AD.ddc[tmp].gain_db ? 6 : 0);
            AD9208_adc_set_ddc_dcm(AD9208_MCSPI1_CS0, tmp, AD.ddc[tmp].decimation);
            AD9208_adc_set_ddc_nco_mode(AD9208_MCSPI1_CS0, tmp, AD.ddc[tmp].nco_mode);
            AD9208_adc_set_ddc_nco(AD9208_MCSPI1_CS0, &AD, tmp, AD.ddc[tmp].carrier_freq_hz); // вызывать 4 раза для настройки на центральную частоту для  ddc, для каждой AD9208
            AD9208_adc_set_ddc_nco_phase(AD9208_MCSPI1_CS0, tmp, AD.ddc[tmp].po);

            AD9208_adc_set_ddc_gain(AD9208_MCSPI1_CS1, tmp, AD.ddc[tmp].gain_db ? 6 : 0);
            AD9208_adc_set_ddc_dcm(AD9208_MCSPI1_CS1, tmp, AD.ddc[tmp].decimation);
            AD9208_adc_set_ddc_nco_mode(AD9208_MCSPI1_CS1, tmp, AD.ddc[tmp].nco_mode);
            AD9208_adc_set_ddc_nco(AD9208_MCSPI1_CS1, &AD, tmp, AD.ddc[tmp].carrier_freq_hz);
            AD9208_adc_set_ddc_nco_phase(AD9208_MCSPI1_CS1, tmp, AD.ddc[tmp].po);


            AD9208_adc_set_ddc_gain(AD9208_MCSPI1_CS2, tmp, AD.ddc[tmp].gain_db ? 6 : 0);
            AD9208_adc_set_ddc_dcm(AD9208_MCSPI1_CS2, tmp,AD.ddc[tmp].decimation);
            AD9208_adc_set_ddc_nco_mode(AD9208_MCSPI1_CS2, tmp, AD.ddc[tmp].nco_mode);
            AD9208_adc_set_ddc_nco(AD9208_MCSPI1_CS2, &AD, tmp, AD.ddc[tmp].carrier_freq_hz);
            AD9208_adc_set_ddc_nco_phase(AD9208_MCSPI1_CS2, tmp, AD.ddc[tmp].po);

        }


}
