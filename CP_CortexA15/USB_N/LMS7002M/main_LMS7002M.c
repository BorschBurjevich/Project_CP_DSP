/********************************************************
 *  \file   main.c***
 **********************************/

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include "delay/delay.h"

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Seconds.h>
#include <time.h>


/* Board Header files */
#include <ti/board/board.h>
#include <ti/board/src/flash/include/board_flash.h>

#include "USB_N/LMS7002M/LMS7002M.h"
#include "USB_N/LMS7002M/LMS7002M_logger.h"
#include "USB_N/LMS7002M/LMS7002M_impl.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <math.h>

#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/timers/timer64/Timer.h>
#include <ti/ndk/inc/socket.h>
#include <ti/ndk/inc/netmain.h>

#include <ti/csl/csl_chip.h>
#include <ti/sysbios/family/arm/a15/Mmu.h>
#include "ti/board/board.h"


#include "I2C/I2C.h"
#include <ti/drv/i2c/soc/I2C_soc.h>
#include <ti/csl/csl_clec.h>
#include <ti/csl/arch/r5/csl_arm_r5.h>

#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/Types.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>

#include <ti/sysbios/hal/Hwi.h>
#include <xdc/runtime/Error.h>
//////////////////////////////////////////////
#include "ti/drv/usb/example/common/hardware.h"
#include <ti/fs/fatfs/diskio.h>
#include <ti/fs/fatfs/FATFS.h>
#include "timer.h"
#include "types.h"
#include <fatfs_port_usbmsc.h>
#include <fs_shell_app_utils.h>
#include <ti/drv/usb/usb_drv.h>
#include <ti/drv/usb/usb_osal.h>
#include <usblib.h>
#include <usbhost.h>
#include "usbhmsc.h"
#include <ti/drv/uart/UART_stdio.h>
#include "C:\ti\pdk_am57xx_1_0_19\packages\ti\drv\usb\src\xhci\usb_hcd_xhci.h"
//////////////////////////////////////////////
#include "USB_N/LMS7002M/Common_inf_LMS/Com_USB_Func.h"
#include "Lorandit_functions/Cod_com.h"
#include "Zdnglobdef.h"
//////////////////////////////////////////////
/*****************************************************************************
*
* The instance data for the MSC driver.
*
*****************************************************************************/

/**********************************************************************
 ************************** Macros ************************************
 **********************************************************************/

#define REF_FREQ  (50.000e6) //  20.000e6 - Олег

/**********************************************************************
 ************************** Internal functions ************************
 **********************************************************************/
void LMS_7002m_set();


/**********************************************************************
 ************************** Ixternal functions ************************
 **********************************************************************/

extern int AM_Demod(uint16_t dataforDAC);
extern int FM_Demod(uint32_t dataforDAC);

void LMS7002M_cal_rbb(LMS7002M_t *self, const LMS7002M_chan_t ch);
extern float GetFrequencySX(LMS7002M_t *self, bool tx);
extern float GetFrequencyCGEN(LMS7002M_t *self);
extern float GetReferenceClk_TSP_MHz(LMS7002M_t *self, bool tx);
extern int SetGFIRCoefficients(LMS7002M_t *self, bool tx, uint8_t GFIR_index, const int16_t *coef, uint8_t coefCount);
extern int SetInterfaceFrequency(LMS7002M_t *self, float cgen_freq_Hz, const uint8_t interpolation, const uint8_t decimation);
extern uint8_t TuneRxFilter(LMS7002M_t *self,const float rx_lpf_freq_RF);
extern void ConfigureGFIR(LMS7002M_t *self);
extern void MCU(LMS7002M_t *self);
uint32_t GetRSSI(LMS7002M_t *self);
extern void run_calib_procedure(LMS7002M_t *self, int calib);
extern void update_BW_or_clock(LMS7002M_t *self, uint8_t kHz_LSB, uint8_t kHz_MSB, uint8_t MHz);
extern void test_read_write_MPU();
extern void Modify_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit, unsigned short new_bits_data);
extern double GetRate (LMS7002M_t *self, bool tx, double rf_rate_Hz);

