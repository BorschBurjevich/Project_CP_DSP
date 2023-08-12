#ifndef VARIABLES_SW_H_
#define VARIABLES_SW_H_

#include <xdc/std.h>
#include "DEFINES_SW.h"
#include "variables_SW.h"
#include "defines.h"
/////////////////////// Пороги /////////////////
///////////////////////////////////////////////
const float bound_detection_SW = 0.8;
const float bound_max_distance_SW = 5;
const float bound_grouping_SW = 0.8;
const float bound_noise_SW = 0.3;
const float bound_min_adjacent_bin_SW = 2.0;
const float bound_min_bin_signal_SW = 2.0;

const float Half_Band_POO_SW  = 9.0;                 /*fd ПЛИС в МГц*/
const float bin_resolution_POO_SW = 0.09;
const float POO_CurrentCentralFreq_SW = 95.0;




/////////////////// Подыгрыш для отладки  /////////////////////
//////////////////////////////////////////////////////////////
////////// Входные комплексные спектральные отсчёты размерность Nb * Nr//////////

float ReS0_SW[SIZE_S_SW] ={
                     #include "ReS0.txt"
};

float ImS0_SW[SIZE_S_SW] ={
                     #include "ImS0.txt"
};
float ReS1_SW[SIZE_S_SW] ={
                     #include "ReS1.txt"
};
float ImS1_SW[SIZE_S_SW] = {
                      #include "ImS1.txt"
};
float ReS2_SW[SIZE_S_SW] = {
                      #include "ReS2.txt"
};
float ImS2_SW[SIZE_S_SW] = {
                      #include "ImS2.txt"
};
float ReS3_SW[SIZE_S_SW] = {
                      #include "ReS3.txt"
};
float ImS3_SW[SIZE_S_SW] = {
                      #include "ImS3.txt"
};

const uint8_t    count_read_SW = 3;
uint16_t    count_bin_SW;


#pragma DATA_SECTION (ReW_Bin_SW, ".internal_data");
#pragma DATA_ALIGN(ReW_Bin_SW, 4);
float   ReW_Bin_SW[SIZE_NAC_SW];

#pragma DATA_SECTION (ImW_Bin_SW, ".internal_data");
#pragma DATA_ALIGN(ImW_Bin_SW, 4);
float   ImW_Bin_SW[SIZE_NAC_SW];



float    stat_detection_SW;
float    stat_grouping_SW;

float central_Freq_Signal_SW;

#pragma DATA_SECTION (ReW_Accumulation_SW, ".internal_data");
#pragma DATA_ALIGN(ReW_Accumulation_SW, 4);
float   ReW_Accumulation_SW[SIZE_ACCUM];

#pragma DATA_SECTION (ImW_Accumulation_SW, ".internal_data");
#pragma DATA_ALIGN(ImW_Accumulation_SW, 4);
float   ImW_Accumulation_SW[SIZE_ACCUM];

#pragma DATA_SECTION (mas_ReQ, ".internal_data");
#pragma DATA_ALIGN(mas_ReQ, 4);
float   mas_ReQ[SIZE_Q];

#pragma DATA_SECTION (mas_ImQ, ".internal_data");
#pragma DATA_ALIGN(mas_ImQ, 4);
float   mas_ImQ[SIZE_Q];

#pragma DATA_SECTION (mas_ReW, ".internal_data");
#pragma DATA_ALIGN(mas_ReW, 4);
float   mas_ReW[SIZE_SW];

#pragma DATA_SECTION (mas_ImW, ".internal_data");
#pragma DATA_ALIGN(mas_ImW, 4);
float   mas_ImW[SIZE_SW];

float   end_Freq_Signal_SW;
float   begin_Freq_Signal_SW;
//////////////////////////////////////////////////


float Mas_AC_Loop[SIZE_MAS_AC_SW] = {
                                  #include "Mas_AC_Loop.txt"
};
float Mas_Inform_AC_Loop[SIZE_INF] ={
                                  #include "Mas_Inform_AC_Loop.txt"
};

float Fc_SW;
float F_start_SW;
float F_stop_SW;
float F_shag_SW;
int   c_azimuth_SW;
int   c_channel_SW;
float ind_start_SW;
float c_frequency_SW;

#pragma DATA_SECTION (ReU_SW, ".internal_data");
#pragma DATA_ALIGN(ReU_SW, 4);
float   ReU_SW[SIZE_U_SW];

#pragma DATA_SECTION (ReU_SW, ".internal_data");
#pragma DATA_ALIGN(ImU_SW, 4);
float   ImU_SW[SIZE_U_SW];

float value_interpolation_SW;

#pragma DATA_SECTION (Mtheta_SW, ".internal_data");
#pragma DATA_ALIGN(Mtheta_SW, 4);
float   Mtheta_SW[SIZE_THETA];

#pragma DATA_SECTION (znam_Mtheta_SW, ".internal_data");
#pragma DATA_ALIGN(znam_Mtheta_SW, 4);
float   znam_Mtheta_SW[SIZE_THETA];

int     ind_max_2;
int     ind_max_1;

float    dy;
float    xL;
float    xR;
float    yL;
float    yR;
float    dyL;
float    dyR;

float    y_SW;
float    max_value_SDN;

float    AMP_IRI_dB_SW;
float    coeff_reability_SW;
float    azimuth_Mtheta_SW;

float    deltaP;

/////Выходной формуляр//////////
#pragma DATA_SECTION (Inform_SW, ".internal_data");
#pragma DATA_ALIGN(Inform_SW, 4);

Bearing_SW Inform_SW;

#endif /* VARIABLES_SW_H_ */
