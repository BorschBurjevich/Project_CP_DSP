#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <xdc/std.h>
#include <ti/board/board.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/Timestamp.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/family/arm/a15/Mmu.h>
#include <ti/sysbios/family/arm/a15/Cache.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/osal/osal.h>

#include <ti/sysbios/knl/Clock.h>
#include <ti/drv/spi/SPI.h>
#include <ti/drv/spi/soc/SPI_v1.h>
#include <ti/drv/spi/test/src/SPI_log.h>
#include <ti/drv/spi/test/src/SPI_test.h>
#include <ti/drv/spi/soc/SPI_soc.h>
#include <ti/drv/spi/src/SPI_osal.h>
#include "gpio/gpio.h"
#include "Func_Loopback.h"
#include "delay/delay.h"
#include "SPI/SPI_header.h"
#include "Lorandit_functions/Cod_com.h"
#include "Lorandit_functions/reciever/reciever.h"
#include "I2C/I2C.h"
#include "gpio/gpio.h"
#include "uart/uart.h"
#include <ti/drv/i2c/soc/I2C_soc.h>
#include "Lorandit_functions/modes/modes.h"
#include "Lorandit_functions/modes/mode_Calibration/Calibration.h"
#include "delay/delay.h"
#include "Shared_mem_communication/Shared.h"
#include "AD9208_spi/AD9208.h"
#include "Lorandit_functions/Structs.h"
#include "Lorandit_functions/GPS_KOMPAS/GPS_Kompas.h"
#include "Zdnglobdef.h"

/////////////////////////////////////////////////////
#include "Lorandit_functions/Supression/targets_fch.h"

#define VIP_CFG_SC21_slice0 (*((unsigned int*)0x48975854))
#define VIP_CFG_SC21_slice1 (*((unsigned int*)0x48975d54))

extern fch_targets_           fch;
extern total_signals_         total[MAX_FREQS];
extern pp_                    pp[MAX_PP];
extern uint8_t count_Start_data;
extern uint8_t data_flow;
/////////////////////////////////////////////////////
extern LpbkApp_Obj           *appObj;
extern LpbkApp_TestParams    *testPrms;
extern BspOsal_SemHandle      overflowSem;
extern LpbkApp_TestParams     gLpbkAppTestPrms;
extern Semaphore_Handle       semen;
extern void send_eth_ticket(SOCKET s, uint16_t com);
extern void pcie_init();
extern void Set_LMK04832(SPI_Handle spi4lmk);
void   get_data_vip(void);
void   choose_channel_ADC(int flag_rebuild);
static volatile UInt32        gExitApp;
uint8_t                       vip_flag;
uint8_t                       sysref_flag = 0;
uint8_t                       ftw_send_flag = 0;
bool                          ftw_freq_sel = 1;
uint8_t                       Label_Control_First;
int flag_option;
//////////////////////////////////////
SPI_Handle LMK04832_SPI_4_0;
UART_Handle  KOMPAS =  NULL;
UART_Handle  GPS = NULL;
UART_Handle  GLON = NULL;
extern uint8_t first_strt_ADC;

//////////////////////

