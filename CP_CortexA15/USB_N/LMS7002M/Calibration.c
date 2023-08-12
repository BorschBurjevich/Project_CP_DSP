/*
 *  ======= Calibration ========
 *
 *  Created on: 21 ‰ÂÍ. 2020 „.
 *  Author:     OLEG
 */
#include <xdc/runtime/System.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "LMS7002M_impl.h"
#include "USB_N/LMS7002M/LMS7002M.h"
#include "USB_N/LMS7002M/LMS7002M_cal.h"

/*8unsigned char RBB_1_4MHZ = 0;
unsigned char RBB_3_0MHZ = 1;
unsigned char RBB_5_0MHZ = 2;
unsigned char RBB_10_0MHZ = 3;
unsigned char RBB_15_0MHZ = 4;
unsigned char RBB_20_0MHZ = 5;*/
int Calibration_LowBand_RBB (LMS7002M_t *self, unsigned char ch);
void Algorithm_A_RBB (LMS7002M_t *self);
int Algorithm_F_RBB (LMS7002M_t *self, unsigned char Band_id, unsigned short LowFreqAmp);
int Algorithm_B_RBB (LMS7002M_t *self, unsigned short *LowFreqAmp);

/*
 * ======== main ========
 */
void Resistor_calibration_p (LMS7002M_t *self, float *ratio)
{
    unsigned short RP_CALIB_BIAS, RP_CALIB_BIAS_cal;
    unsigned short BestValue, ADCOUT;
    RP_CALIB_BIAS_cal = 16;
    RP_CALIB_BIAS = 0;
    //self->regs->reg_0x0084_rp_calib_bias = 0;
    //LMS7002M_regs_spi_write(self, 0x0084);//Modify_SPI_Reg_bits (0x0084, 10, 6, RP_CALIB_BIAS); // write RP_CALIB_BIAS value
    self->regs->reg_0x0084_mux_bias_out = 1;
    LMS7002M_regs_spi_write(self, 0x0084);//Modify_SPI_Reg_bits (0x0084, 12, 11, 1); // MUX_BIAS_OUT = 1
    self->regs->reg_0x040a_agc_mode = 1;
    LMS7002M_regs_spi_write(self, 0x040A);
    self->regs->reg_0x0400_capture = 1;
    LMS7002M_regs_spi_write(self, 0x0400);
    while (RP_CALIB_BIAS <= 31)
        {
            self->regs->reg_0x0084_rp_calib_bias = RP_CALIB_BIAS;
            LMS7002M_regs_spi_write(self, 0x0084);//Modify_SPI_Reg_bits (0x0084, 10, 6, RP_CALIB_BIAS); // write RP_CALIB_BIAS value

            self->regs->reg_0x0400_capture = 0;
            LMS7002M_regs_spi_write(self, 0x0400);
            self->regs->reg_0x0400_capture = 1;
            LMS7002M_regs_spi_write(self, 0x0400);

            ADCOUT =  LMS7002M_spi_read(self, 0x040E);  //ADCOUT = Get_SPI_Reg_bits(0x040B, 15, 0); //RSSI value
            ADCOUT =  LMS7002M_spi_read(self, 0x040F);


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
        }

    self->regs->reg_0x0084_rp_calib_bias = RP_CALIB_BIAS_cal;
    LMS7002M_regs_spi_write(self, 0x0084); //Modify_SPI_Reg_bits (0x0084, 10, 6, RP_CALIB_BIAS_cal); // set the control RP_CAL_BIAS to stored calibrated value
    *ratio = (float) 16/RP_CALIB_BIAS_cal; //calculate ratio
}

int Calibration_LowBand_RBB (LMS7002M_t *self, unsigned char ch)
{
    unsigned char result = 0;
    unsigned short LowFreqAmp;
    //Save_config_RBB (); //save current configuration
    //MIMO_Ctrl (ch);

    self->regs->reg_0x040a_agc_mode = REG_0X040A_AGC_MODE_RSSI;//Modify_SPI_Reg_bits (0x040A, 13, 12, 1); // AGC Mode = 1 (RSSI mode);
    Algorithm_A_RBB (self); // Aproximate resistor value for RBB RBANKS (Algorithm A)

    //Set_cal_path_RBB (7); // Set control signals to path 7 (RX LowBand)

    if (Algorithm_B_RBB (self, &LowFreqAmp) != 1) goto RESTORE; // Calibrate and Record the low frequency output amplitude (Algorithm B)

    Algorithm_F_RBB (self, RBB_1_4MHZ, LowFreqAmp);// CalibrateByCap the output cuttoff frequency at 0,7 MHz and store
    Algorithm_F_RBB (self, RBB_3_0MHZ, LowFreqAmp);// CalibrateByCap the output cuttoff frequency at 1,5 MHz MHz and store
    Algorithm_F_RBB (self, RBB_5_0MHZ, LowFreqAmp);// CalibrateByCap the output cuttoff frequency at 2,5 MHz MHz and store
    Algorithm_F_RBB (self, RBB_10_0MHZ, LowFreqAmp);// CalibrateByCap the output cuttoff frequency at 5 MHz MHz and store
    Algorithm_F_RBB (self, RBB_15_0MHZ, LowFreqAmp);// CalibrateByCap the output cuttoff frequency at 7,5 MHz and store
    Algorithm_F_RBB (self, RBB_20_0MHZ, LowFreqAmp);// CalibrateByCap the output cuttoff frequency at 10 MHz MHz and store

    result = 1;

    RESTORE:
    //Restore_config_RBB (); //restore configuration
    return result;
}
void Algorithm_A_RBB (LMS7002M_t *self)
{
    unsigned char R_CTL_LPF_RBB;
    float ratio;

    Resistor_calibration_p(self,&ratio);
    R_CTL_LPF_RBB = (unsigned char)(16 * ratio); // Default control value multiply by ratio

    self->regs->reg_0x0116_r_ctl_lpf_rbb = R_CTL_LPF_RBB;//Modify_SPI_Reg_bits (0x0116, 15, 11, R_CTL_LPF_RBB);

    //RBB_RBANK[MIMO_ch] = R_CTL_LPF_RBB; // Store RBANK Values (R_CTL_LPF_RBB)
}

