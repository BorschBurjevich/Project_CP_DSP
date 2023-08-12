/*
 * targets.c
 *
 *  Created on: 14 мая 2021 г.
 *      Author: Sergey_Trushin
 */
#include "targets_fch.h"

fch_targets_      fch                     __attribute__((aligned(4), section(".bss:supression")));
total_signals_    total[MAX_FREQS]        __attribute__((aligned(4), section(".bss:supression")));
pp_               pp[MAX_PP]              __attribute__((aligned(4), section(".bss:supression")));
start_supression_ start_supression        __attribute__((aligned(4), section(".bss:supression")));

uint32_t interval_idx = 0,//счетчик интервалов КП

         supr_mass[6] = {0x00,//импульс дикретизации
                         0x00,//число сигналов на подавление
                         0x00,//ид сигнала
                         0x00,//ид сигнала
                         0x00,//ид сигнала
                         0x00},//ид сигнала

          total_signals = 0,//всего сигналов
          total_nonctrl = 0,//всего без контроля
          shift = 0;
/*
 * Функция раскладки в структуру из приемного буфера
 */
static void fill_fch_struct(void)
{
    uint32_t i = 0,
             j = 0,
             l = 0,
             f = 0,
             k = 0,
             shifft = 0;

    for (i = 0; i < 377; i++)
    {
         memset(&fch.signals_ddc[i].count_signals[0], 0x0, sizeof(fch.signals_ddc[i].count_signals[0])*4);
    }

    //идентификатор ЦР
    memcpy(&fch.cr_id, &pBuf[k], sizeof(fch.cr_id));
    k += sizeof(fch.cr_id);

    //уровень мощности ПП
    memcpy(&fch.power_pp, &pBuf[k], sizeof(fch.power_pp));
    k += sizeof(fch.power_pp);

    //макс скважность
    memcpy(&fch.max_Q, &pBuf[k], sizeof(fch.max_Q));
    k += sizeof(fch.max_Q);

    //число ПП
    memcpy(&fch.num_pp, &pBuf[k], sizeof(fch.num_pp));
    k += sizeof(fch.num_pp);

    for(i = 0; i < fch.num_pp; i++)
    {
        //номер ПП
        memcpy(&fch.nonctrl[i].pp_id, &pBuf[k], sizeof(fch.nonctrl[i].pp_id));
        k += sizeof(fch.nonctrl[i].pp_id);

        //номер литеры
        memcpy(&fch.nonctrl[i].litera_id, &pBuf[k], sizeof(fch.nonctrl[i].litera_id));
        k += sizeof(fch.nonctrl[i].litera_id);

        //общее число сигналов без контроля
        memcpy(&fch.nonctrl[i].total_signals, &pBuf[k], sizeof(fch.nonctrl[i].total_signals));
        k += sizeof(fch.nonctrl[i].total_signals);
        for(j = 0; j < fch.nonctrl[i].total_signals; j++)
        {
            //идент сигнала
            memcpy(&fch.nonctrl[i].signals[j].id, &pBuf[k], sizeof(fch.nonctrl[i].signals[j].id));
            k += sizeof(fch.nonctrl[i].signals[j].id);

            //добавляем сигнал в общую статистику
            total[total_signals].id = fch.nonctrl[i].signals[j].id;
           // total[total_signals].num_intervals = MAX_INTERVALS;
            fch.num_intervals = MAX_INTERVALS;
            //ставим активность в третье состояние, как признак без контроля
            memset(&total[total_signals].is_active, 2, MAX_INTERVALS*sizeof(total[total_signals].is_active));

            //частота РП в кгц
            memcpy(&fch.nonctrl[i].signals[j].freq_rp, &pBuf[k], sizeof(fch.nonctrl[i].signals[j].freq_rp));
            k += sizeof(fch.nonctrl[i].signals[j].freq_rp);
            total[total_signals].freq_rp = fch.nonctrl[i].signals[j].freq_rp;

            //тип помехи
            memcpy(&fch.nonctrl[i].signals[j].type, &pBuf[k], sizeof(fch.nonctrl[i].signals[j].type));
            k += sizeof(fch.nonctrl[i].signals[j].type);
            total[total_signals].type = fch.nonctrl[i].signals[j].type;

            //длительность импульса дискретизации
            memcpy(&fch.nonctrl[i].signals[j].discr, &pBuf[k], sizeof(fch.nonctrl[i].signals[j].discr));
            k += sizeof(fch.nonctrl[i].signals[j].discr);
            total[total_signals].discr = fch.nonctrl[i].signals[j].discr;

            //макс скважность
            memcpy(&fch.nonctrl[i].signals[j].Q, &pBuf[k], sizeof(fch.nonctrl[i].signals[j].Q));
            k += sizeof(fch.nonctrl[i].signals[j].Q);
            total[total_signals].Q = fch.nonctrl[i].signals[j].Q;

            //параметр помехи 1
            memcpy(&fch.nonctrl[i].signals[j].param1, &pBuf[k], sizeof(fch.nonctrl[i].signals[j].param1));
            k += sizeof(fch.nonctrl[i].signals[j].param1);
            total[total_signals].param1 = fch.nonctrl[i].signals[j].param1;

            //параметр помехи 2
            memcpy(&fch.nonctrl[i].signals[j].param2, &pBuf[k], sizeof(fch.nonctrl[i].signals[j].param2));
            k += sizeof(fch.nonctrl[i].signals[j].param2);
            total[total_signals].param2 = fch.nonctrl[i].signals[j].param2;
            total_signals++;
        }
    }
    total_nonctrl = total_signals;

    //число каналов РР
    memcpy(&fch.total_channels, &pBuf[k], sizeof(fch.total_channels));
    k += sizeof(fch.total_channels);

    for(i = 0; i < fch.total_channels; i++)
    {
        //номер канала
        memcpy(&fch.channel[i].ch_id, &pBuf[k], sizeof(fch.channel[i].ch_id));
        k += sizeof(fch.channel[i].ch_id);

        //число полос на канал
        memcpy(&fch.channel[i].total_bands, &pBuf[k], sizeof(fch.channel[i].total_bands));
        k += sizeof(fch.channel[i].total_bands);
        for(j = 0; j < fch.channel[i].total_bands; j++)
        {
            //номер полопсы
            memcpy(&fch.channel[i].bands[j].band_idx, &pBuf[k], sizeof(fch.channel[i].bands[j].band_idx));
            k += sizeof(fch.channel[i].bands[j].band_idx);

            //число сигналов в полосе
            memcpy(&fch.channel[i].bands[j].total_signals, &pBuf[k], sizeof(fch.channel[i].bands[j].total_signals));
            k += sizeof(fch.channel[i].bands[j].total_signals);

            fch.Max_POO_in_Channel = fch.channel[0].total_bands; // Находим максимальное колич-во полос в каналах
            if (fch.channel[j+1].total_bands > fch.Max_POO_in_Channel)
            {
                fch.Max_POO_in_Channel = fch.channel[j+1].total_bands;
            }

            for(l = 0; l < fch.channel[i].bands[j].total_signals; l++)
            {
                //идентификатор сигнала на котроль
                memcpy(&fch.channel[i].bands[j].oncontrol[l].id, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].id));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].id);

                //добавляем сигнал в общую статистику
                total[total_signals].id = fch.channel[i].bands[j].oncontrol[l].id;
                //total[total_signals].num_intervals = MAX_INTERVALS;
                fch.num_intervals = MAX_INTERVALS;

                //номер передатчика помех
                memcpy(&fch.channel[i].bands[j].oncontrol[l].pp_idx, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].pp_idx));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].pp_idx);
                total[total_signals].pp_idx = fch.channel[i].bands[j].oncontrol[l].pp_idx;

                //чатота левая
                memcpy(&fch.channel[i].bands[j].oncontrol[l].freq_left, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].freq_left));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].freq_left);

                //чатота правая
                memcpy(&fch.channel[i].bands[j].oncontrol[l].freq_right, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].freq_right));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].freq_right);


                /******** Определяем к какому DDC относится сигнал *********/
                if (fch.channel[i].bands[j].oncontrol[l].freq_right <= bands.fl[fch.channel[i].bands[j].band_idx] + 2)
                {
                    fch.signals_ddc[shifft].count_signals[0] += 1;
                }
                else if (fch.channel[i].bands[j].oncontrol[l].freq_right <= bands.fl[fch.channel[i].bands[j].band_idx] + 4 && fch.channel[i].bands[j].oncontrol[l].freq_left > bands.fl[fch.channel[i].bands[j].band_idx] + 2)
                {
                    fch.signals_ddc[shifft].count_signals[1] += 1;
                }
                else if (fch.channel[i].bands[j].oncontrol[l].freq_right <= bands.fl[fch.channel[i].bands[j].band_idx] + 8 && fch.channel[i].bands[j].oncontrol[l].freq_left > bands.fl[fch.channel[i].bands[j].band_idx] + 4)
                {
                    fch.signals_ddc[shifft].count_signals[2] += 1;
                }
                else if (fch.channel[i].bands[j].oncontrol[l].freq_right < bands.fh[fch.channel[i].bands[j].band_idx] && fch.channel[i].bands[j].oncontrol[l].freq_left > bands.fl[fch.channel[i].bands[j].band_idx] + 8)
                {
                    fch.signals_ddc[shifft].count_signals[3] += 1;
                }
                ////////////////////////////////////////////////////////////

                //частота РП
                memcpy(&fch.channel[i].bands[j].oncontrol[l].freq_rp, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].freq_rp));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].freq_rp);
                total[total_signals].freq_rp = fch.channel[i].bands[j].oncontrol[l].freq_rp;

                //тип помехового сигнала
                memcpy(&fch.channel[i].bands[j].oncontrol[l].type, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].type));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].type);
                total[total_signals].type = fch.channel[i].bands[j].oncontrol[l].type;

                //длительность импульса дискретизации
                memcpy(&fch.channel[i].bands[j].oncontrol[l].discr, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].discr));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].discr);
                total[total_signals].discr = fch.channel[i].bands[j].oncontrol[l].discr;

                //макс скважность
                memcpy(&fch.channel[i].bands[j].oncontrol[l].Q, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].Q));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].Q);
                total[total_signals].Q = fch.channel[i].bands[j].oncontrol[l].Q;

                //параметр помехи 1
                memcpy(&fch.channel[i].bands[j].oncontrol[l].param1, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].param1));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].param1);
                total[total_signals].param1 = fch.channel[i].bands[j].oncontrol[l].param1;

                //параметр помехи 2
                memcpy(&fch.channel[i].bands[j].oncontrol[l].param2, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].param2));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].param2);
                total[total_signals].param2 = fch.channel[i].bands[j].oncontrol[l].param2;

                //признак охраняемой
                memcpy(&fch.channel[i].bands[j].oncontrol[l].guard, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].guard));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].guard);
                total[total_signals].guard = fch.channel[i].bands[j].oncontrol[l].guard;

                //всего охраняемых (колич-во подавляемых частот для охраняемой)
                memcpy(&fch.channel[i].bands[j].oncontrol[l].total_guarded, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].total_guarded));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].total_guarded);
                total[total_signals].total_guarded = fch.channel[i].bands[j].oncontrol[l].total_guarded;

                for(f = 0; f < fch.channel[i].bands[j].oncontrol[l].total_guarded; f++)
                {
                    //иднтификатор подаляемого ИРИ
                    memcpy(&fch.channel[i].bands[j].oncontrol[l].guarded_id[f], &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].guarded_id[f]));
                    k += sizeof(fch.channel[i].bands[j].oncontrol[l].guarded_id[f]);

                    //добавляем в общую статистику индекс охраняемого
                    total[total_signals].guard_idxs[f] = fch.channel[i].bands[j].oncontrol[l].guarded_id[f];
                }

                //приоритет
                memcpy(&fch.channel[i].bands[j].oncontrol[l].priority, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].priority));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].priority);
                total[total_signals].priority = fch.channel[i].bands[j].oncontrol[l].priority;

                //коэф загрузки
                memcpy(&fch.channel[i].bands[j].oncontrol[l].coeff, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].coeff));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].coeff);
                total[total_signals].priority = fch.channel[i].bands[j].oncontrol[l].coeff;
                total_signals++;
            }
            shifft ++;
        }
    }

}

