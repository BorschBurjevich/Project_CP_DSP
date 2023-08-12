#include <Bearing_SW/DEFINES_SW.h>
#include <Bearing_SW/FUNC_SW.h>
#include <math.h>
#include "variables_SW.h"

void FUNC_Calculating_Kotel_Point_X_SW(float x, float *mas, int count_point)
{
// 11 770 // 
	
	float ddelta;
	float inv_count_point;
	float B1, B2;
	int   A1;
	int   i;
	float P1, P2, P3, P4;
	float R1, R2;
	float Pi_Inv;
	 
	ddelta  		= 0.001745;
	inv_count_point = (float) 1.0 / count_point;
	Pi_Inv 			= PII * inv_count_point;
	x               = x * 0.5;
	B1 				= (float) x / Pi_Inv;
	A1 				= (int) B1;
	B1				= B1 - A1;

	
		
	if ( fabsf(B1) <= ddelta || fabsf(B1 - 1.0) <= ddelta )
	{
	    
		if (fabsf(B1) <= ddelta)
		{
			y_SW = mas[A1];
		}
		else
		{
		
			y_SW = mas[A1 + 1];
		}
					
	}
	else
	{
				
		P1 = cosf(x);
		P2 = sinf(x);
		P3 = cosf(Pi_Inv);
		P4 = sinf(Pi_Inv);
		B2 = 1.0;
		y_SW  = mas[0] * P1 / P2;
		
		for (i = 1; i < count_point; i++ )
		{
			 R1 = P1 * P3 + P2 * P4;
			 R2 = P2 * P3 - P1 * P4;
			 B2 = -1.0 * B2;
			 y_SW  = y_SW + mas[i] * B2 * R1 / R2;
			 P1 = R1;
			 P2 = R2;
			  		 		
		}
		y_SW  = y_SW * sinf(count_point * x) * inv_count_point;
	}		 
	
	
}
