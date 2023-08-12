/*
 * targets.c
 *
 *  Created on: 14 ��� 2021 �.
 *      Author: Sergey_Trushin
 */
#include "targets_fch.h"

fch_targets_      fch                     __attribute__((aligned(4), section(".bss:supression")));
total_signals_    total[MAX_FREQS]        __attribute__((aligned(4), section(".bss:supression")));
pp_               pp[MAX_PP]              __attribute__((aligned(4), section(".bss:supression")));
start_supression_ start_supression        __attribute__((aligned(4), section(".bss:supression")));

uint32_t interval_idx = 0,//������� ���������� ��

         supr_mass[6] = {0x00,//������� ������������
                         0x00,//����� �������� �� ����������
                         0x00,//�� �������
                         0x00,//�� �������
                         0x00,//�� �������
                         0x00},//�� �������

          total_signals = 0,//����� ��������
          total_nonctrl = 0,//����� ��� ��������
          shift = 0;
/*
 * ������� ��������� � ��������� �� ��������� ������
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

    //������������� ��
    memcpy(&fch.cr_id, &pBuf[k], sizeof(fch.cr_id));
    k += sizeof(fch.cr_id);

    //������� �������� ��
    memcpy(&fch.power_pp, &pBuf[k], sizeof(fch.power_pp));
    k += sizeof(fch.power_pp);

    //���� ����������
    memcpy(&fch.max_Q, &pBuf[k], sizeof(fch.max_Q));
    k += sizeof(fch.max_Q);

    //����� ��
    memcpy(&fch.num_pp, &pBuf[k], sizeof(fch.num_pp));
    k += sizeof(fch.num_pp);

    for(i = 0; i < fch.num_pp; i++)
    {
        //����� ��
        memcpy(&fch.nonctrl[i].pp_id, &pBuf[k], sizeof(fch.nonctrl[i].pp_id));
        k += sizeof(fch.nonctrl[i].pp_id);

        //����� ������
        memcpy(&fch.nonctrl[i].litera_id, &pBuf[k], sizeof(fch.nonctrl[i].litera_id));
        k += sizeof(fch.nonctrl[i].litera_id);

        //����� ����� �������� ��� ��������
        memcpy(&fch.nonctrl[i].total_signals, &pBuf[k], sizeof(fch.nonctrl[i].total_signals));
        k += sizeof(fch.nonctrl[i].total_signals);
        for(j = 0; j < fch.nonctrl[i].total_signals; j++)
        {
            //����� �������
            memcpy(&fch.nonctrl[i].signals[j].id, &pBuf[k], sizeof(fch.nonctrl[i].signals[j].id));
            k += sizeof(fch.nonctrl[i].signals[j].id);

            //��������� ������ � ����� ����������
            total[total_signals].id = fch.nonctrl[i].signals[j].id;
           // total[total_signals].num_intervals = MAX_INTERVALS;
            fch.num_intervals = MAX_INTERVALS;
            //������ ���������� � ������ ���������, ��� ������� ��� ��������
            memset(&total[total_signals].is_active, 2, MAX_INTERVALS*sizeof(total[total_signals].is_active));

            //������� �� � ���
            memcpy(&fch.nonctrl[i].signals[j].freq_rp, &pBuf[k], sizeof(fch.nonctrl[i].signals[j].freq_rp));
            k += sizeof(fch.nonctrl[i].signals[j].freq_rp);
            total[total_signals].freq_rp = fch.nonctrl[i].signals[j].freq_rp;

            //��� ������
            memcpy(&fch.nonctrl[i].signals[j].type, &pBuf[k], sizeof(fch.nonctrl[i].signals[j].type));
            k += sizeof(fch.nonctrl[i].signals[j].type);
            total[total_signals].type = fch.nonctrl[i].signals[j].type;

            //������������ �������� �������������
            memcpy(&fch.nonctrl[i].signals[j].discr, &pBuf[k], sizeof(fch.nonctrl[i].signals[j].discr));
            k += sizeof(fch.nonctrl[i].signals[j].discr);
            total[total_signals].discr = fch.nonctrl[i].signals[j].discr;

            //���� ����������
            memcpy(&fch.nonctrl[i].signals[j].Q, &pBuf[k], sizeof(fch.nonctrl[i].signals[j].Q));
            k += sizeof(fch.nonctrl[i].signals[j].Q);
            total[total_signals].Q = fch.nonctrl[i].signals[j].Q;

            //�������� ������ 1
            memcpy(&fch.nonctrl[i].signals[j].param1, &pBuf[k], sizeof(fch.nonctrl[i].signals[j].param1));
            k += sizeof(fch.nonctrl[i].signals[j].param1);
            total[total_signals].param1 = fch.nonctrl[i].signals[j].param1;

            //�������� ������ 2
            memcpy(&fch.nonctrl[i].signals[j].param2, &pBuf[k], sizeof(fch.nonctrl[i].signals[j].param2));
            k += sizeof(fch.nonctrl[i].signals[j].param2);
            total[total_signals].param2 = fch.nonctrl[i].signals[j].param2;
            total_signals++;
        }
    }
    total_nonctrl = total_signals;

    //����� ������� ��
    memcpy(&fch.total_channels, &pBuf[k], sizeof(fch.total_channels));
    k += sizeof(fch.total_channels);

    for(i = 0; i < fch.total_channels; i++)
    {
        //����� ������
        memcpy(&fch.channel[i].ch_id, &pBuf[k], sizeof(fch.channel[i].ch_id));
        k += sizeof(fch.channel[i].ch_id);

        //����� ����� �� �����
        memcpy(&fch.channel[i].total_bands, &pBuf[k], sizeof(fch.channel[i].total_bands));
        k += sizeof(fch.channel[i].total_bands);
        for(j = 0; j < fch.channel[i].total_bands; j++)
        {
            //����� �������
            memcpy(&fch.channel[i].bands[j].band_idx, &pBuf[k], sizeof(fch.channel[i].bands[j].band_idx));
            k += sizeof(fch.channel[i].bands[j].band_idx);

            //����� �������� � ������
            memcpy(&fch.channel[i].bands[j].total_signals, &pBuf[k], sizeof(fch.channel[i].bands[j].total_signals));
            k += sizeof(fch.channel[i].bands[j].total_signals);

            fch.Max_POO_in_Channel = fch.channel[0].total_bands; // ������� ������������ �����-�� ����� � �������
            if (fch.channel[j+1].total_bands > fch.Max_POO_in_Channel)
            {
                fch.Max_POO_in_Channel = fch.channel[j+1].total_bands;
            }

            for(l = 0; l < fch.channel[i].bands[j].total_signals; l++)
            {
                //������������� ������� �� �������
                memcpy(&fch.channel[i].bands[j].oncontrol[l].id, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].id));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].id);

                //��������� ������ � ����� ����������
                total[total_signals].id = fch.channel[i].bands[j].oncontrol[l].id;
                //total[total_signals].num_intervals = MAX_INTERVALS;
                fch.num_intervals = MAX_INTERVALS;

                //����� ����������� �����
                memcpy(&fch.channel[i].bands[j].oncontrol[l].pp_idx, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].pp_idx));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].pp_idx);
                total[total_signals].pp_idx = fch.channel[i].bands[j].oncontrol[l].pp_idx;

                //������ �����
                memcpy(&fch.channel[i].bands[j].oncontrol[l].freq_left, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].freq_left));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].freq_left);

                //������ ������
                memcpy(&fch.channel[i].bands[j].oncontrol[l].freq_right, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].freq_right));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].freq_right);


                /******** ���������� � ������ DDC ��������� ������ *********/
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

                //������� ��
                memcpy(&fch.channel[i].bands[j].oncontrol[l].freq_rp, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].freq_rp));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].freq_rp);
                total[total_signals].freq_rp = fch.channel[i].bands[j].oncontrol[l].freq_rp;

                //��� ���������� �������
                memcpy(&fch.channel[i].bands[j].oncontrol[l].type, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].type));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].type);
                total[total_signals].type = fch.channel[i].bands[j].oncontrol[l].type;

                //������������ �������� �������������
                memcpy(&fch.channel[i].bands[j].oncontrol[l].discr, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].discr));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].discr);
                total[total_signals].discr = fch.channel[i].bands[j].oncontrol[l].discr;

                //���� ����������
                memcpy(&fch.channel[i].bands[j].oncontrol[l].Q, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].Q));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].Q);
                total[total_signals].Q = fch.channel[i].bands[j].oncontrol[l].Q;

                //�������� ������ 1
                memcpy(&fch.channel[i].bands[j].oncontrol[l].param1, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].param1));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].param1);
                total[total_signals].param1 = fch.channel[i].bands[j].oncontrol[l].param1;

                //�������� ������ 2
                memcpy(&fch.channel[i].bands[j].oncontrol[l].param2, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].param2));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].param2);
                total[total_signals].param2 = fch.channel[i].bands[j].oncontrol[l].param2;

                //������� ����������
                memcpy(&fch.channel[i].bands[j].oncontrol[l].guard, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].guard));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].guard);
                total[total_signals].guard = fch.channel[i].bands[j].oncontrol[l].guard;

                //����� ���������� (�����-�� ����������� ������ ��� ����������)
                memcpy(&fch.channel[i].bands[j].oncontrol[l].total_guarded, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].total_guarded));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].total_guarded);
                total[total_signals].total_guarded = fch.channel[i].bands[j].oncontrol[l].total_guarded;

                for(f = 0; f < fch.channel[i].bands[j].oncontrol[l].total_guarded; f++)
                {
                    //������������ ����������� ���
                    memcpy(&fch.channel[i].bands[j].oncontrol[l].guarded_id[f], &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].guarded_id[f]));
                    k += sizeof(fch.channel[i].bands[j].oncontrol[l].guarded_id[f]);

                    //��������� � ����� ���������� ������ �����������
                    total[total_signals].guard_idxs[f] = fch.channel[i].bands[j].oncontrol[l].guarded_id[f];
                }

                //���������
                memcpy(&fch.channel[i].bands[j].oncontrol[l].priority, &pBuf[k], sizeof(fch.channel[i].bands[j].oncontrol[l].priority));
                k += sizeof(fch.channel[i].bands[j].oncontrol[l].priority);
                total[total_signals].priority = fch.channel[i].bands[j].oncontrol[l].priority;

                //���� ��������
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
 * ������� ��������� �������� ������������� � ������ �� ���������� ��� ��������
 *
 * �� ���� ����� �����������
 *
 * ��������� supr_mass[], ���
 * 0 ������� - ������� �������������
 * 1 ������� - ����� ������ �� ���������
 * 2 ������� - ������ �������
 * � �. �.
 */
