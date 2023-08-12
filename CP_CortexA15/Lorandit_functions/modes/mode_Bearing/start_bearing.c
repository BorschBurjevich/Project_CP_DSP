#include "Lorandit_functions/modes/modes.h"
#include "Lorandit_functions/Structs.h"
#include "Shared_mem_communication/Shared.h"

int8_t start_bearing(uint32_t indx)

{

    uint32_t n,i,shift0,shift1,k,n_signals;
    uint8_t  flag,count_read;
    uint32_t left_frequency,indx_old;
    uint16_t j;
    uint8_t  mas[2048];
    bearing_inform_ inform[100];
    int8_t  status;

    float E[5];

    /*инициализация переменных*/
    n = 0;
    i = 0;
    flag = 0;
    status = 0;     // status == 1 - overflow, status == -1 no overflow
    shift0 = 0;
    shift1 = 0;
    k   = 0;
    j = 0;          // размер массива амплитуд
    n_signals = 0;

    memset(&mas[0],0x0,sizeof(mas[0])*2048);
    memset(&inform[0],0x0,sizeof(inform[0])*100);
    memset(&E[0],0x0,sizeof(E[0])*5);

    /// проверка условий  ////

    if (indx < init_bear.num_bands && indx > 0)
    {
       indx_old = indx - 1;

    }

    else if ((int32_t)indx - 1 < 0)
    {
       indx_old = init_bear.num_bands - 1;

    }

  /*  left_frequency = mass_bands[init_bear.band_idxs[indx]] & 0x0000ffff;   // значение крайней левой частоты, считанной из массива с полосами для текущего индекса полосы
                                                                         // основная формула определяется как  mass_band[i] = = ((uint16_t)(f_h << 16)) | (uint16_t)(f_l))
    ////////////// настройка  на следующую полосу  ////////////
    if (init_bear.mas_freq_range[indx] == 11)
    {
       freq_mhz        = 2650 - freq_mhz;
       left_frequency  = freq_mhz - (init_bear.nddc[indx] << 1);
    }
    else if (init_bear.mas_freq_range[indx] == 15)
    {
       freq_mhz        = 5000  - freq_mhz;
       left_frequency  = freq_mhz - (init_bear.nddc[indx] << 1);
    }*/


    for (count_read = 0; count_read < 3; count_read++)  // цикл по временным реализациям
    {
        for (n = 0; n < init_bear.nddc[indx_old]; n++)
        {
            //снятие данных
            Start_data();

            /*
             * добавить перекладку из вип порта  в н массив
             */

            //спектры
            sh_mem_send_time_seq(SPECTRUM_DSP0,(uint32_t *)&txDSPBuf0[0],(uint32_t *)&txDSPBuf0[0],useful_struct.fft_points);
            sh_mem_send_time_seq(SPECTRUM_DSP1,(uint32_t *)&txDSPBuf1[0],(uint32_t *)&txDSPBuf1[0],useful_struct.fft_points);
            sh_mem_recieve(DSP0_);
            sh_mem_recieve(DSP1_);

            //случай пеленга со спектром
            if ((tresholds[BEARING_WITH_SPECTRUM] == 1) && (spectrum_cnt % tresholds[SPECTRUM_STEP] == 0) && (count_read == 0))
            {
                for (i = 0; i < useful_struct.fft_points << 1; i += 2)
                {
                    //формируем спектральный массив с 0 канала
                    mas[j++] = 10*log10f(txDSPBuf0[i]*txDSPBuf0[i] + txDSPBuf0[i+1]*txDSPBuf0[i+1]) + 10;       // спектр мощности
                }
            }

            sh_mem_send_time_seq(SPECTRUM_DSP0,(uint32_t *)&txDSPBuf2[0],(uint32_t *)&txDSPBuf2[0], useful_struct.fft_points);
            sh_mem_send_time_seq(SPECTRUM_DSP1,(uint32_t *)&txDSPBuf3[0],(uint32_t *)&txDSPBuf3[0], useful_struct.fft_points);
            sh_mem_recieve(DSP0_);
            sh_mem_recieve(DSP1_);

            sh_mem_send_time_seq(SPECTRUM_DSP0,(uint32_t *)&txDSPBuf4[0],(uint32_t *)&txDSPBuf4[0], useful_struct.fft_points);
            sh_mem_recieve(DSP0_);


            //считаем энергию по каналам
            for (i = 0; i < useful_struct.fft_points << 1; i += 2)
            {
                E[0] += (txDSPBuf0[i]*txDSPBuf0[i] + txDSPBuf0[i+1]*txDSPBuf0[i+1]);
                E[1] += (txDSPBuf1[i]*txDSPBuf1[i] + txDSPBuf1[i+1]*txDSPBuf1[i+1]);
                E[2] += (txDSPBuf2[i]*txDSPBuf2[i] + txDSPBuf2[i+1]*txDSPBuf2[i+1]);
                E[3] += (txDSPBuf3[i]*txDSPBuf3[i] + txDSPBuf3[i+1]*txDSPBuf3[i+1]);
                E[4] += (txDSPBuf4[i]*txDSPBuf4[i] + txDSPBuf4[i+1]*txDSPBuf4[i+1]);

            }

            //////////// Проверка на переполнение ////////////

            for (i = 0; i < 5; i++)
            {
                E[i] = E[i] / useful_struct.fft_points;

                if(E[i] > tresholds_energy)
                {
                    status = 1;
                    bearing_bands[bearing_cnt].band_idx = init_bear.band_idxs[indx_old];
                    bearing_bands[bearing_cnt].overflow = 1;
                    return (status);
                }

            }

            status   = -1;
            //формирование массивов для дсп ядер
            if (init_bear.nddc[indx_old] == 1)
            {
                //1 массив с 1 ддс
                memcpy(&BearingBuf00[shift0], &txDSPBuf0[useful_struct.start_val], useful_struct.stop_val); // одновременно делаем обрезку спектра
                memcpy(&BearingBuf01[shift0], &txDSPBuf1[useful_struct.start_val], useful_struct.stop_val);
                memcpy(&BearingBuf02[shift0], &txDSPBuf2[useful_struct.start_val], useful_struct.stop_val);
                memcpy(&BearingBuf03[shift0], &txDSPBuf3[useful_struct.start_val], useful_struct.stop_val);
                memcpy(&BearingBuf04[shift0], &txDSPBuf4[useful_struct.start_val], useful_struct.stop_val);

                shift0 += useful_struct.stop_val;
            }

            else if (init_bear.nddc[indx_old] == 2)
            {
                //склейка в 2 массива по 1 ддс
                if (flag == 0)
                {
                    memcpy(&BearingBuf00[shift0], &txDSPBuf0[useful_struct.start_val], useful_struct.stop_val);
                    memcpy(&BearingBuf01[shift0], &txDSPBuf1[useful_struct.start_val], useful_struct.stop_val);
                    memcpy(&BearingBuf02[shift0], &txDSPBuf2[useful_struct.start_val], useful_struct.stop_val);
                    memcpy(&BearingBuf03[shift0], &txDSPBuf3[useful_struct.start_val], useful_struct.stop_val);
                    memcpy(&BearingBuf04[shift0], &txDSPBuf4[useful_struct.start_val], useful_struct.stop_val);

                    shift0 += useful_struct.stop_val;
                }

                if (flag == 1)
                {
                    memcpy(&BearingBuf10[shift1], &txDSPBuf0[useful_struct.start_val], useful_struct.stop_val);
                    memcpy(&BearingBuf11[shift1], &txDSPBuf1[useful_struct.start_val], useful_struct.stop_val);
                    memcpy(&BearingBuf12[shift1], &txDSPBuf2[useful_struct.start_val], useful_struct.stop_val);
                    memcpy(&BearingBuf13[shift1], &txDSPBuf3[useful_struct.start_val], useful_struct.stop_val);
                    memcpy(&BearingBuf14[shift1], &txDSPBuf4[useful_struct.start_val], useful_struct.stop_val);

                    shift1 += useful_struct.stop_val;
                }
                flag++;
            }

            else
            {
                //склейка в 2 массива по 2 ддс
                if ((flag == 0) || (flag == 1))
                {
                    memcpy(&BearingBuf00[shift0], &txDSPBuf0[useful_struct.start_val], useful_struct.stop_val);
                    memcpy(&BearingBuf01[shift0], &txDSPBuf1[useful_struct.start_val], useful_struct.stop_val);
                    memcpy(&BearingBuf02[shift0], &txDSPBuf2[useful_struct.start_val], useful_struct.stop_val);
                    memcpy(&BearingBuf03[shift0], &txDSPBuf3[useful_struct.start_val], useful_struct.stop_val);
                    memcpy(&BearingBuf04[shift0], &txDSPBuf4[useful_struct.start_val], useful_struct.stop_val);

                    shift0 += useful_struct.stop_val;
                }

                if ((flag == 2) || (flag == 3))
                {
                    memcpy(&BearingBuf10[shift1], &txDSPBuf0[useful_struct.start_val], useful_struct.stop_val);
                    memcpy(&BearingBuf11[shift1], &txDSPBuf1[useful_struct.start_val], useful_struct.stop_val);
                    memcpy(&BearingBuf12[shift1], &txDSPBuf2[useful_struct.start_val], useful_struct.stop_val);
                    memcpy(&BearingBuf13[shift1], &txDSPBuf3[useful_struct.start_val], useful_struct.stop_val);
                    memcpy(&BearingBuf14[shift1], &txDSPBuf4[useful_struct.start_val], useful_struct.stop_val);

                    shift1 += useful_struct.stop_val;
                }
                flag++;
            }

        }

    }

    //отправка спектра в режиме пеленг

    if ((tresholds_item.bearing_with_spectrum == 1) && (spectrum_cnt % tresholds_item.spectrum_step == 0))
    {
        k = sizeof(hat.n_bytes);

        hat.command = start_spectrum_;
        memcpy(&txEthBuf[k], &hat.command, sizeof(hat.command));
        k += sizeof(hat.command);

        spectrum.ch_num = 0x0;
        memcpy(&txEthBuf[k], &spectrum.ch_num, sizeof(spectrum.ch_num));
        k += sizeof(spectrum.ch_num);

        memcpy(&txEthBuf[k], &init_bear.band_idxs[indx_old], sizeof(init_bear.band_idxs[indx_old]));
        k += sizeof(init_bear.band_idxs[indx_old]);

        bearing_bands[bearing_cnt].overflow = 0;
        memcpy(&txEthBuf[k], &bearing_bands[bearing_cnt].overflow, sizeof(bearing_bands[bearing_cnt].overflow));
        k += sizeof(bearing_bands[bearing_cnt].overflow);


        memcpy(&txEthBuf[k],&useful_struct.bin_width,sizeof(useful_struct.bin_width));
        k += sizeof(useful_struct.bin_width);

        j = init_bear.nddc[indx_old] * (useful_struct.fft_points - useful_struct.bins_tocut)<<1;
        memcpy(&txEthBuf[k], &j, sizeof(j));                       // размер массива амплитуд
        k += sizeof(j);

        memcpy(&txEthBuf[k], &mas, sizeof(mas)*j);                // массив амлитуд
        k += sizeof(mas)*j;

        hat.n_bytes = k - sizeof(hat.n_bytes);
        memcpy(&txEthBuf[0], &hat.n_bytes, sizeof(hat.n_bytes));
        send_eth(arm, &txEthBuf[0], k);
    }

    //считаем пеленг
    if(init_bear.nddc[indx_old] == 1)             // если одна полоса 2 Мгц
    {
        sh_mem_bearing(BEARING_DSP0, shift0);
        sh_mem_recieve(DSP0_);

        memcpy(&n, &rxDSPBuf0[0], sizeof(uint32_t));                 // число сигналов с 1 ого ядра
        memcpy(&inform[0], &rxDSPBuf0[1], sizeof(bearing_inform_)*n); // информация по найденным сигналам
    }
    else
    {
        sh_mem_bearing(BEARING_DSP0, shift0);                         //
        sh_mem_bearing(BEARING_DSP1, shift1);                         //
        sh_mem_recieve(DSP0_);
        sh_mem_recieve(DSP1_);

        memcpy(&n, &rxDSPBuf0[0], sizeof(uint32_t));
        memcpy(&inform[0], &rxDSPBuf0[1], sizeof(bearing_inform_)*n);

        memcpy(&i, &rxDSPBuf1, sizeof(uint32_t));                   //число сигналов со 2 ядра
        memcpy(&inform[n], &rxDSPBuf1[1], sizeof(bearing_inform_)*i);
        n += i;                                                     // общее количество сигналов
    }

    //отбрасываем, что не подходит под секторы пеленгования
    for (i = 0; i < n; i++)
    {
        flag = 0;
        for (j = 0; j < init_bear.num_sectors; j++)
        {
            if((init_bear.left_border[j] <= inform[i].azimuth) && (inform[i].azimuth <= init_bear.right_border[j]))
            {
                flag++;
                break;
            }
        }

        if (flag > 0)
        {
            bearing_bands[bearing_cnt].band_idx = init_bear.band_idxs[indx_old];
            bearing_bands[bearing_cnt].overflow = 0;
            memcpy(&bearing_bands[bearing_cnt].inform[n_signals++], &inform[i], sizeof(bearing_inform_));
        }
    }
    bearing_bands[bearing_cnt].num_signals = n_signals;

    //отправка результатов
    if (bearing_cnt == (tresholds_item.num_poo - 1))
    {
        k = sizeof(hat.n_bytes);

        hat.command = start_bearing_;
        memcpy(&txEthBuf[k], &hat.command, sizeof(hat.command));
        k += sizeof(hat.command);

        memcpy(&txEthBuf[k], &tresholds_item.num_poo, sizeof(init_bear.num_bands));
        k += sizeof(init_bear.num_bands);

        for(i = 0; i < bearing_cnt; i++)
        {
            memcpy(&txEthBuf[k], &bearing_bands[i].band_idx, sizeof(bearing_bands[i].band_idx));
            k += sizeof(bearing_bands[i].band_idx);

            memcpy(&txEthBuf[k], &bearing_bands[i].overflow, sizeof(bearing_bands[i].overflow));
            k += sizeof(bearing_bands[i].overflow);

            if(bearing_bands[i].overflow != 1)
            {
                memcpy(&txEthBuf[k], &bearing_bands[i].num_signals, sizeof(bearing_bands[i].num_signals));
                k += sizeof(bearing_bands[i].num_signals);

                for (j = 0; j < bearing_bands[i].num_signals; j++)
                {
                    memcpy(&txEthBuf[k], &bearing_bands[i].inform[j].central_Freq, sizeof(bearing_bands[i].inform[j].central_Freq));
                    k += sizeof(bearing_bands[i].inform[j].central_Freq);

                    memcpy(&txEthBuf[k], &bearing_bands[i].inform[j].left_Freq, sizeof(bearing_bands[i].inform[j].left_Freq));
                    k += sizeof(bearing_bands[i].inform[j].left_Freq);

                    memcpy(&txEthBuf[k], &bearing_bands[i].inform[j].right_Freq, sizeof(bearing_bands[i].inform[j].right_Freq));
                    k += sizeof(bearing_bands[i].inform[j].right_Freq);

                    memcpy(&txEthBuf[k], &bearing_bands[i].inform[j].energy_db, sizeof(bearing_bands[i].inform[j].energy_db));
                    k += sizeof(bearing_bands[i].inform[j].energy_db);

                    memcpy(&txEthBuf[k], &bearing_bands[i].inform[j].azimuth, sizeof(bearing_bands[i].inform[j].azimuth));
                    k += sizeof(bearing_bands[i].inform[j].azimuth);

                    memcpy(&txEthBuf[k], &bearing_bands[i].inform[j].coeff_reliability, sizeof(bearing_bands[i].inform[j].coeff_reliability));
                    k += sizeof(bearing_bands[i].inform[j].coeff_reliability);
                }
            }
        }
        hat.n_bytes = k - sizeof(hat.n_bytes);
        memcpy(&txEthBuf[0], &hat.n_bytes, sizeof(hat.n_bytes));
        send_eth(arm, &txEthBuf[0], k);
    }


    return status;
}
