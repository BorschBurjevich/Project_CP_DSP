/*Функция кубической интерполяции по 4-м точкам (значениям табличной функции)
с единичным шагом между аргументами табличной функции*/ 

#include "variables_SW.h"


void FUNC_Cubic_Interpolation_SW ( float F_diff, float y_0, float y_1, float y_2, float y_3 )

{
	float a_0;
	float a_1;
	float a_2;
	float a_3;
	float res_1;
	float res_2;
    
    a_0   					= y_0;
    a_1   					= (-11 * y_0) + (18 * y_1) - (9 * y_2) + (2 * y_3);
    a_2   					= (6 * y_0) - (15 * y_1) + (12 * y_2) - (3 * y_3);
    a_3   					= (- y_0) + (3 * y_1) - (3 * y_2) + (y_3);
    res_1 					= F_diff * F_diff;
    res_2    				= res_1  * F_diff; 
    value_interpolation_SW  = (( a_1 * F_diff + a_2 * res_1 + a_3 * res_2 ) / 6.0) + a_0;
     	
}