static void get_nonctrl_freqs(int pp_id)
{
    uint32_t   i = 0,
               j = 0,
               max = 0,
               group;

    typedef struct{
        uint32_t    discr;      //��������� ��������
        uint32_t    count;      //������� �������� ��� �������
        uint32_t    id[50];     //������ �������
    }tmp_;
    tmp_ tmp[10] = {0};

    /*
     * ���������� �� �������� �������������;
     * ������� ����� �������� � ������;
     */
    for(i = 0; i < fch.nonctrl[pp_id].total_signals; i++)
    {
        if(total[shift].on_guard[interval_idx] == 1)
        {
            //���������� � �����, �� ����� � ������������
            continue;
        }

        //����������� �� �������� �������������
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
     * ����� ������ � ������������ ������ ��������
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
     * �������� �� ����������
     */
    if(tmp[group].count > fch.max_Q)
    {
        tmp[group].count = fch.max_Q;
    }

    /*
     * ������������ ��������� ������� �� ����������
     */
    supr_mass[0] = tmp[group].discr;//������� �����
    supr_mass[1] = tmp[group].count;//����� ��������
    j = 2;
    for(i = 0; i < tmp[group].count; i++)
    {
        supr_mass[j++] = tmp[group].id[i];//id ������� �� total[]
    }
}

/*
 * ������� ��������� �������� �� ���������� ��� ������� ������ ��� ��������
 * �� ���� ����� �����������
 *
 * �������� supr_mass[] ����������� ���������
 */
static void get_ctrl_freq_with_nonctrl(int pp_id)
{
    uint32_t i = 0,
             min = 0xffffffff, // ��������� �������� ��� ������ ����������
             count_prior = 0,
             ttl = 0,//����� ����� �������
             tmp = 0,
             group[20] = {0};//������ �������� (�� signal) �������� � ������������ �����������

    float   coef = 0;//���������� ��� ���������� �� ���� ��������

    typedef struct{
        uint32_t     id;
        float        coef;
        uint32_t     priority;
    }s;
    s       signal[20] = {0};//����� ���������� ��������� ��� ������ ������� �������������

    /*
     * �������� ������ ��� �������� ������� �������������;
     * ��������� ������������ ��������� ��������;
     * ���������� ����� ����� ��������;
     */
    for(i = 0; i < pp[pp_id].count; i++)
    {
        if(total[pp[pp_id].signal_id[i]].discr != supr_mass[0])
        {
            // �� �������� ������� �������������
            continue;
        }
        else
        {
            signal[ttl].id       = pp[pp_id].signal_id[i];
            signal[ttl].priority = total[pp[pp_id].signal_id[i]].priority;
            signal[ttl].coef     = total[pp[pp_id].signal_id[i]].coef;

            //���������� ����������� �����, �����. ����. ����������
            if(min >= signal[ttl].priority)
            {
                min = signal[ttl].priority;
            }
            ttl++;
        }
    }

    /*
     * C������ ����� ����� �������� � ���� ����������� � ���������� �� �������
     */
    for(i = 0; i < ttl; i++)
    {
        if(signal[i].priority == min)
        {
            group[count_prior++] = i;//��������� ������
        }
    }

    /*
     * ����� ������ �������� � ��������� � ��� ����� ���� ����������
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
     * ����� ������ �������� � ��������� � ��� ������ ���� ����������
     * ��������� �� ���� ��������
     */
    else if((supr_mass[1] + count_prior) > fch.max_Q)
    {
        while(fch.max_Q - supr_mass[1])
        {
            for(i = 0; i < count_prior; i++)
            {
                //���������� ������������ ���� ��������
                if(signal[group[i]].coef > coef)
                {
                    coef = signal[group[i]].coef;
                    tmp = i;
                }
            }
            //���������� ���������
            supr_mass[2 + supr_mass[1]] = signal[group[tmp]].id;
            supr_mass[1]++;

            //�������� �� ������������
            signal[group[tmp]].coef = 0;
            coef = 0;
        }
        return;
    }

    /*
     * ����� ����� �������� ������ ���� ����������
     */
    else if((supr_mass[1] + count_prior) < fch.max_Q)
    {
        //����������� ������� � ������ �����������
        for(i = 0; i < count_prior; i++)
        {
            supr_mass[2 + supr_mass[1] + i] = signal[group[i]].id;
        }
        supr_mass[1] += count_prior;

        /*
         * ��� ������� �����������, �������������� ���, ��� �����
         */
        if(ttl == count_prior)
        {
            return;
        }

        /*
         * ���� ������� � ����������� ����;
         * �������� �� ���
         */
        else
        {
            tmp = count_prior;//��������� ����� ��������

            //���������� �� ������������ ������� � ������ ����������� � ������� ������ �����������
            for(i = 0; i < count_prior; i++)
            {
                signal[group[i]].priority = 0xffffffff;
                group[i] = 0;
            }
            count_prior = 0;

            //��������� ��������� ����
            for(i = 0; i < ttl; i++)
            {
                if(min >= signal[i].priority)
                {
                    min = signal[i].priority;
                }
            }

            //������� ����� ����� �������� � ���� ����������� � ���������� �� �������
            for(i = 0; i < ttl; i++)
            {
                if(signal[i].priority == min)
                {
                    group[count_prior++] = i;
                }
            }

            /*
             * ����� ������ �������� � ��������� � ��� ����� ���� ����������
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
             * ����� ������ �������� � ��������� � ��� ������ ���� ����������
             * ��������� �� ���� ��������
             */
            else if((supr_mass[1] + count_prior) > fch.max_Q)
            {
                coef = 0.0;
                while(fch.max_Q - supr_mass[1])
                {
                    for(i = 0; i < count_prior; i++)
                    {
                        //���������� ������������ ���� ��������
                        if(signal[group[i]].coef >= coef)
                        {
                            coef = signal[group[i]].coef;
                            tmp = i;
                        }
                    }
                    //���������� ���������
                    supr_mass[2 + supr_mass[1]] = signal[group[tmp]].id;
                    supr_mass[1]++;

                    //�������� �� ������������
                    signal[group[tmp]].coef = 0;
                    coef = 0;
                }
                return;
            }

            /*
             * ����� ����� �������� ������ ���� ����������
             */
            else if((supr_mass[1] + count_prior) < fch.max_Q)
            {
                //����������� ������� � ������ �����������
                for(i = 0; i < count_prior; i++)
                {
                    supr_mass[2 + supr_mass[1] + i] = signal[group[i]].id;
                }
                supr_mass[1] += count_prior;

                /*
                 * ��� ������� �����������, �������������� ���, ��� �����
                 */
                if((total - tmp - count_prior) == 0)
                {
                    return;
                }
                else
                {
                    //���������� �� ������������ ������� � ������ ����������� � ������� ������ �����������
                    for(i = 0; i < count_prior; i++)
                    {
                        signal[group[i]].priority = 0xffffffff;
                        group[i] = 0;
                    }
                    count_prior = 0;

                    //��������� ��������� ����
                    for(i = 0; i < ttl; i++)
                    {
                        if(min >= signal[i].priority)
                        {
                            min = signal[i].priority;
                        }
                    }

                    //������� ����� ����� �������� � ����� �����������
                    for(i = 0; i < ttl; i++)
                    {
                        if(signal[i].priority == min)
                        {
                            group[count_prior++] = i;
                        }
                    }

                    /*
                     * ����� ������ �������� � ��������� � ��� ����� ���� ����������
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
                     * ����� ������ �������� � ��������� � ��� ������ ���� ����������
                     * ��������� �� ���� ��������
                     */
                    else if((supr_mass[1] + count_prior) > fch.max_Q)
                    {
                        coef = 0.0;
                        while(fch.max_Q - supr_mass[1])
                        {
                            for(i = 0; i < count_prior; i++)
                            {
                                //���������� ������������ ���� ��������
                                if(signal[group[i]].coef >= coef)
                                {
                                    coef = signal[group[i]].coef;
                                    tmp = i;
                                }
                            }
                            //���������� ���������
                            supr_mass[2 + supr_mass[1]] = signal[group[tmp]].id;
                            supr_mass[1]++;

                            //�������� �� ������������
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
 * ������� ��������� �������� �� ���������� ��� ��� ������� ������ ��� ��������
 * �� ���� ����� �����������
 *
 * �������� supr_mass[] ����������� ���������
 */
static void get_ctrl_freq(int pp_id)
{
    uint32_t i = 0,
             j = 0,
             idx = 0,
             min[NUM_PULS_DISCR] = {0xffffffff},//������������ ��������� ��� ������ ��  �����
             count_prior[NUM_PULS_DISCR] = {0},//����� �������� � ���� ���������� � ������
             weight = 0,
             equal[10] = {0},
             tmp = 0,
             mean_weight[NUM_PULS_DISCR] = {0};

    float    mean_coeff[NUM_PULS_DISCR] = {0.0},
             tmpf = 0.0;

    typedef struct{
        uint32_t  discr;//������� ������������� ������
        uint32_t  count;//����� ����� �������� � ������
        uint32_t  id[20];
        uint32_t  coef[20];
        uint32_t  priority[20];
    }s_;
    s_      signal[NUM_PULS_DISCR] = {0};//��������� ��� ������ ��������� �������������

    /*
     * �������� ������ �� �������� �������������;
     * ��������� ������������ ��������� � ������ ������;
     * ������� ����� ����� �������� � ������
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

                //���������� ����������� �����, �����. ����. ����������
                if(min[j] >= signal[j].priority[idx])
                {
                    min[j] = signal[j].priority[idx];
                }
                signal[j].count++;//����� ����� �������� ��� ������ �������������

                break;
            }
            else if(signal[j].discr == 0)
            {
                idx = signal[j].count;

                signal[j].discr         = total[pp[pp_id].signal_id[i]].discr;
                signal[j].id[idx]       = pp[pp_id].signal_id[i];
                signal[j].priority[idx] = total[pp[pp_id].signal_id[i]].priority;
                signal[j].coef[idx]     = total[pp[pp_id].signal_id[i]].coef;

                //���������� ����������� �����, �����. ����. ����������
                if(min[j] >= signal[j].priority[idx])
                {
                    min[j] = signal[j].priority[idx];
                }
                signal[j].count++;;//����� ����� �������� ��� ������ �������������
                break;
            }
        }
    }

    /*
     * ��������� ������ �� ����������;
     * ��������� ������ ������ � ������� ���� ������ ���������
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
                signal[i].count = 0;//���������� ����������������� ������
            }
        }
    }

    /*
     * ����������� ����� �������� � ���� ����������� � ������ ������;
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

        //���� ������������ ��� ����� ���� �����
        if(count_prior[i] > weight)
        {
            weight = count_prior[i];//��������� ���
        }
    }

    /*
     * ���������, ��� ������ � ������������ ������ ���� ������������ �������� ������������
     */
    for(i = 0; i < NUM_PULS_DISCR; i++)
    {
        if(count_prior[i] == weight)
        {
            equal[0]++;//����� ����� �� �����
            equal[equal[0]++] = i;//������ ������ signal[]
        }
    }

    /*
     * ������ ������������;
     * ��������� �� ������ signal[idx]
     */
    if(equal[0] == 1)
    {
        idx = equal[1];
    }
    /*
     * ������ �� ������������
     */
    else
    {
        /*
         * ��������� ������ � ���������� ����� �� ���������� ���� ��������
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

            //���� ������������ ��������� ���� ��������
            if(mean_coeff[idx] > tmpf)
            {
                tmpf = mean_coeff[idx];//��������� ���
            }
        }

        //�������
        memset(equal, 0, sizeof(equal));

        /*
         * ���������, ��� ������ � ������������ ���� �������� ������������
         */
        for(i = 0; i < NUM_PULS_DISCR; i++)
        {
            if(mean_coeff[i] == tmpf)
            {
                equal[0]++;//����� ����� �� �����
                equal[equal[0]++] = i;//������ ������ signal[]
            }
        }

        /*
         * ������ ������������
         */
        if(equal[0] == 1)
        {
            idx = equal[1];
        }

        /*
         * ������ �� ������������;
         * ���������� �� ���������� ���������� ������ (��� ����� ������, ��� � �������)
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

                //���� ������������ ��������� ���������, ��������������� ������������ �����
                if(mean_weight[equal[i+1]] < weight)
                {
                    weight = mean_weight[equal[i+1]];//��������� ���
                    idx = equal[i+1];
                }
            }
        }
    }

    /*
     * ����� ����� �������� c ���� ����������� ����� ���� ����������
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
     * ����� ����� �������� ������ ������������ ����������;
     * ��������� �� ���� �������� � �������� ������ ����������
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
                    //���������� ������������ ���� ��������
                    if(signal[idx].coef[i] > tmpf)
                    {
                        tmpf = signal[idx].coef[i];
                        tmp = i;
                    }
                }
            }

            //���������� ���������
            supr_mass[2 + j] = signal[idx].id[tmp];

            //�������� �� ������������
            signal[idx].coef[tmp] = 0;
            tmpf = 0.0;
            j++;
        }
        return;
    }

    /*
     * ����� ����� �������� ���� ���������� �� ����� ���� ����������;
     * �������� ������� � ����������� ����;
     */
    else if(count_prior[idx] < fch.max_Q)
    {
        //����������� ������� � ������ �����������
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
         * ��� ������� �����������, �������������� ���, ��� �����
         */
        if(signal[idx].count == count_prior[idx])
        {
            return;
        }
        /*
         * �������� � ���������� ����
         */
        else
        {
            tmp = count_prior[idx];//��������� ����� ��������

            //���������� �� ������������ ������� � ������ ����������� � ������� ������ �����������
            for(i = 0; i < signal[idx].count; i++)
            {
                if(signal[idx].priority[i] == min[idx])
                {
                    signal[idx].priority[i] = 0xffffffff;;
                }
            }
            count_prior[idx] = 0;

            //��������� ��������� ����
            for(i = 0; i < signal[idx].count; i++)
            {
                if(min[idx] >= signal[idx].priority[i])
                {
                    min[idx] = signal[idx].priority[i];
                }
            }

            //������� ����� ����� �������� � ���� �����������
            for(i = 0; i < signal[idx].count; i++)
            {
                if(signal[idx].priority[i] == min[idx])
                {
                    count_prior[idx]++;
                }
            }

            /*
             * ����� ����� �������� ����� ���� ����������
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
             * ����� ����� �������� ������ ����������;
             * �������� �� ���� ��������
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
                            //���������� ������������ ���� ��������
                            if(signal[idx].coef[i] >= tmpf)
                            {
                                tmpf = signal[idx].coef[i];
                                tmp = i;
                            }
                        }
                    }

                    //���������� ���������
                    supr_mass[2 + j] = signal[idx].id[tmp];
                    supr_mass[1]++;

                    //�������� �� ������������
                    signal[idx].coef[tmp] = 0;
                    tmpf = 0.0;
                    j++;
                }
                return;
            }

            /*
             * �������� ������ ����������;
             * �������� ��������� ����
             */
            else if((supr_mass[1] + count_prior[idx]) < fch.max_Q)
            {
                //����������� ������ ���������
                for(i = 0; i < signal[idx].count; i++)
                {
                    if(signal[idx].priority[i] == min[idx])
                    {
                        supr_mass[2 + supr_mass[1] + i] = signal[idx].id[i];
                    }
                }
                supr_mass[1] += count_prior[idx];

                /*
                 * ������� ���������, �������
                 */
                if((signal[idx].count - tmp - count_prior[idx]) == 0)
                {
                    return;
                }
                else
                {
                    //���������� �� ������������ ������� � ������ ����������� � ������� ������ �����������
                    for(i = 0; i < signal[idx].count; i++)
                    {
                        if(signal[idx].priority[i] == min[idx])
                        {
                            signal[idx].priority[i] = 0xffffffff;;
                        }
                    }
                    count_prior[idx] = 0;

                    //��������� ��������� ����
                    for(i = 0; i < signal[idx].count; i++)
                    {
                        if(min[idx] >= signal[idx].priority[i])
                        {
                            min[idx] = signal[idx].priority[i];
                        }
                    }

                    //������� ����� ����� �������� � ���� �����������
                    for(i = 0; i < signal[idx].count; i++)
                    {
                        if(signal[idx].priority[i] == min[idx])
                        {
                            count_prior[idx]++;
                        }
                    }

                    /*
                     * ����� ����� �������� ����� ���� ����������
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
                     * �������� ������ ��� ���� ����������;
                     * ��������� �� ���� ��������
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
                                    //���������� ������������ ���� ��������
                                    if(signal[idx].coef[i] > tmpf)
                                    {
                                        tmpf = signal[idx].coef[i];
                                        tmp = i;
                                    }
                                }
                            }

                            //���������� ���������
                            supr_mass[2 + j] = signal[idx].id[tmp];
                            supr_mass[1]++;

                            //�������� �� ������������
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
 * ������� ����������� �������� �� ������������
 * �� ������ ����������� ������ ����������� � ��� ������� ����������� ������� ������� ����� ��������
 */
static void group_signals_in_pp(void)
{
    uint32_t i = 0,
             pp_idx = 0;

    for(i = 0; i < total_signals; i++)
    {
        //������ �� �������� ��� ��� ��������, ����������
        if(total[i].is_active[interval_idx] != 1)
        {
            continue;
        }

        //���������� � �����, ����������
        if((total[i].on_guard[interval_idx] != 1))
        {
            continue;
        }

        //������ ����������, ����������
        if((total[i].guard == 1))
        {
            continue;
        }

        pp_idx = total[i].pp_idx;
        //������ ����� �������� ��������� � ��������� ����������
        pp[pp_idx].signal_id[pp[pp_idx].count++] = i;
    }
}

/*
 * ������� ������������� ������ �������� ��
 */
void init_fch()
{
    int count;
    //������� ������� ��������� ��������
    memset(&fch, 0, sizeof(fch));
    memset(&total, 0, sizeof(total));
    memset(&pp, 0, sizeof(pp));
    fill_fch_struct();
    Lable_first_control = 1;

}

/*
 * ������� �������� �����
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

    switch(start_supression.number)// � ����������� � ������� ����������� ��������� �������
    {
        case(0):  // (200/5/20) ��
        {
            // ��� ������ ���� �������� ������������� �� ���� = ���� ������������� (�� ��������� ������ �������, ����� ������� ������������, ����� ������ ����������)
            break;
        }

        case(1):  // (200/0.25/1) ����
        {
            break;
        }

        case(2):  // (100/2.5/5)  ��, ����
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

        case(5):  // ����������� ����������
        {
            break;
        }

        case(6): // ���������� ��������� ��� ���������� ����
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

            freqmhz = mass_bands[fch.channel[counter].bands[0].band_idx] >> 16; // fr �� ��������� bands
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
    for (j = 0; j < fch.Max_POO_in_Channel; j++)  // ���� ��  ������� Max_POO_in_Channel
    {
        c = total_nonctrl + s_sig;
        for (i = 0; i < fch.total_channels; i++)  // ���� �� ����� �������
        {
            for (k = 0; k < bands.n_ddc[fch.channel[i].bands[0].band_idx]; k ++)   // ���� �� ddc � ������
            {

                for (z = 0; z < fch.signals_ddc[s_poo].count_signals[k]; z++)    //// ���� �� �������� � �������� 1 ddc
                {
                    total[0].is_active[(c*MAX_INTERVALS) + fch.num_intervals] = 0;
                    total[0].on_supression[(c*MAX_INTERVALS) + fch.num_intervals] = 0;
                    total[0].on_guard[(c*MAX_INTERVALS) + fch.num_intervals] = 0;

                    Start_data();
                    for(counter = 0; counter < 1024; counter++)                       // �������� �������� ��� (512Re,512Im)  //������//
                    {
                       txDSPBuf0[i] = -20 + (20+20)*(float)rand()/RAND_MAX;
                    }
                    sh_mem_send_time_seq(SPECTRUM_POWER_DSP0,(uint32_t *)&txDSPBuf0[0],(uint32_t *)&txDSPBuf0[0],512);
                    sh_mem_recieve(DSP0_);
                    memcpy(&ch_num[i].mass[0], &txDSPBuf0[useful_struct.start_val], sizeof(&txDSPBuf0[useful_struct.start_val]) * useful_struct.stop_val); // ������� ����
                    ///�������� �� ������������ ///

                    ///////////////////////////////
                    for (counter = 0; counter < useful_struct.stop_val; counter ++)
                    {
                        if (ch_num[i].mass[counter] > tresholds_item.energy_uhf)
                        {
                           total[c].is_active[(c*MAX_INTERVALS) + fch.num_intervals] = 1;         // ������� � �����

                           // ���� ���������� ����������� �������� ���������� � �����
                           if ((total[c].guard == 1) && (total[c].is_active[(c*MAX_INTERVALS) + fch.num_intervals] == 1))
                           {
                               for (l = 0; l < total[c].total_guarded; l++)
                               {
                                   for (f = 0; f < total_signals; f++)  // ������� ���� ����������� ��� ����������
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
            //������� � ��������� ������
            s_poo += fch.channel[i+1].total_bands;
            //s_sig ���������������� �� ����� �������� �� ������� ������


        }
        ind = ind +1;

        /// ����������� �� ��������� ������

        if (ind != fch.Max_POO_in_Channel)
        {
            freqmhz = mass_bands[fch.channel[counter].bands[ind].band_idx] >> 16; // fr �� ��������� bands
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

    //���������� �������� �� ������������//
    group_signals_in_pp();

    //�������� ���

    for (i = 0; i < MAX_PP; i ++)
    {
        if (fch.nonctrl[i].total_signals > 0)
        {
            get_nonctrl_freqs(i);
            // �������� ���������� ��� ������� � ���������
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

        if (supr_mass[1] == 0)  // ��� �������� �� ����������
        {
            fch.num_intervals ++;
            if (fch.num_intervals > MAX_INTERVALS)
            {
                // �������� ���������� �� ��������

                fch.num_intervals = 0;
            }
            return;
        }

        // ��������� ������� �� ����������� //

        for (j = 0; j < supr_mass[1]; j++)
        {
            // ����������� ���������� � ���������� �������
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


