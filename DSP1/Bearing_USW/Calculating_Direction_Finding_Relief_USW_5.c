///////////////// Alg №5 //////////////////////

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include "Header_Func_USW_5.h"
#include "Header.h"
#include "variables_USW.h"


void Calculating_Direction_Finding_Relief_USW_5 (float *rW_signal,float *iW_signal)
{

	int    na;                          // номер азимута

	float  rA0,rA1,rA2,rA3,rA4;
	float  iA0,iA1,iA2,iA3,iA4;
	float  B1;             				// знаменатель
	float  B2;             				// числитель
	float  Mteta_chisl;
	float  Mteta_itog;
	float  D_Mteta_chisl;
	float  D_Mteta_itog;

	int    C2,C3,C4;
	
	
	C2  = 2 * c_azimuth; 
    C3  = 3 * c_azimuth;
    C4  = 4 * c_azimuth;		
	
	
//////////////// Цикл по азимутам для рассчета M_teta и D_Mteta  ///////////////////
	
    for ( na = 0; na < c_azimuth; na++ )
    {
		  B1 			  = 0.0;
		  B2 			  = 0.0;
		
		  B1  = B1+ReU[na]*ReU[na]  + ImU[na] * ImU[na];
		  B2  = B2+ReU[na]*D_ReU[na]+ ImU[na] * D_ImU[na];
		   
		  B1  = B1+ReU[c_azimuth + na]*ReU[c_azimuth + na]   + ImU[c_azimuth + na] * ImU[c_azimuth + na];
		  B2  = B2+ReU[c_azimuth + na]*D_ReU[c_azimuth + na] + ImU[c_azimuth + na] * D_ImU[c_azimuth + na];
		  
		  B1  = B1+ReU[C2 + na]*ReU[C2 + na]  + ImU[C2 + na] * ImU[C2 + na];
		  B2  = B2+ReU[C2 + na]*D_ReU[C2 + na]+ ImU[C2 + na] * D_ImU[C2 + na];
		  
		  B1  = B1+ReU[C3 + na]*ReU[C3 + na]  + ImU[C3 + na] * ImU[C3 + na];
		  B2  = B2+ReU[C3 + na]*D_ReU[C3 + na]+ ImU[C3 + na] * D_ImU[C3 + na];
		  
		  B1  = B1+ReU[C4 + na]*ReU[C4 + na]  + ImU[C4 + na] * ImU[C4 + na];
		  B2  = B2+ReU[C4 + na]*D_ReU[C4 + na]+ ImU[C4 + na] * D_ImU[C4 + na];
		  		  
		  		  		
		 // for (  i = 0; i < c_channel; i++ )
		  //{ 
		//	     B1  = B1+ReU[i * c_azimuth + na]*ReU[i * c_azimuth + na]  + ImU[i * c_azimuth + na] * ImU[i * c_azimuth + na];
		//	     B2  = B2+ReU[i * c_azimuth + na]*D_ReU[i * c_azimuth + na]+ ImU[i * c_azimuth + na] * D_ImU[i * c_azimuth + na];
		//  }
	 
                             
          rA0    = ReU[na]*rW_signal[0] + ReU[c_azimuth + na]*rW_signal[1]  + ReU[C2 + na]*rW_signal[2]  + ReU[C3 + na]*rW_signal[3]  + ReU[C4 + na]* rW_signal[4]     
                 + ImU[na]*iW_signal[0] - ImU[c_azimuth + na]*iW_signal[1]  - ImU[C2 + na]*iW_signal[2]  - ImU[C3 + na]*iW_signal[3]  - ImU[C4 + na]*iW_signal[4];
               
          rA1    = ReU[na]*rW_signal[1] + ReU[c_azimuth + na]*rW_signal[5]  + ReU[C2 + na]*rW_signal[6]  + ReU[C3 + na]*rW_signal[7]  + ReU[C4 + na]*rW_signal[8]
                 + ImU[na]*iW_signal[1] + ImU[c_azimuth + na]*iW_signal[5]  - ImU[C2 + na]*iW_signal[6]  - ImU[C3 + na]*iW_signal[7]  - ImU[C4 + na]*iW_signal[8];
                                    
          rA2    = ReU[na]*rW_signal[2] + ReU[c_azimuth + na]*rW_signal[6]  + ReU[C2 + na]*rW_signal[9]  + ReU[C3 + na]*rW_signal[10] + ReU[C4 + na]*rW_signal[11]
                 + ImU[na]*iW_signal[2] + ImU[c_azimuth + na]*iW_signal[6]  + ImU[C2 + na]*iW_signal[9]  - ImU[C3 + na]*iW_signal[10] - ImU[C4 + na]*iW_signal[11];
               
          rA3    = ReU[na]*rW_signal[3] + ReU[c_azimuth + na]*rW_signal[7]  + ReU[C2 + na]*rW_signal[10] + ReU[C3 + na]*rW_signal[12] + ReU[C4 + na]*rW_signal[13] 
                 + ImU[na]*iW_signal[3] + ImU[c_azimuth + na]*iW_signal[7]  + ImU[C2 + na]*iW_signal[10] + ImU[C3 + na]*iW_signal[12] - ImU[C4 + na]*iW_signal[13];
               
          rA4    = ReU[na]*rW_signal[4] + ReU[c_azimuth + na]*rW_signal[8]  + ReU[C2 + na]*rW_signal[11] + ReU[C3 + na]*rW_signal[13] + ReU[C4 + na]*rW_signal[14]
                 + ImU[na]*iW_signal[4] + ImU[c_azimuth + na]*iW_signal[8]  + ImU[C2 + na]*iW_signal[11] + ImU[C3 + na]*iW_signal[13] + ImU[C4 + na]*iW_signal[14];
               
          
                         
          iA0    = ReU[na]*iW_signal[0] - ReU[c_azimuth + na]*iW_signal[1]  - ReU[C2 + na]*iW_signal[2]  - ReU[C3 + na]*iW_signal[3]  - ReU[C4 + na]*iW_signal[4]
                 - ImU[na]*rW_signal[0] - ImU[c_azimuth + na]*rW_signal[1]  - ImU[C2 + na]*rW_signal[2]  - ImU[C3 + na]*rW_signal[3]  - ImU[C4 + na]*rW_signal[4];
              
		  iA1    = ReU[na]*iW_signal[1] + ReU[c_azimuth + na]*iW_signal[5]  - ReU[C2 + na]*iW_signal[6]  - ReU[C3 + na]*iW_signal[7]  - ReU[C4 + na]*iW_signal[8]
		         - ImU[na]*rW_signal[1] - ImU[c_azimuth + na]*rW_signal[5]  - ImU[C2 + na]*rW_signal[6]  - ImU[C3 + na]*rW_signal[7]  - ImU[C4 + na]*rW_signal[8];
		      
		  iA2    = ReU[na]*iW_signal[2] + ReU[c_azimuth + na]*iW_signal[6]  + ReU[C2 + na]*iW_signal[9] - ReU[C3 + na]*iW_signal[10]  - ReU[C4 + na]*iW_signal[11]
		         - ImU[na]*rW_signal[2] - ImU[c_azimuth + na]*rW_signal[6]  - ImU[C2 + na]*rW_signal[9] - ImU[C3 + na]*rW_signal[10]  - ImU[C4 + na]*rW_signal[11];
		      
		  iA3    = ReU[na]*iW_signal[3] + ReU[c_azimuth + na]*iW_signal[7]  + ReU[C2 + na]*iW_signal[10] + ReU[C3 + na]*iW_signal[12] - ReU[C4 + na]*iW_signal[13]
		         - ImU[na]*rW_signal[3] - ImU[c_azimuth + na]*rW_signal[7]  - ImU[C2 + na]*rW_signal[10] - ImU[C3 + na]*rW_signal[12] - ImU[C4 + na]*rW_signal[13];
		      
		  iA4    = ReU[na]*iW_signal[4] + ReU[c_azimuth + na]*iW_signal[8]  + ReU[C2 + na]*iW_signal[11] + ReU[C3 + na]*iW_signal[13] + ReU[C4 + na]*iW_signal[14]
		         - ImU[na]*rW_signal[4] - ImU[c_azimuth + na]*rW_signal[8]  - ImU[C2 + na]*rW_signal[11] - ImU[C3 + na]*rW_signal[13] - ImU[C4 + na]*rW_signal[14];
		      
		 

		  Mteta_chisl   =                 rA0*ReU[na]  +rA1*ReU[c_azimuth + na]  +rA2*ReU[C2 + na]   +rA3*ReU[C3 + na] + rA4*ReU[C4 + na];
		  Mteta_itog    = Mteta_chisl   - iA0*ImU[na]  -iA1*ImU[c_azimuth + na]  -iA2*ImU[C2 + na]   -iA3*ImU[C3 + na] - iA4*ImU[C4 + na];
     
		  Mteta[na]     = Mteta_itog / B1;
		
		  D_Mteta_chisl =                 rA0*D_ReU[na]+rA1*D_ReU[c_azimuth + na]+rA2*D_ReU[C2 + na] +rA3*D_ReU[C3 + na]+rA4*D_ReU[C4 + na];
		  D_Mteta_itog  = D_Mteta_chisl - iA0*D_ImU[na]-iA1*D_ImU[c_azimuth + na]-iA2*D_ImU[C2 + na] -iA3*D_ImU[C3 + na]-iA4*D_ImU[C4 + na];
		
		  D_Mteta[na]   = 2.0 * (D_Mteta_itog - (Mteta_itog * B2)) / B1;
		
    }
			   	   	   	   
}
