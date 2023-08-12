#include "GPS_Kompas.h"

// ��������� �������� ���������
int8_t set_recive_trs_GPS_Kompas(UART_Handle uartHnd_1)
{
    int8_t status = 0;
    uint16_t npcket;
    int count;

    npcket = 0x40;           // ��������� ���������� ������ �� ���������������� ������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x80;           // ������ ���������� ������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x41;           // ��������� ������� ��������� � ��������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x81;           // ������ ������������� ������� ���������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x42;           // ��������� ������������ ������� ���������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x82;           // ������ ������������ ������� ���������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x43;           // ��������� ������������ ���������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x83;           // ������ ������������ ���������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x44;           // ��������� ��������������� ���������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x84;           // ������ ��������������� ���������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x45;           // ��������� ������� �����
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x85;           // ������ ������� �����
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x46;           // ��������� ��������� ������ ��������� �������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x86;           // ������ �������� ��������� ������ ��������� �������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x47;           // ��������� ��������� ������ ����� �������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x87;           // ������ ��������� ������ ����� �������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x48;           // ��������� ���������� ������� ��������� ������������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x88;           // ������ ���������� ������� ��������� ������������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x49;           // ��������� ��������� ������ ����������� ������������� �����������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x89;           // ������ ��������� ������ �����������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x4A;           // ��������� �������� �� ��������� �����
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x8A;           // ������ �������� �� ��������� �����
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x4B;           // �������� � ����������� � ������������� ��������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x8B;           // ������ ����������� � ������������� ���������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x4C;           // ��������� �� ����������� � ������������� ���������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x8C;           // ������ ���������� ����������� � �������������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x4D;           // ���������� ������ ������������� ���������������� ��������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x8D;           // ������ �������� ������� ������������� ���������������� ��������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x4E;           // ���������� ������ ������������� RAIM
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x8E;           // ������ ������� ������������� RAIM
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x4F;           // ���������� ����������� ���������� �������� ���������������� ��������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x8F;           // ������ ����������� ���������� �������� ��� ��������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x50;           // ���������� ����. ���������� ������ �� ������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x90;           // ������ ���������� ������ ����������� ������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x51;           // ���������� ����. ���������� ������ � �����
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x91;           // ������� ���������� ������ ��������� �������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x52;           // ���������� ����. ���������� ������ �� �������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x92;           // ������ ����. ���������� ������ �� �������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x53;           // ���������� ������������ ����� ��������� ���� ��� ������������� �������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x93;           // ������ ������������� ������� ��������� ���� ��� ����� �������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x54;           // ���������� ����� ������ ����������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x94;           // ������ ������ ������ ����������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x55;           // ���������� ����� ������������� �����������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x95;           // ������ ������ ������������� �����������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x57;           // ���������� ���������� ������ ����������� ���������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x97;           // ������ ���������� ������ ���������� ���������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x58;           // ���������� ������� ����� � ����
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x98;           // ������ �������� ������� � ����
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x59;           // ���������� ��������� ���������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x99;           // ������ ���������� ���������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x5A;           // ���������� �������� � �� ������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x9A;           // ������ �������� � �� ������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x5B;           //���������� ����� ����� �������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x9B;           // ������ �������� ������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x5C;           // ���������� ����������� ���� ����� � ��������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x9C;           // ������� ������������ ���� �����
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x5D;           // ������������� ��������� ���� ���������� ���
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x9D;           // ������ ������� ��������� ���� ���
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x5E;           // �������� � ���������� ������������� �������� ������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x9E;           // ������ �����, ����������� � ������ �������� �������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x5F;           // ��������� �� ���������� ������������� �������� �������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x9F;           // ����� �������, ����������� � ������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);

    npcket = 0x60;           // �������� � ���������� ������������� NMEA-�������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0xA0;             // ������ ����������� � ������ NMEA ������� ������������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x61;           // ��������� �� ���������� ������������� NMEA
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0xA1;           // ����� ����������� � ������ NMEA
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x62;           // �������� ��� ��������� ���������� �������������� ����������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0xA2;           // ������ ��������� ���������� �������������� ����������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x63;           // ���������� ����� �������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0xA3;           // ������ �������������� ������ �������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x64;           // �������� ��� ��������� ���������� ������������� �����������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0xA4;           // ������ ��������� ���������� ������������� �����������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x65;           // ���������� ���������� �������������������� ������ ���������������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0xA5;           // ������ �������������������� ������ ���������������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x66;           // ���������/��������� ������������� ���
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x67;           // ���������/��������� �������������� ������ ��������� ������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);

    npcket = 0x68;           // ���������/��������� �������������� ������ NMEA-�������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x69;           // �������� � ��������� �������� ��������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x6A;           // �������� � ��������� �������� GPS
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x6F;           // ���������� �������� ��������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x72;           // ���������� ���� ���������
    set_tasks_GPS_Kompas(npcket,uartHnd_1);




    return status;
}

