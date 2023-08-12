
#include <Bearing_SW/DEFINES_SW.h>
#include <Bearing_SW/FUNC_SW.h>
#include <math.h>
#include "variables_SW.h"

void FUNC_Elimination_Uncert_Choise_MainLobe(float *ReW)
{

	float B;
	float div_b_max_value;
		
//	Вычисление фазового критерия разрешения неопределенности выбора главного лепестка пеленгационного рельефа//	
	
	if (deltaP >= PII * 0.5)
	{
	
		FUNC_Calculating_Derivate_Value_Argue_Points_SW (ind_max_2, &Mtheta_SW[0], c_azimuth_SW);
		FUNC_Maximising_Direction_F_SW();  // Mtheta, c_azimuth на вход
			
		
	}
	else
	{
		
		FUNC_Maximising_Direction_F_SW();	
	}
	
	// 	Вычисление энергии (амплитуды) ИРИ с направления (азимута) //
	FUNC_Calculating_Kotel_Point_X_SW(azimuth_Mtheta_SW, &znam_Mtheta_SW[0], c_azimuth_SW);
	B          		 = y_SW;
	div_b_max_value  = max_value_SDN / B;
	AMP_IRI_dB_SW 		 = 10.0 * log10f (div_b_max_value);

	
	// Пересчет значения азимута в градусах //
	
	azimuth_Mtheta_SW   = azimuth_Mtheta_SW * 57.2958;
	
	// Вычисление коэффициента достоверности //
	
	B 				    = ReW[0] + ReW[4] + ReW[8];
	coeff_reability_SW  = max_value_SDN / B;;
	 
			
	
}
