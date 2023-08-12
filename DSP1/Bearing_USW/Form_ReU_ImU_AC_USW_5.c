///////////////////////////ALG�3///////////////////////////////////////////
//////////// �������� ������������ �������� � ������ ������ //////////////
/////������������ ������������� (��, AC - Alignment Coefficients)/////////
////////////������������� ���������������� ��� ������� ������� /////////// 
/////////� �������������� ���������� ������������ �� 4-� ������ //////////
///////////////// � ����������� ��������� ����� /////////////////////////



#include "Header_Func_USW_5.h"
#include "Header.h"
#include "variables_USW.h"


void Form_ReU_ImU_AC_USW_5 (void)
{
	float B1;
    int   ind_left;           // ������ ��������� ���������������� �������
    float B2;                 // �������
    float F_diff;             // �������� ������ �� F_shag
    int   nc;                 // ����� ������
    int   ind_first_element;  
    int   ind_start;          // ������ ������ ������� Mas_Ac
    int   ind_left_first;     // ������� ������������ ������
    int   na;                 // ������
    int   ind_1;              // ��������� ������
    float y_0;
    float y_1;
    float y_2;
    float y_3;
    int   c_elements_F;
   
    B1  = 0.0;
   
    
/////////////////// ����������� 4-� ������, ���������� ��������, ������ Fc //////////////////////////////////////
	
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
   
    //////// ������������ �������� ������ ���������� ���� ///////// 
   for ( nc = 0; nc < c_channel; nc++ )
   {
         ind_first_element = nc * c_azimuth;
    	 ind_start         = ind_first + ind_left_first + 2 * nc;
    
    		
    			
    ///////////// ���� �� �������� //////////////////
    
    
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
   
   ////////// ������������ ������ ������ ����� ���� ///////////
   for ( nc = 0; nc < c_channel; nc++ )
   {
         ind_first_element = nc * c_azimuth;
    	 ind_start         = ind_first + ind_left_first + 2 * nc + 1;
    
    		
    			
    ///////////// ���� �� �������� //////////////////
    
    
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
