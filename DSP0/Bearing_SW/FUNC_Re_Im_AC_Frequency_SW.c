/*Алгоритм формирования реальных и мнимых частей юстировочных коэффициентов (ЮК, AC - Alignment Coefficients)  
азимутального радиопеленгатора для входной частоты  с использованием кубической интерполяции по 4-м точкам 
с равномерным единичным шагом */

#include <Bearing_SW/DEFINES_SW.h>
#include <math.h>
#include "variables_SW.h"
#include "FUNC_SW.h"

void FUNC_Re_Im_AC_Frequency_SW (void)
{
	float B1;
    int   A_1;                // индекс последней затабулированной частоты 
    float B2;                 // остаток
    float F_diff;             // локально нормир на  F_shag
    int   nc;                 // номер канала
 //   int   ind_first_element;
    int   start_index;
    int   A_2;                
    int   na;                 // номер азимута
    int   ind_1;              // Локальный индекс
    float y_0;
    float y_1;
    float y_2;
    float y_3;
    int   c_elements_F;
  

    B1  = 0.0;
   
    
/////////////////// Определение 4-х частот, образующих диапазон, содержащих Fc //////////////////////////////////////
	
    B1       = ( Fc_SW - F_start_SW ) / F_shag_SW;
    A_1      = (int)B1;
    B2       = B1 - A_1;
    
    if (( B2 > 0.0 ) && ( A_1 == 0 )) 
    {
    	F_diff   = B2;
    }
    if (( B2 > 0.0 ) && ( A_1 == c_frequency_SW - 2 ))
    {
     	F_diff   = B2 + A_1 - c_frequency_SW + 4.0;
     	A_1 = c_frequency_SW - 4;
    }
    if (( B2 > 0.0 ) && ( (A_1 >= 1) && ( A_1 <= c_frequency_SW - 3 ) ))
    {
    	F_diff 	 = B2 + 1.0;
    	A_1      = A_1 - 1;
    }
    
   
    A_2 =  A_1 * 2 * c_channel_SW * c_azimuth_SW;
   
    //////// Формирование реальных частей юстировочных коэффициентов ///////// 
   for ( nc = 0; nc < c_channel_SW; nc++ )
   {
         ind_1             = nc * c_azimuth_SW;
    	 start_index       = ind_start_SW + A_2 + 2 * nc;
    
    		
    			
    ///////////// Цикл по азимутам //////////////////
    
    
    	 for ( na = 0; na < c_azimuth_SW; na ++ )
    	 {
    			
    		   A_1 = start_index +  na * 2 * c_channel_SW;
    			
    		   if ( B2 > 0.0 )
    		   {
    			    A_1          = ind_start_SW + na * 2 * c_channel_SW;
    			    c_elements_F = c_azimuth_SW * 2 * c_channel_SW;
    			    y_0          = Mas_AC_Loop[A_1];
    			    A_1          = A_1 + c_elements_F;
    			    y_1 		 = Mas_AC_Loop[A_1];
    			    A_1		     = A_1 + c_elements_F;
    			    y_2 		 = Mas_AC_Loop[A_1];
    			    A_1 		 = A_1 + c_elements_F;
    			    y_3			 = Mas_AC_Loop[A_1];
    			    
    			    FUNC_Cubic_Interpolation_SW( F_diff,y_0,y_1,y_2,y_3 );
    	
    	 		    ReU_SW[ind_1 + na] = value_interpolation_SW;
    			   	
    		   }	
    				
    		   if ( B2 == 0.0 )
    		   {
    			    ReU_SW[ind_1 + na] = Mas_AC_Loop[A_1];
    		   }
    						
    		
    	 }
    		
   }
   
   ////////// Формирование мнимых частей юстировочных коэффициентов ///////////
  for ( nc = 0; nc < c_channel_SW; nc++ )
  {
        ind_1             = nc * c_azimuth_SW;
    	start_index       = ind_start_SW + A_2 + 2 * nc + 1;
    
    		
    			
    ///////////// Цикл по азимутам //////////////////
    
    
    	for ( na = 0; na < c_azimuth_SW; na ++ )
    	{
    			
    		  A_1 = start_index +  na * 2 * c_channel_SW;
    			
    		  if ( B2 > 0.0 )
    		  {
    			   A_1          = ind_start_SW + na * 2 * c_channel_SW;
    			   c_elements_F = c_azimuth_SW * 2 * c_channel_SW;
    			   y_0          = Mas_AC_Loop[A_1];
    			   A_1          = A_1 + c_elements_F;
    			   y_1 		    = Mas_AC_Loop[A_1];
    			   A_1		    = A_1 + c_elements_F;
    			   y_2 		    = Mas_AC_Loop[A_1];
    			   A_1 		    = A_1 + c_elements_F;
    			   y_3			= Mas_AC_Loop[A_1];
    			    
    			   FUNC_Cubic_Interpolation_SW( F_diff,y_0,y_1,y_2,y_3 );
    	
    	 		   ImU_SW[ind_1 + na] = value_interpolation_SW;
    			   	
    		  }	
    				
    		  if ( B2 == 0.0 )
    		  {
    			   ImU_SW[ind_1 + na] = Mas_AC_Loop[A_1];
    		  }
    						
    		
    	}
    		
   }
   
   
  
}
