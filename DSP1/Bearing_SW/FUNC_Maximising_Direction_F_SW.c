/*Функция максимизации пеленгационного рельефа, оценка азимута*/
#include <Bearing_SW/DEFINES_SW.h>
#include <Bearing_SW/FUNC_SW.h>
#include  <math.h>
#include "variables_SW.h"

void FUNC_Maximising_Direction_F_SW()
{
	float delta_D;
	float dy_new;
	int   iter;
	float x_new;
	float y_new;
	
	delta_D = 0.5;
	
	if (yL > yR)
	{
		max_value_SDN     = yL;
		azimuth_Mtheta_SW = xL;
		dy_new		      = dyL;
				
	}
	else
	{
		max_value_SDN     = yR;
		azimuth_Mtheta_SW = xR;
		dy_new		      = dyR;
			
	}	 	
		
	iter   = 0;
	
	while (fabsf(dy_new) > delta_D && iter < 4)
	{
		   if (xL > xR)
		   {
			   xR     = xR + 2.0 * PII;
				
		   }
				
		   x_new  = (dyR * xL - dyL * xR) / (dyR - dyL);
		   //	вызов функции "Функция вычисления в точке x значения 2pi периодической интерполируемой функции" и ввод локальной переменной//
			
		   FUNC_Calculating_Kotel_Point_X_SW(x_new,&Mtheta_SW[0],c_azimuth_SW);
		   y_new  = y_SW;
		   //	Вызов функции "Функция вычисления в точке x производной значения 2pi периодической интерполируемой функции" и ввод локальной переменной//
			
		   FUNC_Modif_Kotel_Derivative_Point_SW(x_new,&Mtheta_SW[0],c_azimuth_SW);
		   dy_new = dy;
			
		   if (y_new > max_value_SDN)
		   {
			   max_value_SDN     = y_new;
			   azimuth_Mtheta_SW = x_new;
					
		   }
			
		   if (dy_new >= 0.0)
		   {
			   xL  = x_new;
			   yL  = y_new;
			   dyL = dy_new;
			
		   }
		   else
		   {
			   xR  = x_new;
			   yR  = y_new;
			   dyR = dy_new;
						
		   }	
			
		   iter = iter + 1;
					
					  						
			
	}
										
}
