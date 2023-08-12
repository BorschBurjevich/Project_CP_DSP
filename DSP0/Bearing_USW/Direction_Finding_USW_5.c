#include "Header_Func_USW_5.h"
#include "Header.h"
#include "variables_USW.h"


void Direction_Finding_USW_5()
{
   
	
	Read_Params_Freqdiap_USW_5();
	Form_ReU_ImU_AC_USW_5();
	Kotel_D_ReU_D_ImU_AC_USW_5(&ReU[0],&D_ReU[0],&ImU[0],&D_ImU[0]);
	Calculating_Direction_Finding_Relief_USW_5(&ReW_Accumulation[0],&ImW_Accumulation[0]);
	Iteration_Search_Global_Max_USW_5(&ReW_Accumulation[0],&ImW_Accumulation[0]);

		
}	
