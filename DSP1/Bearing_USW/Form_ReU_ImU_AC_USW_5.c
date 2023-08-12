///////////////////////////ALG№3///////////////////////////////////////////
//////////// Алгоритм формирования реальных и мнимых частей //////////////
/////юстировочных коэффициентов (ЮК, AC - Alignment Coefficients)/////////
////////////азимутального радиопеленгатора для входной частоты /////////// 
/////////с использованием кубической интерполяции по 4-м точкам //////////
///////////////// с равномерным единичным шагом /////////////////////////



#include "Header_Func_USW_5.h"
#include "Header.h"
#include "variables_USW.h"


void Form_ReU_ImU_AC_USW_5 (void)
{
	float B1;
    int   ind_left;           // Индекс последней затабулированной частоты
    float B2;                 // Остаток
    float F_diff;             // Локально нормир на F_shag
    int   nc;                 // Номер канала
    int   ind_first_element;  
    int   ind_start;          // Индекс ачейки массива Mas_Ac
    int   ind_left_first;     // Нулевой азимутальный отсчёт
    int   na;                 // Азимут
    int   ind_1;              // Локальный индекс
    float y_0;
    float y_1;
    float y_2;
    float y_3;
    int   c_elements_F;
   
    B1  = 0.0;
   
    
/////////////////// Определение 4-х частот, образующих диапазон, содерж Fc //////////////////////////////////////
	
    B1       = ( Fc - F_start ) / F_shag;
    ind_left = (int) B1;
    B2       = B1 - ind_left;
    
    if (( B2 > 0.0 ) && ( ind_left == 0 )) 
    {
    	F_diff   = B2;
    }
    if (( B2 > 0.0 ) && ( ind_left == (c_freq - 2) ))
    {
     	F_diff   = B2 + ind_left - c_freq + 4.0;
     	ind_left = c_freq - 4;
    }
    if (( B2 > 0.0 ) && ( (ind_left >= 1) && ( ind_left <= c_freq - 3 ) ))
    {
    	F_diff 	 = B2 + 1.0;
    	ind_left = ind_left - 1;
    }

    
   ind_left_first =  ind_left * 2 * c_channel * c_azimuth;
   
    //////// Формирование реальных частей юстировочн коэф ///////// 
   for ( nc = 0; nc < c_channel; nc++ )
   {
         ind_first_element = nc * c_azimuth;
    	 ind_start         = ind_first + ind_left_first + 2 * nc;
    
    		
    			
    ///////////// Цикл по азимутам //////////////////
    
    
    		for ( na = 0; na < c_azimuth; na ++ )
    		{
    			
    			  ind_1 = ind_start +  na * 2 * c_channel;
    			
    			  if ( B2 > 0.0 )
    			  {
    			       ind_1        = ind_start + na * 2 * c_channel;
    			       c_elements_F = c_azimuth * 2 * c_channel;
    			       y_0          = Mas_AC[ind_1];
    			       ind_1        = ind_1 + c_elements_F;
    			       y_1 		    = Mas_AC[ind_1];
    			       ind_1		= ind_1 + c_elements_F;
    			       y_2 		    = Mas_AC[ind_1];
    			       ind_1 		= ind_1 + c_elements_F;
    			       y_3			= Mas_AC[ind_1];
    			    
    			       Cubic_Interpolation_USW_5 ( F_diff,y_0,y_1,y_2,y_3 );
    	
    	 		       ReU[ind_first_element + na] = y;  	
    			   	
    			  }	
    				
    			  if ( B2 == 0.0 )
    			  {
    			       ReU[ind_first_element + na] = Mas_AC[ind_1]; 	
    			  }
    						
    		
    		}
    		
   }
   
   ////////// Формирование мнимых частей юстир коэф ///////////
   for ( nc = 0; nc < c_channel; nc++ )
   {
         ind_first_element = nc * c_azimuth;
    	 ind_start         = ind_first + ind_left_first + 2 * nc + 1;
    
    		
    			
    ///////////// Цикл по азимутам //////////////////
    
    
    		for ( na = 0; na < c_azimuth; na ++ )
    		{
    			
    			  ind_1 = ind_start +  na * 2 * c_channel;
    			
    			  if ( B2 > 0.0 )
    			  {
    			       ind_1        = ind_start + na * 2 * c_channel;
    			       c_elements_F = c_azimuth * 2 * c_channel;
    			       y_0          = Mas_AC[ind_1];
    			       ind_1        = ind_1 + c_elements_F;
    			       y_1 		    = Mas_AC[ind_1];
    			       ind_1		= ind_1 + c_elements_F;
    			       y_2 		    = Mas_AC[ind_1];
    			       ind_1 		= ind_1 + c_elements_F;
    			       y_3			= Mas_AC[ind_1];
    			    
    			       Cubic_Interpolation_USW_5 ( F_diff,y_0,y_1,y_2,y_3 );
    	
    	 		       ImU[ind_first_element + na] = y;  	
    			   	
    			  }	
    				
    			  if ( B2 == 0.0 )
    			  {
    			       ImU[ind_first_element + na] = Mas_AC[ind_1]; 	
    			  }
    						
    		
    		}
    		
   }
   
   		
}
