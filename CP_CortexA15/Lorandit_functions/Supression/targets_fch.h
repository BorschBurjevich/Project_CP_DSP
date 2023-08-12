/*
 * targets.h
 *
 *  Created on: 14 ��� 2021 �.
 *      Author: Sergey_Trushin
 */

#ifndef LORANDIT_FUNCTIONS_SUPRESSION_TARGETS_FCH_H_
#define LORANDIT_FUNCTIONS_SUPRESSION_TARGETS_FCH_H_

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdbool.h>
#include "Lorandit_functions/Interference_Txs/Interference_Txs.h"
#include "Lorandit_functions/modes/modes.h"
#include "Lorandit_functions/Structs.h"
#include "Lorandit_functions/reciever/reciever.h"
#include"Lorandit_functions/modes/mode_Calibration/Calibration.h"


#define MAX_PP          (4U)//����� ��
#define MAX_NONCONTROL  (MAX_PP*4)//����� ������������ �� ���� ����������
#define MAX_CHANNELS    (5U)//����� ������� (8U)
#define MAX_ONCONTROL   (50U)//���� ����� �������� �� �������� �� ������
#define MAX_GUARDED     (50U)//����������� ����� ����������� ��� ���������� ������
#define MAX_BANDS       (5U)//���� ����� ����� �� �����
#define MAX_INTERVALS   (10U)//����� ���������� �+� ����� �������� �������� ���������� �� ���
#define MAX_FREQS       (1000U)//������������ ����� ���

///
#define MAX_FREQS_ON_PP (50)
#define NUM_PULS_DISCR  (3U) // ����� ��������� ��������� �������������
//

//��������� ��� ������� � ���������
typedef struct{
    uint32_t        id;                         // �������� ���
    uint8_t         pp_idx;                     // ����� �.�.
    uint32_t        freq_left;//� ���
    uint32_t        freq_right;//� ���
    uint32_t        freq_rp;//� ���
    modulation_type type;
    uint8_t         discr;//������������ ������������� � ���
    uint8_t         Q;    // ���� ��������� ����������
    uint32_t        param1;
    uint32_t        param2;
    uint8_t         guard;              // ������� ���������� �������
    uint32_t        total_guarded;      // ���������� ����������
    uint32_t        guarded_id[MAX_GUARDED];
    uint32_t        priority;           // ���������
    float           coeff;              // ����������� ��������
}oncontrol_;

//��������� � ����������� ��� ������
typedef struct{
    uint16_t band_idx;
    uint8_t  total_signals;                         // ���������� �������������� ������ � ��
    oncontrol_ oncontrol[MAX_ONCONTROL];
}bands_;

//��������� � ����������� ��� ������
typedef struct{
    uint8_t  ch_id;
    uint8_t  total_bands;
    bands_   bands[MAX_BANDS];
}channel_;

typedef struct{
    uint32_t        id;
    uint32_t        freq_rp;
    modulation_type type;
    uint8_t         discr;//������������ ������������� � ���
    uint8_t         Q;
    uint32_t        param1;
    uint32_t        param2;
}signals_;

//��������� � ����������� ��� ������� ��� ��������
typedef struct{
    uint8_t      pp_id;//����� ��
    uint8_t      litera_id;//����� ������
    uint16_t     total_signals;//���-�� ������ ��� ��������
    signals_     signals[MAX_NONCONTROL];
}noncontrol_;

typedef struct{
    uint32_t    count_signals[4];
} signals_in_ddc;

//����� ��������� �� ���� ������
typedef struct{
    uint32_t    cr_id;//������������� ��
    uint8_t     power_pp;//������� �������� ��
    uint8_t     max_Q;//���� ����������
    uint8_t     num_pp;//���-�� ������������ �����
    noncontrol_ nonctrl[MAX_PP];//��������� ��� ������ ��� ��������
    uint8_t     total_channels;//����� ������� ��
    channel_    channel[MAX_CHANNELS];
    uint16_t    Max_POO_in_Channel;
    uint32_t    num_intervals;                  // ����� - �� ���������� �+�
    signals_in_ddc  signals_ddc[377]; //�����-�� �������� ��� ������� ddc � ������

}fch_targets_;


//����� ������ �������� � ����������� � ���������� ����������
typedef struct{
    uint32_t    id;
   // uint32_t    num_intervals;                  // ����� - �� ���������� �+�
    uint8_t     is_active[MAX_INTERVALS];       // ������� � �����
    uint8_t     on_supression[MAX_INTERVALS];
    uint8_t     on_guard[MAX_INTERVALS];

    //��������� ��� ����������
    uint8_t     guard;                   // ������� ����������
    uint32_t    total_guarded;           // ����� ����������  (�����-�� ����������� ������ ��� ����������)
    uint32_t    guard_idxs[MAX_GUARDED]; // ������ ������������



    //��������� ��� ��
    uint8_t     pp_idx; // ����� ����������� ����� ��� �������
    uint32_t    priority;
    float       coef;
    uint32_t    freq_rp;

    modulation_type type;
    uint8_t         discr;//������������ ������������� � ���
    uint8_t         Q;
    uint32_t        param1;
    uint32_t        param2;
}total_signals_;

//��������� ��� ����������� �� ����������
typedef struct
{
    uint32_t    count;
    uint32_t    signal_id[MAX_FREQS];
}pp_;

typedef struct
{
    uint8_t cyclogram;//��� �����������
    uint8_t number;//����� �����������
    uint8_t dorazv;//������� ����������

    uint32_t duration;
}start_supression_;

void init_fch();
void result_fch();
void start_rp();
void start_control();

void result_fch(void);

#endif /* LORANDIT_FUNCTIONS_SUPRESSION_TARGETS_FCH_H_ */
