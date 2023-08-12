/*
 * reciever.h
 *
 *  Created on: 18 сент. 2020 г.
 *
 */

#ifndef LORANDIT_FUNCTIONS_RECIEVER_RECIEVER_H_
#define LORANDIT_FUNCTIONS_RECIEVER_RECIEVER_H_


#include "Lorandit_functions/Cod_com.h"
#include "delay/delay.h"
#include "gpio/gpio.h"
#include "SPI/SPI_header.h"

#define  set_cmd_1          0x04        // режим установки блока команда 1 без ячейки ПРСУ-Л
#define  set_cmd_1_PRSUL    0x05        // режим установки блока команда 1 с ячейкой ПРСУ-Л
#define  set_cmd_2          0x02        // режим диагностики
#define  set_cmd_4          0x10        // вкл выкл питания без ПРСУ-Л
#define  set_cmd_4_PRSUL    0x11        // вкл выкл питания ПРСУ-Л

uint32_t diag_rec_status_ch[8];

/*
 * num_ch  - выбор приемного канала          0b00000000 - ни одного канала
 *                                           0b00000001 - 1 канал
 *                                           0b00000011 - 1 и 2 канал
 * freq_rage - используемый частотный диапазон
 * att - ослабление аттенюатора, итоговое ослабление K_dB = att * 4, max(att) = 0xE
 * lock - блокировка входа: 1-вкл, 0-выкл
 */
int8_t set_rec_mode(uint8_t set_cmd,MCSPI_Handle mcspi2_cs3, uint8_t num_ch, uint8_t freq_domain, uint8_t att, uint8_t lock, uint8_t flag20db_on);

/*
 * result_flag - флаг необходимости приема результатов диагностики
 */
int8_t rec_diagnostic(uint8_t set_cmd, uint8_t result_flag, MCSPI_Handle mcspi2_cs3);


/*
 * Включение/выключение питания каналов
 * num_ch  - выбор приемного канала          0b00000000 - ни одного канала
 *                                           0b00000001 - 1 канал
 *                                           0b00000011 - 1 и 2 канал
 */
int8_t on_off_rec_ch(uint8_t set_cmd,uint8_t num_ch,MCSPI_Handle mcspi2_cs3);


/*
 * Выбор частотного диапазона приемников
 */
int8_t pres_freq_range(uint32_t freq);
int8_t pres_freq_range_PRSUL(uint32_t freq);


#endif /* LORANDIT_FUNCTIONS_RECIEVER_RECIEVER_H_ */
