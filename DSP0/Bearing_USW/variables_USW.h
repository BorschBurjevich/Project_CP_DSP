

#ifndef  VARIABLES_USW_H
#define  VARIABLES_USW_H

#include "Header.h"
#include <xdc/std.h>
#include <stdio.h>
#include <string.h>
#include "defines.h"
/////////////////////// Пороги /////////////////
/////////////////////////////////////////////// 
extern const float bound_detection_USW;
extern const uint16_t bound_max_distance_USW;
extern const float bound_grouping_USW;
extern const float bound_noise_USW;
extern const uint16_t bound_min_adjacent_bin_USW;
extern const uint16_t  bound_min_bin_signal_USW;

extern const float Half_Band_POO;
extern const float bin_resolution_POO;
extern const float POO_CurrentCentralFreq;
extern const float Bound_D_Global_Max;
extern const float Bound_Coun_It;



///////////////////////////////////////////////////////
/// Могут изменяться, задаются оператором за АРМ //

extern const uint8_t count_read;   // кол-во временных реализаций
extern uint16_t count_bin;    // кол-во спектральных отсчётов

///////////////////////////////////////////
////////////////////////////////////////////


/////////////////// Подыгрыш для отладки  /////////////////////
//////////////////////////////////////////////////////////////
////////// Входные комплексные спектральные отсчёты размерность Nb * Nr//////////

extern float ReS0[SIZE_S];
extern float ImS0[SIZE_S];
extern float ReS1[SIZE_S];
	
extern float ImS1[SIZE_S];
extern float ReS2[SIZE_S];
extern float ImS2[SIZE_S];
extern float ReS3[SIZE_S];
extern float ImS3[SIZE_S];
extern float ReS4[SIZE_S];
extern float ImS4[SIZE_S];



///////////////////////////////////////

////////////////////////////////////////
////////////////////////////////////////


////////////////////////Блок обнаружения и группирования/////////////////
/////////////////////////////////////////////////////////////////////////
extern float ReW_Bin[SIZE_NAC];        // накопленные матрицы по  трём реализациям компл энергий, элементы соответсвуют номерам каналам
extern float ImW_Bin[SIZE_NAC];
extern float stat_detection;
extern float stat_grouping;



////////////   Выходные данные после блока обнаружения   //////////////
extern float central_Freq_Signal;
extern float ReW_Accumulation[SIZE];
extern float ImW_Accumulation[SIZE];
extern float end_Freq_Signal;
extern float begin_Freq_Signal;

//////////////////////////////////////////////////




//// Параметры содержащие струтуру данных  блока "Частотный диапазон"///////// 
//считываются из Mas_Inform_AC в функции FUNC_Read_Params_Freqdiap_USW_5 //////////////
////////////////////////////////////////////////////////////////////////////

extern uint32_t c_freq;					// количество частот
extern uint32_t c_azimuth;					// колич-во азимутальных отсчётов
extern uint32_t c_channel;					// количество каналов антенной системы
extern int ind_first;                 // индекс 1-ого элемента блока част. диап. в массиве Mas_AC
extern float Signal_Start_Freq;         // Начальная частота, Мгц
extern float Signal_Final_Freq;			// Конечная частота, Мгц
extern float F_start;
extern float F_stop;
extern float F_shag;
extern float azimuth_start;				// нач. знач. азимута, град
extern float azimuth_stop;				// конечное знач азимута, град
extern float azimuth_shag;				//шаг азимутыльных отсчётов, град


////////////////////////////////////////////////////////////////////////////////////


///////////////// Входные массивы на Пеленг ////////
/////////////////////////////////////////////////////

extern float Mas_AC[SIZE_MAS_AC];                        // Массив юстировочных коэффициентов

extern float Mas_Inform_AC[SIZE_INF];       // Массив, содержащий информаацию о структуре данных и параметрах массива юстиров коэф


/////////////////////////////////////////////
////////////////////////////////////////////

extern float Fc;						// Центральная частота, Мгц
							

/////////////// Выходные массивы после формирования действительных и мнимых частей юстир коэффициентов, и производных /////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern float ReU[SIZE_U];				// массив действ. частей юстировочных коэф. для цетральной частоты Fc
extern float ImU[SIZE_U];				// массив мнимых. частей юстировочных коэф. для цетральной частоты Fc
extern float D_ReU[SIZE_U];			// массив действ. знач. производных юстировочных коэф. для цетральной частоты Fc
extern float D_ImU[SIZE_U];			// массив мнимых. знач. производных юстировочных коэф. для цетральной частоты Fc

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////// Выходное значение функции FUNC_Cubic_Interpolation_USW_5l /////////////////////
extern float y;						// выходное значение функции интерпол для частоты F_diff локально нормир на F_shag

/////////////////////////////////////////////////////////////////////////////


///////////// Выходные массивы после функции FUNC_Calculating_Direction_Finding_Relief /////////////////////
///////////////////////////////////////////////////////////////////////////////						
extern float Mteta[SIZE_Theta];			// массив значений для дискретной сетки азимутальных отсчётов
extern float D_Mteta[SIZE_Theta];			// массив производных знач для дискретной сетки азим отсчётов
        
/////////////////////////////////////////////////////////////////////////




////////////////////////////////Iter_Poisk_Max  определение значения пеленга, оценки достоверности и амплитуды  ////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



extern float Mas_Vertex[2];				// массив с координатами вершин 2-х смежных дисретных отсчётов между которыми есть глоб максимум
extern float x;							// азимутальный отсчёт
extern float coeff_reliability;			// Оценка достоверности
extern float max_value_Mtheta;			// максимальное значение дискретной сетки азимутальных отсчётов
extern float Amp_Azimuth_Mtheta;		// Амлитуда ИРИ  с азимутального отсчёта, дБ
extern float Amp_IRI_db;
extern float azimuth_Mtheta;			// значение азимута
extern float Mteta_x;					// значение фунции для произвольного азимутального отсчёта
extern float D_Mteta_x;					// производная знач. фунции для произвольного азимутального отсчёта
extern float TrW;						// След матрицы взаимных энергий
extern float Mas_ReU_x[SIZE_VEC];		// массив действительных знач вектора U_x
extern float Mas_ImU_x[SIZE_VEC];		// массив мнимых знач вектора U_x
extern float Mas_D_ReU_x[SIZE_VEC];		// массив действительных знач вектора DU_x
extern float Mas_D_ImU_x[SIZE_VEC];		// массив мнимых знач вектора DU_x


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct
{
    float azimuth;
    float elevation;
    float central_Freq;
    float width;
    float left_Freq;
    float right_Freq;
    float energy_db;
    float coeff_reliability;
    float max_value_Mtheta;
    float reserved;
}Bearing_Inform_;

typedef struct
{
    uint32_t        num_signals;
    Bearing_Inform_ signal[SIZE_FORM];
}Bearing;
extern Bearing Inform;
/////////////////////////////////////////////////////

///////////////////////////////////////////////////



/////////////Замены для упрощения рассчётов//////////
/////////////////////////////////////////////////////
extern float fletch;    // Замена используетса в функции  Rasch_Proizv_Znach_Vtochke( ) дла упрощениа делениа
extern float Znam;     // 1 / c_azimuth;



////////////////////////////////////////////////////

#endif /*VARIABLES_USW_H*/
