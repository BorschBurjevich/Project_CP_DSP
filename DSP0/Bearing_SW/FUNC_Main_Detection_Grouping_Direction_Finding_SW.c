#include <Bearing_SW/DEFINES_SW.h>
#include <Bearing_SW/FUNC_SW.h>
#include "variables_SW.h"


void FUNC_Main_Detection_Grouping_Direction_Finding_SW()
{ 
// Локальные переменные и массивы
	int i,j;
	int count_signal;
	int bin_in_signal;
	int flag_group_open;
	int flag_group_close;	
	int start_group_bin;
	int stop_group_bin;	
	int count_adjacent_bin;
	int max_adjacent_bin;
		
	float P;
	float numerator_freq;
	float denominator_freq;	
	
	//Инициализациа переменных и массивов
	count_signal          = 0;
	bin_in_signal         = 0;	
	flag_group_open       = 0;
	flag_group_close      = 0;
	numerator_freq        = 0.0;
	denominator_freq      = 0.0;
	count_bin_SW          = word13;
	
	/////////////////////////////

		
	for(i = 0; i < count_bin_SW; i++)
	{
		//Расчет решающей статистики детектирования (сигнальный, шумовой или сложный) спектрального отсчета
		FUNC_Correlation_Detector_4_SW(count_read_SW,count_bin_SW,&ReS0_SW[0],&ReS1_SW[0],&ReS2_SW[0],&ReS3_SW[0],&ImS0_SW[0],&ImS1_SW[0],&ImS2_SW[0],&ImS3_SW[0],i);
		
		if(stat_detection_SW >= bound_detection_SW)
		{
			
			
			//Продолжение итерации "Обнаружение, группирование и пеленгование"
			if(flag_group_open == 1)
			{
				//Проверка 2-х смежных сигнальных бинов по критерию биновой (частотной) близости
				if((i-stop_group_bin) <= bound_max_distance_SW)
				{
					//Расчет решающей статистики группирования 2-х спектральных отсчетов
					FUNC_Correlation_Grouping_4_SW(&ReW_Accumulation_SW[0],&ImW_Accumulation_SW[0],&ReW_Bin_SW[0],&ImW_Bin_SW[0]);
					
					//Проверка 2-х смежных сигнальных бинов на принадлежность к одному сигналу по решающей стаистике группирования
					if(stat_grouping_SW >= bound_grouping_SW )
					{
						//Накопление для последующего расчета взвешенной частоты в бинах группируемого сигнала (участка)
                		P                 = ReW_Bin_SW[0]+ReW_Bin_SW[3]+ReW_Bin_SW[5]+ReW_Bin_SW[9];
                		numerator_freq   += (float)i*P;
                		denominator_freq += P;
                		
                		//Накопление кол-ва бинов в группируемом сигнале (участке)
                		bin_in_signal     = bin_in_signal+1;
                		
                		//Накопление кол-ва смежных бинов в новом группируемом сигнале (участке) и поиск максимума
                		if((i-stop_group_bin) == 1)
                		{
                			count_adjacent_bin = count_adjacent_bin+1;
                			
                			if(count_adjacent_bin > max_adjacent_bin)
                			{
                    		   max_adjacent_bin = count_adjacent_bin;
                    		}
                			
                		}
                		else
                		{
                			count_adjacent_bin = 1;
                		}
                		
                		//Накопление реальных и мнимых частей элементов комплексной матрицы взаимных энергий группируемого сигнала
                		for(j = 0; j < 10; j++)
        				{
        					ReW_Accumulation_SW[j] += ReW_Bin_SW[j];
        					ImW_Accumulation_SW[j] += ImW_Bin_SW[j];
        				}

                		//Запоминание индекса текущего cгруппированного бина
                		stop_group_bin = i;
					}
					else
					{
						flag_group_close = 1;
					}
					
				}
				else
				{
					flag_group_close = 1;
				}
				
				
				//Проверка и Обработка закрытой группы
				if(flag_group_close == 1)
				{
					if(bin_in_signal >= bound_min_bin_signal_SW && max_adjacent_bin >= bound_min_adjacent_bin_SW)
					{
						// Частотные параметры сгруппированного сигнала (сигнального участка)
						central_Freq_Signal_SW = (POO_CurrentCentralFreq_SW - Half_Band_POO_SW)+ bin_resolution_POO_SW*numerator_freq/denominator_freq;
                    	begin_Freq_Signal_SW   = (POO_CurrentCentralFreq_SW - Half_Band_POO_SW)+ bin_resolution_POO_SW*start_group_bin;
                    	end_Freq_Signal_SW     = (POO_CurrentCentralFreq_SW - Half_Band_POO_SW)+ bin_resolution_POO_SW*stop_group_bin;
                    	 
                    	//Блок формирования массивов mas_ReW, mas_ImW, mas_ReQ, mas_ImQ
                    	mas_ReW[0] = ReW_Accumulation_SW[0]; mas_ImW[0] =  0.0;
                    	mas_ReW[1] = ReW_Accumulation_SW[1]; mas_ImW[1] =  ImW_Accumulation_SW[1];
                    	mas_ReW[2] = ReW_Accumulation_SW[2]; mas_ImW[2] =  ImW_Accumulation_SW[2];
                    	mas_ReW[3] = ReW_Accumulation_SW[1]; mas_ImW[3] = -ImW_Accumulation_SW[1];
                    	mas_ReW[4] = ReW_Accumulation_SW[3]; mas_ImW[4] =  0.0;
                    	mas_ReW[5] = ReW_Accumulation_SW[4]; mas_ImW[5] =  ImW_Accumulation_SW[4];
                    	mas_ReW[6] = ReW_Accumulation_SW[2]; mas_ImW[6] = -ImW_Accumulation_SW[2];
                    	mas_ReW[7] = ReW_Accumulation_SW[4]; mas_ImW[7] = -ImW_Accumulation_SW[4];
                    	mas_ReW[8] = ReW_Accumulation_SW[5]; mas_ImW[8] =  0.0;

                    	mas_ReQ[0] = ReW_Accumulation_SW[6]; mas_ImQ[0] = ImW_Accumulation_SW[6];
                    	mas_ReQ[1] = ReW_Accumulation_SW[7]; mas_ImQ[1] = ImW_Accumulation_SW[7];
                    	mas_ReQ[2] = ReW_Accumulation_SW[8]; mas_ImQ[2] = ImW_Accumulation_SW[8];


                    	
                    	// Вызов главной функции блока "Пеленгование"
                    	FUNC_Direction_F_SW();
                    	
                    	//Запись результатов по сигналу (сигнальному участку) в выходной массив


                    	Inform_SW.signal[count_signal].azimuth  = azimuth_Mtheta_SW;// азимут, град.
                    	Inform_SW.signal[count_signal].elevation = 0.0;// угол места, град.
                        Inform_SW.signal[count_signal].central_Freq = central_Freq_Signal_SW;// центральная частота, МГц
                        Inform_SW.signal[count_signal].width  = end_Freq_Signal_SW-begin_Freq_Signal_SW;; // ширина, МГц
                        Inform_SW.signal[count_signal].left_Freq  = begin_Freq_Signal_SW;// начальная частота, МГц
                        Inform_SW.signal[count_signal].right_Freq  = end_Freq_Signal_SW;// конечная частота, МГц
                        Inform_SW.signal[count_signal].energy_db  = AMP_IRI_dB_SW;// энергия сигнала, дБ
                        Inform_SW.signal[count_signal].coeff_reliability  = coeff_reability_SW;// коэффициент достоверности
                        Inform_SW.signal[count_signal].max_value_Mtheta  = max_value_SDN;// максимальное значение диаграммы направленности
                        Inform_SW.signal[count_signal].reserved = 0.0;

                    	//Счетчик сгруппированных сигналов (сигнальных участков)
                    	count_signal                = count_signal+1;

					}
					
					//Обнуление флага открытия группы для начала новой итерации "Обнаружение, группирование и пеленгование"
                    flag_group_open   = 0;
                    
                   	//Обнуление флага закрытия группы для начала новой итерации "Обнаружение, группирование и пеленгование"
                    flag_group_close = 0;
				}
				
			}
			
			//Начало новой итерации "Обнаружение, группирование и пеленгование"
        	if(flag_group_open == 0)
        	{
        		//Инициализация массивов реальных и мнимых частей элементов комплексной матрицы взаимных энергий нового группируемого сигнала
        		for(j = 0; j < 10; j++)
        		{
        			ReW_Accumulation_SW[j] = ReW_Bin_SW[j];
        			ImW_Accumulation_SW[j] = ImW_Bin_SW[j];
        		}
        		
        		//Инициализация переменных для последующего накопления и расчета взвешенной частоты в бинах нового группируемого сигнала (участка)
            	P                  = ReW_Bin_SW[0]+ReW_Bin_SW[3]+ReW_Bin_SW[5]+ReW_Bin_SW[9];
            	numerator_freq     = (float)i*P;
            	denominator_freq   = P;
            	
            	//Инициализация счетчика кол-ва бинов в новом группируемом сигнале (участке)
            	bin_in_signal      = 1;
            	
            	//Инициализация счетчика кол-ва смежных бинов в новом группируемом сигнале (участке) и значения максимального кол-ва смежных бин
            	count_adjacent_bin = 1;
            	max_adjacent_bin   = 1;
            	
            	//Инициализация начального и конечного бинов нового группируемого сигнала (участка)
            	start_group_bin    = i;
            	stop_group_bin     = i;
            	
            	//Инициализация флага группирования для продолжения новой итерации "Обнаружение, группирование и пеленгование"
            	flag_group_open     = 1;

        	}
        		
		}

		else
        {
            if(stat_detection_SW < bound_noise_SW)
            {

            }
        }
	  
	} 	

	//Обработка случая, когда последний бин (индекс бина: (count_bin-1)) сигнальный и соотнесен (сгруппирован) к сигнальному участку 
	if(flag_group_open == 1 && bin_in_signal >= bound_min_bin_signal_SW && max_adjacent_bin >= bound_min_adjacent_bin_SW)
	{
		// Частотные параметры сгруппированного сигнала (сигнального участка)
		central_Freq_Signal_SW = (POO_CurrentCentralFreq_SW - Half_Band_POO_SW)+ bin_resolution_POO_SW*numerator_freq/denominator_freq;
    	begin_Freq_Signal_SW   = (POO_CurrentCentralFreq_SW - Half_Band_POO_SW)+ bin_resolution_POO_SW*start_group_bin;
    	end_Freq_Signal_SW     = (POO_CurrentCentralFreq_SW - Half_Band_POO_SW)+ bin_resolution_POO_SW*stop_group_bin;
    
    	//Блок формирования массивов mas_ReW, mas_ImW, mas_ReQ, mas_ImQ
    	mas_ReW[0] = ReW_Accumulation_SW[0]; mas_ImW[0] =  0.0;
    	mas_ReW[1] = ReW_Accumulation_SW[1]; mas_ImW[1] =  ImW_Accumulation_SW[1];
    	mas_ReW[2] = ReW_Accumulation_SW[2]; mas_ImW[2] =  ImW_Accumulation_SW[2];
    	mas_ReW[3] = ReW_Accumulation_SW[1]; mas_ImW[3] = -ImW_Accumulation_SW[1];
    	mas_ReW[4] = ReW_Accumulation_SW[3]; mas_ImW[4] =  0.0;
    	mas_ReW[5] = ReW_Accumulation_SW[4]; mas_ImW[5] =  ImW_Accumulation_SW[4];
    	mas_ReW[6] = ReW_Accumulation_SW[2]; mas_ImW[6] = -ImW_Accumulation_SW[2];
    	mas_ReW[7] = ReW_Accumulation_SW[4]; mas_ImW[7] = -ImW_Accumulation_SW[4];
    	mas_ReW[8] = ReW_Accumulation_SW[5]; mas_ImW[8] =  0.0;

    	mas_ReQ[0] = ReW_Accumulation_SW[6]; mas_ImQ[0] = ImW_Accumulation_SW[6];
    	mas_ReQ[1] = ReW_Accumulation_SW[7]; mas_ImQ[1] = ImW_Accumulation_SW[7];
    	mas_ReQ[2] = ReW_Accumulation_SW[8]; mas_ImQ[2] = ImW_Accumulation_SW[8];
    	// Вызов главной функции блока "Пеленгование"
    	FUNC_Direction_F_SW();
                    	
    	//Запись результатов по сигналу (сигнальному участку) в выходной массив


    	Inform_SW.signal[count_signal].azimuth  = azimuth_Mtheta_SW;// азимут, град.
    	Inform_SW.signal[count_signal].elevation = 0.0;// угол места, град.
        Inform_SW.signal[count_signal].central_Freq = central_Freq_Signal_SW;// центральная частота, МГц
        Inform_SW.signal[count_signal].width  = end_Freq_Signal_SW-begin_Freq_Signal_SW;; // ширина, МГц
        Inform_SW.signal[count_signal].left_Freq  = begin_Freq_Signal_SW;// начальная частота, МГц
        Inform_SW.signal[count_signal].right_Freq  = end_Freq_Signal_SW;// конечная частота, МГц
        Inform_SW.signal[count_signal].energy_db  = AMP_IRI_dB_SW;// энергия сигнала, дБ
        Inform_SW.signal[count_signal].coeff_reliability  = coeff_reability_SW;// коэффициент достоверности
        Inform_SW.signal[count_signal].max_value_Mtheta  = max_value_SDN;// максимальное значение диаграммы направленности
        Inform_SW.signal[count_signal].reserved = 0.0;
                    
     	//Счетчик сгруппированных сигналов (сигнальных участков)
    	count_signal                = count_signal+1;
   
	}
    

	//Запись кол-ва обнаруженных и сгруппированных сигналов (сигнальных участков) в полосе одновременного обзора (ПОО)
    Inform_SW.num_signals = count_signal;
    word08 = (uint32_t)&Inform_SW;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


}		
