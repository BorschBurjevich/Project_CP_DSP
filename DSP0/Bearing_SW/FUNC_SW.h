#ifndef FUNC_SW_H

#define FUNC_SW_H

void FUNC_Main_Detection_Grouping_Direction_Finding_SW();

void FUNC_Correlation_Detector_4_SW(int Nr,int Nb,float *Re0,float *Re1,float *Re2,float *Re3,float *Im0,float *Im1,float *Im2,float *Im3,int number_bin);

void FUNC_Correlation_Grouping_4_SW(float *RW_1,float *IW_1,float *RW_2,float *IW_2);

/////////////////////////////////////////////////////////////////////////////////

void FUNC_Direction_F_SW();

void FUNC_Read_Params_Freqdiap_SW();

void FUNC_Re_Im_AC_Frequency_SW();

void FUNC_Cubic_Interpolation_SW( float F_diff, float y_0, float y_1, float y_2, float y_3 );

void FUNC_Direction_Finding_Reports_SW( float *mmas_ReW, float *mmas_ImW);

void FUNC_Search_Related_Pointers_Index_SW ();

void FUNC_Calculating_Derivate_Value_Argue_Points_SW (int ind_max, float *mas, int count_point);

void FUNC_Modif_Kotel_Derivative_Point_SW(float x, float *mmas, int count_point);

void FUNC_Calculating_PhaseCrit_Choise_MainLobe_SW();

void FUNC_Maximising_Direction_F_SW();

void FUNC_Calculating_Kotel_Point_X_SW(float x, float *mas, int count_point);

void FUNC_Elimination_Uncert_Choise_MainLobe(float *ReW);

#endif  /* FUNC_SW_H */
