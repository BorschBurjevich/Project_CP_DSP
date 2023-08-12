/*
 * Structs.h
 *
 *  Created on: 15 дек. 2020 г.
 *
 */

#ifndef LORANDIT_FUNCTIONS_STRUCTS_H_
#define LORANDIT_FUNCTIONS_STRUCTS_H_


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint16_t     fl[377];
    uint16_t     fh[377];
    uint8_t      n_ddc[377];
}bands_sruct;
extern bands_sruct bands;

typedef struct
{
    uint16_t fpga;
    uint16_t cpu;
}temperature_struct;
extern temperature_struct temperature;

typedef struct
{
    uint32_t n_bytes;  // колич-во байт в сообщении
    uint16_t command;  // код комманды
}hat_struct;
extern hat_struct hat;


typedef struct
{
    float   bin_freq[84];
    uint8_t   error[84];
    float   Re_K0[84];
    float   Im_K0[84];
    float   Re_K1[84];
    float   Im_K1[84];
    float   Re_K2[84];
    float   Im_K2[84];
    float   Re_K3[84];
    float   Im_K3[84];
    float   Re_K4[84];
    float   Im_K4[84];

}band_coeff;                    // калибровочные коэффициенты

extern float rsw0[84], isw0[84]; // размерность определяется как 21(коли - во колибровочный коээфициентов в полосе 2 МГц) * 4(каналы DDC))
extern float rsw1[84], isw1[84];
extern float rsw2[84], isw2[84];
extern float rsw3[84], isw3[84];
extern float rsw4[84], isw4[84];

extern float Re_Bin_Ch0[84], Im_Bin_Ch0[84];
extern float Re_Bin_Ch1[84], Im_Bin_Ch1[84];
extern float Re_Bin_Ch2[84], Im_Bin_Ch2[84];
extern float Re_Bin_Ch3[84], Im_Bin_Ch3[84];
extern float Re_Bin_Ch4[84], Im_Bin_Ch4[84];

extern short    rxvipBuf[24576];
typedef struct
{
    uint8_t  step;          //шаг калибровки
    uint8_t  auto_att;
    uint8_t  bkl_noise;     // признак калибровки по БК-Л или шуму
    uint16_t num_bands;     // количество полос
    uint16_t band_idx[377];
    uint8_t  att_prm[377];
    uint8_t  att_bk[393];
    uint32_t overflow[377]; // переполнение в режиме калибровка
    band_coeff calibr_coeff[377];
    uint8_t  mas_freq_range[377];
    uint8_t  n_ddc[377];
    uint16_t  left_freq[377];
}callibr_stuct;
extern callibr_stuct callibration;

typedef struct
{
    uint8_t  mas_freq_range[377];
}rcvr_channel;

typedef struct
{
    rcvr_channel  mas_channel[5];

}reciever_struct;
extern reciever_struct rcvr;


typedef struct
{
    uint8_t  ch_num;
    uint16_t num_bands;    // колич - во частотных диапазонов
    uint16_t band_idxs[377];
    uint8_t  att_prm[16];
    uint8_t  overflow;
    uint8_t  nddc[377];
    uint16_t mass_size;    // размер массива амплитуд
    uint8_t  mas_freq_range[377];


}spectrum_struct;
extern spectrum_struct spectrum;

extern uint16_t num_tresholds;
extern int tresholds[50];


typedef struct
{
    uint16_t num_bands;
    uint16_t band_idxs[377];
    uint8_t  att_prm[16];
    uint8_t  num_sectors;
    uint8_t  nddc[377];
    float    left_border[3];
    float    right_border[3];
    uint8_t  mas_freq_range[377];
    uint8_t  priznak_bearing;

}init_bearing_struct;
extern init_bearing_struct init_bear;

typedef struct
{
    uint16_t fft_points;
    uint16_t bins_tocut;
    uint16_t start_val;  //стартовое значение для обрезки спектра
    uint16_t stop_val;   //конец спектра для обрезки
    float    bin_width;
} fft_struct;

extern fft_struct useful_struct;

typedef struct
{
    float    azimuth;
    float    elevation;        // угол меаста
    float    central_Freq;
    float    width;            // ширина сигнального участка
    float    left_Freq;
    float    right_Freq;
    float    energy_db;        // амлитуда ИРИ с азимутального отчёта в дБ
    float    coeff_reliability;
    float    max_value_Mtheta; // максимальное значение диаграммы направленности
    float    reserved;
}bearing_inform_;

typedef struct
{
    uint16_t  band_idx;
    uint8_t   overflow;
    uint32_t  num_signals;
    bearing_inform_ inform[100];
}bearing_bands_;
extern bearing_bands_ bearing_bands[50];

typedef struct
{
    uint8_t   ch_num;
    uint8_t   set_num;
    uint16_t  time_on_disc;
    float     bw_disc;
    uint16_t  band_idx;
    uint8_t   num_freq_in_band;
    float     freq[256];
    uint8_t   overflow;
    uint8_t   freq_on_supression;
    uint8_t   freq_idxs[256];
    uint8_t   contact[256];
}fhss_time_struct;
extern fhss_time_struct fhss_time;

typedef struct
{
    uint32_t    mode_of_synch;
    uint32_t    shag;
    uint32_t    active_100ms;
    uint32_t    passive_100ms;
    uint32_t    active_tckp;
    uint32_t    passive_tckp;
    uint32_t    active_tcdkp;
    uint32_t    passive_tcdkp;
    uint32_t    active_200ms;
    uint32_t    passive_200ms;
}internal_synch_struct;
extern internal_synch_struct internal_synch;

