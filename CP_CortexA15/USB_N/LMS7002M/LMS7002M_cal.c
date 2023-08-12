///
/// \file LMS7002M_cal.c
///
/// Calibration algorithms for the LMS7002M C driver.
///
/// \copyright
/// Copyright (c) 2015-2015 Fairwaves, Inc.
/// Copyright (c) 2015-2015 Rice University
/// SPDX-License-Identifier: Apache-2.0
/// http://www.apache.org/licenses/LICENSE-2.0
///

#include <stdlib.h>
#include "LMS7002M_cal.h"
#include "LMS7002M_impl.h"
#include "USB_N/LMS7002M/LMS7002M_logger.h"
void Modify_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit, unsigned short new_bits_data);
/***********************************************************************
 * Mimo control wrapper
 **********************************************************************/
void MIMO_Ctrl (LMS7002M_t *self, unsigned char ch)
{
    LMS7002M_set_mac_ch(self, (ch == 0)?LMS_CHA:LMS_CHB);
}

/***********************************************************************
 * NCO for test signal
 **********************************************************************/
void Set_NCO_Freq (LMS7002M_t *self, float Freq_MHz, float refClk_MHz, bool Rx)
{
    unsigned long fcw = (Freq_MHz/refClk_MHz)*(4294967296.0);//pow(2.0, 32);
    if(Rx)
    {
        Modify_SPI_Reg_bits (self, 0x0440, 4, 0, 0); //TX SEL[3:0] = 0 & MODE = 0
        Modify_SPI_Reg_bits (self, 0x0442, 15, 0, (fcw >> 16)); //FCW0[31:16]: NCO frequency control word register 0. MSB part.
        Modify_SPI_Reg_bits (self, 0x0443, 15, 0, fcw); //FCW0[15:0]: NCO frequency control word register 0. LSB part.
    }
    else
    {
        Modify_SPI_Reg_bits (self, 0x0240, 4, 0, 0); //TX SEL[3:0] = 0 & MODE = 0
        Modify_SPI_Reg_bits (self, 0x0242, 15, 0, (fcw >> 16)); //FCW0[31:16]: NCO frequency control word register 0. MSB part.
        Modify_SPI_Reg_bits (self, 0x0243, 15, 0, fcw); //FCW0[15:0]: NCO frequency control word register 0. LSB part.
    }
}

/***********************************************************************
 * spi access for cal algorithms
 **********************************************************************/
void Modify_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit, unsigned short new_bits_data)
{
    unsigned short mask, SPI_reg_data;
    unsigned char bits_number;

    bits_number = MSB_bit - LSB_bit + 1;

    mask = 0xFFFF;

    //removing unnecessary bits from mask
    mask = mask << (16 - bits_number);
    mask = mask >> (16 - bits_number);

    new_bits_data &= mask; //mask new data

    new_bits_data = new_bits_data << LSB_bit; //shift new data

    mask = mask << LSB_bit; //shift mask
    mask =~ mask;//invert mask

    SPI_reg_data = LMS7002M_spi_read(self, SPI_reg_addr); //read current SPI reg data

    SPI_reg_data &= mask;//clear bits
    SPI_reg_data |= new_bits_data; //set bits with new data
    LMS7002M_usb_write(SPI_reg_addr,SPI_reg_data);
 //  return LMS7002M_spi_write(self, SPI_reg_addr, SPI_reg_data); //write modified data back to SPI reg

    //for debug purposes to track regs touched by cal
    //modifiedRegs.insert(SPI_reg_addr);

    //apply new value to the local register struct
   // LMS7002M_regs_set(&self->regs, SPI_reg_addr, SPI_reg_data);
}

unsigned short Get_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit)
{
    unsigned short bits, mask;
    unsigned char bits_number;

    bits_number = MSB_bit - LSB_bit + 1;

    mask = 0xFFFF;

    //removing unnecessary bits from mask
    mask = mask << (16 - bits_number);
    mask = mask >> (16 - bits_number);

    bits = LMS7002M_spi_read(self, SPI_reg_addr); //read current data

    bits = bits >> LSB_bit; //shift bits to LSB

    bits &= mask; //mask bits

    return bits;
}

/***********************************************************************
 * resistor calibration
 **********************************************************************/
