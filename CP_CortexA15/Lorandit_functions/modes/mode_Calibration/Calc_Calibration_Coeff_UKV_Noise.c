#include"Lorandit_functions/modes/mode_Calibration/Calibration.h"
#include"Lorandit_functions/Structs.h"
#include"Lorandit_functions/Cod_com.h"


/*
 * num_band номер полосы
 */

void Calc_Calibration_Coeff_UKV_Noise(uint32_t num_band, uint32_t k_max)
{
    uint32_t k;

    for(k = 0; k < k_max ; k++)
    {
        callibration.calibr_coeff[num_band].Re_K0[k] = (rsw0[k]*rsw0[k])  / (rsw0[k]*rsw0[k] + isw0[k]*isw0[k]);
        callibration.calibr_coeff[num_band].Im_K0[k] = (-rsw0[k]*isw0[k]) / (rsw0[k]*rsw0[k] + isw0[k]*isw0[k]);

        callibration.calibr_coeff[num_band].Re_K1[k] = (rsw0[k]*rsw1[k])  / (rsw1[k]*rsw1[k] + isw1[k]*isw1[k]);
        callibration.calibr_coeff[num_band].Im_K1[k] = (-rsw0[k]*isw1[k]) / (rsw1[k]*rsw1[k] + isw1[k]*isw1[k]);

        callibration.calibr_coeff[num_band].Re_K2[k] = (rsw0[k]*rsw2[k])  / (rsw2[k]*rsw2[k] + isw2[k]*isw2[k]);
        callibration.calibr_coeff[num_band].Im_K2[k] = (-rsw0[k]*isw2[k]) / (rsw2[k]*rsw2[k] + isw2[k]*isw2[k]);

        callibration.calibr_coeff[num_band].Re_K3[k] = (rsw0[k]*rsw3[k])  / (rsw3[k]*rsw3[k] + isw3[k]*isw3[k]);
        callibration.calibr_coeff[num_band].Im_K3[k] = (-rsw0[k]*isw3[k]) / (rsw3[k]*rsw3[k] + isw3[k]*isw3[k]);

        callibration.calibr_coeff[num_band].Re_K4[k] = (rsw0[k]*rsw4[k])  / (rsw4[k]*rsw4[k] + isw4[k]*isw4[k]);
        callibration.calibr_coeff[num_band].Im_K4[k] = (-rsw0[k]*isw4[k]) / (rsw4[k]*rsw4[k] + isw4[k]*isw4[k]);


    }

}
