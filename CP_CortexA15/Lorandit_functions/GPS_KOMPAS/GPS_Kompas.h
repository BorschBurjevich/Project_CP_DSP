/*
 * GPS_Kompas.h
 *
 *  Created on: 10 ���. 2021 �.
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
    uint16_t pack_40h[3];            // ��������� ���������� ������ �� ���������������� ������
    uint16_t System_coordinate[67];  // ������� ������� ���������
    uint16_t projection[4];          // ��������
    uint16_t sys_navigate[5];        // ������������ ������� ���������
    uint16_t used_protocol[2];       // ����������� ��������
    uint16_t set_infprtcl[2];        // �������������� ��������
    double   B_;                     // ���������� ������� �����
    double   L_;
    double   H_;
    uint16_t Type[3];                 // �������� ������ ��������� �������
    uint16_t Type_metka[5];           // ��� ���������� ������ �����
    uint16_t Always[2];               // ��� ������������
    float    Big_halfaxle_A_;         // ������� ������� � ������
    float    relation;                // ��������� ������� ������� � ������� ������� � ����� �������� (1/f=a/a-b)
    float    D_x;                     // �������� ������ ��������� � ������ ������� ������������ ������� WGS-84 � ������
    float    D_y;
    float    D_z;
    uint16_t GDop[2];                 // �������� ������ �������� �������������� �����������
    int16_t  Hour;                    // �������� � ����� ������������ ������� ������������ ����������
    uint16_t Min;                     // �������� � ������� ������������ ������� ������������ ����������
    uint32_t GLO;                     // ����� ��� ���������/���������� �/�� ����������� ��������� �������
    uint32_t GPS;                     // ����� ��� ���������/���������� �/�� ����������� ��������� GPS
    uint16_t State[3];                // ������ ������������� ���������������� ��������
    int16_t  Age;                     // ������� ���������������� ��������
    int16_t  Max_Err_Height;          // �������� ���������� ������ ����������� ������ � ������
    uint16_t MaxTime;                 // �������� ���������� ������������ ��������������� � ��������
    uint16_t Max_Err_plane;           // �������� ���������� ������ ����������� ��������� ������� � ������
    uint16_t Max_Err_time;            // �������� ���������� ������ ������ ����� ������� � �����������
    uint16_t Max_Time_Place;          // ����� ������ ��������������� �������������� � ��������
    uint16_t Mode_Work_Navig[3];      // ����� ������ ����������
    uint16_t Navigate_Difinition[4];  // ����� ������������� �����������
    uint16_t Enable[2];               // ���������� ������ ����������� ���������
    uint16_t Set_Time[6];             // ������� ����� � ����
    float    Magnetic;                // ��������� ���������
    float    Delay;                   // �������� � �� ������
    float    Shift;                   // ����� ���������� ��������� ����� �������, ������������ ��������� ����� �������� ����� �������
    int16_t  Elevation;               // ����������� ���� ����� � ��������, ����� ����� �� ����� 90
    uint32_t Phase;                   // ������������� ��������� ���� ���������� ���
    uint32_t Mask_Bin[17];            // ��������� �������� ������, �������� ������, ��� ������������� ��������������� ��������� ���������
    uint32_t Mask_NMEA[9];            // ��������� �������� ������, �������� ������, ��� �������������  ��������������� NMEA ���������
    uint16_t State_Information[2];    // �������� ��� ��������� ���������� �������������� ����������
    uint16_t Num;                     // �����, ������� ����� ���������� ����������� ��� �������
    uint16_t State_Filtering_Navig[2];// ���\���� ���������� ������������� �����������
    uint16_t Sigma;                   // ���������� �������������������� ������ ��������� ��������������� � ������
    uint16_t Choose_NKA[2];           // ����� ��� �� ������������ ����������� ������ ���������
    uint16_t Number_NKA;              // ����� ���
    uint16_t NKA_option[2];           // �������� ���
    uint16_t number_packet_binary[17];// ����� ������
    uint16_t parametr;                // ��������� �������������� ������ ������



}set_tasks_struct;



#endif /* LORANDIT_FUNCTIONS_GPS_KOMPAS_GPS_KOMPAS_H_ */
