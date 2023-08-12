#ifndef VARIABLES_USV_H_
#define VARIABLES_USV_H_

#include "Header.h"
#include "variables_USW.h"
#include <xdc/std.h>

//Пороги
const float      bound_detection_USW = 0.9;//  Порог корреляционного обнаружителя
const uint16_t   bound_max_distance_USW = 2;//  Разность между индексами сигнальных бинов
const float      bound_grouping_USW = 0.9;
const float      bound_noise_USW = 0.3;
const uint16_t   bound_min_adjacent_bin_USW = 2;
const uint16_t   bound_min_bin_signal_USW = 2;
uint16_t   count_bin;    // кол-во спектральных отсчётов
const uint8_t    count_read = 3;   // кол-во временных реализаций


const float Half_Band_POO  = 10.0;
const float bin_resolution_POO = 0.01953125;
const float POO_CurrentCentralFreq = 150.0;
const float Bound_D_Global_Max = 1.00;
const float Bound_Coun_It = 5;


/////////////////// Подыгрыш для отладки  /////////////////////
//////////////////////////////////////////////////////////////
////////// Входные комплексные спектральные отсчёты размерность Nb * Nr//////////

float ReS0[SIZE_S] = {
    #include "reS1.txt"
    };

float ImS0[SIZE_S] = {
    #include "imS1.txt"
    };
float ReS1[SIZE_S] = {
    #include "reS2.txt"
    };

float ImS1[SIZE_S] = {
    #include"imS2.txt"
    };
float ReS2[SIZE_S] = {
    #include"reS3.txt"
    };
float ImS2[SIZE_S] = {
    #include"imS3.txt"
    };

float ReS3[SIZE_S] = {
    #include"reS4.txt"
    };
float ImS3[SIZE_S] ={
    #include"imS4.txt"
    };
float ReS4[SIZE_S] = {
    #include"reS5.txt"
    };
float ImS4[SIZE_S] ={
    #include"imS5.txt"
    };



///////////////////////////////////////

////////////////////////Блок обнаружения и группирования/////////////////
/////////////////////////////////////////////////////////////////////////

#pragma DATA_SECTION (ReW_Bin, ".internal_data");
#pragma DATA_ALIGN(ReW_Bin, 4);
float   ReW_Bin[SIZE_NAC];// накопленные матрицы по  трём реализациям компл энергий, элементы соответсвуют номерам каналам

#pragma DATA_SECTION (ImW_Bin, ".internal_data");
#pragma DATA_ALIGN(ImW_Bin, 4);
float   ImW_Bin[SIZE_NAC];

float stat_detection;
float stat_grouping;



////////////   Выходные данные после блока обнаружения   //////////////
float central_Freq_Signal;
#pragma DATA_SECTION (ReW_Accumulation, ".internal_data");
#pragma DATA_ALIGN(ReW_Accumulation, 4);
float   ReW_Accumulation[SIZE];

#pragma DATA_SECTION (ImW_Accumulation, ".internal_data");
#pragma DATA_ALIGN(ImW_Accumulation, 4);
float   ImW_Accumulation[SIZE];

float end_Freq_Signal;
float begin_Freq_Signal;

//////////////////////////////////////////////////


//// Параметры содержащие струтуру данных  блока "Частотный диапазон"/////////
//считываются из Mas_Inform_AC в функции FUNC_Read_Params_Freqdiap_USW_5 //////////////
////////////////////////////////////////////////////////////////////////////

uint32_t c_freq;                 // количество частот
uint32_t c_azimuth;                  // колич-во азимутальных отсчётов
uint32_t c_channel;                  // количество каналов антенной системы
int ind_first;                 // индекс 1-ого элемента блока част. диап. в массиве Mas_AC
float Signal_Start_Freq;         // Начальная частота, Мгц
float Signal_Final_Freq;            // Конечная частота, Мгц
float F_start;
float F_stop;
float F_shag;
float azimuth_start;                // нач. знач. азимута, град
float azimuth_stop;             // конечное знач азимута, град
float azimuth_shag;             //шаг азимутыльных отсчётов, град


////////////////////////////////////////////////////////////////////////////////////