void LpbkApp_tskMain(UArg arg0, UArg arg1)
{
    uint32_t i,spi_bitrate;
    uint8_t  size_frame;      // bites
    uint16_t freq_range_old;
    float   mas_out[2];

    freq_range_old = 0x0;
    tresholds_energy = 0.6;
    spectrum_cnt = 0;
    bearing_cnt = 0;
    flag_option = 0;
    memset(&mas_out[0],0,sizeof(mas_out[0])*2);
    memset(&temperature, 0, sizeof(temperature));

    memset(&hat, 0, sizeof(hat));

    memset(&rsw0, 0, sizeof(rsw0));
    memset(&rsw1, 0, sizeof(rsw1));
    memset(&rsw2, 0, sizeof(rsw2));
    memset(&rsw3, 0, sizeof(rsw3));
    memset(&rsw4, 0, sizeof(rsw4));

    memset(&isw0, 0, sizeof(isw0));
    memset(&isw1, 0, sizeof(isw1));
    memset(&isw2, 0, sizeof(isw2));
    memset(&isw3, 0, sizeof(isw3));
    memset(&isw4, 0, sizeof(isw4));

    memset(&Re_Bin_Ch0, 0, sizeof(Re_Bin_Ch0));
    memset(&Re_Bin_Ch1, 0, sizeof(Re_Bin_Ch1));
    memset(&Re_Bin_Ch2, 0, sizeof(Re_Bin_Ch2));
    memset(&Re_Bin_Ch3, 0, sizeof(Re_Bin_Ch3));
    memset(&Re_Bin_Ch4, 0, sizeof(Re_Bin_Ch4));

    memset(&Im_Bin_Ch0, 0, sizeof(Im_Bin_Ch0));
    memset(&Im_Bin_Ch1, 0, sizeof(Im_Bin_Ch1));
    memset(&Im_Bin_Ch2, 0, sizeof(Im_Bin_Ch2));
    memset(&Im_Bin_Ch3, 0, sizeof(Im_Bin_Ch3));
    memset(&Im_Bin_Ch4, 0, sizeof(Im_Bin_Ch4));

    memset(&callibration, 0, sizeof(callibration));

    memset(&spectrum, 0, sizeof(spectrum));

    num_tresholds = 0;
    memset(&tresholds, 0, sizeof(tresholds));

    memset(&init_bear, 0, sizeof(init_bear));

    memset(&fhss_time, 0, sizeof(fhss_time));

    memset(&tech_analisys, 0, sizeof(tech_analisys));

    memset(&internal_synch, 0, sizeof(internal_synch));

    memset(&rxvipBuf,0,sizeof(rxvipBuf[0])*24576);

    appObj = (LpbkApp_Obj *) arg0;
    BspUtils_MemHeapStatus startHeapStat, startHeapStat1;
    BspUtils_memGetHeapStat(&startHeapStat);
    LpbkApp_init();
    testPrms = &gLpbkAppTestPrms;
    BspUtils_memGetHeapStat(&startHeapStat1);
    testPrms->testId = 0;
    GPIO_init();
    GPIO_write(GPIO8_15, GPIO_PIN_VAL_HIGH); // RESET_PINC7
    GPIO_write( GPIO_VIN1_3_1, GPIO_PIN_VAL_LOW ); // vsync vin1_a
    GPIO_write( GPIO8_2, GPIO_PIN_VAL_LOW );   // enable    vin1_a
    testPrms->captInWidth = 1024;
    testPrms->captOutWidth = 1024;
    testPrms->dispInWidth = 1024;
    testPrms->tarWidth = 1024;
    init_and_start_vip();
    VIP_CFG_SC21_slice0 = 0x00000000;
    VIP_CFG_SC21_slice1 = 0x00000000;
    choose_channel_ADC(flag_option);
    // открытие uart//
    init_uart();
    set_recive_trs_GPS_Kompas(GPS);    
    //открытие spi
    spi_bitrate  = 2000000;
    size_frame   = 3;
    LMK04832_SPI_4_0 = SPI_Master_Open(SPI4_PORT, spi_bitrate, size_frame);
    Set_LMK04832(LMK04832_SPI_4_0);     /// настройка Lmk

    spi_bitrate  = 5000000;
    MCSPI_Master_Open(SPI1_PORT,size_frame,spi_bitrate,1); // Open MCSPI1 for AD9208 (CS0,CS1,C2)

    ad9208_onechannel_setup(AD9208_MCSPI1_CS2);
    printf("-----------------------------------------\n");
    ad9208_setup(AD9208_MCSPI1_CS0,0x2);
    printf("-----------------------------------------\n");
    ad9208_setup(AD9208_MCSPI1_CS1,0x2);
    printf("-----------------------------------------\n");
    // reset C7  //
    udelay(3);
    GPIO_write(GPIO8_15, GPIO_PIN_VAL_LOW); // RESET_PINC7
    udelay(3);
    size_frame = 1;
    MCSPI_Master_Open(SPI2_PORT,size_frame,spi_bitrate,2);           // Open MCSPI2 CS0  (Cell_Switchboard)
    size_frame = 2;
    MCSPI_Master_Open(SPI2_PORT,size_frame,spi_bitrate,3);          // Open MCSPI2 CS3   (приёмник)
    size_frame = 4;
    spi_bitrate  = 2000000;
    MCSPI_Master_Open(SPI3_PORT,size_frame,spi_bitrate,4);          // Open MCSPI3 for SYN1(CS0),SYN2(CS1); CS2,CS3(empty)
    FUNC_SET_HMC830(MCSPI3_CS0,MCSPI3_CS1);
    //// Разбор полос ////
    for(i = 0; i < 377; i++)
    {
       bands.fl[i] = (uint16_t)(mass_bands[i]);
       bands.fh[i] = (uint16_t)(mass_bands[i] >> 16);
       bands.n_ddc[i] = (bands.fh[i] - bands.fh[i]) >> 1;
    }
    Start_data(); // пропуск первых трёх кадров
    udelay(50);
    Start_data();
    udelay(50);
    Start_data();
    udelay(50);
    ///////////////////// Отладка принятия данных с АЦП   /////////

    while(1)
    {
       Start_data();
       get_data_vip();
       printf("New graph allocated!\n");
       printf("-----------------------------------------\n");
       choose_channel_ADC(flag_option);
       mdelay(1); // for test

    }
	///////////////////////////////////////////////////////////////
    while (1)
    {
        Semaphore_pend(semen, BIOS_WAIT_FOREVER);
        switch(hat.command)
        {
            case version_ :
            {
                hat.command = 0;
                send_eth_ticket(arm, (uint32_t)version_);
                break;
            }

            case boot_ :
            {
                send_eth_ticket(arm, (uint32_t)boot_);
                boot_file();
                hat.command = 0;
                break;
            }

            case init_calibration_ukv_ :
            {
                memset(&callibration.att_prm[0],0,sizeof(callibration.att_prm[0])*377);
                flag_aerial = 0;
                init_calibration_ukv();
                freq_range_old = callibration.mas_freq_range[0];
                in_loop = 1;
                send_eth_ticket(arm, (uint32_t)init_calibration_ukv_);
                hat.command = 0;
                break;
            }

            case start_calibration_ukv_ :
            {
                int8_t status_overflow_callibration;
                status_overflow_callibration = 0;

                if (in_loop == 1)
                {
                    send_eth_ticket(arm, (uint32_t)start_calibration_ukv_);
                }

                for (i = in_loop; i < callibration.num_bands; i++)
                {
                    if(hat.command == stop_calibration_ukv_)
                    {
                       break;
                    }

                    status_overflow_callibration =  start_calibration_ukv(i);            // заходит индекс полосы

                    if (status_overflow_callibration == 1)
                    {
                       callibration.att_prm[freq_range_old] = 5;
                       set_rec_mode(set_cmd_1,MCSPI2_LVDS_CS3,0x5,freq_range_old,callibration.att_prm[freq_range_old],0,0);
                    }

                    calibration_result_ukv(i,freq_range_old);

                    ///// перестройка частоты  /////
                    freq_mhz = (mass_bands[callibration.band_idx[i]] >> 16);
                    flag_aerial = selection_litera(freq_mhz,flag_aerial,1);
                    callibration.mas_freq_range[i] = pres_freq_range(freq_mhz*1000000); // индекс используемого частотного диапазона (преселектор)

                    if (callibration.mas_freq_range[i] != freq_range_old)
                    {
                       freq_range_old = callibration.mas_freq_range[i];
                       set_rec_mode(set_cmd_1,MCSPI2_LVDS_CS3,0x5,freq_range_old,callibration.att_prm[freq_range_old],0,0); // настройка приёмника

                    }


                    Task_sleep(5);
                }

                hat.command = 0;
                in_loop = 0;
                break;

            }

            case stop_calibration_ukv_ :
            {
                callibration.step = 0x0;
                callibration.auto_att = 0x0;
                callibration.bkl_noise = 0x0;
                callibration.num_bands = 0x0;
                in_loop            = 0;
                flag_aerial        = 0;
                memset(&callibration.band_idx[0], 0, sizeof(uint32_t)*377);
                memset(&callibration.att_prm[0], 0, sizeof(uint32_t)*377);
                memset(&callibration.att_bk[0], 0, sizeof(uint32_t)*377);
                memset(&callibration.overflow[0], 0, sizeof(uint32_t)*377);
                send_eth_ticket(arm, (uint32_t)stop_calibration_ukv_);
                hat.command = 0;
                break;
            }

            case init_spectrum_ :
            {
                flag_aerial     = 0;
                init_spectr();
                freq_range_old = spectrum.mas_freq_range[0];
                in_loop = 1;
                send_eth_ticket(arm, (uint32_t)init_spectrum_);
                hat.command = 0;
                break;
            }

            case start_spectrum_ :
            {
                int8_t status_overflow_spectr;
                uint32_t left_frequency;
                status_overflow_spectr = 0;

                if (in_loop == 1)
                {
                    send_eth_ticket(arm, (uint32_t)start_spectrum_);
                }

                for (i = in_loop; i < spectrum.num_bands; i++)        // цикл по полосам
                {
                    if (hat.command == stop_spectrum_)
                    {

                       break;

                    }

                    if (spectrum.ch_num < 8)
                    {
                        status_overflow_spectr =  start_spectr_single_ch(i);

                    }

                    else
                    {
                        status_overflow_spectr =  start_spectr_multi(i);

                    }

                    if (status_overflow_spectr == 1)
                    {

                        if (spectrum.att_prm[freq_range_old] == 30)
                        {
                            spectrum.att_prm[freq_range_old] = 31;
                            set_rec_mode(set_cmd_1,MCSPI2_LVDS_CS3,spectrum.ch_num,freq_range_old,spectrum.att_prm[freq_range_old],0,0);
                        }

                        else
                        {
                            spectrum.att_prm[freq_range_old] += 5;
                            set_rec_mode(set_cmd_1,MCSPI2_LVDS_CS3,spectrum.ch_num,freq_range_old,spectrum.att_prm[freq_range_old],0,0);
                        }

                    }

                    ///// перестройка частоты  /////
                    freq_mhz = (mass_bands[spectrum.band_idxs[i]] >> 16);
                    flag_aerial = selection_litera(freq_mhz,flag_aerial,1);
                    spectrum.mas_freq_range[i] = pres_freq_range(freq_mhz*1000000); // индекс используемого частотного диапазона (преселектор)

                    if (spectrum.mas_freq_range[i] != freq_range_old)
                    {
                        freq_range_old = spectrum.mas_freq_range[i];
                        set_rec_mode(set_cmd_1,MCSPI2_LVDS_CS3,spectrum.ch_num, freq_range_old,spectrum.att_prm[freq_range_old],0,0); // настройка приёмника

                    }

                    ////////////// настройка ПЛИС на следующую полосу  ////////////
                    if (spectrum.mas_freq_range[i] == 11)
                    {
                      freq_mhz       = 1650 - freq_mhz + 1000;
                      left_frequency  = freq_mhz - (spectrum.nddc[i] * 2);
                    }
                    else if (spectrum.mas_freq_range[i] == 15)
                    {
                      freq_mhz       = 3000  - freq_mhz + 2000;
                      left_frequency  = freq_mhz - (spectrum.nddc[i] * 2);
                    }

                    config_ADC(spectrum.nddc[i],left_frequency,useful_struct.fft_points,spectrum.ch_num);

                    Task_sleep(5);  // wait for transmission by ethernet

                }
                in_loop = 0;
                break;
            }

            case stop_spectrum_ :
            {
                spectrum.ch_num    = 0x0;
                spectrum.num_bands = 0x0;
                spectrum.overflow  = 0x0;
                spectrum.mass_size = 0x0;
                in_loop            = 0;
                memset(&spectrum.band_idxs[0], 0, sizeof(spectrum.band_idxs[0])*377);
                memset(&spectrum.att_prm[0], 0, sizeof(spectrum.att_prm[0])*16);
                memset(&spectrum.mas_freq_range[0],0,sizeof(spectrum.mas_freq_range[0])*377);
                send_eth_ticket(arm, (uint32_t)stop_spectrum_);
                hat.command = 0;
                break;
            }

            case tresholds_ :
            {
                tresholds_func();
                send_eth_ticket(arm, (uint32_t)tresholds_);
                hat.command = 0;
                break;
            }

            case init_bearing_ :
            {
                init_bearing();
                freq_mhz = (mass_bands[init_bear.band_idxs[0]] >> 16);
                freq_range_old = init_bear.mas_freq_range[0];
                in_loop = 1;
                send_eth_ticket(arm, (uint32_t)init_bearing_);  // квитирование
                hat.command = 0;
                break;
            }

            case start_bearing_ :
            {
                int8_t status_overflow_bearing;
                uint32_t left_frequency;

                status_overflow_bearing = 0;
                if (in_loop == 1)
                {
                    send_eth_ticket(arm, (uint32_t)start_bearing_);
                }
                ///////////////////////
                /*if (init_bear.priznak_bearing == bearing_in_recon_ || init_bear.priznak_bearing == bearing_in_control_)
                {
                    hat.command = 0;
                }
                save_idx_band = &sliser_.slser_cmd[0]
                sliser(&save_idx_band, slsr.band_next, &sliser_.slser_cmd[0]);
                slsr.band_next - следующий индекс полосы со слайсера
                band_now - текущий индекс полосы со слайсера*/
                ///////////////////////


                if (init_bear.priznak_bearing == nosynchrojob_) // пеленгуем все полосы, работа без синхры
                {

                    //убрать цикл по полосам, добавить вызов слайсера

                    for (i = in_loop; i < spectrum.num_bands; i++)        // цикл по полосам
                    {

                        if (hat.command == stop_bearing_)
                        {
                            break;
                        }
                        status_overflow_bearing = start_bearing(i);
                        if ((status_overflow_bearing == 1) && (tresholds[AUTO_ATT] == 1))
                        {
                            init_bear.att_prm[freq_range_old] += 5;
                            set_rec_mode(set_cmd_1,MCSPI2_LVDS_CS3,0x5, freq_range_old, init_bear.att_prm[freq_range_old],0,0);
                        }

                        ///// перестройка частоты  /////
                        freq_mhz = (mass_bands[init_bear.band_idxs[i]] >> 16);
                        flag_aerial = selection_litera(freq_mhz,flag_aerial,0);
                        init_bear.mas_freq_range[i] = pres_freq_range(freq_mhz*1000000); // индекс используемого частотного диапазона (преселектор)

                        if (init_bear.mas_freq_range[i] != freq_range_old)
                        {

                            freq_range_old = init_bear.mas_freq_range[i];
                            set_rec_mode(set_cmd_1,MCSPI2_LVDS_CS3,0x5, freq_range_old,init_bear.att_prm[freq_range_old],0,0); // настройка приёмника
                        }
                        ////////////// настройка  на следующую полосу  ////////////
                        if (init_bear.mas_freq_range[i] == 11)
                        {
                          freq_mhz        = 2650 - freq_mhz;
                          left_frequency  = freq_mhz - (init_bear.nddc[i] << 1);
                        }
                        else if (init_bear.mas_freq_range[i] == 15)
                        {
                          freq_mhz        = 5000  - freq_mhz;
                          left_frequency  = freq_mhz - (init_bear.nddc[i] << 1);
                        }
                        config_ADC(init_bear.nddc[i],left_frequency,useful_struct.fft_points,0x5);// настройка AD на следующую полосу
                        spectrum_cnt++;
                        bearing_cnt++;

                        if (spectrum_cnt == 0xffffffff)             // check overflow and reset the counter
                        {
                            spectrum_cnt = 0;
                        }

                        if (bearing_cnt == tresholds[NUM_POO])      // check overflow and reset the counter
                        {
                            bearing_cnt = 0;
                        }

                        Task_sleep(5);  // wait for transmission by ethernet
                    }

                    in_loop = 0;
                    break;
                }

                else if (init_bear.priznak_bearing == bearing_in_recon_)  // работа по синхре, пеленгуем только одну полоску
                {

                        hat.command = 0x0;

                        // вызов слайсера на следующую полосу

                        status_overflow_bearing = start_bearing(i);   // i - новый индекс полосы
                        status_overflow_bearing = start_bearing(i);
                        if ((status_overflow_bearing == 1) && (tresholds[AUTO_ATT] == 1))
                        {
                          init_bear.att_prm[freq_range_old] += 5;
                          set_rec_mode(set_cmd_1,MCSPI2_LVDS_CS3,0x5, freq_range_old, init_bear.att_prm[freq_range_old],0,0);
                        }

                        ///// перестройка частоты  /////
                        freq_mhz = (mass_bands[init_bear.band_idxs[i]] >> 16);
                        flag_aerial = selection_litera(freq_mhz,flag_aerial,0);
                        init_bear.mas_freq_range[i] = pres_freq_range(freq_mhz*1000000); // индекс используемого частотного диапазона (преселектор)

                        if (init_bear.mas_freq_range[i] != freq_range_old)
                        {

                          freq_range_old = init_bear.mas_freq_range[i];
                          set_rec_mode(set_cmd_1,MCSPI2_LVDS_CS3,0x5, freq_range_old,init_bear.att_prm[freq_range_old],0,0); // настройка приёмника
                        }

                        spectrum_cnt++;
                        bearing_cnt++;

                        if (spectrum_cnt == 0xffffffff)             // check overflow and reset the counter
                        {
                          spectrum_cnt = 0;
                        }

                        if (bearing_cnt == tresholds[NUM_POO])      // check overflow and reset the counter
                        {
                           bearing_cnt = 0;
                        }

                        Task_sleep(5);  // убрать


                }
                break;
            }

            case stop_bearing_ :
            {
                init_bear.num_bands = 0x0;
                memset(&init_bear.band_idxs[0], 0, sizeof(uint32_t)*377);
                init_bear.num_sectors = 0x0;
                memset(&init_bear.left_border[0], 0, sizeof(float)*3);
                memset(&init_bear.right_border[0], 0, sizeof(float)*3);
                send_eth_ticket(arm, (uint32_t)stop_bearing_);
                init_bear.priznak_bearing = 66;
                in_loop            = 0;
                hat.command = 0;
                break;
            }

        }
        Semaphore_post(semen);
        Task_sleep(2);//10*10мкс
    }
}


