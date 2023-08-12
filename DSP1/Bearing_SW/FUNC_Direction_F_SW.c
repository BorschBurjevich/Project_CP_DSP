/*****************************************************************************
 * LORANDIT_D_KV.c
 *****************************************************************************/
#include <Bearing_SW/DEFINES_SW.h>
#include <Bearing_SW/FUNC_SW.h>
#include "variables_SW.h"

// 196 160 тактов // 
void FUNC_Direction_F_SW()
{
	FUNC_Read_Params_Freqdiap_SW();
	FUNC_Re_Im_AC_Frequency_SW();
	FUNC_Direction_Finding_Reports_SW(&mas_ReW[0], &mas_ImW[0]);
	FUNC_Search_Related_Pointers_Index_SW();
	FUNC_Calculating_Derivate_Value_Argue_Points_SW(ind_max_1,&Mtheta_SW[0],SIZE_THETA);
	FUNC_Calculating_PhaseCrit_Choise_MainLobe_SW();
	FUNC_Elimination_Uncert_Choise_MainLobe(&mas_ReW[0]);	
}	

