#include "Lorandit_functions/modes/modes.h"
#include "AD9208_spi/AD9208.h"
#include "AD9208_spi/ad9208_reg.h"
#include "Lorandit_functions/modes/mode_Calibration/Calibration.h"
#include "Lorandit_functions/Structs.h"
#include "Shared_mem_communication/Shared.h"
#include "Lorandit_functions/reciever/reciever.h"

int8_t start_spectr_multi(uint32_t indx)
{

    uint32_t i, n;
    uint32_t left_frequency;
    uint32_t k,indx_old;
    uint32_t num1,counter;
    int8_t   status_spectr_multi;
    uint32_t NFFT_CUT;
    int      C;

    typedef struct
    {
       uint8_t mass[4096];          // 1024 * 4 DDC
    }mass_out_;
    mass_out_ ch_num[5];

    typedef struct
    {
       float   E[5];
    }overflow_;
    overflow_ ov_num[5];

    memset(ov_num,0,sizeof(ov_num));
    memset(ch_num,0,sizeof(ch_num));

    /// проверка условий  ////

    if (indx < spectrum.num_bands && indx > 0)
    {
        indx_old = indx - 1;

    }

    else if ((int32_t)indx - 1 < 0)
    {
        indx_old = spectrum.num_bands - 1;

    }

    NFFT_CUT    = useful_struct.fft_points - (useful_struct.bins_tocut << 1);
    spectrum.mass_size  = spectrum.nddc[indx_old] * NFFT_CUT;


    left_frequency = mass_bands[spectrum.band_idxs[indx]] & 0x0000ffff;   // значение крайней левой частоты, считанной из массива с полосами для текущего индекса полосы
                                                                          // основная формула определяется как  mass_band[i] = = ((uint16_t)(f_h << 16)) | (uint16_t)(f_l))
    status_spectr_multi = 0;

    Start_data();           // получил времянку с ПЛИС

     ////////////// настройка ПЛИС на следующую полосу  ////////////

   /* if (spectrum.mas_freq_range[indx] == 11)
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

    for (n = 0; n < spectrum.nddc[indx_old]; n++)
    {
        for (counter = 0; counter < 1024; counter++)  // перекладка
        {
            txDSPBuf0[counter] = rxvipBuf[counter + num1 + C];
            C                 += 1024;
            txDSPBuf1[counter] = rxvipBuf[counter + num1 + C];
            C                 += 1024;
            txDSPBuf2[counter] = rxvipBuf[counter + num1 + C];
            C                 += 1024;
            txDSPBuf3[counter] = rxvipBuf[counter + num1 + C];
            C                 += 1024;
            txDSPBuf4[counter] = rxvipBuf[counter + num1 + C];
            C                 += 1024;

        }

        num1 += 1024;

////////////////////////////////////////////////////////////////////////////////////////////////////
        for(i = 0; i < 1024; i++)                       // подыгрыш времянка шум (512Re,512Im)  //убрать//
        {
            txDSPBuf0[i] = -20 + (20+20)*(float)rand()/RAND_MAX;
            txDSPBuf1[i] = -20 + (20+20)*(float)rand()/RAND_MAX;
            txDSPBuf2[i] = -20 + (20+20)*(float)rand()/RAND_MAX;
            txDSPBuf3[i] = -20 + (20+20)*(float)rand()/RAND_MAX;
            txDSPBuf4[i] = -20 + (20+20)*(float)rand()/RAND_MAX;

        }
/////////////////////////////////////////////////////////////////////////////////////////////////////
        sh_mem_send_time_seq(SPECTRUM_POWER_DSP0,(uint32_t *)&txDSPBuf0[0],(uint32_t *)&txDSPBuf0[0],useful_struct.fft_points);
        sh_mem_send_time_seq(SPECTRUM_POWER_DSP1,(uint32_t *)&txDSPBuf1[0],(uint32_t *)&txDSPBuf1[0],useful_struct.fft_points);
        sh_mem_recieve(DSP0_);
        sh_mem_recieve(DSP1_);

        sh_mem_send_time_seq(SPECTRUM_POWER_DSP0,(uint32_t *)&txDSPBuf2[0],(uint32_t *)&txDSPBuf2[0],useful_struct.fft_points);
        sh_mem_send_time_seq(SPECTRUM_POWER_DSP1,(uint32_t *)&txDSPBuf3[0],(uint32_t *)&txDSPBuf3[0],useful_struct.fft_points);
        sh_mem_recieve(DSP0_);
        sh_mem_recieve(DSP1_);

        sh_mem_send_time_seq(SPECTRUM_POWER_DSP0,(uint32_t *)&txDSPBuf4[0],(uint32_t *)&txDSPBuf4[0],useful_struct.fft_points);
        sh_mem_recieve(DSP0_);

        memcpy(&ch_num[0].mass[0], &txDSPBuf0[useful_struct.start_val], sizeof(&txDSPBuf0[useful_struct.start_val]) * useful_struct.stop_val); // вырезал бины
        memcpy(&ch_num[1].mass[0], &txDSPBuf1[useful_struct.start_val], sizeof(&txDSPBuf1[useful_struct.start_val]) * useful_struct.stop_val); // вырезал бины
        memcpy(&ch_num[2].mass[0], &txDSPBuf2[useful_struct.start_val], sizeof(&txDSPBuf2[useful_struct.start_val]) * useful_struct.stop_val); // вырезал бины
        memcpy(&ch_num[3].mass[0], &txDSPBuf3[useful_struct.start_val], sizeof(&txDSPBuf3[useful_struct.start_val]) * useful_struct.stop_val); // вырезал бины
        memcpy(&ch_num[4].mass[0], &txDSPBuf4[useful_struct.start_val], sizeof(&txDSPBuf4[useful_struct.start_val]) * useful_struct.stop_val); // вырезал бины


        for (i = 0; i < NFFT_CUT; i++)
        {
            ch_num[0].mass[i] = ch_num[0].mass[i] + 10;
            ov_num[0].E[n]   += ch_num[0].mass[i];

            ch_num[1].mass[i] = ch_num[1].mass[i] + 10;
            ov_num[1].E[n]   += ch_num[1].mass[i];

            ch_num[2].mass[i] = ch_num[2].mass[i] + 10;
            ov_num[2].E[n]   += ch_num[2].mass[i];

            ch_num[3].mass[i] = ch_num[3].mass[i] + 10;
            ov_num[3].E[n]   += ch_num[3].mass[i];

            ch_num[4].mass[i] = ch_num[4].mass[i] + 10;
            ov_num[4].E[n]   += ch_num[4].mass[i];


        }


        //////////// Проверка на переполнение ////////////

        for (i = 0; i < 5; i++)
        {
            ov_num[i].E[n] = ov_num[i].E[n] / NFFT_CUT;
        }

    }

    for (i = 0; i < 5; i++)
    {
        k = sizeof(hat.n_bytes);

        hat.command = start_spectrum_;
        memcpy(&txEthBuf[k], &hat.command, sizeof(hat.command));
        k += sizeof(hat.command);

        memcpy(&txEthBuf[k], &useful_struct.bin_width, sizeof(useful_struct.bin_width));
        k += sizeof(useful_struct.bin_width);

        memcpy(&txEthBuf[k], &i, sizeof(spectrum.ch_num));
        k += sizeof(spectrum.ch_num);

        memcpy(&txEthBuf[k], &spectrum.band_idxs[indx_old], sizeof(spectrum.band_idxs[indx_old]));
        k += sizeof(spectrum.band_idxs[indx_old]);


        status_spectr_multi = - 1;

        for (n = 0; n < spectrum.nddc[indx_old]; n++)
        {

            if(ov_num[i].E[n] >= tresholds_energy)
            {
                spectrum.overflow = 1;
                memcpy(&txEthBuf[k], &spectrum.overflow, sizeof(spectrum.overflow));
                k += sizeof(spectrum.overflow);

                hat.n_bytes = k - sizeof(hat.n_bytes);
                memcpy(&txEthBuf[0], &hat.n_bytes, sizeof(hat.n_bytes));
                send_eth(arm, &txEthBuf[0], k);

                status_spectr_multi = 1;
                break;
            }

        }

        if (status_spectr_multi == - 1) // нет переполнения
        {

            memcpy(&txEthBuf[k], &spectrum.overflow, sizeof(spectrum.overflow));
            k += sizeof(spectrum.overflow);

            memcpy(&txEthBuf[k],&spectrum.mass_size, sizeof(spectrum.mass_size));
            k  += sizeof(spectrum.mass_size);

            memcpy(&txEthBuf[k], &ch_num[i].mass[0], sizeof(ch_num[i].mass[0])*spectrum.mass_size); // массив амплитуд для текущего канала, размерность посчитал
            k += sizeof(ch_num[i].mass[0])*spectrum.mass_size;

            hat.n_bytes = k - sizeof(hat.n_bytes);
            memcpy(&txEthBuf[0], &hat.n_bytes, sizeof(hat.n_bytes));
            send_eth(arm, &txEthBuf[0], k);
        }

    }

    return status_spectr_multi;


}
