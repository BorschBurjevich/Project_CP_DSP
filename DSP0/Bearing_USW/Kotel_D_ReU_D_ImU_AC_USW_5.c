//////////////////////////////////// AlG№4 /////////////////////////////////
////////////////Алгоритм формирования реальных и мнимых частей///////////// 
//////////////////производных юстировочных коэффициентов (ЮК)/////////////
//////////////////////////азимутального радиопеленгатора//////////////// 
//////////////////////для входной частоты с использованием//////////// 
///////////////////////модифицированного ряда Котельникова/////////

/////////////////// Функциа расчёта производных //////////////

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include "Header_Func_USW_5.h"
#include "Header.h"
#include "variables_USW.h"

void Kotel_D_ReU_D_ImU_AC_USW_5(float *mas_in_RE,float *mas_out_RE, float *mas_in_IM, float *mas_out_IM)
{
		
	int   n;
	int   m;  
	float znach_sum_RE_0,znach_sum_RE_1,znach_sum_RE_2,znach_sum_RE_3,znach_sum_RE_4;
	float znach_sum_IM_0,znach_sum_IM_1,znach_sum_IM_2,znach_sum_IM_3,znach_sum_IM_4;
	float PI_ZNAM;     
	float Arg;
	float B_1;
	float A;
	float P_1, P_2, P_3, P_4, R_1, R_2;
	float S1;
	float S2;
	int   ind_i;
   
	PI_ZNAM    		 = (-1.0) * M_PI * Znam;     // Znam = 1 / c_azimuth
	Arg        	     = M_PI * Znam;				 // M_PI = Pi 
     
	n          		 = 0;
	B_1 	      	 = -0.5;
	P_1        		 = cosf(PI_ZNAM);
	P_2        		 = sinf(PI_ZNAM);
	P_3        		 = P_1;
	P_4        		 = P_2;
	S1         		 = B_1 * P_1 / P_2;

	ind_i            = 1;
   
   	    
	znach_sum_RE_0    = mas_in_RE[ind_i] * S1;
	znach_sum_IM_0    = mas_in_IM[ind_i] * S1;
	ind_i             = ind_i + c_azimuth; 
   
	znach_sum_RE_1    = mas_in_RE[ind_i] * S1;
	znach_sum_IM_1    = mas_in_IM[ind_i] * S1;
	ind_i             = ind_i + c_azimuth;
   
	znach_sum_RE_2    = mas_in_RE[ind_i] * S1;
	znach_sum_IM_2    = mas_in_IM[ind_i] * S1;
	ind_i             = ind_i + c_azimuth;
   
	znach_sum_RE_3    = mas_in_RE[ind_i] * S1;
	znach_sum_IM_3    = mas_in_IM[ind_i] * S1;
	ind_i             = ind_i + c_azimuth;
   
	znach_sum_RE_4    = mas_in_RE[ind_i] * S1;
	znach_sum_IM_4    = mas_in_IM[ind_i] * S1;
	
  
	B_1               = B_1 * (-1.0);
        
    for ( m = 2; m < c_azimuth; m++ )
    {
   	      R_1 		      = P_1 * P_3 - P_2 * P_4;
   	      R_2 		      = P_2 * P_3 + P_4 * P_1;
   	      S2              = B_1 * R_1 / R_2;
   	      ind_i			  = m;
   	      znach_sum_RE_0 += mas_in_RE[ind_i] * S2;
   	      znach_sum_IM_0 += mas_in_IM[ind_i] * S2;
   	      ind_i           = ind_i + c_azimuth;
   	      
   	      znach_sum_RE_1 += mas_in_RE[ind_i] * S2;
   	      znach_sum_IM_1 += mas_in_IM[ind_i] * S2;
   	      ind_i           = ind_i + c_azimuth;
   	     
   	      znach_sum_RE_2 += mas_in_RE[ind_i] * S2;
   	      znach_sum_IM_2 += mas_in_IM[ind_i] * S2;
   	      ind_i           = ind_i + c_azimuth;
   	     
   	      znach_sum_RE_3 += mas_in_RE[ind_i] * S2;
   	      znach_sum_IM_3 += mas_in_IM[ind_i] * S2;
   	      ind_i           = ind_i + c_azimuth;
   	     
   	      znach_sum_RE_4 += mas_in_RE[ind_i] * S2;
   	      znach_sum_IM_4 += mas_in_IM[ind_i] * S2;
   	      ind_i           = ind_i + c_azimuth;
   	     
   		  B_1             = B_1 * (-1.0);
   		  P_1             = R_1;
   		  P_2             = R_2;
   	
    }
    ind_i                 = n;
    mas_out_RE[ind_i]     = znach_sum_RE_0;
    mas_out_IM[ind_i]     = znach_sum_IM_0;
    ind_i                 = ind_i + c_azimuth;
   
    mas_out_RE[ind_i]  	  = znach_sum_RE_1;
    mas_out_IM[ind_i]     = znach_sum_IM_1;
    ind_i                 = ind_i + c_azimuth;
   
    mas_out_RE[ind_i]  	  = znach_sum_RE_2;
    mas_out_IM[ind_i]     = znach_sum_IM_2;
    ind_i                 = ind_i + c_azimuth;
   
    mas_out_RE[ind_i]     = znach_sum_RE_3;
    mas_out_IM[ind_i]     = znach_sum_IM_3;
    ind_i                 = ind_i + c_azimuth;
   
    mas_out_RE[ind_i]     = znach_sum_RE_4;
    mas_out_IM[ind_i]     = znach_sum_IM_4;
    ind_i                 = ind_i + c_azimuth;
    
   
         
    A        = - 0.5;
    P_4 	 = (- 1.0) * P_4;
   
    for ( n = 1; n < c_azimuth - 1 ; ++n )
    {
  	
  	      znach_sum_RE_0  = 0.0;
  	      znach_sum_IM_0  = 0.0;
  	      znach_sum_RE_1  = 0.0;
  	      znach_sum_IM_1  = 0.0;
  	      znach_sum_RE_2  = 0.0;
  	      znach_sum_IM_2  = 0.0;
  	      znach_sum_RE_3  = 0.0;
  	      znach_sum_IM_3  = 0.0;
  	      znach_sum_RE_4  = 0.0;
  	      znach_sum_IM_4  = 0.0;
  	     
          B_1           = A;   
          P_1           = cosf((n+1)*Arg);
          P_2           = sinf((n+1)*Arg);
        
 		
         
         
         
  	      for ( m = 0; m < n; m++ )           /// При фиксированном n, считаем дла m все значениа слева, не включаа n
    	  {
    		  
    		    R_1 		      = P_1 * P_3 + P_2 * P_4;
   		        R_2 		      = P_2 * P_3 - P_1 * P_4;
   		        S2                = B_1 * R_1 / R_2;
   		        ind_i             = m;
    		    znach_sum_RE_0   += mas_in_RE[ind_i] * S2;
   	            znach_sum_IM_0   += mas_in_IM[ind_i] * S2;
   	            ind_i             = ind_i + c_azimuth;
   	     	
   	     	    znach_sum_RE_1   += mas_in_RE[ind_i] * S2;
   	     	    znach_sum_IM_1   += mas_in_IM[ind_i] * S2;
   	     	    ind_i             = ind_i + c_azimuth;
   	     	
   	     	    znach_sum_RE_2   += mas_in_RE[ind_i] * S2;
   	     	    znach_sum_IM_2   += mas_in_IM[ind_i] * S2;
   	     	    ind_i             = ind_i + c_azimuth;
   	     	 
   	     	    znach_sum_RE_3   += mas_in_RE[ind_i] * S2;
   	     	    znach_sum_IM_3   += mas_in_IM[ind_i] * S2;
   	     	    ind_i             = ind_i + c_azimuth;
   	     	
   	     	    znach_sum_RE_4   += mas_in_RE[ind_i] * S2;
   	     	    znach_sum_IM_4   += mas_in_IM[ind_i] * S2;
   	     	    
          	    B_1 		      = B_1 * (-1.0);
    		    P_1 		      = R_1;
    		    P_2		          = R_2;	  
    		
    	  }
    	
    	  R_1 		  = P_1 * P_3 + P_2 * P_4;
   		  R_2 		  = P_2 * P_3 - P_1 * P_4;
   		  P_1         = R_1;
   		  P_2		  = R_2;
   		  B_1		  = B_1 * (-1.0);
    	
    	  for ( m = n + 1; m < c_azimuth; ++m ) /// При фиксированном n, считаем дла m все значениа справа, не включаа n
    	  {
    		    
    		    R_1               = P_1 * P_3 + P_2 * P_4;
   		        R_2  	          = P_2 * P_3 - P_1 * P_4;
   		        S2                = B_1 * R_1 / R_2;
   		        ind_i             = m;
   		        
    		    znach_sum_RE_0   += mas_in_RE[ind_i] * S2;
   	            znach_sum_IM_0   += mas_in_IM[ind_i] * S2;
   	            ind_i             = ind_i + c_azimuth;
   	     
   	     	    znach_sum_RE_1   += mas_in_RE[ind_i] * S2;
   	     	    znach_sum_IM_1   += mas_in_IM[ind_i] * S2;
   	     	    ind_i             = ind_i + c_azimuth;
   	     
   	     	    znach_sum_RE_2   += mas_in_RE[ind_i] * S2;
   	     	    znach_sum_IM_2   += mas_in_IM[ind_i] * S2;
   	     	    ind_i             = ind_i + c_azimuth;
   	     
   	     	    znach_sum_RE_3   += mas_in_RE[ind_i] * S2;
   	     	    znach_sum_IM_3   += mas_in_IM[ind_i] * S2;
   	     	    ind_i             = ind_i + c_azimuth;
   	     
   	     	    znach_sum_RE_4   += mas_in_RE[ind_i] * S2;
   	     	    znach_sum_IM_4   += mas_in_IM[ind_i] * S2;
   	     	   
   	     	   
    		    P_1   	         = R_1;
   		        P_2		         = R_2;
   		        B_1			     = B_1 * (-1.0);
    	  }	
    	
    	  ind_i                  = n;	
    	  A 			         = A * (-1.0);	
    	  mas_out_RE[ind_i ]     = znach_sum_RE_0;
   		  mas_out_IM[ind_i ]     = znach_sum_IM_0;
          ind_i                  = ind_i + c_azimuth;
         
   		  mas_out_RE[ind_i]  	 = znach_sum_RE_1;
   		  mas_out_IM[ind_i]  	 = znach_sum_IM_1;
   		  ind_i             	 = ind_i + c_azimuth;
   
   		  mas_out_RE[ind_i]  	 = znach_sum_RE_2;
   		  mas_out_IM[ind_i]  	 = znach_sum_IM_2;
   		  ind_i             	 = ind_i + c_azimuth;
   	
   		  mas_out_RE[ind_i]  	 = znach_sum_RE_3;
   		  mas_out_IM[ind_i]  	 = znach_sum_IM_3;
   		  ind_i             	 = ind_i + c_azimuth;
   
   		  mas_out_RE[ind_i] 	 = znach_sum_RE_4;
   		  mas_out_IM[ind_i] 	 = znach_sum_IM_4;
   		 
   		 
 	  
    	
    }	
   
    n 		      = c_azimuth - 1;
     
    B_1 	      = -0.5;
    P_1           = cosf(Arg * n);
    P_2           = sinf(Arg * n);
    S1            = B_1 * P_1 / P_2;
    ind_i         = 0;
   
    znach_sum_RE_0    = mas_in_RE[ind_i] * S1;
    znach_sum_IM_0    = mas_in_IM[ind_i] * S1;
    ind_i             = ind_i + c_azimuth;
   
    znach_sum_RE_1    = mas_in_RE[ind_i] * S1;
    znach_sum_IM_1    = mas_in_IM[ind_i] * S1;
    ind_i             = ind_i + c_azimuth;
   
    znach_sum_RE_2    = mas_in_RE[ind_i] * S1;
    znach_sum_IM_2    = mas_in_IM[ind_i] * S1;
    ind_i             = ind_i + c_azimuth;
   
    znach_sum_RE_3    = mas_in_RE[ind_i] * S1;
    znach_sum_IM_3    = mas_in_IM[ind_i] * S1;
    ind_i             = ind_i + c_azimuth;
   
    znach_sum_RE_4    = mas_in_RE[ind_i] * S1;
    znach_sum_IM_4    = mas_in_IM[ind_i] * S1;
   
   
   
    B_1               = B_1 * (-1.0);
   
    for ( m = 1; m < n ; ++m )
    {
    	 
   	     
   		  R_1             = P_1 * P_3 + P_2 * P_4;
   		  R_2  	          = P_2 * P_3 - P_1 * P_4;
   		  S2              = B_1 * R_1 / R_2;
   		  ind_i           = m;
   		  
   		  znach_sum_RE_0 += mas_in_RE[ind_i] * S2;
   	      znach_sum_IM_0 += mas_in_IM[ind_i] * S2;
   	      ind_i           = ind_i + c_azimuth;
   	     
   	      znach_sum_RE_1 += mas_in_RE[ind_i] * S2;
   	      znach_sum_IM_1 += mas_in_IM[ind_i] * S2;
   	      ind_i           = ind_i + c_azimuth;
   	     
   	      znach_sum_RE_2 += mas_in_RE[ind_i] * S2;
   	      znach_sum_IM_2 += mas_in_IM[ind_i] * S2;
   	      ind_i           = ind_i + c_azimuth;
   	     
   	      znach_sum_RE_3 += mas_in_RE[ind_i] * S2;
   	      znach_sum_IM_3 += mas_in_IM[ind_i] * S2;
   	      ind_i           = ind_i + c_azimuth;
   	     
   	      znach_sum_RE_4 += mas_in_RE[ind_i] * S2;
   	      znach_sum_IM_4 += mas_in_IM[ind_i] * S2;
   	     
   		  B_1             = B_1 * (-1.0);
   		  P_1   	      = R_1;
   		  P_2		      = R_2;
   		 
   		 	   	   
    }
   
   	   	  	   	   	
    ind_i             = n; 	  	
    mas_out_RE[ind_i] = znach_sum_RE_0;
    mas_out_IM[ind_i] = znach_sum_IM_0;
    ind_i             = ind_i + c_azimuth;
   
    mas_out_RE[ind_i] = znach_sum_RE_1;
    mas_out_IM[ind_i] = znach_sum_IM_1;
    ind_i             = ind_i + c_azimuth;
   
    mas_out_RE[ind_i] = znach_sum_RE_2;
    mas_out_IM[ind_i] = znach_sum_IM_2;
    ind_i             = ind_i + c_azimuth;
   
    mas_out_RE[ind_i] = znach_sum_RE_3;
    mas_out_IM[ind_i] = znach_sum_IM_3;
    ind_i             = ind_i + c_azimuth;
   
    mas_out_RE[ind_i] = znach_sum_RE_4;
    mas_out_IM[ind_i] = znach_sum_IM_4;
    
   
  
   			 	        	      	
}
