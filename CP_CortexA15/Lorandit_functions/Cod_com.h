#include <ti/ndk/inc/socket.h>
#include <ti/ndk/inc/netmain.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include "Shared_mem_communication/Shared.h"



#define SET_BIT32(x)   (1 << x)
#define CLR_BIT32(x)   ((0xffffffff) & ~(1 << x))

#define SPECTR_BUF_SIZE 532//20b-protocol 512b-data

extern unsigned int mass_bands[377];
float            vipBuf1[24576];
uint32_t     file_boot_loader[5000000];
extern uint8_t      txEthBuf[1677216];
extern uint8_t      pBuf[100000];//ïðèåìíûé áóôåð îò àðìà
extern uint8_t Buf_Write_USB[];
extern uint8_t Buf_Read_USB[];

#define APP_CP              (0x0U)
#define MLO_CP              (0x1U)

float   tresholds_energy;

uint32_t      freq_mhz;



////////////////////////////////////////////

extern uint8_t  flag_aerial;
extern int8_t   status_receiver;
extern uint8_t  in_loop;
/////////////////////////////////////
extern uint8_t  flag_nosynchrobearing;
extern uint8_t  flag_bearing_in_reconn;
extern uint8_t  flag_bearingincontrol;
extern uint32_t flag_allin;
extern uint8_t  Lable_first_control;

extern uint8_t  global_first_RNV;
/////////////////////////
extern uint32_t     bearing_cnt;
extern uint32_t     spectrum_cnt;
extern uint32_t     count100ms;
/////////////////////////////
extern float pitch;
extern float roll;
extern float heading;
extern float magnetic_declination;

/////////////////////////////////////////////////////
/**************ÄÈÀÏÀÇÎÍÛ ÏÐÈÅÌÍÈÊÎÂ*****************/
#define PRES_FREQ_BORDER_HZ_1         (24000000)
#define PRES_FREQ_BORDER_HZ_2         (36000000)
#define PRES_FREQ_BORDER_HZ_3         (50000000)
#define PRES_FREQ_BORDER_HZ_4         (70000000)
#define PRES_FREQ_BORDER_HZ_5         (100000000)
#define PRES_FREQ_BORDER_HZ_6         (168000000)
#define PRES_FREQ_BORDER_HZ_7         (278000000)
#define PRES_FREQ_BORDER_HZ_8         (464000000)
#define PRES_FREQ_BORDER_HZ_9         (774000000)
#define PRES_FREQ_BORDER_HZ_10        (1000000000)
#define PRES_FREQ_BORDER_HZ_11        (1200000000)
#define PRES_FREQ_BORDER_HZ_12        (1450000000)
#define PRES_FREQ_BORDER_HZ_13        (1650000000)
#define PRES_FREQ_BORDER_HZ_14        (2000000000)
#define PRES_FREQ_BORDER_HZ_15        (2200000000)
#define PRES_FREQ_BORDER_HZ_16        (2800000000)
#define PRES_FREQ_BORDER_HZ_17        (3000000000)


#define PRSUL_FREQ_BORDER_HZ_1         (24000000)
#define PRSUL_FREQ_BORDER_HZ_2         (75000000)
#define PRSUL_FREQ_BORDER_HZ_3         (127000000)
#define PRSUL_FREQ_BORDER_HZ_4         (215000000)
#define PRSUL_FREQ_BORDER_HZ_5         (365000000)
#define PRSUL_FREQ_BORDER_HZ_6         (617000000)
#define PRSUL_FREQ_BORDER_HZ_7         (1045000000)
#define PRSUL_FREQ_BORDER_HZ_8         (1771000000)
#define PRSUL_FREQ_BORDER_HZ_9         (3000000000)

/////////////////////////////////////////////////////


//////////////////////////////////////////


///////////////////////////////////

#define Pi 3.14159265
#define GPIO_DATAOUT1 (*((unsigned int *) 0x4805513C))
#define GPIO_DATAOUT5 (*((unsigned int *) 0x4805B13C))

#define GET_TEST_DATA_         (0x0000a5a5)





