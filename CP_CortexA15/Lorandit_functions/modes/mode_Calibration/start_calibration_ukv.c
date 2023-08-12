#include "Lorandit_functions/modes/modes.h"
#include "AD9208_spi/AD9208.h"
#include "AD9208_spi/ad9208_reg.h"
#include "Lorandit_functions/modes/mode_Calibration/Calibration.h"
#include "Lorandit_functions/reciever/reciever.h"
#include "Lorandit_functions/Structs.h"
#include "Shared_mem_communication/Shared.h"


int8_t start_calibration_ukv(uint32_t indx)
{
    uint32_t indx_old, shift0,shift1;
    uint8_t j,status,i;
    int8_t status_callibration;
    uint32_t accum, count, count_2;
    uint16_t fft_points,start_val,stop_val;
    uint32_t C;

    float callibration_Buf0[4776];
    float callibration_Buf1[4776];
    float callibration_Buf2[4776];
    float callibration_Buf3[4776];
    float callibration_Buf4[4776];
    float bin_resolution;
    float length;
    float E[5];

    C = 0; j = 0;
    status = 0;
    fft_points = 512; bin_resolution = 5.00;     // 5.015к√ц
    stop_val   = 398; start_val  = 57;
    shift0     = 0;   shift1     = 0;
    length     = 0;

    memset(&callibration_Buf0[0],0,sizeof(callibration_Buf0[0])*4776);
    memset(&callibration_Buf1[0],0,sizeof(callibration_Buf1[0])*4776);
    memset(&callibration_Buf2[0],0,sizeof(callibration_Buf2[0])*4776);
    memset(&callibration_Buf3[0],0,sizeof(callibration_Buf3[0])*4776);
    memset(&callibration_Buf4[0],0,sizeof(callibration_Buf4[0])*4776);
    memset(&E[0],0,sizeof(E[0])*5);

    /// проверка условий  ////

    if (indx < callibration.num_bands && indx > 0)
    {
        indx_old = indx - 1;

    }

    else if ((int32_t)indx - 1 < 0)
    {
        indx_old = callibration.num_bands - 1;

    }

    status_callibration = 0;
    config_ADC(callibration.n_ddc[indx],callibration.left_freq[indx],fft_points,0x55);
    length = (uint32_t)(2 * callibration.step / bin_resolution);  // рассто€ние между исследуемыми бинами, куда попадает нужна€ нам частота
    for(accum = 0; accum < 3; accum++)          // калибруемс€ по шуму,  поэтому накапливаем дл€ усреднени€
    {

        for (count = 0; count < callibration.n_ddc[indx_old]; count++)
        {
            Start_data();

            sh_mem_send_time_seq(SPECTRUM_DSP0,(uint32_t *)&txDSPBuf0[0],(uint32_t *)&txDSPBuf0[0], 512);
            sh_mem_send_time_seq(SPECTRUM_DSP1,(uint32_t *)&txDSPBuf1[0],(uint32_t *)&txDSPBuf1[0], 512);
            sh_mem_recieve(DSP0_);
            sh_mem_recieve(DSP1_);

            sh_mem_send_time_seq(SPECTRUM_DSP0,(uint32_t *)&txDSPBuf2[0],(uint32_t *)&txDSPBuf2[0], 512);
            sh_mem_send_time_seq(SPECTRUM_DSP1,(uint32_t *)&txDSPBuf3[0],(uint32_t *)&txDSPBuf3[0], 512);
            sh_mem_recieve(DSP0_);
            sh_mem_recieve(DSP1_);

            sh_mem_send_time_seq(SPECTRUM_DSP0,(uint32_t *)&txDSPBuf4[0],(uint32_t *)&txDSPBuf4[0], 512);
            sh_mem_recieve(DSP0_);

            memcpy(&callibration_Buf0[shift0], &txDSPBuf0[start_val], sizeof(&txDSPBuf0[start_val])*stop_val); // одновременно делаем обрезку спектра
            memcpy(&callibration_Buf1[shift0], &txDSPBuf1[start_val], sizeof(&txDSPBuf1[start_val])*stop_val);
            memcpy(&callibration_Buf2[shift0], &txDSPBuf2[start_val], sizeof(&txDSPBuf2[start_val])*stop_val);
            memcpy(&callibration_Buf3[shift0], &txDSPBuf3[start_val], sizeof(&txDSPBuf3[start_val])*stop_val);
            memcpy(&callibration_Buf4[shift0], &txDSPBuf4[start_val], sizeof(&txDSPBuf4[start_val])*stop_val);

            //считаем энергию по каналам
            for (i = 0; i < 398; i += 2)
            {
                E[0] += (callibration_Buf0[i]*callibration_Buf0[i] + callibration_Buf0[i+1]*callibration_Buf0[i+1]);
                E[1] += (callibration_Buf1[i]*callibration_Buf1[i] + callibration_Buf1[i+1]*callibration_Buf1[i+1]);
                E[2] += (callibration_Buf2[i]*callibration_Buf2[i] + callibration_Buf2[i+1]*callibration_Buf2[i+1]);
                E[3] += (callibration_Buf3[i]*callibration_Buf3[i] + callibration_Buf3[i+1]*callibration_Buf3[i+1]);
                E[4] += (callibration_Buf4[i]*callibration_Buf4[i] + callibration_Buf4[i+1]*callibration_Buf4[i+1]);

            }

            //////////// ѕроверка на переполнение ////////////
            if (status != 1)
            {

                for (i = 0; i < 5; i++)
                {
                    E[i] = E[i] / 398;

                    if (E[i] > tresholds_energy)
                    {
                        status = 1;
                        status_callibration = 1;

                    }

                }

            }

            for (count_2 = 0; count_2 < 21; count_2++)
            {

                Re_Bin_Ch0[count_2 + shift1] = callibration_Buf0[C];
                Re_Bin_Ch1[count_2 + shift1] = callibration_Buf1[C];
                Re_Bin_Ch2[count_2 + shift1] = callibration_Buf2[C];
                Re_Bin_Ch3[count_2 + shift1] = callibration_Buf3[C];
                Re_Bin_Ch4[count_2 + shift1] = callibration_Buf4[C];

                Im_Bin_Ch0[count_2 + shift1] = callibration_Buf0[C+1];
                Im_Bin_Ch1[count_2 + shift1] = callibration_Buf1[C+1];
                Im_Bin_Ch2[count_2 + shift1] = callibration_Buf2[C+1];
                Im_Bin_Ch3[count_2 + shift1] = callibration_Buf3[C+1];
                Im_Bin_Ch4[count_2 + shift1] = callibration_Buf4[C+1];

                callibration.calibr_coeff[indx_old].bin_freq[count_2 + shift1] = callibration.left_freq[indx_old] + length; // запись каллибруемой частоты
                callibration.calibr_coeff[indx_old].error[count] = 0;
                C = C + length;

            }


            shift1 += 21;
            shift0 += stop_val;
            C       = shift0;   // сдвиг на след 2 ћ√ц

        }

        for (count = 0; count < 21*callibration.n_ddc[indx_old]; count++)
        {
            Acumulation_Mutual_Spectrum_UKV_Noise(count, j);
            j = 1;
        }

        shift1 = 0;
        C      = 0;
        shift0 = 0;

    }

    Calc_Calibration_Coeff_UKV_Noise(callibration.band_idx[indx_old], callibration.n_ddc[indx_old]);

    return status_callibration;
}