void GPS_GLON(UART_Handle RNM_handle)
{
    uint8_t data[2];
    uint16_t ck_sum;
    uint32_t cntr_Write = 0;

    if (global_first_RNV == 1)
    {
        // ��� ������ ���������, ����������� ��������� ���. ����������� �������
        data[0] = 0x02; // ��������� ������� �������(������ ��������) � ������� GPS
        data[1] = 0x00;

        GPS_Kompas_Buf[cntr_Write] = 0x10; // <����>
        cntr_Write++;
        GPS_Kompas_Buf[cntr_Write] = 0x42; // <��������������>
        cntr_Write++;
        ck_sum = 0x0002; // ����� �� ������ 2 (XOR)
        GPS_Kompas_Buf[cntr_Write] = data[0];
        cntr_Write++;
        GPS_Kompas_Buf[cntr_Write] = data[1];
        cntr_Write++;
        GPS_Kompas_Buf[cntr_Write] = (uint8_t)(ck_sum & 0x00ff);
        GPS_Kompas_Buf[cntr_Write] = (uint8_t)((ck_sum & 0xff00)>>8);
        GPS_Kompas_Buf[cntr_Write] = 0x10; // <����>
        cntr_Write++;
        GPS_Kompas_Buf[cntr_Write] = 0x03; // <���>
        cntr_Write++;
        UART_write(RNM_handle, (void *)&GPS_Kompas_Buf[0], cntr_Write);
        cntr_Write = 0;
    }
    InitSP(RNM_handle);
}

///  �����-�� ����������  �������������� ������ ��������� ������
void InitSP(UART_Handle local_RNM)
{

    uint32_t cntr_Write = 0;
/// ��������� �������������� ������ ���� � �������///
    GPS_Kompas_Buf[cntr_Write] = 0x10; // <����>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x67; // <��������������>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x2E; // ���� � �����
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x01;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x2F; // ��
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x10;// <����>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x03;// <���>
    cntr_Write++;
    UART_write(local_RNM, (void *)&GPS_Kompas_Buf[0], cntr_Write);
    cntr_Write = 0;

    /////  ��������� �������������� ������ ������������� �����������
    GPS_Kompas_Buf[cntr_Write] = 0x10; // <����>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x67; // <��������������>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x2A; // ������������� �����������
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x01; // ���������� ��������� ������
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x2B; // ����������� �����
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x10;// <����>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x03;// <���>
    cntr_Write++;
    UART_write(local_RNM, (void *)&GPS_Kompas_Buf[0], cntr_Write);
    cntr_Write = 0;

    /// �������� ����������� ����� ��� ��������� ������� ��������� � ��������

    GPS_Kompas_Buf[cntr_Write] = 0x10; // <����>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x41; // <��������������>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x01; // ����� ������� ��������� (��-42)
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00; // ����� ������������� ��������
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x01; // ����������� �����
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x10;// <����>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x03;// <���>
    cntr_Write++;
    UART_write(local_RNM, (void *)&GPS_Kompas_Buf[0], cntr_Write);
    cntr_Write = 0;

}

void InitSSys(UART_Handle local_RNM)
{
    ;;
}

// ������� ���������� ��������� �������� ���������, � ����� ��������� ���������� �������� �����
void Save_changes(UART_Handle local_RNM)
{
    uint32_t cntr_Write = 0;

    GPS_Kompas_Buf[cntr_Write] = 0x10; // <����>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0xD0; // <��������������>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x01;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x01; // ��
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x10;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x03;
    cntr_Write++;

}

