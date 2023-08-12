 /*
 * Lorandit_functions.h
 *
 *  Created on: 13 июл. 2020 г.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ti/ndk/inc/netmain.h>
#include <SPI/SPI_header.h>
#include "Lorandit_functions/Cod_com.h"
#include <ti/drv/i2c/soc/I2C_soc.h>
//#include <ti/drv/i2c/test/eeprom_read/src/I2C_log.h>
#include "I2C/I2C.h"
#include "gpio/gpio.h"
#include "Shared_mem_communication/Shared.h"


#ifndef LORANDIT_FUNCTIONS_LORANDIT_FUNCTIONS_H_
#define LORANDIT_FUNCTIONS_LORANDIT_FUNCTIONS_H_



//глобальные переменные
extern    uint16_t       command;
extern    SOCKET         arm;
extern    uint8_t        pBuf[];
extern    uint32_t       file_boot_loader[];
extern    uint32_t       size;//размер файла
extern    uint32_t       length;//число блоков по 511


//объ€вление функций
extern void Start_data();
extern void send_eth_error(SOCKET s, uint32_t error);
extern void write_app_to_emmc(void);
extern void write_mlo_to_emmc(void);
extern void send_eth(SOCKET s, uint8_t *tx_buf,int size);
void boot_file();

void init_calibration_ukv(void);
int8_t start_calibration_ukv(uint32_t indx);
void calibration_result_ukv(uint32_t ind, uint16_t old_freq_range);
uint32_t get_board_temp(I2C_Handle h);
void init_spectr(void);
int8_t start_spectr_multi(uint32_t indx);
int8_t start_spectr_single_ch(uint32_t indx);
void error_chek(uint32_t pcie_buf_chek);
void tresholds_func(void);
void init_bearing(void);
int8_t start_bearing(uint32_t indx);
void init_tech_analisys();
void sliser(int32_t *j10, int32_t *j12, int32_t *j13);
void send_sysref(SPI_Handle h);
extern void config_ADC(uint8_t num_DDC, uint32_t left_fr,uint16_t n_fft,uint8_t channel);
void  func_cut_the_array(float* mas_in, uint8_t* mas_out, uint16_t size, int val_bin);

void init_mode_SK();
void start_mode_SK();
#endif /* LORANDIT_FUNCTIONS_LORANDIT_FUNCTIONS_H_ */
