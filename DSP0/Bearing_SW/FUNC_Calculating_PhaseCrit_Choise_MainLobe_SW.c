/*Функция вычисления фазового критерия разрешения неопределенности выбора главного лепестка*/

#include <Bearing_SW/DEFINES_SW.h>
#include <math.h>
#include "variables_SW.h"

void FUNC_Calculating_PhaseCrit_Choise_MainLobe_SW()
{

	float delta;
	float inv_2pi;
	float B;
	int   ind_max;
	int   ind;
	float C0, C1, C2;
	float D0, D1, D2;
	float P1, P2;
	
	delta   = (float) 0.1 / c_azimuth_SW;
	inv_2pi = 0.159154943091895;
	
	if (yL > yR)
	{

		B       =(float) (xL * c_azimuth_SW * inv_2pi + delta);
		ind_max = (int) B;
				
	}	 	
	else
	{
	
		B       = (float)(xR * c_azimuth_SW * inv_2pi + delta);
		ind_max = (int) B;	
	}
	
	ind    = ind_max;
	C0     = ReU_SW[ind];
	D0     = ImU_SW[ind];
	ind    = ind + c_azimuth_SW;
	C1     = ReU_SW[ind];
	D1     = ImU_SW[ind];
	ind    = ind + c_azimuth_SW;
	C2     = ReU_SW[ind];
	D2     = ImU_SW[ind];
	
	B      = (C0 * C0 + D0 * D0) + (C1 * C1 + D1 * D1) + (C2 * C2 + D2 * D2);
	P1     = C0 * mas_ReQ[0] + D0 * mas_ImQ[0];
	P1     = P1 + C1 * mas_ReQ[1] + D1 * mas_ImQ[1];
	P1     = P1 + C2 * mas_ReQ[2] + D2 * mas_ImQ[2];
	
	P2     = -D0 * mas_ReQ[0] + C0 * mas_ImQ[0];
	P2     = P2 - D1 * mas_ReQ[1] + C1 * mas_ImQ[1];
	P2     = P2 - D2 * mas_ReQ[2] + C2 * mas_ImQ[2];
	P1     = P1 / B;
	P2     = P2 / B;
	
	deltaP = atan2f(P2,P1);  // значение арктангенса, подлежащее сравнению с порогом для устранения неопределенности относительно выбора главного лепестка.
	   
		
}
