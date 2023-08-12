#ifndef VARIABLES_USV_H_
#define VARIABLES_USV_H_

#include "Header.h"
#include "variables_USW.h"
#include <xdc/std.h>

//������
const float      bound_detection_USW = 0.9;//  ����� ��������������� ������������
const uint16_t   bound_max_distance_USW = 2;//  �������� ����� ��������� ���������� �����
const float      bound_grouping_USW = 0.9;
const float      bound_noise_USW = 0.3;
const uint16_t   bound_min_adjacent_bin_USW = 2;
const uint16_t   bound_min_bin_signal_USW = 2;
uint16_t   count_bin;    // ���-�� ������������ ��������
const uint8_t    count_read = 3;   // ���-�� ��������� ����������


const float Half_Band_POO  = 10.0;
const float bin_resolution_POO = 0.01953125;
const float POO_CurrentCentralFreq = 150.0;
const float Bound_D_Global_Max = 1.00;
const float Bound_Coun_It = 5;


/////////////////// �������� ��� �������  /////////////////////
//////////////////////////////////////////////////////////////
////////// ������� ����������� ������������ ������� ����������� Nb * Nr//////////

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

////////////////////////���� ����������� � �������������/////////////////
/////////////////////////////////////////////////////////////////////////

#pragma DATA_SECTION (ReW_Bin, ".internal_data");
#pragma DATA_ALIGN(ReW_Bin, 4);
float   ReW_Bin[SIZE_NAC];// ����������� ������� ��  ��� ����������� ����� �������, �������� ������������ ������� �������

#pragma DATA_SECTION (ImW_Bin, ".internal_data");
#pragma DATA_ALIGN(ImW_Bin, 4);
float   ImW_Bin[SIZE_NAC];

float stat_detection;
float stat_grouping;



////////////   �������� ������ ����� ����� �����������   //////////////
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


//// ��������� ���������� �������� ������  ����� "��������� ��������"/////////
//����������� �� Mas_Inform_AC � ������� FUNC_Read_Params_Freqdiap_USW_5 //////////////
////////////////////////////////////////////////////////////////////////////

uint32_t c_freq;                 // ���������� ������
uint32_t c_azimuth;                  // �����-�� ������������ ��������
uint32_t c_channel;                  // ���������� ������� �������� �������
int ind_first;                 // ������ 1-��� �������� ����� ����. ����. � ������� Mas_AC
float Signal_Start_Freq;         // ��������� �������, ���
float Signal_Final_Freq;            // �������� �������, ���
float F_start;
float F_stop;
float F_shag;
float azimuth_start;                // ���. ����. �������, ����
float azimuth_stop;             // �������� ���� �������, ����
float azimuth_shag;             //��� ������������ ��������, ����


////////////////////////////////////////////////////////////////////////////////////

///////////////// ������� ������� �� ������ ////////
/////////////////////////////////////////////////////

float Mas_AC[SIZE_MAS_AC] = {
    #include "Mas_Ac.txt"
    };                        // ������ ������������ �������������

float Mas_Inform_AC[SIZE_INF] = {
    #include "Mas_Inform_AC.txt"
    };       // ������, ���������� ����������� � ��������� ������ � ���������� ������� ������� ����


/////////////////////////////////////////////
////////////////////////////////////////////

float Fc;                       // ����������� �������, ���
/////////////// �������� ������� ����� ������������ �������������� � ������ ������ ����� �������������, � ����������� /////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma DATA_SECTION (ReU, ".internal_data");
#pragma DATA_ALIGN(ReU, 4);
float   ReU[SIZE_U];// ������ ������. ������ ������������ ����. ��� ���������� ������� Fc

#pragma DATA_SECTION (ImU, ".internal_data");
#pragma DATA_ALIGN(ImU, 4);
float   ImU[SIZE_U];// ������ ������. ������ ������������ ����. ��� ���������� ������� Fc

#pragma DATA_SECTION (D_ReU, ".internal_data");
#pragma DATA_ALIGN(D_ReU, 4);
float   D_ReU[SIZE_U];// ������ ������. ����. ����������� ������������ ����. ��� ���������� ������� Fc

#pragma DATA_SECTION (D_ImU, ".internal_data");
#pragma DATA_ALIGN(D_ImU, 4);
float   D_ImU[SIZE_U];// ������ ������. ����. ����������� ������������ ����. ��� ���������� ������� Fc


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////// �������� �������� ������� FUNC_Cubic_Interpolation_USW_5l /////////////////////
float y;                        // �������� �������� ������� �������� ��� ������� F_diff �������� ������ �� F_shag

/////////////////////////////////////////////////////////////////////////////

///////////// �������� ������� ����� ������� FUNC_Calculating_Direction_Finding_Relief /////////////////////
///////////////////////////////////////////////////////////////////////////////
#pragma DATA_SECTION (Mteta, ".internal_data");
#pragma DATA_ALIGN(Mteta, 4);
float   Mteta[SIZE_Theta];// ������ �������� ��� ���������� ����� ������������ ��������

#pragma DATA_SECTION (D_Mteta, ".internal_data");
#pragma DATA_ALIGN(D_Mteta, 4);
float   D_Mteta[SIZE_Theta];// ������ ����������� ���� ��� ���������� ����� ���� ��������

/////////////////////////////////////////////////////////////////////////
////////////////////////////////Iter_Poisk_Max  ����������� �������� �������, ������ ������������� � ���������  ////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



float Mas_Vertex[2];                // ������ � ������������ ������ 2-� ������� ��������� �������� ����� �������� ���� ���� ��������
float x;                            // ������������ ������
float coeff_reliability;            // ������ �������������
float max_value_Mtheta;         // ������������ �������� ���������� ����� ������������ ��������
float Amp_Azimuth_Mtheta;       // �������� ���  � ������������� �������
float Amp_IRI_db;
float azimuth_Mtheta;           // �������� �������
float Mteta_x;
float D_Mteta_x;                    // ����������� ����. ������ ��� ������������� ������������� �������
float TrW;                      // ���� ������� �������� �������

float Mas_ReU_x[SIZE_VEC];      // ������ �������������� ���� ������� U_x
float Mas_ImU_x[SIZE_VEC];      // ������ ������ ���� ������� U_x
float Mas_D_ReU_x[SIZE_VEC];        // ������ �������������� ���� ������� DU_x
float Mas_D_ImU_x[SIZE_VEC];        // ������ ������ ���� ������� DU_x


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/////////////������ ��� ��������� ���������//////////
/////////////////////////////////////////////////////
float  fletch;    // ������ ������������ � �������  Rasch_Proizv_Znach_Vtochke( ) ��� ��������� �������
float Znam;     // 1 / c_azimuth;

/////�������� ��������//////////
#pragma DATA_SECTION (Inform, ".internal_data");
#pragma DATA_ALIGN(Inform, 4);
Bearing Inform;

#endif /* VARIABLES_USV_H_ */
