/*
 * reciever.h
 *
 *  Created on: 18 ����. 2020 �.
 *
 */

#ifndef LORANDIT_FUNCTIONS_RECIEVER_RECIEVER_H_
#define LORANDIT_FUNCTIONS_RECIEVER_RECIEVER_H_


#include "Lorandit_functions/Cod_com.h"
#include "delay/delay.h"
#include "gpio/gpio.h"
#include "SPI/SPI_header.h"

#define  set_cmd_1          0x04        // ����� ��������� ����� ������� 1 ��� ������ ����-�
#define  set_cmd_1_PRSUL    0x05        // ����� ��������� ����� ������� 1 � ������� ����-�
#define  set_cmd_2          0x02        // ����� �����������
#define  set_cmd_4          0x10        // ��� ���� ������� ��� ����-�
#define  set_cmd_4_PRSUL    0x11        // ��� ���� ������� ����-�

uint32_t diag_rec_status_ch[8];

/*
 * num_ch  - ����� ��������� ������          0b00000000 - �� ������ ������
 *                                           0b00000001 - 1 �����
 *                                           0b00000011 - 1 � 2 �����
 * freq_rage - ������������ ��������� ��������
 * att - ���������� �����������, �������� ���������� K_dB = att * 4, max(att) = 0xE
 * lock - ���������� �����: 1-���, 0-����
 */
int8_t set_rec_mode(uint8_t set_cmd,MCSPI_Handle mcspi2_cs3, uint8_t num_ch, uint8_t freq_domain, uint8_t att, uint8_t lock, uint8_t flag20db_on);

/*
 * result_flag - ���� ������������� ������ ����������� �����������
 */
int8_t rec_diagnostic(uint8_t set_cmd, uint8_t result_flag, MCSPI_Handle mcspi2_cs3);


/*
 * ���������/���������� ������� �������
 * num_ch  - ����� ��������� ������          0b00000000 - �� ������ ������
 *                                           0b00000001 - 1 �����
 *                                           0b00000011 - 1 � 2 �����
 */
int8_t on_off_rec_ch(uint8_t set_cmd,uint8_t num_ch,MCSPI_Handle mcspi2_cs3);


/*
 * ����� ���������� ��������� ����������
 */
int8_t pres_freq_range(uint32_t freq);
int8_t pres_freq_range_PRSUL(uint32_t freq);


#endif /* LORANDIT_FUNCTIONS_RECIEVER_RECIEVER_H_ */
