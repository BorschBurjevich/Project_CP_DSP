/*
 * targets.h
 *
 *  Created on: 14 мая 2021 г.
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


#define MAX_PP          (4U)//число ПП
#define MAX_NONCONTROL  (MAX_PP*4)//Число передатчиков на макс скважность
#define MAX_CHANNELS    (5U)//число каналов (8U)
#define MAX_ONCONTROL   (50U)//макс число сигналов на контроль на полосу
#define MAX_GUARDED     (50U)//максимльное число подавляемых для охраняемых частот
#define MAX_BANDS       (5U)//макс число полос на канал
#define MAX_INTERVALS   (10U)//число интервалов К+П после которого отдается статистика на АРМ
#define MAX_FREQS       (1000U)//максимальное число ИРИ

///
#define MAX_FREQS_ON_PP (50)
#define NUM_PULS_DISCR  (3U) // число возможных импульсов дискретизации
//

//структура для сигнала с контролем
typedef struct{
    uint32_t        id;                         // индентиф ИРИ
    uint8_t         pp_idx;                     // номер П.П.
    uint32_t        freq_left;//в кГц
    uint32_t        freq_right;//в кГц
    uint32_t        freq_rp;//в кГц
    modulation_type type;
    uint8_t         discr;//длительность дискретизации в мкс
    uint8_t         Q;    // макс возможная скважность
    uint32_t        param1;
    uint32_t        param2;
    uint8_t         guard;              // признак охраняемой частоты
    uint32_t        total_guarded;      // количество охраняемых
    uint32_t        guarded_id[MAX_GUARDED];
    uint32_t        priority;           // приоритет
    float           coeff;              // коэффициент загрузки
}oncontrol_;

//структура с параметрами для полосы
typedef struct{
    uint16_t band_idx;
    uint8_t  total_signals;                         // количество контролируемых частот с ФЧ
    oncontrol_ oncontrol[MAX_ONCONTROL];
}bands_;

//структура с параметрами для канала
typedef struct{
    uint8_t  ch_id;
    uint8_t  total_bands;
    bands_   bands[MAX_BANDS];
}channel_;

typedef struct{
    uint32_t        id;
    uint32_t        freq_rp;
    modulation_type type;
    uint8_t         discr;//длительность димернтизации в мкс
    uint8_t         Q;
    uint32_t        param1;
    uint32_t        param2;
}signals_;

//структура с параметрами для сигнала без контроля
typedef struct{
    uint8_t      pp_id;//номер пп
    uint8_t      litera_id;//номер литеры
    uint16_t     total_signals;//кол-во частот без контроля
    signals_     signals[MAX_NONCONTROL];
}noncontrol_;

typedef struct{
    uint32_t    count_signals[4];
} signals_in_ddc;

//общая структура на инит режима
typedef struct{
    uint32_t    cr_id;//идентификатор ЦР
    uint8_t     power_pp;//уровень мощности ПП
    uint8_t     max_Q;//макс скважность
    uint8_t     num_pp;//кол-во передатчиков помех
    noncontrol_ nonctrl[MAX_PP];//параметры для частот без контроля
    uint8_t     total_channels;//число каналов рр
    channel_    channel[MAX_CHANNELS];
    uint16_t    Max_POO_in_Channel;
    uint32_t    num_intervals;                  // колич - во интервалов К+П
    signals_in_ddc  signals_ddc[377]; //колич-во сигналов для каждого ddc в полосе

}fch_targets_;


//общий список сигналов с параметрами и сигнальная статистика
typedef struct{
    uint32_t    id;
   // uint32_t    num_intervals;                  // колич - во интервалов К+П
    uint8_t     is_active[MAX_INTERVALS];       // частота в эфире
    uint8_t     on_supression[MAX_INTERVALS];
    uint8_t     on_guard[MAX_INTERVALS];

    //параметры для сортировок
    uint8_t     guard;                   // признак охраняемой
    uint32_t    total_guarded;           // всего охраняемых  (колич-во подавляемых частот для охраняемой)
    uint32_t    guard_idxs[MAX_GUARDED]; // индекс подавляемого



    //параметры для пп
    uint8_t     pp_idx; // номер передатчика помех для сигнала
    uint32_t    priority;
    float       coef;
    uint32_t    freq_rp;

    modulation_type type;
    uint8_t         discr;//длительность дискретизации в мкс
    uint8_t         Q;
    uint32_t        param1;
    uint32_t        param2;
}total_signals_;

//структура для передатчика на подавление
typedef struct
{
    uint32_t    count;
    uint32_t    signal_id[MAX_FREQS];
}pp_;

typedef struct
{
    uint8_t cyclogram;//тип циклограммы
    uint8_t number;//номер циклограммы
    uint8_t dorazv;//наличие доразведки

    uint32_t duration;
}start_supression_;

void init_fch();
void result_fch();
void start_rp();
void start_control();

void result_fch(void);

#endif /* LORANDIT_FUNCTIONS_SUPRESSION_TARGETS_FCH_H_ */
