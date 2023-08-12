//*Алгоритм расчета отсчетов пеленгационного рельефа в эквидистантных точках*/

#include <Bearing_SW/DEFINES_SW.h>
#include <math.h>
#include "variables_SW.h"

void FUNC_Direction_Finding_Reports_SW(float *mmas_ReW, float *mmas_ImW)
{

	int   ind;
	int   i, j;
	float C_0, C_1, C_2;
	float D_0, D_1, D_2;
	float P_1, P_2;
	
	//  В цикле по азимутальным отсчетам рассчитывается Mtheta // 
	for ( i = 0; i < c_azimuth_SW; i++ )
	{
		//Расчет знаменателя:инициализация переменных и ввод локальных переменных //


		  ind = i;
		  C_0 = ReU_SW[ind];
		  D_0 = ImU_SW[ind];
		  ind = ind + c_azimuth_SW;
		  C_1 = ReU_SW[ind];
		  D_1 = ImU_SW[ind];
		  ind = ind + c_azimuth_SW;
		  C_2 = ReU_SW[ind];
		  D_2 = ImU_SW[ind];
		  ind = i;
		 
		 //Формирование массива значений знаменателя пеленгационного рельефа  
		  znam_Mtheta_SW[i] = ( C_0 * C_0 + D_0 * D_0 ) + ( C_1 * C_1 + D_1 * D_1 ) + ( C_2 * C_2 + D_2 * D_2  );
		  
		  
		  for ( j = 0; j < c_channel_SW; j ++ )
		  {
		  	    //расчет числителя
		  	
		  		P_1 = C_0 * mmas_ReW[j] + D_0 * mmas_ImW[j];
		  		P_1 = P_1 + C_1 * mmas_ReW[j+3] + D_1 * mmas_ImW[j + 3];
		  		P_1 = P_1 + C_2 * mmas_ReW[j+6] + D_2 * mmas_ImW[j + 6];
		  		P_2 = - D_0 * mmas_ReW[j] + C_0 * mmas_ImW[j];
		  		P_2 = P_2 - D_1 * mmas_ReW[j+3] + C_1 * mmas_ImW[j+3];
		  		P_2 = P_2 - D_2 * mmas_ReW[j+6] + C_2 * mmas_ImW[j+6];
		  		
		  		Mtheta_SW[i]  = Mtheta_SW[i] + P_1 * ReU_SW[ind] - P_2 * ImU_SW[ind];
		  		ind 	   = ind + c_azimuth_SW;
		  }
		  
		  // Формирование массива значений пеленгационного рельефа Mtheta 
		  Mtheta_SW[i]  =  Mtheta_SW[i] / znam_Mtheta_SW[i];
		  	
	}		

		
}
