/*****************************************************************************
 * ALG_LORANDIT_D_UKV_5
 *****************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include "Header_Func_USW_5.h"
#include "Header.h"
#include "variables_USW.h"


void Main_Detection_Grouping_Direction_Finding_USW_5(void)
{ 
// Локальные переменные и массивы
	uint32_t i,count_signal,bin_in_signal,start_group_bin,stop_group_bin,count_adjacent_bin,max_adjacent_bin;

	uint8_t flag_group_open;
	uint8_t flag_group_close;


		
	float P;
	float numerator_freq;
	float denominator_freq;	

	count_bin = word13;
	
	//Инициализациа переменных и массивов
	count_signal          = 0;
	bin_in_signal         = 0;	
	flag_group_open       = 0;
	flag_group_close      = 0;
	numerator_freq        = 0.0;
	denominator_freq      = 0.0;

	////////////////////////////////
	for(i = 0; i < count_bin; i++)
	{
		//Расчет решающей статистики детектирования (сигнальный, шумовой или сложный) спектрального отсчета
		stat_detection = Correlation_Detector_5(i);
		
		if(stat_detection >= bound_detection_USW)
		{
			
			
			//Продолжение итерации "Обнаружение, группирование и пеленгование"
			if(flag_group_open == 1)
			{
				//Проверка 2-х смежных сигнальных бинов по критерию биновой (частотной) близости
				if((i-stop_group_bin) <= bound_max_distance_USW)
				{
					//Расчет решающей статистики группирования 2-х спектральных отсчетов
					Correlation_Grouping_5(&ReW_Accumulation[0],&ImW_Accumulation[0],&ReW_Bin[0],&ImW_Bin[0]);
					
					//Проверка 2-х смежных сигнальных бинов на принадлежность к одному сигналу по решающей стаистике группирования
					if(stat_grouping >= bound_grouping_USW )
					{
						//Накопление для последующего расчета взвешенной частоты в бинах группируемого сигнала (участка)
                		P                 = ReW_Bin[0]+ReW_Bin[7]+ReW_Bin[13]+ReW_Bin[18]+ReW_Bin[22];
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
                		ReW_Accumulation[0]  += ReW_Bin[0];
						ReW_Accumulation[1]  += ReW_Bin[1];
						ReW_Accumulation[2]  += ReW_Bin[2];
						ReW_Accumulation[3]  += ReW_Bin[3];
						ReW_Accumulation[4]  += ReW_Bin[4];
						ReW_Accumulation[5]  += ReW_Bin[7];
						ReW_Accumulation[6]  += ReW_Bin[8];
						ReW_Accumulation[7]  += ReW_Bin[9];
						ReW_Accumulation[8]  += ReW_Bin[10];
						ReW_Accumulation[9]  += ReW_Bin[13];
						ReW_Accumulation[10] += ReW_Bin[14];
						ReW_Accumulation[11] += ReW_Bin[15];
						ReW_Accumulation[12] += ReW_Bin[18];
						ReW_Accumulation[13] += ReW_Bin[19];
						ReW_Accumulation[14] += ReW_Bin[22];


						ImW_Accumulation[0]  += ImW_Bin[0];
						ImW_Accumulation[1]  += ImW_Bin[1];
						ImW_Accumulation[2]  += ImW_Bin[2];
						ImW_Accumulation[3]  += ImW_Bin[3];
						ImW_Accumulation[4]  += ImW_Bin[4];
						ImW_Accumulation[5]  += ImW_Bin[7];
						ImW_Accumulation[6]  += ImW_Bin[8];
						ImW_Accumulation[7]  += ImW_Bin[9];
						ImW_Accumulation[8]  += ImW_Bin[10];
						ImW_Accumulation[9]  += ImW_Bin[13];
						ImW_Accumulation[10] += ImW_Bin[14];
						ImW_Accumulation[11] += ImW_Bin[15];
						ImW_Accumulation[12] += ImW_Bin[18];
						ImW_Accumulation[13] += ImW_Bin[19];
						ImW_Accumulation[14] += ImW_Bin[22];


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
					if(bin_in_signal >= bound_min_bin_signal_USW && max_adjacent_bin >= bound_min_adjacent_bin_USW)
					{
						// Частотные параметры сгруппированного сигнала (сигнального участка)
						central_Freq_Signal = (POO_CurrentCentralFreq - Half_Band_POO)+ bin_resolution_POO*numerator_freq/denominator_freq;
                    	begin_Freq_Signal   = (POO_CurrentCentralFreq - Half_Band_POO)+ bin_resolution_POO*start_group_bin;
                    	end_Freq_Signal     = (POO_CurrentCentralFreq - Half_Band_POO)+ bin_resolution_POO*stop_group_bin;
                    	
                    	// Вызов главной функции блока "Пеленгование"
                    	Direction_Finding_USW_5();
                    	
                    	//Запись результатов по сигналу (сигнальному участку) в выходной массив
                    	Inform.signal[count_signal].azimuth  = azimuth_Mtheta;// азимут, град.
                        Inform.signal[count_signal].elevation = 0.0;// угол места, град.
                        Inform.signal[count_signal].central_Freq = central_Freq_Signal;// центральная частота, МГц
                        Inform.signal[count_signal].width  = end_Freq_Signal-begin_Freq_Signal; // ширина, МГц
                        Inform.signal[count_signal].left_Freq  = begin_Freq_Signal;// начальная частота, МГц
                        Inform.signal[count_signal].right_Freq  = end_Freq_Signal;// конечная частота, МГц
                        Inform.signal[count_signal].energy_db  = Amp_IRI_db;// энергия сигнала, дБ
                        Inform.signal[count_signal].coeff_reliability  = coeff_reliability;// коэффициент достоверности
                        Inform.signal[count_signal].max_value_Mtheta  = max_value_Mtheta;// максимальное значение диаграммы направленности
                        Inform.signal[count_signal].reserved = 0.0;


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
        	if (flag_group_open == 0)
        	{
        		//Инициализация массивов реальных и мнимых частей элементов комплексной матрицы взаимных энергий нового группируемого сигнала
        		

        		ReW_Accumulation[0]  = ReW_Bin[0];
        		ReW_Accumulation[1]  = ReW_Bin[1];
        		ReW_Accumulation[2]  = ReW_Bin[2];
        		ReW_Accumulation[3]  = ReW_Bin[3];
        		ReW_Accumulation[4]  = ReW_Bin[4];
        		ReW_Accumulation[5]  = ReW_Bin[7];
        		ReW_Accumulation[6]  = ReW_Bin[8];
        		ReW_Accumulation[7]  = ReW_Bin[9];
        		ReW_Accumulation[8]  = ReW_Bin[10];
        		ReW_Accumulation[9]  = ReW_Bin[13];
        		ReW_Accumulation[10] = ReW_Bin[14];
        		ReW_Accumulation[11] = ReW_Bin[15];
        		ReW_Accumulation[12] = ReW_Bin[18];
        		ReW_Accumulation[13] = ReW_Bin[19];
        		ReW_Accumulation[14] = ReW_Bin[22];
        		
        		
        		ImW_Accumulation[0]  = ImW_Bin[0];
        		ImW_Accumulation[1]  = ImW_Bin[1];
        		ImW_Accumulation[2]  = ImW_Bin[2];
        		ImW_Accumulation[3]  = ImW_Bin[3];
        		ImW_Accumulation[4]  = ImW_Bin[4];
        		ImW_Accumulation[5]  = ImW_Bin[7];
        		ImW_Accumulation[6]  = ImW_Bin[8];
        		ImW_Accumulation[7]  = ImW_Bin[9];
        		ImW_Accumulation[8]  = ImW_Bin[10];
        		ImW_Accumulation[9]  = ImW_Bin[13];
        		ImW_Accumulation[10] = ImW_Bin[14];
        		ImW_Accumulation[11] = ImW_Bin[15];
        		ImW_Accumulation[12] = ImW_Bin[18];
        		ImW_Accumulation[13] = ImW_Bin[19];
        		ImW_Accumulation[14] = ImW_Bin[22];
        		

        		//Инициализация переменных для последующего накопления и расчета взвешенной частоты в бинах нового группируемого сигнала (участка)
            	P                  = ReW_Bin[0]+ReW_Bin[7]+ReW_Bin[13]+ReW_Bin[18]+ReW_Bin[22];
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
			if(stat_detection < bound_noise_USW)
			{
				
			}	
		}
	} 	




//Обработка случая, когда последний бин (индекс бина: (count_bin-1)) сигнальный и соотнесен (сгруппирован) к сигнальному участку 
    if(flag_group_open == 1 && bin_in_signal >= bound_min_bin_signal_USW && max_adjacent_bin >= bound_min_adjacent_bin_USW)
    {
        // Частотные параметры сгруппированного сигнала (сигнального участка)
        central_Freq_Signal = (POO_CurrentCentralFreq - Half_Band_POO)+ bin_resolution_POO*numerator_freq/denominator_freq;
        begin_Freq_Signal   = (POO_CurrentCentralFreq - Half_Band_POO)+ bin_resolution_POO*start_group_bin;
        end_Freq_Signal     = (POO_CurrentCentralFreq - Half_Band_POO)+ bin_resolution_POO*stop_group_bin;

        // Вызов главной функции блока "Пеленгование"
        Direction_Finding_USW_5();

        Inform.signal[count_signal].azimuth  = azimuth_Mtheta;// азимут, град.
        Inform.signal[count_signal].elevation = 0.0;// угол места, град.
        Inform.signal[count_signal].central_Freq = central_Freq_Signal;// центральная частота, МГц
        Inform.signal[count_signal].width  = end_Freq_Signal-begin_Freq_Signal; // ширина, МГц
        Inform.signal[count_signal].left_Freq  = begin_Freq_Signal;// начальная частота, МГц
        Inform.signal[count_signal].right_Freq  = end_Freq_Signal;// конечная частота, МГц
        Inform.signal[count_signal].energy_db  = Amp_IRI_db;// энергия сигнала, дБ
        Inform.signal[count_signal].coeff_reliability  = coeff_reliability;// коэффициент достоверности
        Inform.signal[count_signal].max_value_Mtheta  = max_value_Mtheta;// максимальное значение диаграммы направленности
        Inform.signal[count_signal].reserved = 0.0;


        //Счетчик сгруппированных сигналов (сигнальных участков)
        count_signal                = count_signal+1;

    }

    
    //Запись кол-ва обнаруженных и сгруппированных сигналов (сигнальных участков) в полосе одновременного обзора (ПОО)
    Inform.num_signals = count_signal;
    word08 = (uint32_t)&Inform;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\