float Resistor_calibration (LMS7002M_t *self)
{
    LMS7_log(LMS7_INFO, "Resistor calibration started");
    unsigned char RP_CALIB_BIAS, RP_CALIB_BIAS_cal;
    unsigned short BestValue, ADCOUT;
    float ratio;

    RP_CALIB_BIAS_cal = 16;
    RP_CALIB_BIAS = 0;
    Modify_SPI_Reg_bits (self, 0x0084, 10, 6, RP_CALIB_BIAS); // write RP_CALIB_BIAS value
    Modify_SPI_Reg_bits (self, 0x0084, 12, 11, 1); // MUX_BIAS_OUT = 1
    Modify_SPI_Reg_bits (self, 0x040A, 13, 12, 1); // AGC Mode = 1 (RSSI mode)
    Modify_SPI_Reg_bits(self, 0x0400, 15, 15, 0); //Capture 0
    Modify_SPI_Reg_bits(self, 0x0400, 14, 13, 0); //Capsel 0 - RSSI

    while (RP_CALIB_BIAS <= 31)
    {
        Modify_SPI_Reg_bits(self, 0x0400, 15, 15, 0); //Capture 0
        Modify_SPI_Reg_bits(self, 0x0400, 15, 15, 1); //Capture 1
        ADCOUT = (Get_SPI_Reg_bits(self, 0x040F, 15, 0) << 2) | Get_SPI_Reg_bits(self, 0x040E, 1, 0); //RSSI value

        //REG: debug
        unsigned long capd_lo = LMS7002M_spi_read(self, 0x40E);
        unsigned long capd_hi = LMS7002M_spi_read(self, 0x40F);
        LMS7_logf(LMS7_DEBUG, "%s: RP_CALIB_BIAS = %d, ADCOUT, = %d, CAPD = %X.%X", __FUNCTION__, RP_CALIB_BIAS, ADCOUT, capd_hi, capd_lo);

        if(RP_CALIB_BIAS == 0)
        {
            BestValue = ADCOUT;
        }

        if ( ADCOUT < BestValue )
        {
            BestValue = ADCOUT;
            RP_CALIB_BIAS_cal = RP_CALIB_BIAS; //store calibrated value
        }

        RP_CALIB_BIAS++;
        Modify_SPI_Reg_bits (self, 0x0084, 10, 6, RP_CALIB_BIAS); // write RP_CALIB_BIAS value
    }
    Modify_SPI_Reg_bits(self, 0x0400, 15, 15, 0); //Capture 0
    Modify_SPI_Reg_bits (self, 0x0084, 10, 6, RP_CALIB_BIAS_cal); // set the control RP_CAL_BIAS to stored calibrated value
    ratio = 16.0/RP_CALIB_BIAS_cal; //calculate ratio
    LMS7_logf(LMS7_INFO, "%s: Resistor calibration finished = %f", __FUNCTION__, ratio);
    return ratio;
}

unsigned char VCO_CoarseTuning_SXT_SXR (LMS7002M_t *self, float Fref_MHz, float Fvco_des_MHz, unsigned char ch)
{
    unsigned short Nround;
    unsigned char i, try_cnt, MAX_TRY_CNT = 100;

//    MIMO_Ctrl(ch);//SXT SXR selection

    // Initialization
    Modify_SPI_Reg_bits (self, 0x011C, 12, 12, 1); // 1) EN_COARSEPLL=1, a. VCO control voltage is switched to a DC =VDD/2
    Modify_SPI_Reg_bits (self, 0x0121, 0, 0, 0); // 2) COARSE_START=0
    Modify_SPI_Reg_bits (self, 0x011C, 9, 9, 1); // 3) EN_INTONLY_SDM=1
    Modify_SPI_Reg_bits (self, 0x011C, 14, 14, 1); // 4) SHORT_NOISEFIL=1 SPDUP_VCO_ Short the noise filter resistor to speed up the settling time
    Nround = (unsigned short)(4*Fvco_des_MHz/Fref_MHz+0.5); // 5) Nround=round(4*Fvco_des/Fref)
    Modify_SPI_Reg_bits (self, 0x011D, 15, 0, 0); // 6) FRAC_SDM=0
    Modify_SPI_Reg_bits (self, 0x011E, 3, 0, 0);
    Modify_SPI_Reg_bits (self, 0x011E, 13, 4, (Nround-4)); // 7) INT_SDM=Nround-4
    Modify_SPI_Reg_bits (self, 0x0121, 10, 3, 0); // 9) Set SWC_VCO<7:0>=<00000000>
    i=7;// 10) i=7
    //Decision - Loop Section
    while(1)
    {
        Modify_SPI_Reg_bits (self, 0x0121, 3 + i, 3 + i, 1); // SWC_VCO<i>=1
        Modify_SPI_Reg_bits (self, 0x0121, 0, 0, 1); // COARSE_START=1
        while (Get_SPI_Reg_bits(self, 0x0123, 15, 15) != 1) //wait till COARSE_STEPDONE=1
        {
            try_cnt++;
            if(try_cnt > MAX_TRY_CNT) return 0;
        }
        if (Get_SPI_Reg_bits(self, 0x0123, 14, 14) == 1) //check CAORSEPLL_COMPO
        {
            Modify_SPI_Reg_bits (self, 0x0121, 3 + i, 3 + i, 0); // SWC_VCO<i>=0
        }
        Modify_SPI_Reg_bits (self, 0x0121, 0, 0, 0); // COARSE_START=0
        if(i==0) break;
        i--;
    }
    //Normal mode setting
    Modify_SPI_Reg_bits (self, 0x011C, 12, 12, 0); // EN_COARSEPLL=0
    Modify_SPI_Reg_bits (self, 0x011C, 9, 9, 0); // EN_INTONLY_SDM=0
    Modify_SPI_Reg_bits (self, 0x011C, 14, 14, 0); // SHORT_NOISEFIL=0 SPDUP_VCO_ Short the noise filter resistor to speed up the settling time
    Modify_SPI_Reg_bits (self, 0x011C, 1, 1, 0); // включение VCO
    return 1;
}