/*
 * Функция получения импульса дискретизации и частот на подавление без контроля
 *
 * на вход номер передатчика
 *
 * Заполняет supr_mass[], где
 * 0 элемент - импульс дискретизации
 * 1 элемент - число частот на подавлени
 * 2 элемент - индекс частоты
 * и т. д.
 */
static void get_nonctrl_freqs(int pp_id)
{
    uint32_t   i = 0,
               j = 0,
               max = 0,
               group;

    typedef struct{
        uint32_t    discr;      //значенине импульса
        uint32_t    count;      //счетчик сигналов под импульс
        uint32_t    id[50];     //индекс сигнала
    }tmp_;
    tmp_ tmp[10] = {0};

    /*
     * Группируем по импульсу дискретизации;
     * Считаем число сигналов в группе;
     */
    for(i = 0; i < fch.nonctrl[pp_id].total_signals; i++)
    {
        if(total[shift].on_guard[interval_idx] == 1)
        {
            //охраняемая в эфире, не берем в рассмотрение
            continue;
        }

        //группировка по импульсу дискретизации
        for(j = 0; j < 10; j++)
        {
            if(total[shift].discr == tmp[j].discr)
            {
                tmp[j].id[tmp[j].count++] = shift;
                break;
            }
            else if(tmp[j].discr == 0)
            {
                tmp[j].discr = total[shift].discr;
                tmp[j].id[tmp[j].count++] = shift;
                break;
            }
        }
        shift++;
    }

    /*
     * Поиск группы с максимальным числом сигналов
     */
    for(i = 0; i < 10; i++)
    {
        if(tmp[i].count > max)
        {
            max = tmp[i].count;
            group = i;
        }
    }

    /*
     * Проверка по скважности
     */
    if(tmp[group].count > fch.max_Q)
    {
        tmp[group].count = fch.max_Q;
    }

    /*
     * Формирование выходного массива на подавление
     */
    supr_mass[0] = tmp[group].discr;//импульс дискр
    supr_mass[1] = tmp[group].count;//число сигналов
    j = 2;
    for(i = 0; i < tmp[group].count; i++)
    {
        supr_mass[j++] = tmp[group].id[i];//id сигнала из total[]
    }
}

