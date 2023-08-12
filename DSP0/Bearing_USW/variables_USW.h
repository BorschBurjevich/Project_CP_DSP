

#ifndef  VARIABLES_USW_H
#define  VARIABLES_USW_H

#include "Header.h"
#include <xdc/std.h>
#include <stdio.h>
#include <string.h>
#include "defines.h"
/////////////////////// ������ /////////////////
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
/// ����� ����������, �������� ���������� �� ��� //

extern const uint8_t count_read;   // ���-�� ��������� ����������
extern uint16_t count_bin;    // ���-�� ������������ ��������

///////////////////////////////////////////
////////////////////////////////////////////


/////////////////// �������� ��� �������  /////////////////////
//////////////////////////////////////////////////////////////
////////// ������� ����������� ������������ ������� ����������� Nb * Nr//////////

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


////////////////////////���� ����������� � �������������/////////////////
/////////////////////////////////////////////////////////////////////////
extern float ReW_Bin[SIZE_NAC];        // ����������� ������� ��  ��� ����������� ����� �������, �������� ������������ ������� �������
extern float ImW_Bin[SIZE_NAC];
extern float stat_detection;
extern float stat_grouping;



////////////   �������� ������ ����� ����� �����������   //////////////
extern float central_Freq_Signal;
extern float ReW_Accumulation[SIZE];
extern float ImW_Accumulation[SIZE];
extern float end_Freq_Signal;
extern float begin_Freq_Signal;

//////////////////////////////////////////////////




//// ��������� ���������� �������� ������  ����� "��������� ��������"///////// 
//����������� �� Mas_Inform_AC � ������� FUNC_Read_Params_Freqdiap_USW_5 //////////////
////////////////////////////////////////////////////////////////////////////

extern uint32_t c_freq;					// ���������� ������
extern uint32_t c_azimuth;					// �����-�� ������������ ��������
extern uint32_t c_channel;					// ���������� ������� �������� �������
extern int ind_first;                 // ������ 1-��� �������� ����� ����. ����. � ������� Mas_AC
extern float Signal_Start_Freq;         // ��������� �������, ���
extern float Signal_Final_Freq;			// �������� �������, ���
extern float F_start;
extern float F_stop;
extern float F_shag;
extern float azimuth_start;				// ���. ����. �������, ����
extern float azimuth_stop;				// �������� ���� �������, ����
extern float azimuth_shag;				//��� ������������ ��������, ����


////////////////////////////////////////////////////////////////////////////////////


///////////////// ������� ������� �� ������ ////////
/////////////////////////////////////////////////////

extern float Mas_AC[SIZE_MAS_AC];                        // ������ ������������ �������������

extern float Mas_Inform_AC[SIZE_INF];       // ������, ���������� ����������� � ��������� ������ � ���������� ������� ������� ����


/////////////////////////////////////////////
////////////////////////////////////////////

extern float Fc;						// ����������� �������, ���
							

/////////////// �������� ������� ����� ������������ �������������� � ������ ������ ����� �������������, � ����������� /////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern float ReU[SIZE_U];				// ������ ������. ������ ������������ ����. ��� ���������� ������� Fc
extern float ImU[SIZE_U];				// ������ ������. ������ ������������ ����. ��� ���������� ������� Fc
extern float D_ReU[SIZE_U];			// ������ ������. ����. ����������� ������������ ����. ��� ���������� ������� Fc
extern float D_ImU[SIZE_U];			// ������ ������. ����. ����������� ������������ ����. ��� ���������� ������� Fc

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////// �������� �������� ������� FUNC_Cubic_Interpolation_USW_5l /////////////////////
extern float y;						// �������� �������� ������� �������� ��� ������� F_diff �������� ������ �� F_shag

/////////////////////////////////////////////////////////////////////////////


///////////// �������� ������� ����� ������� FUNC_Calculating_Direction_Finding_Relief /////////////////////
///////////////////////////////////////////////////////////////////////////////						
extern float Mteta[SIZE_Theta];			// ������ �������� ��� ���������� ����� ������������ ��������
extern float D_Mteta[SIZE_Theta];			// ������ ����������� ���� ��� ���������� ����� ���� ��������
        
/////////////////////////////////////////////////////////////////////////




////////////////////////////////Iter_Poisk_Max  ����������� �������� �������, ������ ������������� � ���������  ////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



extern float Mas_Vertex[2];				// ������ � ������������ ������ 2-� ������� ��������� �������� ����� �������� ���� ���� ��������
extern float x;							// ������������ ������
extern float coeff_reliability;			// ������ �������������
extern float max_value_Mtheta;			// ������������ �������� ���������� ����� ������������ ��������
extern float Amp_Azimuth_Mtheta;		// �������� ���  � ������������� �������, ��
extern float Amp_IRI_db;
extern float azimuth_Mtheta;			// �������� �������
extern float Mteta_x;					// �������� ������ ��� ������������� ������������� �������
extern float D_Mteta_x;					// ����������� ����. ������ ��� ������������� ������������� �������
extern float TrW;						// ���� ������� �������� �������
extern float Mas_ReU_x[SIZE_VEC];		// ������ �������������� ���� ������� U_x
extern float Mas_ImU_x[SIZE_VEC];		// ������ ������ ���� ������� U_x
extern float Mas_D_ReU_x[SIZE_VEC];		// ������ �������������� ���� ������� DU_x
extern float Mas_D_ImU_x[SIZE_VEC];		// ������ ������ ���� ������� DU_x


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



/////////////������ ��� ��������� ���������//////////
/////////////////////////////////////////////////////
extern float fletch;    // ������ ������������ � �������  Rasch_Proizv_Znach_Vtochke( ) ��� ��������� �������
extern float Znam;     // 1 / c_azimuth;



////////////////////////////////////////////////////

#endif /*VARIABLES_USW_H*/