unsigned char VCO_CoarseTuning_CGEN (LMS7002M_t *self, float Fref_MHz, float Fvco_des_MHz)
{
    unsigned short Nround;
    unsigned char i, try_cnt, MAX_TRY_CNT = 100;

    // Initialization
    Modify_SPI_Reg_bits (self, 0x0086, 10, 10, 1); // 1) EN_COARSE_CKLGEN=1, a. VCO control voltage is switched to a DC =VDD/2
    Modify_SPI_Reg_bits (self, 0x008B, 0, 0, 0); // 2) COARSE_START_CGEN=0
    Modify_SPI_Reg_bits (self, 0x0086, 9, 9, 1); // 3) EN_INTONLY_SDM_CGEN=1
    Modify_SPI_Reg_bits (self, 0x0086, 15, 15, 1); // 4) SHORT_NOISEFIL=1 SPDUP_VCO_CGEN Short the noise filter resistor to speed up the settling time
    Nround = (unsigned short)(Fvco_des_MHz/Fref_MHz); // 5) Nround=round(4*Fvco_des/Fref)
    Modify_SPI_Reg_bits (self, 0x0087, 15, 0, 0); // 6) FRAC_SDM_CGEN=0
    Modify_SPI_Reg_bits (self, 0x0088, 3, 0, 0);
    Modify_SPI_Reg_bits (self, 0x0088, 13, 4, (Nround)); // 7) INT_SDM_CGEN =Nround-4
    Modify_SPI_Reg_bits (self, 0x008B, 8, 1, 0); // 9) Set CSW_VCO_CGEN<7:0>=<00000000>
    i=7;// 10) i=7
    // Loop Section
    while(1)
    {
        Modify_SPI_Reg_bits (self, 0x008B, 1 + i, 1 + i, 1); // CSW_VCO_CGEN<i>=1
        Modify_SPI_Reg_bits (self, 0x008B, 0, 0, 1); // COARSE_START_CGEN=1
        while ( Get_SPI_Reg_bits(self, 0x008C, 15, 15) != 1 ) //wait till COARSE_STEPDONE_CGEN=1
        {
            try_cnt++;
            if(try_cnt > MAX_TRY_CNT) return 0;
        }
        if ( Get_SPI_Reg_bits(self, 0x008C, 14, 14) == 1) //check COARSEPLL_COMPO_CGEN
        {
            Modify_SPI_Reg_bits (self, 0x008B, 1 + i, 1 + i, 0); // SWC_VCO<i>=0
        }

        Modify_SPI_Reg_bits (self, 0x008B, 0, 0, 0); // 2) COARSE_START_CGEN=0
        if(i==0) break;
        i--;
    }
    Modify_SPI_Reg_bits (self, 0x0086, 10, 10, 0); // 1) EN_COARSE_CKLGEN=0
    Modify_SPI_Reg_bits (self, 0x0086, 9, 9, 0); // 3) EN_INTONLY_SDM_CGEN=0
    Modify_SPI_Reg_bits (self, 0x0086, 15, 15, 0); // 4) SHORT_NOISEFIL=0 SPDUP_VCO_CGEN Short the noise filter resistor to speed up the settling time
    Modify_SPI_Reg_bits (self, 0x0086, 2, 2, 0);
    return 1;
}
