#ifndef HEADER_FUNC_USW_5
#define HEADER_FUNC_USW_5


void Main_Detection_Grouping_Direction_Finding_USW_5(void);

float Correlation_Detector_5(int number_bin);

void Correlation_Grouping_5(float *RW_1,float *IW_1,float *RW_2,float *IW_2);

extern void Direction_Finding_USW_5(void);

void Read_Params_Freqdiap_USW_5(void);

void Form_ReU_ImU_AC_USW_5(void);

void Cubic_Interpolation_USW_5( float F_diff, float y_0, float y_1, float y_2, float y_3 );
           
void Kotel_D_ReU_D_ImU_AC_USW_5(float *mas_in_RE, float *mas_out_RE,float *mas_in_IM, float *mas_out_IM);

void Calculating_Direction_Finding_Relief_USW_5(float *rW_signal,float *iW_signal);

void Iteration_Search_Global_Max_USW_5(float *rW_signal, float *iW_signal);

void Finding_Coord_Vertex_USW_5(float x_1, float x_2, float y_1, float y_2, float dy_1);

void Rasch_Proizv_Znach_Vtochke(float *rW_signal,float *iW_signal);

void Interpol_Modif_Kotel_Chet_5(float *masin_RE, float *masin_D_RE, float *masin_IM,float *masin_D_IM, float *masout_RE, float *masout_D_RE, float *masout_IM, float *masout_D_IM  );



#endif  /* HEADER_FUNC_USW_5 */