void  set_Jesd207_Mode(LMS7002M_t *self);
void  read_data_from_Jesd(uint32_t num_words);

/**********************************************************************
 ************************** Global Variables **************************
 **********************************************************************/

const int position[4] = {0, 1, 1, 0};
#define flag                 (*((uint16_t *) 0x0C000104))

//int Mas_Global[1600];
int Global_counter = 0;
/*
 *  ======== Board_LMS7002_driver ========
 */
void LMS_7002m_set()
{

    LMS7002mHandle  LMSHandle;


    double actualRate = 0.0;
    float printf_rate = 0.0;
    int ret = 0;
    double sampleRate = 0.0;
    uint32_t get_data = 0;
    uint32_t Jesd_Length = 25; // колич-во принимаемых пар (I,Q)

    test_read_write_MPU();
    printf("Running LMS_7002m\n");
    LMS7002M_t *lms = LMS7002M_create(LMSHandle); /*create and test lms....*/

    get_data = LMS7002M_regs_spi_read(lms, 0x002F);//Чтение ревизии LMS


    printf("rev 0x%x\n", LMS7002M_regs(lms)->reg_0x002f_rev);
    printf("ver 0x%x\n", LMS7002M_regs(lms)->reg_0x002f_ver);

    LMS7002M_power_on(lms);//Включение всех модулей LMS
    LMS7002M_set_diq_mux(lms, LMS_RX, &position[0] );//Установка порядка чтения Re Im


    ////////////////////////черновой вариант настройки Jesd207 mode /////////////////////
    set_Jesd207_Mode(lms);

    /////////////////////////////////////////////////////////////////////////////////////

    LMS7002M_afe_enable(lms, LMS_RX, LMS_CHA, true);
    LMS7002M_afe_enable(lms, LMS_RX, LMS_CHB, false);
    LMS7002M_rxtsp_enable(lms, LMS_CHA, true);
    LMS7002M_rxtsp_enable(lms, LMS_CHB, false);
    LMS7002M_rbb_enable(lms, LMS_CHA, true);
    LMS7002M_rbb_enable(lms, LMS_CHB, false);
    LMS7002M_rfe_enable(lms, LMS_CHA, true);
    LMS7002M_rfe_enable(lms, LMS_CHB, false);
    LMS7002M_sxx_enable(lms, LMS_RX, true);

    LMS7002M_usb_write(0x0084,0x0400);
    LMS7002M_usb_write(0x0085,0x0001);

    ret = LMS7002M_set_data_clock(lms, REF_FREQ, 16e6 /*20e6*/, &actualRate);

    if (ret != 0)
    {
        printf("clock tune failure %d\n", ret);
    }
    printf("%d - Actual clock freq %f MHz\n", ret, actualRate/1e6);

    LMS7002M_rfe_set_lna(lms, LMS_CHA, 40.0);//Установка малошумящего усилителя
    LMS7002M_rfe_set_tia(lms, LMS_CHA, 10.0);//Установка трансимпедансного усилителя
    LMS7002M_rbb_set_pga(lms, LMS_CHA, 5.0);//Установка усилителя с программируемым коэффициентом усиления

    float Clk_TSP = GetReferenceClk_TSP_MHz(lms, 0);
    printf("%d - Actual Clk_TSP freq %f MHz\n", ret, Clk_TSP/1e6);

    sampleRate = GetRate(lms, 0, REF_FREQ);
    printf("%d - sampleRate %f MHz\n", ret, sampleRate/1e6);

    ret = LMS7002M_set_lo_freq(lms, LMS_RX, REF_FREQ, 105.7e6, &actualRate);// Настройка приемника LMS на частоту
    printf("%d - Actual RX LO freq %f MHz\n", ret, actualRate/1e6);

    actualRate = GetFrequencySX(lms, 0);
    printf("%d - Actual RX LO freq %f MHz\n", ret, actualRate/1e6);

    //LMS7002M_usb_write(0x040C, 0x60A1);///////enable def 0x00fb  0x00C3 // управление фильтрами и тд
   // LMS7002M_usb_write(0x0403, 0x4000); // IQ_CORR и HBD_OVR
   // ConfigureGFIR(lms);
  //  Modify_SPI_Reg_bits(lms, 0x00AE, 1, 0, 1); //3:0 // задержки в TXTSP и RXTSP
  //  Modify_SPI_Reg_bits(lms, 0x00AD, 15, 14, 1);      // пока закомментил


    SetInterfaceFrequency(lms, GetFrequencyCGEN(lms), 7, 4);
    LMS7002M_rfe_set_path(lms, LMS_CHA, LMS7002M_RFE_LNAW);//Включение входа приемника LMS
    LMS7002M_usb_write(0x0200, 0x0081);
    LMS7002M_usb_write(0x0400, 0x0081);
    read_data_from_Jesd(Jesd_Length);
}

