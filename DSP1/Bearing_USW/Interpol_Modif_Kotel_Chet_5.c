
////////////////////// Alg №11  ////////////////////////

/////////// Интерполяциа период функциии с помощью модефициров рада Котельникова ////////////


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include "Header_Func_USW_5.h"
#include "Header.h"

#include "variables_USW.h"


void Interpol_Modif_Kotel_Chet_5 ( float *masin_RE, float *masin_D_RE, float *masin_IM,float *masin_D_IM, float *masout_RE, float *masout_D_RE, float *masout_IM, float *masout_D_IM  )
{

   int 	 m;           // номер азимута дла цикла   
   int   A;
   float arg_sin;
   float Half_x;
   float SINF;
   float S1,S2,S3;
   float P_1, P_2, P_3, P_4, R_1, R_2;
   int   ind_i;
   
 
   S3         = 1.0;
  
   Half_x	  = x / 2.0;
   arg_sin	  = c_azimuth * Half_x;
   SINF       = sinf (arg_sin);
   
   
  if (( A = (int)(arg_sin /  M_PI )) &&( (arg_sin /  M_PI) - A == 0.0 ))	
  {
        masout_RE[0]    = masin_RE[A];
        masout_RE[1]    = masin_RE[A];
        masout_RE[2]    = masin_RE[A];
        masout_RE[3]    = masin_RE[A];
        masout_RE[4]    = masin_RE[A];
       
        masout_D_RE[0]  = masin_D_RE[A];
        masout_D_RE[1]  = masin_D_RE[A];
        masout_D_RE[2]  = masin_D_RE[A];
        masout_D_RE[3]  = masin_D_RE[A];
        masout_D_RE[4]  = masin_D_RE[A];
        
         
        masout_IM[0]    = masin_IM[A];
        masout_IM[1]    = masin_IM[A];
        masout_IM[2]    = masin_IM[A];
        masout_IM[3]    = masin_IM[A];
        masout_IM[4]    = masin_IM[A];
       
          
        masout_D_IM[0]  = masin_D_IM[A]; 
        masout_D_IM[1]  = masin_D_IM[A];
        masout_D_IM[2]  = masin_D_IM[A];
        masout_D_IM[3]  = masin_D_IM[A]; 
        masout_D_IM[4]  = masin_D_IM[A]; 
              
	
  }	
   
  else
  {
   
   		P_1             = cosf(Half_x);
   		P_2             = sinf(Half_x);
   		P_3             = cosf(M_PI * Znam);
   		P_4             = sinf(M_PI * Znam);
   		S1              = SINF * S3 * P_1 / P_2;
        ind_i           = 0;
         
   		masout_RE  [0]    = masin_RE  [ind_i]  * S1;
   		masout_D_RE[0]    = masin_D_RE[ind_i]  * S1;
   		masout_IM  [0]    = masin_IM  [ind_i]  * S1;
   		masout_D_IM[0]    = masin_D_IM[ind_i]  * S1;
   		ind_i			  = ind_i + c_azimuth;
   		
   		masout_RE  [1]    = masin_RE  [ind_i]  * S1;
   		masout_D_RE[1]    = masin_D_RE[ind_i]  * S1;
   		masout_IM  [1]    = masin_IM  [ind_i]  * S1;
   		masout_D_IM[1]    = masin_D_IM[ind_i]  * S1;
   		ind_i			  = ind_i + c_azimuth;
   		
		masout_RE  [2]    = masin_RE  [ind_i]  * S1;
		masout_D_RE[2]    = masin_D_RE[ind_i]  * S1;
		masout_IM  [2]    = masin_IM  [ind_i]  * S1;
		masout_D_IM[2]    = masin_D_IM[ind_i]  * S1;
		ind_i			  = ind_i + c_azimuth;
		
		masout_RE  [3]    = masin_RE  [ind_i]  * S1;
		masout_D_RE[3]    = masin_D_RE[ind_i]  * S1;
		masout_IM  [3]    = masin_IM  [ind_i]  * S1;
		masout_D_IM[3]    = masin_D_IM[ind_i]  * S1;
		ind_i			  = ind_i + c_azimuth;
		
		masout_RE  [4]    = masin_RE  [ind_i]  * S1;
		masout_D_RE[4]    = masin_D_RE[ind_i]  * S1;
		masout_IM  [4]    = masin_IM  [ind_i]  * S1;
		masout_D_IM[4]    = masin_D_IM[ind_i]  * S1;
		
   
        S3               = S3 * (-1.0);
        for ( m = 1; m < c_azimuth ; ++m )
        {
    	 
   		 	  R_1             = P_1 * P_3 + P_2 * P_4;
   		 	  R_2  	          = P_2 * P_3 - P_1 * P_4;
   		 	  S2              = SINF * S3 * R_1 / R_2;
   		 	  ind_i           = m;
   		 
   		 	  masout_RE  [0] += masin_RE  [ind_i]  * S2;
   		 	  masout_D_RE[0] += masin_D_RE[ind_i]  * S2;
   		 	  masout_IM  [0] += masin_IM  [ind_i]  * S2;
   		 	  masout_D_IM[0] += masin_D_IM[ind_i]  * S2;
   		 	  ind_i  		  = ind_i + c_azimuth;
   		 	  
   		 	  masout_RE  [1] += masin_RE  [ind_i]  * S2;
   		 	  masout_D_RE[1] += masin_D_RE[ind_i]  * S2;
   		 	  masout_IM  [1] += masin_IM  [ind_i]  * S2;
   		 	  masout_D_IM[1] += masin_D_IM[ind_i]  * S2;
   		 	  ind_i  		  = ind_i + c_azimuth;
   		 	  
   		 	  
   		 	  masout_RE  [2] += masin_RE  [ind_i]  * S2;
   		 	  masout_D_RE[2] += masin_D_RE[ind_i]  * S2;
   		 	  masout_IM  [2] += masin_IM  [ind_i]  * S2;
   		 	  masout_D_IM[2] += masin_D_IM[ind_i]  * S2;
   		 	  ind_i  		  = ind_i + c_azimuth;
   		 	  
	   		  masout_RE  [3] += masin_RE  [ind_i]  * S2;
	   		  masout_D_RE[3] += masin_D_RE[ind_i]  * S2;
	   		  masout_IM  [3] += masin_IM  [ind_i]  * S2;
	   		  masout_D_IM[3] += masin_D_IM[ind_i]  * S2;
	   		  ind_i  		  = ind_i + c_azimuth;
	   		  
	   		  
	   		  masout_RE  [4] += masin_RE  [ind_i]  * S2;
	   		  masout_D_RE[4] += masin_D_RE[ind_i]  * S2;
	   		  masout_IM  [4] += masin_IM  [ind_i]  * S2;
	   		  masout_D_IM[4] += masin_D_IM[ind_i]  * S2;
	   		  
   	      
   		      S3              = S3 * (-1.0);
   		      P_1   	      = R_1;
   		      P_2		      = R_2;
   		 
   		 	   	   
        }
   
   	   	  	   	   	   
  }
   
   
   
   		
}
