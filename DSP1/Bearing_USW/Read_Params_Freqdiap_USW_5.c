///////////////////////////// Alg №2 ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////Алгоритм считывания из массива Mas_Inform_AC////////////////////////////////
////////параметров о структуре данных блока  "Частотный диапазон" массива Mas_AC,////////
/////////////////к которому (блоку) принадлежит входная частота ///////////////////////// 




#include "Header_Func_USW_5.h"
#include "Header.h"
#include "variables_USW.h"

void Read_Params_Freqdiap_USW_5 (void)
{
	int           Nd;         // кол-во частотных диапазонов
	int           ii;
 
	Nd = Mas_Inform_AC[0];
	Fc = central_Freq_Signal;
   	for ( ii = 0; ii < Nd; ii++ )
   	{
   		  F_stop  = Mas_Inform_AC[3+ii*10];
   		
   		  if ( Fc <=  F_stop )
   		  {	
   		 	   c_freq        = Mas_Inform_AC[1+ii*10];
   	   		   F_start       = Mas_Inform_AC[2+ii*10];
   	   		   F_shag        = Mas_Inform_AC[4+ii*10];
   	   		   c_azimuth     = Mas_Inform_AC[5+ii*10];	
   	   		   azimuth_start = Mas_Inform_AC[6+ii*10];
   	   		   azimuth_stop  = Mas_Inform_AC[7+ii*10];
   	   		   azimuth_shag  = Mas_Inform_AC[8+ii*10];
   	   		   c_channel     = Mas_Inform_AC[9+ii*10];
   	   		   ind_first     = Mas_Inform_AC[10+ii*10];
   	   		   Znam          = (float) 1.0 / c_azimuth;
   	   		   break;
   		 	
   		  }	
   		
    }		
  
  	
}
