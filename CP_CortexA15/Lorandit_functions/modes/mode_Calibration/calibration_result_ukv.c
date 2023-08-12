#include "Lorandit_functions/modes/modes.h"
#include "AD9208_spi/AD9208.h"
#include "AD9208_spi/ad9208_reg.h"
#include "Lorandit_functions/modes/mode_Calibration/Calibration.h"
#include "Lorandit_functions/reciever/reciever.h"
#include "Lorandit_functions/Structs.h"

void calibration_result_ukv(uint32_t ind, uint16_t old_freq_range)
{
    uint32_t k = 0;
    uint32_t j = 0;
    uint8_t num_calib_freq; // количество откалиброванных частот
    uint32_t indx_old;

    /// проверка условий  ////

    if (ind < callibration.num_bands && ind > 0)
    {
       indx_old = ind - 1;

    }

    else if ((int32_t)ind - 1 < 0)
    {
       indx_old = callibration.num_bands - 1;

    }

    k = sizeof(hat.n_bytes);

    hat.command = start_calibration_ukv_;
    memcpy(&txEthBuf[k], &hat.command, sizeof(hat.command));
    k += sizeof(hat.command);

    memcpy(&txEthBuf[k], &callibration.band_idx[indx_old], sizeof(callibration.band_idx[indx_old]));
    k += sizeof(callibration.band_idx[indx_old]);

    callibration.overflow[indx_old] = 0x0;
    memcpy(&txEthBuf[k], &callibration.overflow[indx_old], sizeof(callibration.overflow[indx_old]));
    k += sizeof(callibration.overflow[indx_old]);

    memcpy(&txEthBuf[k], &callibration.att_prm[old_freq_range], sizeof(callibration.att_prm[old_freq_range]));
    k += sizeof(callibration.att_prm[old_freq_range]);

    memcpy(&txEthBuf[k], &callibration.att_bk[0], sizeof(callibration.att_bk[0]));
    k += sizeof(callibration.att_bk[0]);

    num_calib_freq = callibration.n_ddc[indx_old]*21;

    memcpy(&txEthBuf[k], &num_calib_freq, sizeof(num_calib_freq));
    k += sizeof(num_calib_freq);

    for (j = 0; j < num_calib_freq; j++)
    {

        memcpy(&txEthBuf[k], &callibration.calibr_coeff[indx_old].bin_freq[j], sizeof(callibration.calibr_coeff[indx_old].bin_freq[j]));
        k += sizeof(callibration.calibr_coeff[indx_old].bin_freq[j]);

        memcpy(&txEthBuf[k], &callibration.calibr_coeff[indx_old].error[j], sizeof(callibration.calibr_coeff[indx_old].error[j]));
        k += sizeof(callibration.calibr_coeff[indx_old].error[j]);

        memcpy(&txEthBuf[k], &callibration.calibr_coeff[indx_old].Re_K0[j], sizeof(callibration.calibr_coeff[indx_old].Re_K0[j]));
        k += sizeof(callibration.calibr_coeff[indx_old].Re_K0[j]);

        memcpy(&txEthBuf[k], &callibration.calibr_coeff[indx_old].Im_K0[j], sizeof(callibration.calibr_coeff[indx_old].Im_K0[j]));
        k += sizeof(callibration.calibr_coeff[indx_old].Im_K0[j]);

        memcpy(&txEthBuf[k], &callibration.calibr_coeff[indx_old].Re_K1[j], sizeof(callibration.calibr_coeff[indx_old].Re_K1[j]));
        k += sizeof(callibration.calibr_coeff[indx_old].Re_K1[j]);

        memcpy(&txEthBuf[k], &callibration.calibr_coeff[indx_old].Im_K1[j], sizeof(callibration.calibr_coeff[indx_old].Im_K1[j]));
        k += sizeof(callibration.calibr_coeff[indx_old].Im_K1[j]);

        memcpy(&txEthBuf[k], &callibration.calibr_coeff[indx_old].Re_K2[j], sizeof(callibration.calibr_coeff[indx_old].Re_K2[j]));
        k += sizeof(callibration.calibr_coeff[indx_old].Re_K2[j]);

        memcpy(&txEthBuf[k], &callibration.calibr_coeff[indx_old].Im_K2[j], sizeof(callibration.calibr_coeff[indx_old].Im_K2[j]));
        k += sizeof(callibration.calibr_coeff[indx_old].Im_K2[j]);

        memcpy(&txEthBuf[k], &callibration.calibr_coeff[indx_old].Re_K3[j], sizeof(callibration.calibr_coeff[indx_old].Re_K3[j]));
        k += sizeof(callibration.calibr_coeff[indx_old].Re_K3[j]);

        memcpy(&txEthBuf[k], &callibration.calibr_coeff[indx_old].Im_K3[j], sizeof(callibration.calibr_coeff[indx_old].Im_K3[j]));
        k += sizeof(callibration.calibr_coeff[indx_old].Im_K3[j]);

        memcpy(&txEthBuf[k], &callibration.calibr_coeff[indx_old].Re_K4[j], sizeof(callibration.calibr_coeff[indx_old].Re_K4[j]));
        k += sizeof(callibration.calibr_coeff[indx_old].Re_K4[j]);

        memcpy(&txEthBuf[k], &callibration.calibr_coeff[indx_old].Im_K4[j], sizeof(callibration.calibr_coeff[indx_old].Im_K4[j]));
        k += sizeof(callibration.calibr_coeff[indx_old].Im_K4[j]);

    }

    hat.n_bytes = k - sizeof(hat.n_bytes);
    memcpy(&txEthBuf[0], &hat.n_bytes, sizeof(hat.n_bytes));
    send_eth(arm, &txEthBuf[0], k);

}
