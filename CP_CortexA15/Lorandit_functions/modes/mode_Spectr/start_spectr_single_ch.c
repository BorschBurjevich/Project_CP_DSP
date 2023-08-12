#include "Lorandit_functions/modes/modes.h"
#include "AD9208_spi/AD9208.h"
#include "AD9208_spi/ad9208_reg.h"
#include "Lorandit_functions/modes/mode_Calibration/Calibration.h"
#include "Lorandit_functions/reciever/reciever.h"
#include "Lorandit_functions/Structs.h"


// status_spectr_single == -1  // нет переполнения
// status_spectr_single ==  1  // есть переполнение

int8_t start_spectr_single_ch( uint32_t indx)
{
    uint8_t mas_out[4096];   // накапливаем с каждого канала DDC
    uint8_t mas[4096];       // промежуточный массив для хранения урезанной полосы
    int8_t  status_spectr_single;
//    uint32_t left_frequency;
    uint32_t k,j;
    uint32_t tmp,n, indx_old;
    uint32_t num1,counter = 0;
    float    energy_in_chan;
    float    NFFT_CUT;

    /// проверка условий  ////

    if (indx < spectrum.num_bands && indx > 0)
    {
       indx_old = indx - 1;

    }

    else if ((int32_t)indx - 1 < 0)
    {
       indx_old = spectrum.num_bands - 1;

    }
    ///////////////////////////
    NFFT_CUT    = useful_struct.fft_points - (useful_struct.bins_tocut << 1);
    spectrum.mass_size  = spectrum.nddc[indx_old] * NFFT_CUT;

    j                   = 0;
  //  left_frequency = mass_bands[spectrum.band_idxs[indx]] & 0x0000ffff; // значение крайней левой частоты, считанной из массива с полосами для текущего индекса полосы
                                                                        // основная формула определяется как  mass_band[i] = = ((uint16_t)(f_h << 16)) | (uint16_t)(f_l))
    status_spectr_single = 0;
    energy_in_chan = 0.0;
    Start_data(); // получил времянку с ПЛИС для предыдущего индекса

   /*  ////////////// настройка ПЛИС на следующую полосу  ////////////
    if (spectrum.mas_freq_range[indx] == 11)
    {
       freq_mhz       = 1650 - freq_mhz + 1000;
       left_frequency  = freq_mhz - (spectrum.nddc[indx] * 2);
    }
    else if (spectrum.mas_freq_range[indx] == 15)
    {
       freq_mhz       = 3000  - freq_mhz + 2000;
       left_frequency  = freq_mhz - (spectrum.nddc[indx] * 2);
    }

    config_ADC(spectrum.nddc[indx],left_frequency,useful_struct.fft_points,spectrum.ch_num);*/

    for (n = 0; n < spectrum.nddc[indx_old]; n ++)    // spectrum.nddc  для предыдущей полосы
    {
        for (counter = 0; counter < 1024; counter++)        // перекладка
        {
            txDSPBuf0[counter] = rxvipBuf[counter + num1];

        }
        num1 += 1024;

        ///////////////// Подыгрыш на 512 точки  //////////////////////////
        for(tmp = 0; tmp < 1024; tmp++)
        {
            txDSPBuf0[tmp] = -20 + (20+20)*(float)rand()/RAND_MAX;
        }
        //////////////////////////////////////////////////////////////////
        sh_mem_send_time_seq(SPECTRUM_POWER_DSP0,(uint32_t *)&txDSPBuf0[0],(uint32_t *)&txDSPBuf0[0], useful_struct.fft_points);
        sh_mem_recieve(DSP0_);

        for(tmp = 0; tmp < useful_struct.fft_points; tmp++)
        {
            txDSPBuf0[tmp] = txDSPBuf0[tmp] + 10;
        }

        memcpy(&mas[0], &txDSPBuf0[useful_struct.start_val], sizeof(&txDSPBuf0[useful_struct.start_val])*useful_struct.stop_val); // вырезал бины

        //////////// Проверка на переполнение ////////////

        for (tmp = 0; tmp < NFFT_CUT; tmp++)
        {
            energy_in_chan += mas[tmp];
        }

        energy_in_chan = energy_in_chan / NFFT_CUT;

        if (energy_in_chan > tresholds_energy)  // переполнение
        {

            k = sizeof(hat.n_bytes);
            hat.command = start_spectrum_;
            memcpy(&txEthBuf[k], &hat.command, sizeof(hat.command));
            k += sizeof(hat.command);

            memcpy(&txEthBuf[k], &useful_struct.bin_width, sizeof(useful_struct.bin_width));
            k += sizeof(useful_struct.bin_width);

            memcpy(&txEthBuf[k], &spectrum.ch_num, sizeof(spectrum.ch_num));
            k += sizeof(spectrum.ch_num);

            memcpy(&txEthBuf[k], &spectrum.band_idxs[indx_old], sizeof(spectrum.band_idxs[indx_old]));
            k += sizeof(spectrum.band_idxs[indx_old]);

            spectrum.overflow = 0x1;
            memcpy(&txEthBuf[k], &spectrum.overflow, sizeof(spectrum.overflow));
            k += sizeof(spectrum.overflow);

            hat.n_bytes = k - sizeof(hat.n_bytes);
            memcpy(&txEthBuf[0], &hat.n_bytes, sizeof(hat.n_bytes));
            send_eth(arm, &txEthBuf[0], k);                                 //  отправил на арм, сигнализирую переполнение


            status_spectr_single = 1;
            return status_spectr_single;

        }

        else                                    // нет переполнения
        {
            memcpy(&mas_out[j], &mas[0], sizeof(mas[0])*NFFT_CUT);              // запись со сдвигом для каждой полосы ддс
            j += sizeof(mas[0])*NFFT_CUT;
            status_spectr_single = - 1;
        }

    }

    k = sizeof(hat.n_bytes);
    hat.command = start_spectrum_;

    memcpy(&txEthBuf[k], &hat.command, sizeof(hat.command));
    k += sizeof(hat.command);

    memcpy(&txEthBuf[k], &useful_struct.bin_width, sizeof(useful_struct.bin_width));
    k += sizeof(useful_struct.bin_width);

    memcpy(&txEthBuf[k], &spectrum.ch_num, sizeof(spectrum.ch_num));
    k += sizeof(spectrum.ch_num);

    memcpy(&txEthBuf[k], &useful_struct.bin_width, sizeof(useful_struct.bin_width));
    k += sizeof(useful_struct.bin_width);

    memcpy(&txEthBuf[k], &spectrum.band_idxs[indx_old], sizeof(spectrum.band_idxs[indx_old]));
    k += sizeof(spectrum.band_idxs[indx_old]);

    spectrum.overflow = 0x0;
    memcpy(&txEthBuf[k], &spectrum.overflow, sizeof(spectrum.overflow));
    k += sizeof(spectrum.overflow);

    memcpy(&txEthBuf[k],&spectrum.mass_size, sizeof(spectrum.mass_size));
    k  += sizeof(spectrum.mass_size);

    memcpy(&txEthBuf[k], &mas_out[0], sizeof(mas_out[0])* spectrum.mass_size);
    k += sizeof(mas_out[0])*spectrum.mass_size;

    hat.n_bytes = k - sizeof(hat.n_bytes);
    memcpy(&txEthBuf[0], &hat.n_bytes, sizeof(hat.n_bytes));

    send_eth(arm, &txEthBuf[0], k);                                         //  отправил на арм

    return status_spectr_single;
}