/*
 * Функция получения сигналов на подавление при наличии частот без контроля
 * на вход номер передатчика
 *
 * добивает supr_mass[] оставшимися сигналами
 */
static void get_ctrl_freq_with_nonctrl(int pp_id)
{
    uint32_t i = 0,
             min = 0xffffffff, // начальное значение для поиска приоритета
             count_prior = 0,
             ttl = 0,//общее число сигнало
             tmp = 0,
             group[20] = {0};//массив индексов (из signal) сигналов с максимальным приоритетом

    float   coef = 0;//переменная для сортировки по коэф загрузки

    typedef struct{
        uint32_t     id;
        float        coef;
        uint32_t     priority;
    }s;
    s       signal[20] = {0};//общая сигнальная структура под данный импульс дискретизации

    /*
     * Собираем группу под заданный импульс дискретизации;
     * Определям максимальный приоритет сигналов;
     * Определяем общее число сигналов;
     */
    for(i = 0; i < pp[pp_id].count; i++)
    {
        if(total[pp[pp_id].signal_id[i]].discr != supr_mass[0])
        {
            // не подходит импульс дискретизации
            continue;
        }
        else
        {
            signal[ttl].id       = pp[pp_id].signal_id[i];
            signal[ttl].priority = total[pp[pp_id].signal_id[i]].priority;
            signal[ttl].coef     = total[pp[pp_id].signal_id[i]].coef;

            //определяем минимальное число, соотв. макс. приоритету
            if(min >= signal[ttl].priority)
            {
                min = signal[ttl].priority;
            }
            ttl++;
        }
    }

    /*
     * Cчитаем общее число сигналов с макс приоритетом и запоминаем их индексы
     */
    for(i = 0; i < ttl; i++)
    {
        if(signal[i].priority == min)
        {
            group[count_prior++] = i;//сохраняем индекс
        }
    }

    /*
     * Общее числов сигналов с контролем и без равно макс скважности
     */
    if((supr_mass[1] + count_prior) == fch.max_Q)
    {
        for(i = 0; i < count_prior; i++)
        {
            supr_mass[2 + supr_mass[1] + i] = signal[group[i]].id;
        }
        supr_mass[1] += count_prior;
        return;
    }

    /*
     * Общее числов сигналов с контролем и без больше макс скважности
     * Фильтруем по коэф загрузки
     */
    else if((supr_mass[1] + count_prior) > fch.max_Q)
    {
        while(fch.max_Q - supr_mass[1])
        {
            for(i = 0; i < count_prior; i++)
            {
                //определяем максимальный коеф загрузки
                if(signal[group[i]].coef > coef)
                {
                    coef = signal[group[i]].coef;
                    tmp = i;
                }
            }
            //запоминаем найденный
            supr_mass[2 + supr_mass[1]] = signal[group[tmp]].id;
            supr_mass[1]++;

            //выкинули из рассмотрения
            signal[group[tmp]].coef = 0;
            coef = 0;
        }
        return;
    }

    /*
     * Общее число сигналов меньше макс скважности
     */
    else if((supr_mass[1] + count_prior) < fch.max_Q)
    {
        //проставляем сигналы с высшим приоритетом
        for(i = 0; i < count_prior; i++)
        {
            supr_mass[2 + supr_mass[1] + i] = signal[group[i]].id;
        }
        supr_mass[1] += count_prior;

        /*
         * Все сигналы рассмотрены, ограничиваемся тем, что имеем
         */
        if(ttl == count_prior)
        {
            return;
        }

        /*
         * Есть сигналы с приоритетом ниже;
         * Добираем из них
         */
        else
        {
            tmp = count_prior;//запомнили число сигналов

            //выкидываем из рассмотрения сигналы с висшим приоритетом и очищаем группу приоритетов
            for(i = 0; i < count_prior; i++)
            {
                signal[group[i]].priority = 0xffffffff;
                group[i] = 0;
            }
            count_prior = 0;

            //определям приоритет ниже
            for(i = 0; i < ttl; i++)
            {
                if(min >= signal[i].priority)
                {
                    min = signal[i].priority;
                }
            }

            //считаем общее число сигналов с макс приоритетом и запоминаем их индексы
            for(i = 0; i < ttl; i++)
            {
                if(signal[i].priority == min)
                {
                    group[count_prior++] = i;
                }
            }

            /*
             * Общее числов сигналов с контролем и без равно макс скважности
             */
            if((supr_mass[1] + count_prior) == fch.max_Q)
            {
                for(i = 0; i < count_prior; i++)
                {
                    supr_mass[2 + supr_mass[1] + i] = signal[group[i]].id;
                }
                supr_mass[1] += count_prior;
                return;
            }

            /*
             * Общее числов сигналов с контролем и без больше макс скважности
             * Фильтруем по коэф загрузки
             */
            else if((supr_mass[1] + count_prior) > fch.max_Q)
            {
                coef = 0.0;
                while(fch.max_Q - supr_mass[1])
                {
                    for(i = 0; i < count_prior; i++)
                    {
                        //определяем максимальный коеф загрузки
                        if(signal[group[i]].coef >= coef)
                        {
                            coef = signal[group[i]].coef;
                            tmp = i;
                        }
                    }
                    //запоминаем найденный
                    supr_mass[2 + supr_mass[1]] = signal[group[tmp]].id;
                    supr_mass[1]++;

                    //выкинули из рассмотрения
                    signal[group[tmp]].coef = 0;
                    coef = 0;
                }
                return;
            }

            /*
             * Общее число сигналов меньше макс скважности
             */
            else if((supr_mass[1] + count_prior) < fch.max_Q)
            {
                //проставляем сигналы с высшим приоритетом
                for(i = 0; i < count_prior; i++)
                {
                    supr_mass[2 + supr_mass[1] + i] = signal[group[i]].id;
                }
                supr_mass[1] += count_prior;

                /*
                 * Все сигналы рассмотрены, ограничиваемся тем, что имеем
                 */
                if((total - tmp - count_prior) == 0)
                {
                    return;
                }
                else
                {
                    //выкидываем из рассмотрения сигналы с висшим приоритетом и очищаем группу приоритетов
                    for(i = 0; i < count_prior; i++)
                    {
                        signal[group[i]].priority = 0xffffffff;
                        group[i] = 0;
                    }
                    count_prior = 0;

                    //определям приоритет ниже
                    for(i = 0; i < ttl; i++)
                    {
                        if(min >= signal[i].priority)
                        {
                            min = signal[i].priority;
                        }
                    }

                    //считаем общее число сигналов с новым приоритетом
                    for(i = 0; i < ttl; i++)
                    {
                        if(signal[i].priority == min)
                        {
                            group[count_prior++] = i;
                        }
                    }

                    /*
                     * Общее числов сигналов с контролем и без равно макс скважности
                     */
                    if((supr_mass[1] + count_prior) == fch.max_Q)
                    {
                        for(i = 0; i < count_prior; i++)
                        {
                            supr_mass[2 + supr_mass[1] + i] = signal[group[i]].id;
                        }
                        supr_mass[1] += count_prior;
                        return;
                    }

                    /*
                     * Общее числов сигналов с контролем и без больше макс скважности
                     * Фильтруем по коэф загрузки
                     */
                    else if((supr_mass[1] + count_prior) > fch.max_Q)
                    {
                        coef = 0.0;
                        while(fch.max_Q - supr_mass[1])
                        {
                            for(i = 0; i < count_prior; i++)
                            {
                                //определяем максимальный коеф загрузки
                                if(signal[group[i]].coef >= coef)
                                {
                                    coef = signal[group[i]].coef;
                                    tmp = i;
                                }
                            }
                            //запоминаем найденный
                            supr_mass[2 + supr_mass[1]] = signal[group[tmp]].id;
                            supr_mass[1]++;

                            //выкинули из рассмотрения
                            signal[group[tmp]].coef = 0;
                            coef = 0;
                        }
                        return;
                    }
                }
            }
        }
    }
}

