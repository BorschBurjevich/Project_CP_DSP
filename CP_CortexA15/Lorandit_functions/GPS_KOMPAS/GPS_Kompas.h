/*
 * GPS_Kompas.h
 *
 *  Created on: 10 авг. 2021 г.
 *      Author: Korostelev
 */

#ifndef LORANDIT_FUNCTIONS_GPS_KOMPAS_GPS_KOMPAS_H_
#define LORANDIT_FUNCTIONS_GPS_KOMPAS_GPS_KOMPAS_H_

#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/drv/uart/src/UART_osal.h>
#include <ti/drv/uart/soc/UART_soc.h>
#include <xdc/std.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "Lorandit_functions/Cod_com.h"

extern int8_t set_recive_trs_GPS_Kompas(UART_Handle uartHnd_1);
void set_tasks_GPS_Kompas(uint16_t iden, UART_Handle uartHnd_GPS);
int Kompas_BMR_3000(UART_Handle uartHnd);
uint8_t get_Pitch_Angle(UART_Handle Kompas_Hnd);
uint8_t get_Roll_Angle(UART_Handle Kompas_Hnd);
uint8_t get_Heading_Angle(UART_Handle Kompas_Hnd);
uint8_t get_All_Angles(UART_Handle Kompas_Hnd);
uint8_t set_the_Magnetic(UART_Handle Kompas_Hnd,float Magnetic);
uint8_t read_the_Magnetic(UART_Handle Kompas_Hnd);
uint8_t set_baud_rate_BMR(UART_Handle Kompas_Hnd,uint8_t rate);
uint8_t set_address_BMR(UART_Handle Kompas_Hnd,uint8_t addr);
uint8_t set_Query_address(UART_Handle Kompas_Hnd);
uint8_t set_output_mode(UART_Handle Kompas_Hnd, uint8_t cmd);
uint8_t save_settings(UART_Handle Kompas_Hnd);
uint8_t switch_calib_output(UART_Handle Kompas_Hnd);

void GPS_GLON(UART_Handle RNM_handle);
void InitSP(UART_Handle local_RNM);
void InitSSys(UART_Handle local_RNM);
void Save_changes(UART_Handle local_RNM);
unsigned int search_repeat_data(uint8_t *buf_in,const int val_ck,uint8_t read_flag);
uint8_t tx_UART_buf[8];
uint8_t rx_UART_buf[8];

uint8_t tx_Kompas[32];
uint8_t rx_Kompas[32];

uint8_t GPS_Kompas_Buf[100];
///////////////////////
#define  RATE_2400_      (0x00U)
#define  RATE_4800_      (0x01U)
#define  RATE_9600_      (0x02U)
#define  RATE_19200_     (0x03U)
#define  RATE_115200_    (0x04U)
#define  RATE_38400_     (0x05U)
#define  RATE_57600_     (0x06U)

#define  MODE_Default_   (0x00U)
#define  MODE_5Hz_       (0x01U)
#define  MODE_10Hz_      (0x02U)
#define  MODE_20Hz_      (0x03U)
#define  MODE_25Hz_      (0x04U)
#define  MODE_50Hz_      (0x05U)
#define  MODE_100Hz_     (0x06U)
//////////////////////


////////define GPS_Glonass///
#define READ_ON               0x1
#define READ_OFF              0x0
/////////////////////////////

typedef struct
{
    uint16_t pack_40h[3];            // Установка параметров обмена по последовательным портам
    uint16_t System_coordinate[67];  // Признак системы координат
    uint16_t projection[4];          // Проекция
    uint16_t sys_navigate[5];        // Используемая система спутников
    uint16_t used_protocol[2];       // Управляющий протокол
    uint16_t set_infprtcl[2];        // Информационный протокол
    double   B_;                     // Координаты опорной точки
    double   L_;
    double   H_;
    uint16_t Type[3];                 // Алгоритм выдачи амплитуды сигнала
    uint16_t Type_metka[5];           // Тип синхронной работы метки
    uint16_t Always[2];               // Тип несинхронной
    float    Big_halfaxle_A_;         // большая полуось в метрах
    float    relation;                // отношение большой полуоси к разнице большой и малой полуосей (1/f=a/a-b)
    float    D_x;                     // смещение центра координат в данной системе относительно системы WGS-84 в метрах
    float    D_y;
    float    D_z;
    uint16_t GDop[2];                 // Параметр оценки точности навигационнных определений
    int16_t  Hour;                    // Смещение в часах гринвичского времени относительно локального
    uint16_t Min;                     // Смещение в минутах гринвичского времени относительно локального
    uint32_t GLO;                     // Маска для включения/исключения в/из разрешённых спутников Глонасс
    uint32_t GPS;                     // Маска для включения/исключения в/из разрешённых спутников GPS
    uint16_t State[3];                // Режимы использования дифференциальных поправок
    int16_t  Age;                     // Возраст дифференциальных поправок
    int16_t  Max_Err_Height;          // Значение предельной ошибки определения высоты в метрах
    uint16_t MaxTime;                 // значение предельной длительности прогнозирования в секундах
    uint16_t Max_Err_plane;           // значение предельной ошибки определения планового решения в метрах
    uint16_t Max_Err_time;            // значение предельной ошибки выдачи метки времени в наносекндах
    uint16_t Max_Time_Place;          // время выдачи прогнозируемого местоположения в секундах
    uint16_t Mode_Work_Navig[3];      // Режим работы навигатора
    uint16_t Navigate_Difinition[4];  // Режим навигационных определений
    uint16_t Enable[2];               // разрешение выдачи осредненных измерений
    uint16_t Set_Time[6];             // текущее время и дата
    float    Magnetic;                // магнитное склонение
    float    Delay;                   // задержку в ВЧ тракте
    float    Shift;                   // сдвиг выдаваемой секундной метки времени, относительно секундной метки заданной шкалы времени
    int16_t  Elevation;               // минимальный угол места в градусах, целое число не более 90
    uint32_t Phase;                   // устанавливает начальную фазу генератора ПСП
    uint32_t Mask_Bin[17];            // позволяет включить пакеты, заданные маской, при использовании информационного бинарного протокола
    uint32_t Mask_NMEA[9];            // позволяет включить пакеты, заданные маской, при использовании  информационного NMEA протокола
    uint16_t State_Information[2];    // включает или выключает накопление статистической информации
    uint16_t Num;                     // номер, который будет выдаваться навигатором для станции
    uint16_t State_Filtering_Navig[2];// вкл\выкл фильтрации навигационных определений
    uint16_t Sigma;                   // эквивалент среднеквадратической ошибки измерений псевдодальности в метрах
    uint16_t Choose_NKA[2];           // выбор НКА из используемых спутниковых систем навигации
    uint16_t Number_NKA;              // номер НКА
    uint16_t NKA_option[2];           // параметр НКА
    uint16_t number_packet_binary[17];// Номер пакета
    uint16_t parametr;                // состояние автоматической выдачи пакета



}set_tasks_struct;



#endif /* LORANDIT_FUNCTIONS_GPS_KOMPAS_GPS_KOMPAS_H_ */
