#include <Bearing_SW/DEFINES_SW.h>
#include <Bearing_SW/FUNC_SW.h>
#include "variables_SW.h"

void FUNC_Correlation_Grouping_4_SW(float *RW_1,float *IW_1,float *RW_2,float *IW_2)
{
	float         Tr_1, Tr_2, Tr_12;

	
	//���� ������� �������� ������� 1-�� ������������� ����������
	Tr_1 = RW_1[0]+RW_1[3]+RW_1[5]+RW_1[9];
	
	//���� ������� �������� ������� 2-�� ������������� ����������
	Tr_2 = RW_2[0]+RW_2[3]+RW_2[5]+RW_2[9];

	//���� ������������ ������
	Tr_12  = RW_1[1] *RW_2[1]  + IW_1[1] *IW_2[1];
	Tr_12 += RW_1[2] *RW_2[2]  + IW_1[2] *IW_2[2];
	Tr_12 += RW_1[4] *RW_2[4]  + IW_1[4] *IW_2[4];
	

	Tr_12 += RW_1[6] *RW_2[6]  + IW_1[6] *IW_2[6];
	Tr_12 += RW_1[7] *RW_2[7]  + IW_1[7] *IW_2[7];
	
	Tr_12 += RW_1[8] *RW_2[8]  + IW_1[8] *IW_2[8];
	
	
	
	Tr_12  = 2.0*Tr_12;
	
	
    Tr_12 += RW_1[0] *RW_2[0];
	Tr_12 += RW_1[3] *RW_2[3];     
	Tr_12 += RW_1[5] *RW_2[5];
	Tr_12 += RW_1[9] *RW_2[9];    
	

	//�������� ����������
	stat_grouping_SW = Tr_12/(Tr_1*Tr_2);
										
}