/*
 * Функция получения сигналов на подавление при без наличия частот без контроля
 * на вход номер передатчика
 *
 * добивает supr_mass[] оставшимися сигналами
 */
static void get_ctrl_freq(int pp_id)
{
    uint32_t i = 0,
             j = 0,
             idx = 0,
             min[NUM_PULS_DISCR] = {0xffffffff},//максимальный приоритет для каждой из  групп
             count_prior[NUM_PULS_DISCR] = {0},//число сигналов с макс приортетом в группе
             weight = 0,
             equal[10] = {0},
             tmp = 0,
             mean_weight[NUM_PULS_DISCR] = {0};

    float    mean_coeff[NUM_PULS_DISCR] = {0.0},
             tmpf = 0.0;

    typedef struct{
        uint32_t  discr;//импульс дискретизации группы
        uint32_t  count;//общее чсило сигналов в группе
        uint32_t  id[20];
        uint32_t  coef[20];
        uint32_t  priority[20];
    }s_;
    s_      signal[NUM_PULS_DISCR] = {0};//структура для разных импульсов дискртеизации

    /*
     * Собираем группы по импульсу дискретизации;
     * Вычисляем максимальный приоритет в каждой группе;
     * Считаем общее число сигналов в группе
     */
    for(i = 0; i < pp[pp_id].count; i++)
    {
        for(j = 0; j < NUM_PULS_DISCR; j++)
        {
            if(total[pp[pp_id].signal_id[i]].discr == signal[j].discr)
            {
                idx = signal[j].count;
                signal[j].id[idx]       = pp[pp_id].signal_id[i];
                signal[j].priority[idx] = total[pp[pp_id].signal_id[i]].priority;
                signal[j].coef[idx]     = total[pp[pp_id].signal_id[i]].coef;

                //определяем минимальное число, соотв. макс. приоритету
                if(min[j] >= signal[j].priority[idx])
                {
                    min[j] = signal[j].priority[idx];
                }
                signal[j].count++;//общее число сигналов под данную дискретизацию

                break;
            }
            else if(signal[j].discr == 0)
            {
                idx = signal[j].count;

                signal[j].discr         = total[pp[pp_id].signal_id[i]].discr;
                signal[j].id[idx]       = pp[pp_id].signal_id[i];
                signal[j].priority[idx] = total[pp[pp_id].signal_id[i]].priority;
                signal[j].coef[idx]     = total[pp[pp_id].signal_id[i]].coef;

                //определяем минимальное число, соотв. макс. приоритету
                if(min[j] >= signal[j].priority[idx])
                {
                    min[j] = signal[j].priority[idx];
                }
                signal[j].count++;;//общее число сигналов под данную дискретизацию
                break;
            }
        }
    }

    /*
     * Фильтруем группы по приоритету;
     * Оставляем только группы в которых есть высший приоритет
     */
    for(i = 0; i < NUM_PULS_DISCR; i++)
    {
        for(j = 0; j < NUM_PULS_DISCR; j++)
        {
            if(i == j)
            {
                continue;
            }
            if(min[i] > min[j])
            {
                signal[i].count = 0;//выкидываем низкоприоритетную группу
            }
        }
    }

    /*
     * Опеределяем число сигналов с макс приоритетом в каждой группе;
     */
    for(i = 0; i < NUM_PULS_DISCR; i++)
    {
        for(j = 0; j < signal[i].count; j++)
        {
            if(signal[i].priority[j] == min[i])
            {
                count_prior[i]++;
            }
        }

        //ищем максимальный вес среди всех групп
        if(count_prior[i] > weight)
        {
            weight = count_prior[i];//сохраняем вес
        }
    }

    /*
     * Проверяем, что группа с максимальным числом макс приоритетных сигналов единственная
     */
    for(i = 0; i < NUM_PULS_DISCR; i++)
    {
        if(count_prior[i] == weight)
        {
            equal[0]++;//число таких же групп
            equal[equal[0]++] = i;//индекс группы signal[]
        }
    }

    /*
     * Группа единственная;
     * Сохраняем ее индекс signal[idx]
     */
    if(equal[0] == 1)
    {
        idx = equal[1];
    }
    /*
     * Группа не единственная
     */
    else
    {
        /*
         * Фильтруем группы с одинаковым весом по суммарному коэф загрузки
         */
        for(i = 0; i < equal[0]; i++)
        {
            idx = equal[i+1];
            for(j = 0; j < signal[idx].count; j++)
            {
                if(signal[idx].priority[j] == min[idx])
                {
                    mean_coeff[idx] += signal[idx].coef[j];
                }
            }

            //ищем максимальный суммарный коэф загрузки
            if(mean_coeff[idx] > tmpf)
            {
                tmpf = mean_coeff[idx];//сохраняем вес
            }
        }

        //очищаем
        memset(equal, 0, sizeof(equal));

        /*
         * Проверяем, что группа с максимальным коэф загрузки единственная
         */
        for(i = 0; i < NUM_PULS_DISCR; i++)
        {
            if(mean_coeff[i] == tmpf)
            {
                equal[0]++;//число таких же групп
                equal[equal[0]++] = i;//индекс группы signal[]
            }
        }

        /*
         * Группа единственная
         */
        if(equal[0] == 1)
        {
            idx = equal[1];
        }

        /*
         * Группа не единственная;
         * Сравниваем по суммарному приоритету группы (где число меньше, тот и оставим)
         */
        else
        {
            weight = 0xffffffff;
            for(i = 0; i < equal[0]; i++)
            {
                for(j = 0; j < signal[equal[i+1]].count; j++)
                {
                    mean_weight[equal[i+1]] += signal[equal[i+1]].priority[j];
                }

                //ищем максимальный суммарный приоритет, соответствующий минимальному числу
                if(mean_weight[equal[i+1]] < weight)
                {
                    weight = mean_weight[equal[i+1]];//сохраняем вес
                    idx = equal[i+1];
                }
            }
        }
    }

    /*
     * Общее число сигналов c макс приоритетом равно макс скважности
     */
    if(count_prior[idx] == fch.max_Q)
    {
        supr_mass[0] = signal[idx].discr;
        supr_mass[1] = fch.max_Q;
        for(i = 0; i < signal[idx].count; i++)
        {
            if(signal[idx].priority[i] == min[idx])
            {
                supr_mass[2 + i] = signal[idx].id[i];
            }
        }
        return;
    }

    /*
     * Общее число сигналов больше максимальной скважности;
     * Фильтруем по коеф загрузки в пределах одного приоритета
     */
    else if(count_prior[idx] > fch.max_Q)
    {
        tmpf = 0.0;
        j = 0;
        supr_mass[0] = signal[idx].discr;
        supr_mass[1] = fch.max_Q;
        while(fch.max_Q != j)
        {
            for(i = 0; i < signal[idx].count; i++)
            {
                if(signal[idx].priority[i] == min[idx])
                {
                    //определяем максимальный коеф загрузки
                    if(signal[idx].coef[i] > tmpf)
                    {
                        tmpf = signal[idx].coef[i];
                        tmp = i;
                    }
                }
            }

            //запоминаем найденный
            supr_mass[2 + j] = signal[idx].id[tmp];

            //выкинули из рассмотрения
            signal[idx].coef[tmp] = 0;
            tmpf = 0.0;
            j++;
        }
        return;
    }

    /*
     * Общее число сигналов макс приоритета не равно макс скважности;
     * Добираем сигналы с приоритетом ниже;
     */
    else if(count_prior[idx] < fch.max_Q)
    {
        //проставляем сигналы с высшим приоритетом
        supr_mass[0] = signal[idx].discr;
        supr_mass[1] = count_prior[idx];
        for(i = 0; i < signal[idx].count; i++)
        {
            if(signal[idx].priority[i] == min[idx])
            {
                supr_mass[2 + i] = signal[idx].id[i];
            }
        }

        /*
         * Все сигналы рассмотрены, ограничиваемся тем, что имеем
         */
        if(signal[idx].count == count_prior[idx])
        {
            return;
        }
        /*
         * Добираем с пиоритетом ниже
         */
        else
        {
            tmp = count_prior[idx];//запомнили число сигналов

            //выкидываем из рассмотрения сигналы с висшим приоритетом и очищаем группу приоритетов
            for(i = 0; i < signal[idx].count; i++)
            {
                if(signal[idx].priority[i] == min[idx])
                {
                    signal[idx].priority[i] = 0xffffffff;;
                }
            }
            count_prior[idx] = 0;

            //определям приоритет ниже
            for(i = 0; i < signal[idx].count; i++)
            {
                if(min[idx] >= signal[idx].priority[i])
                {
                    min[idx] = signal[idx].priority[i];
                }
            }

            //считаем общее число сигналов с макс приоритетом
            for(i = 0; i < signal[idx].count; i++)
            {
                if(signal[idx].priority[i] == min[idx])
                {
                    count_prior[idx]++;
                }
            }

            /*
             * Общее число сигналов равно макс скважности
             */
            if((supr_mass[1] + count_prior[idx]) == fch.max_Q)
            {
                for(i = 0; i < signal[idx].count; i++)
                {
                    if(signal[idx].priority[i] == min[idx])
                    {
                        supr_mass[2 + supr_mass[1] + i] = signal[idx].id[i];
                    }
                }
                supr_mass[1] += count_prior[idx];
                return;
            }

            /*
             * Общее чилсо сигналов больше скважности;
             * Сортирум по коэф загрузки
             */
            else if((supr_mass[1] + count_prior[idx]) > fch.max_Q)
            {
                tmpf = 0.0;
                j = 0;
                while(fch.max_Q - supr_mass[1])
                {
                    for(i = 0; i < signal[idx].count; i++)
                    {
                        if(signal[idx].priority[i] == min[idx])
                        {
                            //определяем максимальный коеф загрузки
                            if(signal[idx].coef[i] >= tmpf)
                            {
                                tmpf = signal[idx].coef[i];
                                tmp = i;
                            }
                        }
                    }

                    //запоминаем найденный
                    supr_mass[2 + j] = signal[idx].id[tmp];
                    supr_mass[1]++;

                    //выкинули из рассмотрения
                    signal[idx].coef[tmp] = 0;
                    tmpf = 0.0;
                    j++;
                }
                return;
            }

            /*
             * Сигналов меньше скважности;
             * Добираем приоритет ниже
             */
            else if((supr_mass[1] + count_prior[idx]) < fch.max_Q)
            {
                //проставляем высший приоритет
                for(i = 0; i < signal[idx].count; i++)
                {
                    if(signal[idx].priority[i] == min[idx])
                    {
                        supr_mass[2 + supr_mass[1] + i] = signal[idx].id[i];
                    }
                }
                supr_mass[1] += count_prior[idx];

                /*
                 * Сигналы кончились, выходим
                 */
                if((signal[idx].count - tmp - count_prior[idx]) == 0)
                {
                    return;
                }
                else
                {
                    //выкидываем из рассмотрения сигналы с висшим приоритетом и очищаем группу приоритетов
                    for(i = 0; i < signal[idx].count; i++)
                    {
                        if(signal[idx].priority[i] == min[idx])
                        {
                            signal[idx].priority[i] = 0xffffffff;;
                        }
                    }
                    count_prior[idx] = 0;

                    //определям приоритет ниже
                    for(i = 0; i < signal[idx].count; i++)
                    {
                        if(min[idx] >= signal[idx].priority[i])
                        {
                            min[idx] = signal[idx].priority[i];
                        }
                    }

                    //считаем общее число сигналов с макс приоритетом
                    for(i = 0; i < signal[idx].count; i++)
                    {
                        if(signal[idx].priority[i] == min[idx])
                        {
                            count_prior[idx]++;
                        }
                    }

                    /*
                     * Общее число сигналов равно макс скважности
                     */
                    if((supr_mass[1] + count_prior[idx]) == fch.max_Q)
                    {
                        for(i = 0; i < signal[idx].count; i++)
                        {
                            if(signal[idx].priority[i] == min[idx])
                            {
                                supr_mass[2 + supr_mass[1] + i] = signal[idx].id[i];
                            }
                        }
                        supr_mass[1] += count_prior[idx];
                        return;
                    }

                    /*
                     * Сигналов больше чем макс скважность;
                     * Фильтруем по коеф загрузки
                     */
                    else if((supr_mass[1] + count_prior[idx]) > fch.max_Q)
                    {
                        tmpf = 0.0;
                        j = 0;
                        while(fch.max_Q - supr_mass[1])
                        {
                            for(i = 0; i < signal[idx].count; i++)
                            {
                                if(signal[idx].priority[i] == min[idx])
                                {
                                    //определяем максимальный коеф загрузки
                                    if(signal[idx].coef[i] > tmpf)
                                    {
                                        tmpf = signal[idx].coef[i];
                                        tmp = i;
                                    }
                                }
                            }

                            //запоминаем найденный
                            supr_mass[2 + j] = signal[idx].id[tmp];
                            supr_mass[1]++;

                            //выкинули из рассмотрения
                            signal[idx].coef[tmp] = 0;
                            tmpf = 0.0;
                            j++;
                        }
                        return;
                    }
                }
            }
        }
    }
}


