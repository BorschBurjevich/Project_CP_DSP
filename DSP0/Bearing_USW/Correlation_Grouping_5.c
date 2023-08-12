#include "Header_Func_USW_5.h"
#include "Header.h"

#include "variables_USW.h"
extern float  stat_grouping;

void Correlation_Grouping_5(float *RW_1,float *IW_1,float *RW_2,float *IW_2)
{
	float         Tr_1, Tr_2, Tr_12;

	
	//—лед матрицы взаимных энергий 1-го спектрального компонента
	Tr_1 = RW_1[0]+RW_1[7]+RW_1[13]+RW_1[18]+RW_1[22];
	
	//—лед матрицы взаимных энергий 2-го спектрального компонента
	Tr_2 = RW_2[0]+RW_2[7]+RW_2[13]+RW_2[18]+RW_2[22];

	//—лед произведениа матриц
	Tr_12  = RW_1[1] *RW_2[1]  + IW_1[1] *IW_2[1];
	Tr_12 += RW_1[2] *RW_2[2]  + IW_1[2] *IW_2[2];
	Tr_12 += RW_1[3] *RW_2[3]  + IW_1[3] *IW_2[3];
	Tr_12 += RW_1[4] *RW_2[4]  + IW_1[4] *IW_2[4];

	Tr_12 += RW_1[8] *RW_2[8]  + IW_1[8] *IW_2[8];
	Tr_12 += RW_1[9] *RW_2[9]  + IW_1[9] *IW_2[9];
	Tr_12 += RW_1[10]*RW_2[10] + IW_1[10]*IW_2[10];

	Tr_12 += RW_1[14]*RW_2[14] + IW_1[14]*IW_2[14];
	Tr_12 += RW_1[15]*RW_2[15] + IW_1[15]*IW_2[15];
	
	Tr_12 += RW_1[19]*RW_2[19] + IW_1[19]*IW_2[19];
		
	Tr_12  = 2.0*Tr_12;
	
	
    Tr_12 += RW_1[0] *RW_2[0];
	Tr_12 += RW_1[7] *RW_2[7];     
	Tr_12 += RW_1[13]*RW_2[13];
	Tr_12 += RW_1[18]*RW_2[18];    
	Tr_12 += RW_1[22]*RW_2[22];


	//–ешающаа статистика
	stat_grouping = Tr_12/(Tr_1*Tr_2);	
										
}