void  set_Jesd207_Mode(LMS7002M_t *self)
{

    //LMS7002M_set_diq_mux(self, LMS_RX, &position[0] );//Установка порядка чтения Re Im   (подумать)
    LMS7002M_invert_mclk(self, false);  ///   Not inverted
    LMS7002M_configure_lml_port_TRXIQ(self, LMS_PORT2, LMS_RX, 2);
   // LMS7002M_configure_lml_port(self, LMS_PORT2, LMS_RX, 2);
   // my_LMS7002M_configure_lml_port(self, LMS_PORT2, LMS_RX,2);

    ;;
}

// num_words - количество пар (16 разрядных слов) по текущему протоколу
void read_data_from_Jesd(uint32_t num_words)
{

    uint32_t TOKEN = 2;
    uint32_t counter;
    uint32_t byte_shift = 0;
    uint32_t points;
    uint32_t tmp2;
    uint32_t shift = 0;
    short    rx_jesd[100];
    short tmp = 0;

    memset(&rx_jesd[0],0x0,sizeof(&rx_jesd[0])*50);
    points = num_words;
    num_words = (num_words * 4) + TOKEN;   // количество байт на чтение
    Buf_Write_USB[0] = 0x95;
    Buf_Write_USB[1] = points;                  // количество I,Q

    tmp2 = points * 2;
   while(1)
   {


        USBHostWrite(USB_INSTANCE, 0, &Buf_Write_USB[0],2);     // запрос на чтение
        // memset(&Buf_Read_USB[0],0xBA,sizeof(&Buf_Read_USB[0])*256);
        USBHostRead(USB_INSTANCE, 0, &Buf_Read_USB[0],num_words);
        USBHostRead(USB_INSTANCE, 0, &Buf_Read_USB[0],num_words);
        USBHostRead(USB_INSTANCE, 0, &Buf_Read_USB[0],num_words);

        // Buf_Read_USB[2] = 0xF7;
        // Buf_Read_USB[3] = 0x08;
        shift = 0;
        byte_shift = 0;
        for (counter = 0; counter < tmp2; counter++)
        {
            tmp = Buf_Read_USB[2 + counter + byte_shift];
            byte_shift++;
            tmp |= (Buf_Read_USB[2 + counter + byte_shift] &0xff) << 8;

            if ((tmp & 0x8000) == 0x8000) // Q составляющая
            {
                if ((tmp & 0x800)==0x800)
                {
                    tmp |= 0xF000;
                }
                rx_jesd[shift] = tmp;
                shift++;
               // txDSPBuf5[counter + 1] = (float)tmp;
            }
            else                        // I составляющая
            {
                if ((tmp & 0x800)==0x800)
                {
                    tmp |= 0xF000;
                }
                rx_jesd[shift] = tmp;
                //memcpy(&txDSPBuf5[shift],&tmp,2);
                shift++;
                //txDSPBuf5[counter] = (float)tmp;
            }

        }
        memcpy(&txDSPBuf5[0],&rx_jesd[0],sizeof(&rx_jesd[0])*tmp2);
   }
    sh_mem_send_time_seq(SPECTRUM_POWER_DSP0,(uint32_t *)&txDSPBuf5[0],(uint32_t *)&txDSPBuf5[0], 256);
    sh_mem_recieve(DSP0_);
}
