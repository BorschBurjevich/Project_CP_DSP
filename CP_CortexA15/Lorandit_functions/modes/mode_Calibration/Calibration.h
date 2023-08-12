/*
 * Calibration.h
 *
 *  Created on: 14 ����. 2020 �.
 *
 */

#ifndef CALIBRATION_CALIBRATION_H_
#define CALIBRATION_CALIBRATION_H_


#include"Lorandit_functions/Cod_com.h"
#include "gpio/gpio.h"
#include "SPI/SPI_header.h"

#define   RGM_IN_ANT1        0x08               //  ������� �� ������ ������ 1
#define   RGM_IN_ANT2        0x16               //  ������� �� ������ ������ 2
#define   RGM_IN_ANT3        0x18               //  ������� �� ������ ������ 3

#define   RGM_IN_CAL_ANT1    0x48               //  ���������� ������ 1
#define   RGM_IN_CAL_ANT2    0x50               //  ���������� ������ 2
#define   RGM_IN_CAL_ANT3    0x58               //  ���������� ������ 3

#define   RGM_IN_TEH_ANT1    0x28               //  ��� ������ ������ 1
#define   RGM_IN_TEH_ANT2    0x30               //  ��� ������ ������ 2
#define   RGM_IN_TEH_ANT3    0x38               //  ��� ������ ������ 3


extern void send_eth(SOCKET s, uint8_t *tx_buf,int size);

void Calc_Calibration_Coeff_UKV_Noise(uint32_t num_band, uint32_t k_max);
void Acumulation_Mutual_Spectrum_UKV_Noise(uint32_t bin_number,uint8_t flag);

void Set_Cell_Switchboard(uint8_t rgm_in, uint8_t value_att, uint8_t flag_over);           // ��������� ������ �����������
uint8_t selection_litera(uint32_t freq_in_mhz, uint8_t flag_now_aerial,uint8_t pr_work);
#endif /* CALIBRATION_CALIBRATION_H_ */
