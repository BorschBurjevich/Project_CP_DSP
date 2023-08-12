/*
 * Power_Ampl.h
 *
 *  Created on: 2 ���. 2020 �.
 *
 */

#ifndef LORANDIT_FUNCTIONS_POWER_AMPL_POWER_AMPL_H_
#define LORANDIT_FUNCTIONS_POWER_AMPL_POWER_AMPL_H_

#include "Lorandit_functions/Cod_com.h"
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/drv/uart/src/UART_osal.h>
#include <ti/drv/uart/soc/UART_soc.h>

/*
 * ���� ������
 *             -1 �� ��� �����
 *              0 �� ������� ����������� �����
 *              1 ��� ��
 */


typedef enum
{
    inside  = 0x00,//���� �������� �������
    outside = 0x01,//������� �������������
}PA_input;

typedef enum
{
    antenna_array = 0x00,//������� �������
    antenna_kdn   = 0x01,//������� ���
    internal_load = 0x03,//���������� ��������
}PA_output;

typedef enum
{
    one_freq   = 0x00,//������������� �����
    two_freq   = 0x01,//�����������
    three_freq = 0x02,//�������������
    four_freq  = 0x03,//����������������
}PA_freq_cnt;

typedef enum
{
    PA1_HF_L = CLR_BIT32(0),//��1 �� ����
    PA1_HF_H = SET_BIT32(0),//��1 �� ����
    PA2_HF_L = CLR_BIT32(1),//��2 �� ����
    PA2_HF_H = SET_BIT32(1),//��2 �� ����
    PA3_HF_L = CLR_BIT32(2),//��3 �� ����
    PA3_HF_H = SET_BIT32(2),//��3 �� ����
    PA4_HF_L = CLR_BIT32(3),//��4 �� ����
    PA4_HF_H = SET_BIT32(3),//��4 �� ����
}PA_harm_fltr;

uint8_t tx_amp_buf[8];
uint8_t rx_amp_buf[8];

int8_t get_state_amp(UART_Handle uartHnd);
int8_t get_state_sw(UART_Handle uartHnd);
int8_t set_input(UART_Handle uartHnd, PA_input inside_outside);
int8_t set_output(UART_Handle uartHnd, PA_output output_type);
int8_t set_freq_cnt(UART_Handle uartHnd, PA_freq_cnt freq_cnt);
int8_t set_harm_fltr(UART_Handle uartHnd, uint8_t harm_fltr);
int8_t set_mode_amp(UART_Handle uartHnd, uint8_t mode_amp);
void init_power_amp(UART_Handle uartHnd);

#endif /* LORANDIT_FUNCTIONS_POWER_AMPL_POWER_AMPL_H_ */
