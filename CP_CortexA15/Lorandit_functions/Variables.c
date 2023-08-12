/*
 * Structs.c
 *
 */

#include "Structs.h"
#include "GPS_KOMPAS/GPS_Kompas.h"
#include "Zdnglobdef.h"

bands_sruct         bands        __attribute__((aligned(4), section(".bss:variables")));
temperature_struct  temperature  __attribute__((aligned(4), section(".bss:variables")));
hat_struct          hat          __attribute__((aligned(4), section(".bss:variables")));
float               rsw0[84]     __attribute__((aligned(4), section(".bss:variables")));
float               isw0[84]     __attribute__((aligned(4), section(".bss:variables")));
float               rsw1[84]     __attribute__((aligned(4), section(".bss:variables")));
float               isw1[84]     __attribute__((aligned(4), section(".bss:variables")));
float               rsw2[84]     __attribute__((aligned(4), section(".bss:variables")));
float               isw2[84]     __attribute__((aligned(4), section(".bss:variables")));
float               rsw3[84]     __attribute__((aligned(4), section(".bss:variables")));
float               isw3[84]     __attribute__((aligned(4), section(".bss:variables")));
float               rsw4[84]     __attribute__((aligned(4), section(".bss:variables")));
float               isw4[84]     __attribute__((aligned(4), section(".bss:variables")));

float               Re_Bin_Ch0[84] __attribute__((aligned(4), section(".bss:variables")));
float               Im_Bin_Ch0[84] __attribute__((aligned(4), section(".bss:variables")));
float               Re_Bin_Ch1[84] __attribute__((aligned(4), section(".bss:variables")));
float               Im_Bin_Ch1[84] __attribute__((aligned(4), section(".bss:variables")));
float               Re_Bin_Ch2[84] __attribute__((aligned(4), section(".bss:variables")));
float               Im_Bin_Ch2[84] __attribute__((aligned(4), section(".bss:variables")));
float               Re_Bin_Ch3[84] __attribute__((aligned(4), section(".bss:variables")));
float               Im_Bin_Ch3[84] __attribute__((aligned(4), section(".bss:variables")));
float               Re_Bin_Ch4[84] __attribute__((aligned(4), section(".bss:variables")));
float               Im_Bin_Ch4[84] __attribute__((aligned(4), section(".bss:variables")));

callibr_stuct       callibration    __attribute__((aligned(4), section(".bss:variables")));
spectrum_struct     spectrum        __attribute__((aligned(4), section(".bss:variables")));
tresholds_struct    tresholds_item  __attribute__((aligned(4), section(".bss:variables")));
uint16_t            num_tresholds   __attribute__((aligned(4), section(".bss:variables")));
int                 tresholds[50]   __attribute__((aligned(4), section(".bss:variables")));
init_bearing_struct init_bear       __attribute__((aligned(4), section(".bss:variables")));
fft_struct          useful_struct   __attribute__((aligned(4), section(".bss:variables")));
bearing_bands_      bearing_bands[50]   __attribute__((aligned(4), section(".bss:variables")));
fhss_time_struct    fhss_time           __attribute__((aligned(4), section(".bss:variables")));
tech_analisys_struct  tech_analisys     __attribute__((aligned(4), section(".bss:variables")));
mode_sk_struct      acoustic_control    __attribute__((aligned(4), section(".bss:variables")));
internal_synch_struct internal_synch    __attribute__((aligned(4), section(".bss:variables")));
uint8_t             pBuf[100000]        __attribute__((aligned(4), section(".bss:variables")));//приемный буфер от арма

//uint32_t            file_boot_loader[5000000]   __attribute__((aligned(4), section(".bss:variables")));
uint8_t             txEthBuf[1677216]  __attribute__((aligned(4), section(".bss:variables")));
uint8_t             Buf_Write_USB[size_buf_wright]  __attribute__((aligned(4), section(".bss:variables"))); // for usb
uint8_t             Buf_Read_USB[size_buf_read]     __attribute__((aligned(4), section(".bss:variables")));
uint32_t            bearing_cnt        __attribute__((aligned(4), section(".bss:variables")));
uint32_t            spectrum_cnt       __attribute__((aligned(4), section(".bss:variables")));
reciever_struct     rcvr               __attribute__((aligned(4), section(".bss:variables")));

float               pitch              __attribute__((aligned(4), section(".bss:variables")));
float               roll               __attribute__((aligned(4), section(".bss:variables")));
float               heading            __attribute__((aligned(4), section(".bss:variables")));
float               magnetic_declination            __attribute__((aligned(4), section(".bss:variables")));
/////////// лглобальные флаги ////////////
uint8_t  flag_aerial                   __attribute__((aligned(4), section(".bss:variables")));
int8_t   status_receiver               __attribute__((aligned(4), section(".bss:variables")));
uint8_t  in_loop                       __attribute__((aligned(4), section(".bss:variables")));
uint8_t  Lable_first_control           __attribute__((aligned(4), section(".bss:variables")));
uint8_t  global_first_RNV              __attribute__((aligned(4), section(".bss:variables")));
short    rxvipBuf[24576]               __attribute__((aligned(4), section(".bss:rxvipbuf")));