typedef struct
{
    uint8_t  ch_num;
    float    cenrtal_freq;
    float    bw;
    uint8_t  amp;
    uint8_t  DMR_TETRA;

    int8_t   result;
    float    freq_new;
    float    bw_new;
    int8_t   signl_class;
    float    freq_on;
    float    freq_off;
    float    v_manipulation;
    int8_t   time;
}tech_analisys_struct;
extern tech_analisys_struct tech_analisys;

typedef struct
{
    float   Central_Freq_band;  //центральная частота полосы, МГц
    uint8_t modulation_format;  //вид модуляции
    float   bandpass;           //полоса пропускания

} mode_sk_struct;
extern mode_sk_struct acoustic_control;



typedef enum
{
    AUTO_TRESHOLD_UHF        = 0,// Признак использования автопорога
    ENERGY_HF,                   // Порог по энергии КВ
    ENERGY_UHF,                  // Порог по энергии УКВ
    RESOLUTION_HF,               // Разрешающая способносьт по частоте КВ
    RESOLUTION_UHF,              // Разрешающая способносьт по частоте УКВ
    DURATION_TIME_HF,            // Порог по длительности для исключения частоты из ГН  КВ
    DURATION_TIME_UHF,           // Порог по длительности для исключения частоты из ГН  УКВ
    INTENS_FHSS_HF,              // Порог по интенсивности для исключениа частоты из ГН ППРЧ диапазона УКВ
    INTENS_FHSS_UHF,             // Порог по интенсивности для исключениа частоты из ГН ППРЧ диапазона КВ
    DURATION_TIME_BACK_HF,       // Порог длительности через который исключеннаа частота включаетса обратно в  ГН ППРЧ диапазона УКВ
    DURATION_TIME_BACK_UHF,      // Порог длительности через который исключеннаа частота включаетса обратно в  ГН ППРЧ диапазона КВ
    PHASE_TRASHOLD_UHF1,         // Фазовый порог обнаружения УКВ литера 1
    PHASE_TRASHOLD_UHF2,         // Фазовый порог обнаружения УКВ литера 2
    PHASE_TRASHOLD_HF,           // Фазовый порог обнаружения КВ
    CORRECT_ANTENNA_HF,          // Поправка антенны КВ диапазона
    CORRECT_ANTENNA_UHF1,        // Поправка антенны УКВ1 диапазона
    CORRECT_ANTENNA_UHF2,        // Поправка антенны УКВ2 диапазона
    AUTO_ATT,                    // Автоттенюатор
    ATT_HF,                      // Аттенюатор КВ
    ATT_UHF,                     // Аттенюатор УКВ
    NUM_STATES,                  // Колличесвто состояний на ПОО(коэфф. замедления)
    SYNCH_BLOCK,                 // Признак использования ячейки синхронизации
    BEARING_WITH_SPECTRUM,       // Признак использования пеленга со спектром
    SPECTRUM_STEP,               // Шаг спектра
    NUM_POO,                     // Количество ПОО для пеленга
}tresholds_enum;

typedef struct
{
    uint8_t auto_tresholds_uhf;
    int32_t energy_hf;
    int32_t energy_uhf;
    int32_t resolution_hf;
    int32_t resolution_uhf;
    int32_t duration_time_hf;
    int32_t duration_time_uhf;
    int32_t intens_fhss_hf;
    int32_t intens_fhss_uhf;
    int32_t duration_time_back_hf;
    int32_t duration_time_back_uhf;
    float   phase_trasholds_uhf1;
    float   phase_trasholds_uhf2;
    float   phase_trasholds_hf;
    float   correct_antenna_hf;
    float   correct_antenna_uhf1;
    float   correct_antenna_uhf2;
    uint8_t auto_att;
    int8_t  att_hf;
    int8_t  att_uhf;
    int32_t num_states;
    uint8_t synch_block;
    uint8_t bearing_with_spectrum;
    uint16_t spectrum_step;
    uint16_t num_poo;

}tresholds_struct;

extern tresholds_struct tresholds_item;

typedef enum
{
    ticet_                   = 0x01,
    error_                   = 0x02,
    version_                 = 0x03,
    boot_                    = 0x04,

    init_calibration_ukv_    = 0x05,
    start_calibration_ukv_   = 0x06,
    stop_calibration_ukv_    = 0x07,

    init_spectrum_           = 0x0b,
    start_spectrum_          = 0x0c,
    stop_spectrum_           = 0x0d,

    tresholds_               = 0x0e,

    init_bearing_            = 0x0f,
    start_bearing_           = 0x10,
    stop_bearing_            = 0x11,

    init_fch_                = 0x12,
    result_control_fch_      = 0x13,

    init_fhss_               = 0x14,
    result_control_fhss_     = 0x15,

    hand_supression_         = 0x16,

    start_RP_                = 0x17,
    stop_RP_                 = 0x18,

    init_fhss_duration_      = 0x19,
    start_fhss_duration_     = 0x1A,
    stop_fhss_duration_      = 0x1B,

    init_tech_analisys_      = 0x1C,
    start_tech_analisys_     = 0x1D,
    stop_tech_analisys_      = 0x1E,

    init_SK_                 = 0xA0,
    start_SK_                = 0xA1,
    stop_SK_                 = 0xA2,

    start_control_           = 0xBB,


}commands;

typedef enum
{
    nosynchrojob_             = 0x0,   // пеленг без синхры
    bearing_in_recon_         = 0x1,   // пеленг в доразведки
    bearing_in_control_       = 0x2,   // пеленг по контролю
    bearing_all_in_           = 0x3,   // пеленг all in
}flag_enum;


#endif /* LORANDIT_FUNCTIONS_STRUCTS_H_ */
