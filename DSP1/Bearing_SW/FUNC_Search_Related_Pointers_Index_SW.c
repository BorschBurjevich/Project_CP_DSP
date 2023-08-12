/*‘ункци€ нахождени€ индексов двух пар смежных эквидистантных точек, между которыми содержатс€
 максимумы основного и обратного лепестков пеленгационного рельефа*/

#include <Bearing_SW/DEFINES_SW.h>
#include "variables_SW.h"

void FUNC_Search_Related_Pointers_Index_SW ()
{

	int i;
	float max_value;
	int   A_1;
	int   gr_1,gr_2,gr_3,gr_4;
	float Mas[SIZE_MAS];     // массива суммы значений смежных пар эквидистантных точек пеленгационного рельефа
	
	
	//////////формирование массива суммы значений смежных пар эквидистантных точек пеленгационного рельефа/////
	////////       ѕоиск индекса, соответствующего максимальному значению   дл€ первой пары           ////////

	Mas[0]    = Mtheta_SW[0] + Mtheta_SW[1];
	max_value = Mas[0];
	ind_max_1 = 0;
	  
	for (i = 1; i < c_azimuth_SW; i++)
	{
		 Mas[i]    = Mtheta_SW[i] + Mtheta_SW[i+1];
		 if  (Mas[i] > max_value )
		 {
		      max_value = Mas[i];
		      ind_max_1 = i;
		      	
		 }
		 		
	}

///////////////	ќпределение границ лепестка пеленгационного рельефа, которому принадлежит перва€ пара смежных эквидистантных точек //////////// 
	
	A_1  = (int) c_azimuth_SW / 4.0;
	gr_3 = -1;
	gr_4 = -1;
	
	if (ind_max_1 + A_1 < c_azimuth_SW - 1)
	{
		
		if (ind_max_1 - A_1 > 0 )
		{
			
			gr_1 = 0;
			gr_2 = ind_max_1 - A_1 - 1;
			gr_3 = ind_max_1 + A_1 + 1;
			gr_4 = c_azimuth_SW - 1;
				
		}
		else
		{
			gr_1 = ind_max_1 + A_1 + 1; 	
			gr_2 = gr_1 + 2 * A_1 - 2;  	
		}		
			
	}
	else
	{
		gr_1 = ind_max_1 - 3 * A_1 + 1;
		gr_2 = ind_max_1 - A_1 - 1;
		  
	}
	max_value = Mas[gr_1];
	ind_max_2 = 0;
	
///////////////////	ѕоиск индекса, соответствующего максимальному значению Mas[]  дл€ второй пары //////////////	
	if (gr_3 == -1)
	{
		for (i = gr_1; i <= gr_2; i++ )
		{
			 
			 if (Mas[i]    > max_value)
			 {
			     max_value = Mas[i];
			     ind_max_2 = i;
			            
			 }
			        	
		}	
			
	}
	
	if (gr_3 >= 0)
	{
		
		for (i = gr_1; i <= gr_2; i++ )
		{
			 
			 if (Mas[i]    > max_value)
			 {
			     max_value = Mas[i];
			     ind_max_2 = i;
			            
			 }
			        	
		}
		    
		for (i = gr_3; i <= gr_4; i++ )
		{
			 
			 if (Mas[i]    > max_value)
			 {
			     max_value = Mas[i];
			     ind_max_2 = i;
			            
			 }
			 
			        	
		}
		    
		
	}		
				
	
}
