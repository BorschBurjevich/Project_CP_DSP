/*
 *  ======= Set_rate ========
 *
 *  Created on: 19 но€б. 2021 г.
 *  Author:     OlegO
 */
#include <xdc/runtime/System.h>

/*
 * ======== SetRate ========
 */
//int LMS7_Device::SetRate(double f_Hz, int oversample)
//{
//    double nco_f=0;
//    for (unsigned i = 0; i < GetNumChannels(false);i++)
//    {
//         if (rx_channels[i].cF_offset_nco > nco_f)
//             nco_f = rx_channels[i].cF_offset_nco;
//         if (tx_channels[i].cF_offset_nco > nco_f)
//             nco_f = tx_channels[i].cF_offset_nco;
//         tx_channels[i].sample_rate = f_Hz;
//         rx_channels[i].sample_rate = f_Hz;
//    }
//
//    if (oversample == 0)
//    {
//        const int n = lime::cgenMax/(4*f_Hz);
//        oversample = (n >= 32) ? 32 : (n >= 16) ? 16 : (n >= 8) ? 8 : (n >= 4) ? 4 : 2;
//    }
//
//    if (nco_f != 0)
//    {
//        int nco_over = 2+2*(nco_f-1)/f_Hz;
//        if (nco_over > 32)
//        {
//            lime::error("Cannot achieve desired sample rate: rate too low");
//            return -1;
//        }
//        oversample = oversample > nco_over ? oversample : nco_over;
//    }
//
//    int decim = 4;
//    if (oversample <= 16)
//    {
//        const int decTbl[] = {0, 0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3};
//        decim = decTbl[oversample];
//    }
//
//    oversample = 2<<decim;
//
//    for (unsigned i = 0; i < lms_list.size(); i++)
//    {
//         lime::LMS7002M* lms = lms_list[i];
//        if ((lms->SetFrequencyCGEN(f_Hz*4*oversample) != 0)
//            || (lms->Modify_SPI_Reg_bits(LMS7param(EN_ADCCLKH_CLKGN), 0) != 0)
//            || (lms->Modify_SPI_Reg_bits(LMS7param(CLKH_OV_CLKL_CGEN), 2) != 0)
//            || (lms->Modify_SPI_Reg_bits(LMS7param(MAC), 2) != 0)
//            || (lms->Modify_SPI_Reg_bits(LMS7param(HBD_OVR_RXTSP), decim) != 0)
//            || (lms->Modify_SPI_Reg_bits(LMS7param(HBI_OVR_TXTSP), decim) != 0)
//            || (lms->Modify_SPI_Reg_bits(LMS7param(MAC), 1) != 0)
//            || (lms->SetInterfaceFrequency(lms->GetFrequencyCGEN(), decim, decim) != 0))
//            return -1;
//         lms_chip_id = i;
//         if (SetFPGAInterfaceFreq(decim, decim)!=0)
//             return -1;
//    }
//
//    for (unsigned i = 0; i < GetNumChannels();i++)
//    {
//        if (rx_channels[i].cF_offset_nco != 0)
//           SetNCOFreq(false, i, 0, rx_channels[i].cF_offset_nco);
//
//        if (tx_channels[i].cF_offset_nco != 0)
//           SetNCOFreq(true, i, 0, -tx_channels[i].cF_offset_nco);
//        auto gfir_bw = tx_channels[i].gfir_bw;
//        if (gfir_bw > 0)
//            ConfigureGFIR(true, i, true, gfir_bw);
//        gfir_bw = rx_channels[i].gfir_bw;
//        if (gfir_bw > 0)
//            ConfigureGFIR(false, i, true, gfir_bw);
//    }
//
//    return 0;
//}