///////////////// Входные массивы на Пеленг ////////
/////////////////////////////////////////////////////

float Mas_AC[SIZE_MAS_AC] = {
    #include "Mas_Ac.txt"
    };                        // Массив юстировочных коэффициентов

float Mas_Inform_AC[SIZE_INF] = {
    #include "Mas_Inform_AC.txt"
    };       // Массив, содержащий информаацию о структуре данных и параметрах массива юстиров коэф


/////////////////////////////////////////////
////////////////////////////////////////////

float Fc;                       // Центральная частота, Мгц
/////////////// Выходные массивы после формирования действительных и мнимых частей юстир коэффициентов, и производных /////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma DATA_SECTION (ReU, ".internal_data");
#pragma DATA_ALIGN(ReU, 4);
float   ReU[SIZE_U];// массив действ. частей юстировочных коэф. для цетральной частоты Fc

#pragma DATA_SECTION (ImU, ".internal_data");
#pragma DATA_ALIGN(ImU, 4);
float   ImU[SIZE_U];// массив мнимых. частей юстировочных коэф. для цетральной частоты Fc

#pragma DATA_SECTION (D_ReU, ".internal_data");
#pragma DATA_ALIGN(D_ReU, 4);
float   D_ReU[SIZE_U];// массив действ. знач. производных юстировочных коэф. для цетральной частоты Fc

#pragma DATA_SECTION (D_ImU, ".internal_data");
#pragma DATA_ALIGN(D_ImU, 4);
float   D_ImU[SIZE_U];// массив мнимых. знач. производных юстировочных коэф. для цетральной частоты Fc


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////// Выходное значение функции FUNC_Cubic_Interpolation_USW_5l /////////////////////
float y;                        // выходное значение функции интерпол для частоты F_diff локально нормир на F_shag

/////////////////////////////////////////////////////////////////////////////

///////////// Выходные массивы после функции FUNC_Calculating_Direction_Finding_Relief /////////////////////
///////////////////////////////////////////////////////////////////////////////
#pragma DATA_SECTION (Mteta, ".internal_data");
#pragma DATA_ALIGN(Mteta, 4);
float   Mteta[SIZE_Theta];// массив значений для дискретной сетки азимутальных отсчётов

#pragma DATA_SECTION (D_Mteta, ".internal_data");
#pragma DATA_ALIGN(D_Mteta, 4);
float   D_Mteta[SIZE_Theta];// массив производных знач для дискретной сетки азим отсчётов

/////////////////////////////////////////////////////////////////////////
////////////////////////////////Iter_Poisk_Max  определение значения пеленга, оценки достоверности и амплитуды  ////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



float Mas_Vertex[2];                // массив с координатами вершин 2-х смежных дисретных отсчётов между которыми есть глоб максимум
float x;                            // азимутальный отсчёт
float coeff_reliability;            // Оценка достоверности
float max_value_Mtheta;         // максимальное значение дискретной сетки азимутальных отсчётов
float Amp_Azimuth_Mtheta;       // Амлитуда ИРИ  с азимутального отсчёта
float Amp_IRI_db;
float azimuth_Mtheta;           // значение азимута
float Mteta_x;
float D_Mteta_x;                    // производная знач. фунции для произвольного азимутального отсчёта
float TrW;                      // След матрицы взаимных энергий

float Mas_ReU_x[SIZE_VEC];      // массив действительных знач вектора U_x
float Mas_ImU_x[SIZE_VEC];      // массив мнимых знач вектора U_x
float Mas_D_ReU_x[SIZE_VEC];        // массив действительных знач вектора DU_x
float Mas_D_ImU_x[SIZE_VEC];        // массив мнимых знач вектора DU_x


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/////////////Замены для упрощения рассчётов//////////
/////////////////////////////////////////////////////
float  fletch;    // Замена используетса в функции  Rasch_Proizv_Znach_Vtochke( ) дла упрощениа делениа
float Znam;     // 1 / c_azimuth;

/////Выходной формуляр//////////
#pragma DATA_SECTION (Inform, ".internal_data");
#pragma DATA_ALIGN(Inform, 4);
Bearing Inform;

#endif /* VARIABLES_USV_H_ */
