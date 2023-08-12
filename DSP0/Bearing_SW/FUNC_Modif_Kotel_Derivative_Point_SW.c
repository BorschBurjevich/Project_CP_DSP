/*Функция вычисления в точке x значения производной  -периодической интерполируемой функции,
 заданной массивом дискретных отсчетов с помощью модифицированного ряда Котельникова
*/
#include <Bearing_SW/DEFINES_SW.h>
#include <Bearing_SW/FUNC_SW.h>
#include <math.h>


extern float dy;

void FUNC_Modif_Kotel_Derivative_Point_SW(float x, float *mmas, int count_point)
{

	
	float inv_count_point;
	float B1,B2, B;
	int   A1, A2;
    float P1, P2, P3, P4, P5;
    float R1, R2;
    float Pi_Inv;
    int   ii;
    float y1, y2;
    float ddelta;
    
 
	/////////////////	Инициализация переменных ///////////////
	
	ddelta		    = 0.001745;	
    x 				= x * 0.5;
	inv_count_point = 1.0 / count_point;
	Pi_Inv			= PII*inv_count_point;
	
	B1				= x / Pi_Inv;
	A1				= (int) B1;
	B1				= B1 - A1;

	
	
	///////////////  Проверка условия  ////////////
	
	if ((fabsf(B1) <= ddelta) || (fabsf(B1-1.0) <= ddelta))
	{
	
		if (fabsf(B1-1.0) <= ddelta)
		{
			A1 = A1 + 1;
			
		}	
			dy = 0.0;
			P1 = cosf(A1*Pi_Inv);
			P2 = sinf(A1*Pi_Inv);
			P3 = cosf(Pi_Inv);
			P4 = sinf(Pi_Inv);	
			
			if ((A1>0) && (A1 < count_point - 1.0))
			{
				 B2 = (float)A1 * 0.5;
				 A2 = (int)B2;
				 B2 = B2 - A2;
				 
				 if (B2==0)
				 {
				 	 B = 1.0;
				 	 	 	
				 }
				 else
				 {
				 	 B = - 1.0;
				 	 
				 }
				 
				 P5 = P1 / P2;
				 
				 for (ii = 0; ii < A1; ii ++)
				 {
				 	  dy = dy + mmas[ii] * B * P5;
				 	  R1 = P1 * P3 + P2 * P4;
				 	  R2 = P2 * P3 - P1 * P4; 
				 	  P5 = R1 / R2;
				 	  P1 = R1;
				 	  P2 = R2;
				 	  B  = - 1.0 * B;
				 	  	  
				 }
				 
				 ///////////////////////////////////////////////
				 
				 R1 = P1 * P3 + P2 * P4;
				 R2 = P2 * P3 - P1 * P4;
				 P5 = R1 / R2;
				 P1 = R1;
				 P2 = R2;
				 B  = (- 1.0) * B;
				  
				 
				 
				 //////////////////////////////////////////////
				 
				 for (ii = A1 + 1; ii < count_point; ii ++)  // Проверить границы цикла
				 {
				 	  dy = dy + mmas[ii] * B * P5;
				 	  R1 = P1 * P3 + P2 * P4;
				 	  R2 = P2 * P3 - P1 * P4;
				 	  P5 = R1 / R2;
				 	  P1 = R1;
				 	  P2 = R2;
				 	  B  = - 1.0 * B;
				 	  	   			
				 }			 						
			}
			else
			{
				 B = -1.0;
				 
				 if (A1 == 0)
				 {
				 	 P1 = P3;
				 	 P2 = - P4;
				 	 P5 = P1 / P2;
				 	 
				 	 for (ii =1; ii < count_point; ii++)
				 	 {
				 	 	
				 	 	  dy = dy + mmas[ii] * B * P5;
				 	 	  R1 = P1 * P3 + P2 * P4;
				 	 	  R2 = P2 * P3 - P1 * P4;
				 	 	  P5 = R1 / R2;
				 	 	  P1 = R1;
				 	 	  P2 = R2;
				 	 	  B  = - 1.0 * B; 
				 	 	  	
				 	 }		
				 }
				 else
				 {
				 
				 	 P5 = P1 / P2;
				 	 
				 	 for (ii = 0; ii < count_point - 1; ii++)
				 	 {
				 	 	  dy = dy + mmas[ii] * B * P5;
				 	 	  R1 = P1 * P3 + P2 * P4;
				 	 	  R2 = P2 * P3 - P1 * P4;
				 	 	  P5 = R1 / R2;
				 	 	  P1 = R1;
				 	 	  P2 = R2;
				 	 	  B  = -1.0 * B;	
				 	 }	
				 	  
				 		
				 }			
					
			}			
					
		
	dy = dy * 0.5;									
	}
	
	else
	{
		
		P1 = cosf(x);
		P2 = sinf(x);
		P3 = cosf(Pi_Inv);
		P4 = sinf(Pi_Inv);
	
 		P5 = P1 / P2;
		y1 = mmas[0] * P5;
		y2 = mmas[0] * (1.0 + P5 * P5);
		
		B  = -1.0;
		
		for (ii = 1;ii < count_point; ii ++)
		{
			 R1 = P1 * P3 + P2 * P4;
			 R2 = P2 * P3 - P1 * P4;
			 P5 = R1 / R2;
			 y1 = y1 + mmas[ii] * B * P5;
			 y2 = y2 + mmas[ii] * B * (1.0 + P5 * P5);
			 B  = -1.0 * B;
			 P1 = R1;
			 P2 = R2;
			 
			 	
		}
		
	
		dy = 0.5 * cos(x * count_point)* y1  - sin(x * count_point) * y2 * inv_count_point * 0.5;	
				
	}		
	
		
}