//void GPIO_callback_U3(void)
//{
 //   sysref_flag = 1;
//}

//void GPIO_callback_U4(void)
//{
//    ftw_send_flag=1;
//}

//void Control_callback(void)
//{
//    hat.command = start_control_;

//}

//void Razvedcka_callback(void)
//{
//    ;;
//}

//void Supression_callback(void)
//{
//    ;;
//}

//void Count100ms_callback(void)
//{
//    ;;
//}
float sdvig_signal[1024];
const float Two_PI = 6.2831;
void get_data_vip(void)
{
    uint32_t i;

    float a = 0;
    int k = 0;
    for (i = 0; i < 1024; i ++)
    {
        a =(float) rxvipBuf[i];
        vipBuf1[k] = a;  // a
        a = a * (-1.0 * sin(Two_PI * 300000000 * a));
        sdvig_signal[i] = a;
       // ((k % 2) == 0) ? (vipBuf1[k] = a) : (vipBuf1[k] = -1.0*a);
        k++;
        vipBuf1[k] = a; // 0.0
        k++;

    }

  //  memcpy(&txDSPBuf0[0], &vipBuf1[0],sizeof(vipBuf1[0])*2048);
  //  sh_mem_send_time_seq(SPECTRUM_POWER_DSP0,(uint32_t *)&txDSPBuf0[0],(uint32_t *)&txDSPBuf0[0], 1024);
  //  sh_mem_recieve(DSP0_);
}
void   choose_channel_ADC(int flag_rebuild)
{

    int in_ch = 0xba;
    if (flag_rebuild == 0)
    {
        if (first_strt_ADC == 0)
           {
               New_Channel_Select:
               printf("Select channels from values: 1, 2, 3, 4, 5; \n");
               printf("Enter number of channel for ADC: \n");
               scanf("%d",&in_ch);
               first_strt_ADC = 1;
               flag_rebuild = 2;
               flag_option = 2;
           }

           if (in_ch == 1)
           {
               GPIO8 &= 0xFFFFFF87;
               GPIO8 |= 0x00000000;
               printf("Channel '1'  for ADC0 is used. \n");

           }
           else if (in_ch == 2)
           {
               GPIO8 &= 0xFFFFFF87;
               GPIO8 |= 0x00000008;
               printf("Channel '2'  for ADC0 is used. \n");

           }
           else if (in_ch == 3)
           {
               GPIO8 &= 0xFFFFFF87;
               GPIO8 |= 0x00000010;
               printf("Channel '3'  for ADC1 is used. \n");

           }
           else if (in_ch == 4)
           {
               GPIO8 &= 0xFFFFFF87;
               GPIO8 |= 0x00000018;
               printf("Channel '4'  for ADC1 is used. \n");

           }
           else if (in_ch == 5)
           {
               GPIO8 &= 0xFFFFFF87;
               GPIO8 |= 0x00000020;
               printf("Channel '5'  for ADC2 is used. \n");

           }
    }

    if (flag_rebuild == 1)
    {
        GPIO_write(GPIO8_15, GPIO_PIN_VAL_HIGH); // RESET_PINC7
        udelay(3);
        GPIO_write(GPIO8_15, GPIO_PIN_VAL_LOW); // RESET_PINC7
        udelay(3);
        goto New_Channel_Select;

    }


}

