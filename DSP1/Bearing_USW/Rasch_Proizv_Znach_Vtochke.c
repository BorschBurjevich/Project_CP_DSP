          //////////////////////// Alg № 10 ////////////////////////////////

#include "Header_Func_USW_5.h"
#include "Header.h"
#include "variables_USW.h"
#include <math.h>

void Rasch_Proizv_Znach_Vtochke(float *rW_signal, float *iW_signal )
{

   float  rA0, rA1, rA2, rA3, rA4;
   float  iA0, iA1, iA2, iA3, iA4;
   float  B1;                 // знаменатель
   float  B2;                // числитель
   float  Mteta_chisl;
   float  Mteta_itog;
   float  D_Mteta_chisl;
   float  D_Mteta_itog;
   
 
   B1 			 = 0.0;
   B2 			 = 0.0;
   
 
  
   
   Interpol_Modif_Kotel_Chet_5(&ReU[0],&D_ReU[0],&ImU[0],&D_ImU[0],&Mas_ReU_x[0],&Mas_D_ReU_x[0],&Mas_ImU_x[0],&Mas_D_ImU_x[0]);
			
       							 
   B1 = B1+Mas_ReU_x[0] *  Mas_ReU_x[0]  + Mas_ImU_x[0] * Mas_ImU_x[0]; 
   B2 = B2+Mas_ReU_x[0] *  Mas_D_ReU_x[0]+ Mas_ImU_x[0] * Mas_D_ImU_x[0];
   
   B1 = B1+Mas_ReU_x[1] *  Mas_ReU_x[1]  + Mas_ImU_x[1] * Mas_ImU_x[1];  
   B2 = B2+Mas_ReU_x[1] *  Mas_D_ReU_x[1]+ Mas_ImU_x[1] * Mas_D_ImU_x[1];
   
   B1 = B1+Mas_ReU_x[2] *  Mas_ReU_x[2]  + Mas_ImU_x[2] * Mas_ImU_x[2];   
   B2 = B2+Mas_ReU_x[2] *  Mas_D_ReU_x[2]+ Mas_ImU_x[2] * Mas_D_ImU_x[2];
   
   B1 = B1+Mas_ReU_x[3] *  Mas_ReU_x[3]  + Mas_ImU_x[3] * Mas_ImU_x[3];  
   B2 = B2+Mas_ReU_x[3] *  Mas_D_ReU_x[3]+ Mas_ImU_x[3] * Mas_D_ImU_x[3];
   
   B1 = B1+Mas_ReU_x[4] *  Mas_ReU_x[4]  + Mas_ImU_x[4] * Mas_ImU_x[4];   
   B2 = B2+Mas_ReU_x[4] *  Mas_D_ReU_x[4]+ Mas_ImU_x[4] * Mas_D_ImU_x[4];	    
   
 //  for ( i = 0; i < c_channel; i++ )
//   {
//		 B1 = B1+Mas_ReU_x[i] *  Mas_ReU_x[i]  + Mas_ImU_x[i] * Mas_ImU_x[i];
//		 B2 = B2+Mas_ReU_x[i] *  Mas_D_ReU_x[i]+ Mas_ImU_x[i] * Mas_D_ImU_x[i];
//   }
	
   fletch = 1.0 / B1;
  		
   rA0  = Mas_ReU_x[0]*rW_signal[0] + Mas_ReU_x[1]*rW_signal[1]  + Mas_ReU_x[2]*rW_signal[2]  +  Mas_ReU_x[3]*rW_signal[3]  + Mas_ReU_x[4]*rW_signal[4]
  	    + Mas_ImU_x[0]*iW_signal[0] - Mas_ImU_x[1]*iW_signal[1]  - Mas_ImU_x[2]*iW_signal[2]  -  Mas_ImU_x[3]*iW_signal[3]  - Mas_ImU_x[4]*iW_signal[4];
  	     
   rA1  = Mas_ReU_x[0]*rW_signal[1] + Mas_ReU_x[1]*rW_signal[7]  + Mas_ReU_x[2]*rW_signal[8]   + Mas_ReU_x[3]*rW_signal[9]  + Mas_ReU_x[4]*rW_signal[10]
  	    + Mas_ImU_x[0]*iW_signal[1] + Mas_ImU_x[1]*iW_signal[7]  - Mas_ImU_x[2]*iW_signal[8]   - Mas_ImU_x[3]*iW_signal[9]  - Mas_ImU_x[4]*iW_signal[10];
  	     
   rA2  = Mas_ReU_x[0]*rW_signal[2] + Mas_ReU_x[1]*rW_signal[8]  + Mas_ReU_x[2]*rW_signal[13]  + Mas_ReU_x[3]*rW_signal[14] + Mas_ReU_x[4]*rW_signal[15]
  	    + Mas_ImU_x[0]*iW_signal[2] + Mas_ImU_x[1]*iW_signal[8]  + Mas_ImU_x[2]*iW_signal[13]  - Mas_ImU_x[3]*iW_signal[14] - Mas_ImU_x[4]*iW_signal[15];
  	     
   rA3  = Mas_ReU_x[0]*rW_signal[3] + Mas_ReU_x[1]*rW_signal[9]  + Mas_ReU_x[2]*rW_signal[14]  + Mas_ReU_x[3]*rW_signal[18] + Mas_ReU_x[4]*rW_signal[19]
  	    + Mas_ImU_x[0]*iW_signal[3] + Mas_ImU_x[1]*iW_signal[9]  + Mas_ImU_x[2]*iW_signal[14]  + Mas_ImU_x[3]*iW_signal[18] - Mas_ImU_x[4]*iW_signal[19];
  	
   rA4  = Mas_ReU_x[0]*rW_signal[4] + Mas_ReU_x[1]*rW_signal[10] + Mas_ReU_x[2]*rW_signal[15]  + Mas_ReU_x[3]*rW_signal[19] + Mas_ReU_x[4]*rW_signal[22]
  	    + Mas_ImU_x[0]*iW_signal[4] + Mas_ImU_x[1]*iW_signal[10] + Mas_ImU_x[2]*iW_signal[15]  + Mas_ImU_x[3]*iW_signal[19] + Mas_ImU_x[4]*iW_signal[22];
  	     
   
	
   iA0  = Mas_ReU_x[0]*iW_signal[0] - Mas_ReU_x[1]*iW_signal[1]  - Mas_ReU_x[2]*iW_signal[2]  - Mas_ReU_x[3]*iW_signal[3]  - Mas_ReU_x[4]*iW_signal[4]
	    - Mas_ImU_x[0]*rW_signal[0] - Mas_ImU_x[1]*rW_signal[1]  - Mas_ImU_x[2]*rW_signal[2]  - Mas_ImU_x[3]*rW_signal[3]  - Mas_ImU_x[4]*rW_signal[4];
	       
   iA1  = Mas_ReU_x[0]*iW_signal[1] + Mas_ReU_x[1]*iW_signal[7]  - Mas_ReU_x[2]*iW_signal[8]  - Mas_ReU_x[3]*iW_signal[9]  - Mas_ReU_x[4]*iW_signal[10]
	    - Mas_ImU_x[0]*rW_signal[1] - Mas_ImU_x[1]*rW_signal[7]  - Mas_ImU_x[2]*rW_signal[8]  - Mas_ImU_x[3]*rW_signal[9]  - Mas_ImU_x[4]*rW_signal[10];
	      
   iA2  = Mas_ReU_x[0]*iW_signal[2] + Mas_ReU_x[1]*iW_signal[8]  + Mas_ReU_x[2]*iW_signal[13] - Mas_ReU_x[3]*iW_signal[14] - Mas_ReU_x[4]*iW_signal[15] 
	    - Mas_ImU_x[0]*rW_signal[2] - Mas_ImU_x[1]*rW_signal[8]  - Mas_ImU_x[2]*rW_signal[13] - Mas_ImU_x[3]*rW_signal[14] - Mas_ImU_x[4]*rW_signal[15];
	      
   iA3  = Mas_ReU_x[0]*iW_signal[3] + Mas_ReU_x[1]*iW_signal[9]  + Mas_ReU_x[2]*iW_signal[14] + Mas_ReU_x[3]*iW_signal[18] - Mas_ReU_x[4]*iW_signal[19] 
	    - Mas_ImU_x[0]*rW_signal[3] - Mas_ImU_x[1]*rW_signal[9]  - Mas_ImU_x[2]*rW_signal[14] - Mas_ImU_x[3]*rW_signal[18] - Mas_ImU_x[4]*rW_signal[19];
	      
   iA4  = Mas_ReU_x[0]*iW_signal[4] + Mas_ReU_x[1]*iW_signal[10] + Mas_ReU_x[2]*iW_signal[15] + Mas_ReU_x[3]*iW_signal[19] + Mas_ReU_x[4]*iW_signal[22]
	    - Mas_ImU_x[0]*rW_signal[4] - Mas_ImU_x[1]*rW_signal[10] - Mas_ImU_x[2]*rW_signal[15] - Mas_ImU_x[3]*rW_signal[19] - Mas_ImU_x[4]*rW_signal[22];
	      
   
  
  	  
   Mteta_chisl   =               rA0 * Mas_ReU_x[0]   + rA1 * Mas_ReU_x[1]  + rA2 * Mas_ReU_x[2] + rA3 * Mas_ReU_x[3] + rA4 * Mas_ReU_x[4];
   
   Mteta_itog    = Mteta_chisl - iA0 * Mas_ImU_x[0]   - iA1 * Mas_ImU_x[1]  - iA2 * Mas_ImU_x[2] - iA3 * Mas_ImU_x[3] - iA4 * Mas_ImU_x[4];
   
   Mteta_x       = Mteta_itog * fletch;
   
   
   D_Mteta_chisl =                 rA0*Mas_D_ReU_x[0] +rA1*Mas_D_ReU_x[1]+rA2*Mas_D_ReU_x[2]+rA3*Mas_D_ReU_x[3]+rA4*Mas_D_ReU_x[4];
        
   D_Mteta_itog  = D_Mteta_chisl - iA0*Mas_D_ImU_x[0]- iA1*Mas_D_ImU_x[1]-iA2*Mas_D_ImU_x[2]-iA3*Mas_D_ImU_x[3]-iA4*Mas_D_ImU_x[4];
		
   D_Mteta_x     = 2.0 * (D_Mteta_itog - (Mteta_itog * B2)) * fletch;

   Amp_IRI_db    = 10.0 * log10(Mteta_x);

}
