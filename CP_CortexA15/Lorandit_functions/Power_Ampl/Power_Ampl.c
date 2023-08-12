/*
 * Power_Ampl.с
 *
 *  Created on: 2 окт. 2020 г.
 *
 */


#include "Power_Ampl.h"

/*
 * После вызова функции посмотреть rx_amp_buf[2]
 *
 * Бит 7: 1- УМ4 включен, 0- выключен;
 * Бит 6: 1- УМ3 включен, 0- выключен;
 * Бит 5: 1- УМ2 включен, 0- выключен;
 * Бит 4: 1- УМ1 включен, 0- выключен;
 * Бит 3: 1- УМ4 излучение, 0- авария;
 * Бит 2: 1- УМ3 излучение, 0- авария;
 * Бит 1: 1- УМ2 излучение, 0- авария;
 * Бит 0: 1- УМ1 излучение, 0- авария;
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

    //проверка на не тот ответ
    if(rx_amp_buf[1] != 0x83)
    {
        status = -1;
        goto error;
    }

    //если state_amp, то проверяем котрольную сумму
    if(rx_amp_buf[1] == 0x83)
    {
        for(i = 0; i < (rx_amp_buf[0]-1); i++)
        {
            sum += rx_amp_buf[i];
        }

        //сумма не совпала
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
 * После вызова функции посмотреть rx_amp_buf[2]
 *
 * Бит 7: 1- вх. сигнал внеш., 0- вх. сигнал внутр.;
 * Бит 6,5: 00- вых. сигн. на АР, 01-  вых. сигн. на КДН, 11- вых. сигн. на внутр. нагр.;
 * Бит 4,3: 00- 1-част. режим, 01- 2-част. режим, 10- 3-част. режим, 11- 4-част. режим;
 * Бит 2: х;
 * Бит 1: х;
 * Бит 0: х.
 *
 * rx_amp_buf[3]
 * Бит 7: 1- УМ4 ГФ верхн., 0- нижн;
 * Бит 6: 1- УМ3 ГФ верхн., 0- нижн;
 * Бит 5: 1- УМ2 ГФ верхн., 0- нижн;
 * Бит 4: 1- УМ1 ГФ верхн., 0- нижн;
 * Бит 3: 1- УМ4 внешн. упр. разреш., 0- запрещ.;
 * Бит 2: 1- УМ3 внешн. упр. разреш., 0- запрещ.;
 * Бит 1: 1- УМ2 внешн. упр. разреш., 0- запрещ.;
 * Бит 0: 1- УМ1 внешн. упр. разреш., 0- запрещ.;
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

    //проверка на не тот ответ
    if(rx_amp_buf[1] != 0x84)
    {
        status = -1;
        goto error;
    }

    //если state_sw, то проверяем котрольную сумму
    if(rx_amp_buf[1] == 0x84)
    {
        for(i = 0; i < (rx_amp_buf[0]-1); i++)
        {
            sum += rx_amp_buf[i];
        }

        //сумма не совпала
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
 * inside_outside - 1 - внешний источник сигнала
 *                  0 - внутренний формирователь
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

    //проверка на не тот ответ
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
 * output_type -    0x00 антеная решетка
 *                  0x01 антенна кдн
 *                  0x03 внутренняя нагрузка
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

    //проверка на не тот ответ
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
 *  freq_cnt -  0x00 одночастотный режим
 *              0x01 двучастоный
 *              0x02 трехчастотнай
 *              0x03 четырехчестотный
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

    //проверка на не тот ответ
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
 * harm_fltr - биты 7...4 включение отелючение излучения
 *             биты 3...0 по или значения из enum'а PA_harm_fltr
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
 * mode_amp - при внешнем 0x0f - режим излучение/пауза
 *                        0x00 - внутр. упр
 *
 *                        0x01 - УМ1 вкл
 *                        0x02 - УМ2 вкл
 *                        0x04 - УМ3 вкл
 *                        0x08 - УМ4 вкл
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

    //ответом должен быть ack либо error
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