int Algorithm_F_RBB (LMS7002M_t *self, unsigned char Band_id, unsigned short LowFreqAmp)
{
    unsigned short ADCOUT, CONTROL;
    unsigned char low_band;
    //Modify_SPI_Reg_bits (0x040A, 13, 12, 1); // AGC Mode = 1 (RSSI mode)
    if(Band_id <= RBB_20_0MHZ) //low band
    {
        low_band = 1; // CONTROL=C_CTL_LPFL_RBB
        CONTROL = 0xFF; // Set the CONTROL to maximum value. This should bring the output cutt-off frequency to minimum.
        self->regs->reg_0x0117_c_ctl_lpfl_rbb = CONTROL;// Modify_SPI_Reg_bits (0x0117, 10, 0, CONTROL); // write to C_CTL_LPFL_RBB
    }

    else //high band
    {
        low_band = 0; // CONTROL=C_CTL_LPFH_RBB
        CONTROL = 0x7FF; // Set the CONTROL to maximum value. This should bring the output cutt-off frequency to minimum.
        self->regs->reg_0x0116_c_ctl_lpfh_rbb = CONTROL;// Modify_SPI_Reg_bits (0x0116, 7, 0, CONTROL); // write to C_CTL_LPFH_RBB
    }

    //Set_NCO_Freq (RBB_CalFreq[Band_id]); // Apply a single tone frequency at ìCalFreqî.
    while (1)
    {
        ADCOUT =  LMS7002M_spi_read(self, 0x040b); //ADCOUT = Get_SPI_Reg_bits(0x040B, 15, 0); //RSSI value // Measure the value of the amplitude at the ADC input. This value should be lower than ìLowFreqAmpî.
        if (ADCOUT >= LowFreqAmp)
            break; //If it is lower than ìLowFreqAmpî repeat cycle
        if (CONTROL == 0)
            return 0;
        CONTROL--; // Decrease the CONTROL value by one.
        if (low_band) self->regs->reg_0x0117_c_ctl_lpfl_rbb = CONTROL;//if (low_band) Modify_SPI_Reg_bits (0x0117, 10, 0, CONTROL); // write to C_CTL_LPFL_RBB
        else self->regs->reg_0x0116_c_ctl_lpfh_rbb = CONTROL;         //else Modify_SPI_Reg_bits (0x0116, 7, 0, CONTROL); // write to C_CTL_LPFH_RBB
    }

        //RBB_CBANK[MIMO_ch][Band_id] = CONTROL; // Store CBANK Values
        //RBB_STATUS[MIMO_ch][Band_id] = 1;
        return 1;
    }

int Algorithm_B_RBB (LMS7002M_t *self, unsigned short *LowFreqAmp)
{
    unsigned short ADCOUT;
    unsigned char CG_IAMP_TBB, gain_inc;

    //Set_NCO_Freq (0.1); // Set DAC output to 100kHz (0.1MHz) single tone.
    CG_IAMP_TBB = 24; //set nominal CG_IAMP_TBB value

    self->regs->reg_0x0108_cg_iamp_tbb = CG_IAMP_TBB;//Modify_SPI_Reg_bits (0x0108, 15, 10, CG_IAMP_TBB); //write val to reg
    //Modify_SPI_Reg_bits (0x040A, 13, 12, 1); // AGC Mode = 1 (RSSI mode)

    ADCOUT =  LMS7002M_spi_read(self, 0x040b); //ADCOUT = Get_SPI_Reg_bits(0x040B, 15, 0); //RSSI value // Measure the output level at the ADC input
    if(ADCOUT < 52428) gain_inc = 1; //is it less then 80% of full scale swing (52428 (80% of 16 bits))
    else gain_inc = 0;

    while (1)
    {
        if(gain_inc) CG_IAMP_TBB++;
        else CG_IAMP_TBB--;
        self->regs->reg_0x0108_cg_iamp_tbb = CG_IAMP_TBB; //Modify_SPI_Reg_bits (0x0108, 15, 10, CG_IAMP_TBB); //write val to reg
        ADCOUT =  LMS7002M_spi_read(self, 0x040b);//ADCOUT = Get_SPI_Reg_bits(0x040B, 15, 0); //RSSI value // Measure the output level at the ADC input

        if (gain_inc)
        {
            if(ADCOUT >= 52428) break;
        }
        else
        {
            if(ADCOUT <= 52428) break;
        }
        if( (CG_IAMP_TBB == 0) || (CG_IAMP_TBB == 63)) //gain limit reached
        {
            //return 0;
            break;
        }
        }
        *LowFreqAmp = ADCOUT;
    return 1;
}
