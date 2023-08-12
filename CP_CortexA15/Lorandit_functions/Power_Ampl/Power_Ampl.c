/*
 * Power_Ampl.�
 *
 *  Created on: 2 ���. 2020 �.
 *
 */


#include "Power_Ampl.h"

/*
 * ����� ������ ������� ���������� rx_amp_buf[2]
 *
 * ��� 7: 1- ��4 �������, 0- ��������;
 * ��� 6: 1- ��3 �������, 0- ��������;
 * ��� 5: 1- ��2 �������, 0- ��������;
 * ��� 4: 1- ��1 �������, 0- ��������;
 * ��� 3: 1- ��4 ���������, 0- ������;
 * ��� 2: 1- ��3 ���������, 0- ������;
 * ��� 1: 1- ��2 ���������, 0- ������;
 * ��� 0: 1- ��1 ���������, 0- ������;
 *
 */
int8_t get_state_amp(UART_Handle uartHnd)
{
    int8_t status, i;
    uint32_t sum = 0;

    tx_amp_buf[0] = 0x03;
    tx_amp_buf[1] = 0x01;
    tx_amp_buf[2] = 0x04;
    UART_write(uartHnd, (void *)&tx_amp_buf[0], 3);
    UART_read (uartHnd, (void *)&rx_amp_buf[0], 4);

    //�������� �� �� ��� �����
    if(rx_amp_buf[1] != 0x83)
    {
        status = -1;
        goto error;
    }

    //���� state_amp, �� ��������� ���������� �����
    if(rx_amp_buf[1] == 0x83)
    {
        for(i = 0; i < (rx_amp_buf[0]-1); i++)
        {
            sum += rx_amp_buf[i];
        }

        //����� �� �������
        if((uint8_t)sum != rx_amp_buf[3])
        {
            status = 0;
            goto error;
        }

        else
        {
            status = 1;
        }
    }

error:
    return(status);
}

/*
 * ����� ������ ������� ���������� rx_amp_buf[2]
 *
 * ��� 7: 1- ��. ������ ����., 0- ��. ������ �����.;
 * ��� 6,5: 00- ���. ����. �� ��, 01-  ���. ����. �� ���, 11- ���. ����. �� �����. ����.;
 * ��� 4,3: 00- 1-����. �����, 01- 2-����. �����, 10- 3-����. �����, 11- 4-����. �����;
 * ��� 2: �;
 * ��� 1: �;
 * ��� 0: �.
 *
 * rx_amp_buf[3]
 * ��� 7: 1- ��4 �� �����., 0- ����;
 * ��� 6: 1- ��3 �� �����., 0- ����;
 * ��� 5: 1- ��2 �� �����., 0- ����;
 * ��� 4: 1- ��1 �� �����., 0- ����;
 * ��� 3: 1- ��4 �����. ���. ������., 0- ������.;
 * ��� 2: 1- ��3 �����. ���. ������., 0- ������.;
 * ��� 1: 1- ��2 �����. ���. ������., 0- ������.;
 * ��� 0: 1- ��1 �����. ���. ������., 0- ������.;
 *
 */

int8_t get_state_sw(UART_Handle uartHnd)
{
    int8_t status, i;
    uint32_t sum = 0;

    tx_amp_buf[0] = 0x03;
    tx_amp_buf[1] = 0x02;
    tx_amp_buf[2] = 0x05;
    UART_write(uartHnd, (void *)&tx_amp_buf[0], 3);
    UART_read (uartHnd, (void *)&rx_amp_buf[0], 5);

    //�������� �� �� ��� �����
    if(rx_amp_buf[1] != 0x84)
    {
        status = -1;
        goto error;
    }

    //���� state_sw, �� ��������� ���������� �����
    if(rx_amp_buf[1] == 0x84)
    {
        for(i = 0; i < (rx_amp_buf[0]-1); i++)
        {
            sum += rx_amp_buf[i];
        }

        //����� �� �������
        if((uint8_t)sum != rx_amp_buf[4])
        {
            status = 0;
            goto error;
        }

        else
        {
            status = 1;
        }
    }

error:
    return(status);
}

/*
 * inside_outside - 1 - ������� �������� �������
 *                  0 - ���������� �������������
 */
int8_t set_input(UART_Handle uartHnd, PA_input inside_outside)
{
    int8_t status;

    tx_amp_buf[0] = 0x04;
    tx_amp_buf[1] = 0x04;
    tx_amp_buf[2] = (uint8_t)inside_outside;

    if(inside_outside == outside)
    {
        tx_amp_buf[3] = 0x09;
    }
    else
    {
        tx_amp_buf[3] = 0x08;
    }

    UART_write(uartHnd, (void *)&tx_amp_buf[0], 4);
    UART_read (uartHnd, (void *)&rx_amp_buf[0], 3);

    //�������� �� �� ��� �����
    if(rx_amp_buf[1] != 0x81)
    {
        status = -1;
        goto error;
    }

    else
    {
        status = 1;
    }
error:
    return(status);
}

/*
 * output_type -    0x00 ������� �������
 *                  0x01 ������� ���
 *                  0x03 ���������� ��������
 */