/*
 * Функция группировки сигналов по передатчикам
 * На выходе заполняется массив передачиков и для каждого указываются частоты которые можно подавить
 */
static void group_signals_in_pp(void)
{
    uint32_t i = 0,
             pp_idx = 0;

    for(i = 0; i < total_signals; i++)
    {
        //сигнал не активный или без контроля, пропускаем
        if(total[i].is_active[interval_idx] != 1)
        {
            continue;
        }

        //охраняемая в эфире, пропускаем
        if((total[i].on_guard[interval_idx] != 1))
        {
            continue;
        }

        //сигнал охраняемый, пропускаем
        if((total[i].guard == 1))
        {
            continue;
        }

        pp_idx = total[i].pp_idx;
        //сигнал можно подавить добавляем в структуру предатчика
        pp[pp_idx].signal_id[pp[pp_idx].count++] = i;
    }
}

/*
 * Функция инициализации режима контроль ФЧ
 */
void init_fch()
{
    int count;
    //очищаем прошлый результат контроля
    memset(&fch, 0, sizeof(fch));
    memset(&total, 0, sizeof(total));
    memset(&pp, 0, sizeof(pp));
    fill_fch_struct();
    Lable_first_control = 1;

}

/*
 * Функция стартует режим
 */
void start_rp()
{
    uint32_t   i = 0;

    memcpy(&start_supression.cyclogram, &pBuf[i], sizeof(start_supression.cyclogram));
    i += sizeof(start_supression.cyclogram);

    memcpy(&start_supression.number, &pBuf[i], sizeof(start_supression.number));
    i += sizeof(start_supression.number);

    memcpy(&start_supression.dorazv, &pBuf[i], sizeof(start_supression.dorazv));
    i += sizeof(start_supression.dorazv);

    switch(start_supression.number)// в соответсвии с номером циклограммы разбираем времена
    {
        case(0):  // (200/5/20) ФЧ
        {
            // тут должна быть отправка синхронизации на ПЛИС = блок синхронизации (по протоколу должен сказать, какие времена используются, далее ожидаю прерывания)
            break;
        }

        case(1):  // (200/0.25/1) ППРЧ
        {
            break;
        }

        case(2):  // (100/2.5/5)  ФЧ, ППРЧ
        {
            break;
        }

        case(3):  // (200/5/400(0.25,1))
        {
            break;
        }

        case(4):  // (20/5/400(2.5/5))
        {
            break;
        }

        case(5):  // непрерывное подавление
        {
            break;
        }

        case(6): // автономный суперцикл для подавления ппрч
        {
            break;
        }
    }

}

