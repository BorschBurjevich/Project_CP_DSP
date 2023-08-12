/*�������� ���������� �� ������� Mas_Inform_AC_Loop  
���������� � ��������� ������ ����� "��������� ��������" ������� Mas_AC_Loop  ,
� �������� (�����) ����������� ������� ������� Fc   
*/

#include <Bearing_SW/DEFINES_SW.h>
#include <Bearing_SW/FUNC_SW.h>
#include "variables_SW.h"



void FUNC_Read_Params_Freqdiap_SW()
{
	Fc_SW            = 1.0;
	
    int           count_range;        
	int           ii;
 
	// ���������� ���-�� ��������� ����������//
	
	count_range = Mas_Inform_AC_Loop[0];
	
	// ��������� ��������� ����� "��������� �������� � i Mas_AC_Loop ������� //
	
   	for ( ii = 0; ii < 	count_range ; ii++ )
   	{
   		  F_stop_SW  = Mas_Inform_AC_Loop[3+ii*10];
   		
   		  if ( Fc_SW <=  F_stop_SW )
   		  {	
   		 	   c_frequency_SW     = Mas_Inform_AC_Loop[1+ii*10];
   	   		   F_start_SW         = Mas_Inform_AC_Loop[2+ii*10];
   	   		   F_shag_SW          = Mas_Inform_AC_Loop[4+ii*10];
   	   		   c_azimuth_SW       = Mas_Inform_AC_Loop[5+ii*10];
   	   		   c_channel_SW       = Mas_Inform_AC_Loop[9+ii*10];
   	   		   ind_start_SW       = Mas_Inform_AC_Loop[10+ii*10];
   	   		   break;
   		 	
   		  }	
   		
    }			
	
}