int8_t set_output(UART_Handle uartHnd, PA_output output_type)
{
    int8_t status;

    tx_amp_buf[0] = 0x04;
    tx_amp_buf[1] = 0x05;
    tx_amp_buf[2] = output_type;

    if(output_type == antenna_array)
    {
        tx_amp_buf[3] = 0x09;
    }

    if(output_type == antenna_kdn)
    {
        tx_amp_buf[3] = 0x0a;
    }

    else
    {
        tx_amp_buf[3] = 0x0c;
    }

    UART_write(uartHnd, (void *)&tx_amp_buf[0], 4);
    UART_read (uartHnd, (void *)&rx_amp_buf[0], 3);

    //�������� �� �� ��� �����
    if(rx_amp_buf[1] != 0x81)
    {
        status = -1;
        goto error;
    }

    else
    {
        status = 1;
    }
error:
    return(status);
}

/*
 *  freq_cnt -  0x00 ������������� �����
 *              0x01 �����������
 *              0x02 �������������
 *              0x03 ����������������
 */
int8_t set_freq_cnt(UART_Handle uartHnd, PA_freq_cnt freq_cnt)
{
    int8_t status;

    tx_amp_buf[0] = 0x04;
    tx_amp_buf[1] = 0x06;
    tx_amp_buf[2] = freq_cnt;

    if(freq_cnt == one_freq)
    {
        tx_amp_buf[3] = 0x0a;
    }

    if(freq_cnt == two_freq)
    {
        tx_amp_buf[3] = 0x0a;
    }

    if(freq_cnt == three_freq)
    {
        tx_amp_buf[3] = 0x0c;
    }

    else
    {
        tx_amp_buf[3] = 0x0d;
    }

    UART_write(uartHnd, (void *)&tx_amp_buf[0], 4);
    UART_read (uartHnd, (void *)&rx_amp_buf[0], 3);

    //�������� �� �� ��� �����
    if(rx_amp_buf[1] != 0x81)
    {
        status = -1;
        goto error;
    }

    else
    {
        status = 1;
    }
error:
    return(status);
}

/*
 * harm_fltr - ���� 7...4 ��������� ���������� ���������
 *             ���� 3...0 �� ��� �������� �� enum'� PA_harm_fltr
 */
int8_t set_harm_fltr(UART_Handle uartHnd, uint8_t harm_fltr)
{
    int8_t status;

    tx_amp_buf[0] = 0x04;
    tx_amp_buf[1] = 0x06;
    tx_amp_buf[2] = harm_fltr;
    tx_amp_buf[3] = tx_amp_buf[0] + tx_amp_buf[1] + tx_amp_buf[2];

    UART_write(uartHnd, (void *)&tx_amp_buf[0], 4);
    UART_read (uartHnd, (void *)&rx_amp_buf[0], 3);

    //error
    if(rx_amp_buf[1] != 0x81)
    {
        status = -1;
        goto error;
    }

    else
    {
        status = 1;
    }
error:
    return(status);
}

/*
 * mode_amp - ��� ������� 0x0f - ����� ���������/�����
 *                        0x00 - �����. ���
 *
 *                        0x01 - ��1 ���
 *                        0x02 - ��2 ���
 *                        0x04 - ��3 ���
 *                        0x08 - ��4 ���
 */
int8_t set_mode_amp(UART_Handle uartHnd, uint8_t mode_amp)
{
    int8_t status;

    tx_amp_buf[0] = 0x04;
    tx_amp_buf[1] = 0x06;
    tx_amp_buf[2] = mode_amp;
    tx_amp_buf[3] = tx_amp_buf[0] + tx_amp_buf[1] + tx_amp_buf[2];

    UART_write(uartHnd, (void *)&tx_amp_buf[0], 4);
    UART_read (uartHnd, (void *)&rx_amp_buf[0], 3);

    //������� ������ ���� ack ���� error
    if(rx_amp_buf[1] != 0x81)
    {
        status = -1;
        goto error;
    }

    else
    {
        status = 1;
    }
error:
    return(status);
}

void init_power_amp(UART_Handle uartHnd)
{
    int8_t status;

    do
    {
        status = set_freq_cnt(uartHnd, four_freq);

        if(status ==  0) {printf("wrong_sum\n");}
        if(status == -1) {printf("wrong_answer_from_PA cmd = %d \n", (int)rx_amp_buf[2]);}
    }while(status !=  1);

    do
    {
        status = set_input( uartHnd, outside);

        if(status ==  0) {printf("wrong_sum\n");}
        if(status == -1) {printf("wrong_answer_from_PA cmd = %d \n", (int)rx_amp_buf[2]);}
    }while(status !=  1);

    do
    {
        status = set_output( uartHnd, antenna_array);

        if(status ==  0) {printf("wrong_sum\n");}
        if(status == -1) {printf("wrong_answer_from_PA cmd = %d \n", (int)rx_amp_buf[2]);}
    }while(status !=  1);

    do
    {
        status = set_harm_fltr( uartHnd,(uint8_t)(PA1_HF_L | PA1_HF_H));

        if(status ==  0) {printf("wrong_sum\n");}
        if(status == -1) {printf("wrong_answer_from_PA cmd = %d \n", (int)rx_amp_buf[2]);}
    }while(status !=  1);

    do
    {
        status = set_mode_amp(uartHnd, 0x0f);

        if(status ==  0) {printf("wrong_sum\n");}
        if(status == -1) {printf("wrong_answer_from_PA cmd = %d \n", (int)rx_amp_buf[2]);}
    }while(status !=  1);
}