void start_control()
{
    uint32_t   freqmhz = 0;
    uint16_t    counter,ind;
    uint32_t   j,i,k,NFFT_CUT,z, s_sig,c,l,f,s_poo;

    typedef struct
    {
       uint8_t mass[2048];          // 512 * 4 DDC
    }mass_out_control;
    mass_out_control ch_num[5];
    typedef struct
    {
      float   E[5];
    }overflow_control;
    overflow_control ov_num[5];

    memset(ov_num,0,sizeof(ov_num));
    memset(ch_num,0,sizeof(ch_num));

    if ((init_bear.priznak_bearing == nosynchrojob_ || init_bear.priznak_bearing == bearing_in_recon_) && (Lable_first_control == 1))
    {
        Lable_first_control = 0;

        for (counter = 0; counter < fch.total_channels; counter ++)
        {

            freqmhz = mass_bands[fch.channel[counter].bands[0].band_idx] >> 16; // fr из структуры bands
            rcvr.mas_channel[counter].mas_freq_range[0] = pres_freq_range(freqmhz*1000000);
            flag_aerial = selection_litera(freqmhz,flag_aerial,0);
            set_rec_mode(set_cmd_1,MCSPI2_LVDS_CS3,fch.channel[counter].ch_id,rcvr.mas_channel[counter].mas_freq_range[0],tresholds[ATT_UHF], 0,0);
            config_ADC(bands.n_ddc[fch.channel[counter].bands[0].band_idx],bands.fl[fch.channel[counter].bands[0].band_idx],512,fch.channel[counter].ch_id);
        }

        return;
     /////////////
    }
    s_sig = 0;
    s_poo = 0;
    for (j = 0; j < fch.Max_POO_in_Channel; j++)  // цикл по  полосам Max_POO_in_Channel
    {
        c = total_nonctrl + s_sig;
        for (i = 0; i < fch.total_channels; i++)  // цикл по числу каналов
        {
            for (k = 0; k < bands.n_ddc[fch.channel[i].bands[0].band_idx]; k ++)   // цикл по ddc в полосе
            {

                for (z = 0; z < fch.signals_ddc[s_poo].count_signals[k]; z++)    //// цикл по сигналам в пределах 1 ddc
                {
                    total[0].is_active[(c*MAX_INTERVALS) + fch.num_intervals] = 0;
                    total[0].on_supression[(c*MAX_INTERVALS) + fch.num_intervals] = 0;
                    total[0].on_guard[(c*MAX_INTERVALS) + fch.num_intervals] = 0;

                    Start_data();
                    for(counter = 0; counter < 1024; counter++)                       // подыгрыш времянка шум (512Re,512Im)  //убрать//
                    {
                       txDSPBuf0[i] = -20 + (20+20)*(float)rand()/RAND_MAX;
                    }
                    sh_mem_send_time_seq(SPECTRUM_POWER_DSP0,(uint32_t *)&txDSPBuf0[0],(uint32_t *)&txDSPBuf0[0],512);
                    sh_mem_recieve(DSP0_);
                    memcpy(&ch_num[i].mass[0], &txDSPBuf0[useful_struct.start_val], sizeof(&txDSPBuf0[useful_struct.start_val]) * useful_struct.stop_val); // вырезал бины
                    ///проверка на переполнение ///

                    ///////////////////////////////
                    for (counter = 0; counter < useful_struct.stop_val; counter ++)
                    {
                        if (ch_num[i].mass[counter] > tresholds_item.energy_uhf)
                        {
                           total[c].is_active[(c*MAX_INTERVALS) + fch.num_intervals] = 1;         // частота в эфире

                           // если охраняемая проставляем признаки охраняемой в эфире
                           if ((total[c].guard == 1) && (total[c].is_active[(c*MAX_INTERVALS) + fch.num_intervals] == 1))
                           {
                               for (l = 0; l < total[c].total_guarded; l++)
                               {
                                   for (f = 0; f < total_signals; f++)  // перебор всех подавляемых для охраняемой
                                   {

                                       if (total[c].guard_idxs[l] == total[c].id)
                                       {
                                           total[c].on_guard[f + fch.num_intervals] = 1;
                                           break;
                                       }

                                   }

                               }

                           }

                        }

                    }
                    c++;

                }

            }
            //переход к следующей полосе
            s_poo += fch.channel[i+1].total_bands;
            //s_sig инкрементируется на число сигналов по каждому каналу


        }
        ind = ind +1;

        /// перестройка на следующую полосу

        if (ind != fch.Max_POO_in_Channel)
        {
            freqmhz = mass_bands[fch.channel[counter].bands[ind].band_idx] >> 16; // fr из структуры bands
            rcvr.mas_channel[counter].mas_freq_range[ind] = pres_freq_range(freqmhz*1000000);
            if (rcvr.mas_channel[counter].mas_freq_range[ind] !=  rcvr.mas_channel[counter].mas_freq_range[ind-1])
            {
                flag_aerial = selection_litera(freqmhz,flag_aerial,0);

                for (counter = 0; counter < fch.total_channels; counter ++ )
                {
                    set_rec_mode(set_cmd_1,MCSPI2_LVDS_CS3,fch.channel[counter].ch_id,rcvr.mas_channel[counter].mas_freq_range[ind],tresholds[ATT_UHF], 0,0);
                    config_ADC(bands.n_ddc[fch.channel[counter].bands[ind].band_idx],bands.fl[fch.channel[counter].bands[ind].band_idx],512,fch.channel[counter].ch_id);
                }

            }

        }
        else break;

    }
    result_fch();


}

void result_fch(void)
{
    int i, j,save_pp;
    int priznak = 0;
    uint32_t k = 0;
    interf_matrix matr = {0};
    pp[0].count = 0;
    pp[1].count = 0;
    pp[2].count = 0;
    pp[3].count = 0;

    for (i = 0; i < MAX_FREQS_ON_PP; i++)
    {
        pp[0].signal_id[i] = 0;
        pp[1].signal_id[i] = 0;
        pp[2].signal_id[i] = 0;
        pp[3].signal_id[i] = 0;

    }

    //группируем активные по передатчикам//
    group_signals_in_pp();

    //Алгоритм ЦРа

    for (i = 0; i < MAX_PP; i ++)
    {
        if (fch.nonctrl[i].total_signals > 0)
        {
            get_nonctrl_freqs(i);
            // осталась скважнасть под сигналы с контролем
            if (supr_mass[1] < fch.max_Q)
            {
                get_ctrl_freq_with_nonctrl(fch.nonctrl[i].pp_id);
            }
            save_pp = fch.nonctrl[i].pp_id;
        }
        else
        {
            get_ctrl_freq(fch.nonctrl[i].pp_id);
            save_pp = total[supr_mass[2]].pp_idx;
        }

        if (supr_mass[1] == 0)  // нет сигналов на подавление
        {
            fch.num_intervals ++;
            if (fch.num_intervals > MAX_INTERVALS)
            {
                // отправка результата по кнотролю

                fch.num_intervals = 0;
            }
            return;
        }

        // формируем задание на возбудители //

        for (j = 0; j < supr_mass[1]; j++)
        {
            // проставляем статистику о подавлении сигнала
            matr.freq[j]   = total[supr_mass[2+j]].freq_rp;
            matr.type[j]   = total[supr_mass[2+j]].type;
            matr.param1[j] = total[supr_mass[2+j]].param1;
            matr.param2[j] = total[supr_mass[2+j]].param2;
        }


    }
    fch.num_intervals ++;
    if (fch.num_intervals == MAX_INTERVALS)
    {
        k = sizeof(hat.n_bytes);

        hat.command = result_control_fch_;
        memcpy(&txEthBuf[k], &hat.command, sizeof(hat.command));
        k += sizeof(hat.command);

        memcpy(&txEthBuf[k], &fch.cr_id, sizeof(fch.cr_id));
        k += sizeof(fch.cr_id);

        memcpy(&txEthBuf[k], &total_signals, sizeof(total_signals));
        k += sizeof(total_signals);
        for (i = 0; i < total_signals; i++)
        {
            memcpy(&txEthBuf[k], &total[i].id, sizeof(total[i].id));
            k += sizeof(total[i].id);

            memcpy(&txEthBuf[k], &fch.num_intervals, sizeof(fch.num_intervals));
            k += sizeof(interval_idx);

            for (j = 0; j < fch.num_intervals; j++)
            {
                memcpy(&txEthBuf[k], &total[i].is_active[j], sizeof(total[i].is_active[j]));
                k += sizeof(total[i].is_active[j]);

                memcpy(&txEthBuf[k], &total[i].on_guard[j], sizeof(total[i].on_guard[j]));
                k += sizeof(total[i].on_guard[j]);

                memcpy(&txEthBuf[k], &total[i].on_supression[j], sizeof(total[i].on_supression[j]));
                k += sizeof(total[i].on_supression[j]);
           }
        }
        hat.n_bytes = k - sizeof(hat.n_bytes);
        memcpy(&txEthBuf[0], &hat.n_bytes, sizeof(hat.n_bytes));
        send_eth(arm, &txEthBuf[0], k);
        fch.num_intervals = 0;
    }

}


