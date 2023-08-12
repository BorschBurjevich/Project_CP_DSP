#include "GPS_Kompas.h"
#include <stdlib.h>
void set_tasks_GPS_Kompas(uint16_t iden, UART_Handle uartHnd_GPS)
{
    unsigned int index_buf; // индекс конца заполняемого массива с данными без шапки кодограммы
    uint16_t ck_sum;
    uint16_t ck_sum_mas[12];
    uint32_t status_uart_wite;
    unsigned int retindex,size_Buf;
    set_tasks_struct set_tasks_GPS;
    status_uart_wite = 0;size_Buf = 0;
    retindex        = 0;
    ck_sum          = 0;
    memset(&GPS_Kompas_Buf[0],0x0,sizeof(GPS_Kompas_Buf[0])*100);
    memset(&ck_sum_mas[0],0x0,sizeof(ck_sum_mas[0])*12);
    memset(&set_tasks_GPS,0x0,sizeof(set_tasks_GPS));
    if (iden == 0x40)
    {
        set_tasks_GPS.pack_40h[0] = 1;       // Номер порта RS
        set_tasks_GPS.pack_40h[1] = 65535;   // Скорость обмена по порту

        /* 65535 = 115200 max
         * допустимые значения pack_40h[1]:
         * 150,300,600, 1200, 2400, 4800, 9600, 19200, 38400, 57600
         */
        set_tasks_GPS.pack_40h[2] = 0x1;     // Использование сигнала "handshake", 0x0-off,  0x1-on

        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x40; // <Индентификатор>
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.pack_40h[0], sizeof(set_tasks_GPS.pack_40h[0])*3);
        ck_sum            = set_tasks_GPS.pack_40h[0] ^ set_tasks_GPS.pack_40h[1] ^ set_tasks_GPS.pack_40h[2];
        memcpy(&GPS_Kompas_Buf[8],&ck_sum, sizeof(ck_sum));
        index_buf = 8;
        retindex = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2;
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        status_uart_wite = UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x80)
    {
        set_tasks_GPS.pack_40h[0] = 1;// Номер порта RS
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x80; // <Индентификатор>
        memcpy(&GPS_Kompas_Buf[2], &set_tasks_GPS.pack_40h[0], sizeof(set_tasks_GPS.pack_40h[0]));
        ck_sum            = set_tasks_GPS.pack_40h[0] ^ 0x0000 ^ 0x0000;
        memcpy(&GPS_Kompas_Buf[4],&ck_sum, sizeof(ck_sum));
        index_buf = 4;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2;
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x41)
    {
        uint16_t count;
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x41; // <Индентификатор>
        for (count = 0; count < 68; count++)
        {
            set_tasks_GPS.System_coordinate[count] = count;
        }
        for (count = 0; count < 4; count++)
        {
            set_tasks_GPS.projection[count] = count;
        }
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.System_coordinate[0],sizeof(set_tasks_GPS.System_coordinate[0])); //ПЗ-90 система координат
        memcpy(&GPS_Kompas_Buf[4],&set_tasks_GPS.projection[0],sizeof(set_tasks_GPS.projection[0])); // использую географическую проекцию
        ck_sum            = set_tasks_GPS.System_coordinate[0] ^ set_tasks_GPS.projection[0] ^ 0x0000;
        memcpy(&GPS_Kompas_Buf[6],&ck_sum ,sizeof(ck_sum ));
        index_buf         = 6;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x81)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x81; // <Индентификатор>
        ck_sum            = 0x0;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf         = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x42)
    {
        uint16_t count;

        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x42; // <Индентификатор>
        for (count = 0; count < 5; count++)
        {
            set_tasks_GPS.sys_navigate[count] = count;
        }
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.sys_navigate[0],sizeof(set_tasks_GPS.sys_navigate[0])); // использую систему Глонасс
        ck_sum            = set_tasks_GPS.sys_navigate[0] ^ 0x0000 ^ 0x0000;
        memcpy(&GPS_Kompas_Buf[4],&ck_sum,sizeof(ck_sum));
        index_buf = 4;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x82)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x82; // <Индентификатор>
        ck_sum            = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf         = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x43)
    {
        set_tasks_GPS.used_protocol[0] = 0; // бинарный
        set_tasks_GPS.used_protocol[1] = 1; // NMEA
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x43; // <Индентификатор>
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.used_protocol[0], sizeof(set_tasks_GPS.used_protocol[0]));
        ck_sum            = set_tasks_GPS.used_protocol[0] ^ 0x0000 ^ 0x0000;
        memcpy(&GPS_Kompas_Buf[4],&ck_sum,sizeof(ck_sum));
        index_buf         = 4;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x83)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x83; // <Индентификатор>
        ck_sum            = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf         = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x44)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x44; // <Индентификатор>
        set_tasks_GPS.used_protocol[1] = 1; // порт 1
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.used_protocol[0], sizeof(set_tasks_GPS.used_protocol[0])*2);
        ck_sum            = set_tasks_GPS.used_protocol[0] ^ set_tasks_GPS.used_protocol[1] ^ 0x0000;
        memcpy(&GPS_Kompas_Buf[6], &ck_sum, sizeof(ck_sum));
        index_buf         = 6;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x84)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x84; // <Индентификатор>
        ck_sum            = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf         = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x45)
    {

        uint16_t count;
        for (count = 0; count < 68; count++)
        {
            set_tasks_GPS.System_coordinate[count] = count;
        }
        for (count = 0; count < 4; count++)
        {
            set_tasks_GPS.projection[count] = count;
        }
        /*for test*/
        set_tasks_GPS.B_ = 66.66;
        set_tasks_GPS.L_ = 66.66;
        set_tasks_GPS.H_ = 66.66;
        ////////////////////////
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x45; // <Индентификатор>
        GPS_Kompas_Buf[2] = (uint8_t)  set_tasks_GPS.System_coordinate[0] & 0x00FF;    // LSB part
        GPS_Kompas_Buf[3] = (uint8_t) ((set_tasks_GPS.System_coordinate[0] & 0xFF00)>>8); // MSB part
        GPS_Kompas_Buf[4] = (uint8_t)  set_tasks_GPS.projection[0] & 0x00FF;    // LSB part         использую географическую проекцию
        GPS_Kompas_Buf[5] = (uint8_t) ((set_tasks_GPS.projection[0] & 0xFF00)>>8); // MSB part
        memcpy(&GPS_Kompas_Buf[6], &set_tasks_GPS.B_, sizeof(set_tasks_GPS.B_));
        memcpy(&GPS_Kompas_Buf[14],&set_tasks_GPS.L_, sizeof(set_tasks_GPS.L_));
        memcpy(&GPS_Kompas_Buf[22],&set_tasks_GPS.H_, sizeof(set_tasks_GPS.H_));

        ck_sum_mas[0]     = (uint16_t)(GPS_Kompas_Buf[6] & 0x00000000000000FF);
        ck_sum_mas[0]     = (uint16_t)((GPS_Kompas_Buf[7] & 0x00000000000000FF)<<8)  | ck_sum_mas[0];
        ck_sum_mas[1]     = (uint16_t)(GPS_Kompas_Buf[8] & 0x00000000000000FF);
        ck_sum_mas[1]     = (uint16_t)((GPS_Kompas_Buf[9] & 0x00000000000000FF)<<8)  | ck_sum_mas[1];
        ck_sum_mas[2]     = (uint16_t)(GPS_Kompas_Buf[10] & 0x00000000000000FF);
        ck_sum_mas[2]     = (uint16_t)((GPS_Kompas_Buf[11] & 0x00000000000000FF)<<8) | ck_sum_mas[2];
        ck_sum_mas[3]     = (uint16_t)(GPS_Kompas_Buf[12] & 0x00000000000000FF);
        ck_sum_mas[3]     = (uint16_t)((GPS_Kompas_Buf[13] & 0x00000000000000FF)<<8) | ck_sum_mas[3];
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        ck_sum_mas[4]     = (uint16_t)(GPS_Kompas_Buf[14] & 0x00000000000000FF);
        ck_sum_mas[4]     = (uint16_t)((GPS_Kompas_Buf[15] & 0x00000000000000FF)<<8) | ck_sum_mas[4];
        ck_sum_mas[5]     = (uint16_t)(GPS_Kompas_Buf[16] & 0x00000000000000FF);
        ck_sum_mas[5]     = (uint16_t)((GPS_Kompas_Buf[17] & 0x00000000000000FF)<<8) | ck_sum_mas[5];
        ck_sum_mas[6]     = (uint16_t)(GPS_Kompas_Buf[18] & 0x00000000000000FF);
        ck_sum_mas[6]     = (uint16_t)((GPS_Kompas_Buf[19] & 0x00000000000000FF)<<8) | ck_sum_mas[6];
        ck_sum_mas[7]     = (uint16_t)(GPS_Kompas_Buf[20] & 0x00000000000000FF);
        ck_sum_mas[7]     = (uint16_t)((GPS_Kompas_Buf[21] & 0x00000000000000FF)<<8) | ck_sum_mas[7];
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        ck_sum_mas[8]     = (uint16_t)(GPS_Kompas_Buf[22] & 0x00000000000000FF);
        ck_sum_mas[8]     = (uint16_t)((GPS_Kompas_Buf[23] & 0x00000000000000FF)<<8) | ck_sum_mas[8];
        ck_sum_mas[9]     = (uint16_t)(GPS_Kompas_Buf[24] & 0x00000000000000FF);
        ck_sum_mas[9]     = (uint16_t)((GPS_Kompas_Buf[25] & 0x00000000000000FF)<<8) | ck_sum_mas[9];
        ck_sum_mas[10]    = (uint16_t)(GPS_Kompas_Buf[26] & 0x00000000000000FF);
        ck_sum_mas[10]    = (uint16_t)((GPS_Kompas_Buf[27] & 0x00000000000000FF)<<8) | ck_sum_mas[10];
        ck_sum_mas[11]    = (uint16_t)(GPS_Kompas_Buf[28] & 0x00000000000000FF);
        ck_sum_mas[11]    = (uint16_t)((GPS_Kompas_Buf[29] & 0x00000000000000FF)<<8) | ck_sum_mas[11];
        ck_sum            = ck_sum_mas[0]^ck_sum_mas[1]^ck_sum_mas[2]^ck_sum_mas[3]^ck_sum_mas[4]^ck_sum_mas[5]^
                            ck_sum_mas[6]^ck_sum_mas[7]^ck_sum_mas[8]^ck_sum_mas[9]^ck_sum_mas[10]^ck_sum_mas[11];
        memcpy(&GPS_Kompas_Buf[30],&ck_sum, sizeof(ck_sum));

        index_buf = 29;
        ///////////////////////////////////////////////////
        retindex = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_ON);
        retindex += 6;
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);

        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0],size_Buf);
        bzero(&GPS_Kompas_Buf[0],100);
    }
    else if (iden == 0x85)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x85; // <Индентификатор>
        ck_sum            = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf         = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }
    else if (iden == 0x46)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x46; // <Индентификатор>
        set_tasks_GPS.Type[0] = 0x0; //амплитуда сигнала в аппаратно зависимых единицах;
        set_tasks_GPS.Type[1] = 0x0001; //энергетический потенциал в дБГц
        set_tasks_GPS.Type[2] = 0x0002; // энергетический потенциал в разах
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Type[0],sizeof(set_tasks_GPS.Type[0]));
        ck_sum            = set_tasks_GPS.Type[0] ^ 0x0000 ^ 0x0000;
        memcpy(&GPS_Kompas_Buf[4], &ck_sum, sizeof(ck_sum));
        index_buf         = 4;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }
    else if (iden == 0x86)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x86; // <Индентификатор>
        ck_sum            = 0x0000;
        memcpy(&GPS_Kompas_Buf[2], &ck_sum, sizeof(ck_sum));
        index_buf  = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }
    else if (iden == 0x47)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x47; // <Индентификатор>
        set_tasks_GPS.Type_metka[0] = 0; // выдача синхронно с системной шкалой времени навигационной системы ГЛОНАСС
        set_tasks_GPS.Type_metka[1] = 1; // выдача синхронно с системной шкалой времени навигационной системы GPS
        set_tasks_GPS.Type_metka[2] = 2; // синхронно с секундой UTC SU;
        set_tasks_GPS.Type_metka[3] = 3; // синхронно с секундой UTC
        set_tasks_GPS.Type_metka[4] = 4; // синхронно с моментом проведения измерений
        set_tasks_GPS.Always[0]     = 0; // выдается только при наличии гарантии правильности ее выдачи
        set_tasks_GPS.Always[1]     = 1; // выдается раз в секунду


        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Type_metka[0],sizeof(set_tasks_GPS.Type_metka[0]));
        memcpy(&GPS_Kompas_Buf[4],&set_tasks_GPS.Always[0],sizeof(set_tasks_GPS.Always[0]));
        ck_sum_mas[0] = set_tasks_GPS.Type_metka[0] ^ set_tasks_GPS.Always[0] ^ 0x0000;
        memcpy(&GPS_Kompas_Buf[6], &ck_sum_mas[0],sizeof(ck_sum_mas[0]));
        index_buf  = 6;
        retindex   = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex  += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x87)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x87; // <Индентификатор>
        ck_sum            = 0x0000;
        memcpy(&GPS_Kompas_Buf[2], &ck_sum, sizeof(ck_sum));
        index_buf  = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x48)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x48; // <Индентификатор>

        /*for test*/
        set_tasks_GPS.Big_halfaxle_A_ = 6300000.0;
        set_tasks_GPS.relation        = 290.0;
        set_tasks_GPS.D_x             = 66.66;
        set_tasks_GPS.D_y             = 66.66;
        set_tasks_GPS.D_z             = 66.66;

        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Big_halfaxle_A_, sizeof(set_tasks_GPS.Big_halfaxle_A_));
        memcpy(&GPS_Kompas_Buf[6],&set_tasks_GPS.relation,sizeof(set_tasks_GPS.relation));
        memcpy(&GPS_Kompas_Buf[10],&set_tasks_GPS.D_x,sizeof(set_tasks_GPS.D_x));
        memcpy(&GPS_Kompas_Buf[14],&set_tasks_GPS.D_y,sizeof(set_tasks_GPS.D_y));
        memcpy(&GPS_Kompas_Buf[18],&set_tasks_GPS.D_z,sizeof(set_tasks_GPS.D_z));

        ck_sum_mas[0]     = (uint16_t)(GPS_Kompas_Buf[2] & 0x00000000000000FF);
        ck_sum_mas[0]     = (uint16_t)((GPS_Kompas_Buf[3] & 0x00000000000000FF)<<8)  | ck_sum_mas[0];
        ck_sum_mas[1]     = (uint16_t)(GPS_Kompas_Buf[4] & 0x00000000000000FF);
        ck_sum_mas[1]     = (uint16_t)((GPS_Kompas_Buf[5] & 0x00000000000000FF)<<8)  | ck_sum_mas[1];

        ck_sum_mas[2]     = (uint16_t)(GPS_Kompas_Buf[6] & 0x00000000000000FF);
        ck_sum_mas[2]     = (uint16_t)((GPS_Kompas_Buf[7] & 0x00000000000000FF)<<8)  | ck_sum_mas[2];
        ck_sum_mas[3]     = (uint16_t)(GPS_Kompas_Buf[8] & 0x00000000000000FF);
        ck_sum_mas[3]     = (uint16_t)((GPS_Kompas_Buf[9] & 0x00000000000000FF)<<8)  | ck_sum_mas[3];

        ck_sum_mas[4]     = (uint16_t)(GPS_Kompas_Buf[10] & 0x00000000000000FF);
        ck_sum_mas[4]     = (uint16_t)((GPS_Kompas_Buf[11] & 0x00000000000000FF)<<8) | ck_sum_mas[4];
        ck_sum_mas[5]     = (uint16_t)(GPS_Kompas_Buf[12] & 0x00000000000000FF);
        ck_sum_mas[5]     = (uint16_t)((GPS_Kompas_Buf[13] & 0x00000000000000FF)<<8) | ck_sum_mas[5];

        ck_sum_mas[6]     = (uint16_t)(GPS_Kompas_Buf[14] & 0x00000000000000FF);
        ck_sum_mas[6]     = (uint16_t)((GPS_Kompas_Buf[15] & 0x00000000000000FF)<<8)  | ck_sum_mas[6];
        ck_sum_mas[7]     = (uint16_t)(GPS_Kompas_Buf[16] & 0x00000000000000FF);
        ck_sum_mas[7]     = (uint16_t)((GPS_Kompas_Buf[17] & 0x00000000000000FF)<<8)  | ck_sum_mas[7];

        ck_sum_mas[8]     = (uint16_t)(GPS_Kompas_Buf[18] & 0x00000000000000FF);
        ck_sum_mas[8]     = (uint16_t)((GPS_Kompas_Buf[19] & 0x00000000000000FF)<<8)  | ck_sum_mas[8];
        ck_sum_mas[9]     = (uint16_t)(GPS_Kompas_Buf[20] & 0x00000000000000FF);
        ck_sum_mas[9]     = (uint16_t)((GPS_Kompas_Buf[21] & 0x00000000000000FF)<<8)  | ck_sum_mas[9];

        ck_sum    = ck_sum_mas[0]^ck_sum_mas[1]^ck_sum_mas[2]^ck_sum_mas[3]^ck_sum_mas[4]^ck_sum_mas[5]
                   ^ck_sum_mas[6]^ck_sum_mas[7]^ck_sum_mas[8]^ck_sum_mas[9];
        memcpy(&GPS_Kompas_Buf[22],&ck_sum,sizeof(ck_sum));
        index_buf = 21;
        retindex   = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex  += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x88)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x88; // <Индентификатор>
        ck_sum            = 0x0000;
        memcpy(&GPS_Kompas_Buf[2], &ck_sum, sizeof(ck_sum));
        index_buf  = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x49)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x49; // <Индентификатор>
        set_tasks_GPS.GDop[0] = 0; // метод среднеквадратических погрешностей
        set_tasks_GPS.GDop[1] = 1; // метод геометрического фактора
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.GDop[0],sizeof(set_tasks_GPS.GDop[0]));
        ck_sum = set_tasks_GPS.GDop[0] ^ 0x0000 ^ 0x0000;
        memcpy(&GPS_Kompas_Buf[4], &ck_sum, sizeof(ck_sum));
        index_buf = 4;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x89)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x89; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2], &ck_sum, sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x4A)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x4A; // <Индентификатор>
        set_tasks_GPS.Hour =  -13;
        set_tasks_GPS.Min  =   0x0241;// 57 минут
        memcpy(&GPS_Kompas_Buf[2], &set_tasks_GPS.Hour, sizeof(set_tasks_GPS.Hour));
        memcpy(&GPS_Kompas_Buf[4], &set_tasks_GPS.Min, sizeof(set_tasks_GPS.Min));
        ck_sum = set_tasks_GPS.Hour ^ set_tasks_GPS.Min ^ 0x0000;
        memcpy(&GPS_Kompas_Buf[6],&ck_sum,sizeof(ck_sum));
        index_buf = 6;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x8A)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x4A; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x4B)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x4A; // <Индентификатор>
        set_tasks_GPS.GLO = 0x0001;
        set_tasks_GPS.GPS = 0x0002;
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.GLO,sizeof(set_tasks_GPS.GLO));
        memcpy(&GPS_Kompas_Buf[6],&set_tasks_GPS.GPS,sizeof(set_tasks_GPS.GPS));
        ck_sum_mas[0]     = (uint16_t)(GPS_Kompas_Buf[2] & 0x00000000000000FF);
        ck_sum_mas[0]     = (uint16_t)((GPS_Kompas_Buf[3] & 0x00000000000000FF)<<8)  | ck_sum_mas[0];
        ck_sum_mas[1]     = (uint16_t)(GPS_Kompas_Buf[4] & 0x00000000000000FF);
        ck_sum_mas[1]     = (uint16_t)((GPS_Kompas_Buf[5] & 0x00000000000000FF)<<8)  | ck_sum_mas[1];

        ck_sum_mas[2]     = (uint16_t)(GPS_Kompas_Buf[6] & 0x00000000000000FF);
        ck_sum_mas[2]     = (uint16_t)((GPS_Kompas_Buf[7] & 0x00000000000000FF)<<8)  | ck_sum_mas[2];
        ck_sum_mas[3]     = (uint16_t)(GPS_Kompas_Buf[8] & 0x00000000000000FF);
        ck_sum_mas[3]     = (uint16_t)((GPS_Kompas_Buf[9] & 0x00000000000000FF)<<8)  | ck_sum_mas[3];
        ck_sum            = ck_sum_mas[0]^ck_sum_mas[1]^ck_sum_mas[2]^ck_sum_mas[3];
        memcpy(&GPS_Kompas_Buf[10],&ck_sum,sizeof(ck_sum));
        index_buf = 10;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);


    }

    else if (iden == 0x8B)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x8B; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
}

    else if (iden == 0x4C)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x4A; // <Индентификатор>
        set_tasks_GPS.GLO = 0x0003;
        set_tasks_GPS.GPS = 0x0004;
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.GLO,sizeof(set_tasks_GPS.GLO));
        memcpy(&GPS_Kompas_Buf[6],&set_tasks_GPS.GPS,sizeof(set_tasks_GPS.GPS));
        ck_sum_mas[0]     = (uint16_t)(GPS_Kompas_Buf[2] & 0x00000000000000FF);
        ck_sum_mas[0]     = (uint16_t)((GPS_Kompas_Buf[3] & 0x00000000000000FF)<<8)  | ck_sum_mas[0];
        ck_sum_mas[1]     = (uint16_t)(GPS_Kompas_Buf[4] & 0x00000000000000FF);
        ck_sum_mas[1]     = (uint16_t)((GPS_Kompas_Buf[5] & 0x00000000000000FF)<<8)  | ck_sum_mas[1];

        ck_sum_mas[2]     = (uint16_t)(GPS_Kompas_Buf[6] & 0x00000000000000FF);
        ck_sum_mas[2]     = (uint16_t)((GPS_Kompas_Buf[7] & 0x00000000000000FF)<<8)  | ck_sum_mas[2];
        ck_sum_mas[3]     = (uint16_t)(GPS_Kompas_Buf[8] & 0x00000000000000FF);
        ck_sum_mas[3]     = (uint16_t)((GPS_Kompas_Buf[9] & 0x00000000000000FF)<<8)  | ck_sum_mas[3];
        ck_sum            = ck_sum_mas[0]^ck_sum_mas[1]^ck_sum_mas[2]^ck_sum_mas[3];
        memcpy(&GPS_Kompas_Buf[10],&ck_sum,sizeof(ck_sum));
        index_buf = 10;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x8C)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x8C; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x4D)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x8C; // <Индентификатор>
        set_tasks_GPS.State[0] = 0x0; // автоматический переход в дифференциальный режим
        set_tasks_GPS.State[1] = 0x0001;//решения выдаются только при наличии дифференциальных отправок
        set_tasks_GPS.State[2] = 0x0002;//дифференциальные поправки игнорируются.
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.State[0],sizeof(set_tasks_GPS.State[0])*3);
        ck_sum =  set_tasks_GPS.State[0]^set_tasks_GPS.State[1]^set_tasks_GPS.State[2];
        memcpy(&GPS_Kompas_Buf[8],&ck_sum,sizeof(ck_sum));
        index_buf = 8;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x8D)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x8D; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x4E)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x4E; // <Индентификатор>
        set_tasks_GPS.State[0] = 0x0;// автоматический переход в дифференциальный режим
        set_tasks_GPS.State[1] = 0x0001;//решения выдаются только при наличии дифференциальных отправок
        set_tasks_GPS.State[2] = 0x0002;//режим RAIM выключен
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.State[0],sizeof(set_tasks_GPS.State[0])*3);
        ck_sum =  set_tasks_GPS.State[0]^set_tasks_GPS.State[1]^set_tasks_GPS.State[2];
        memcpy(&GPS_Kompas_Buf[8],&ck_sum,sizeof(ck_sum));
        index_buf = 8;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x8E)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x8E; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x4F)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x4F; // <Индентификатор>
        set_tasks_GPS.Age = 0x0024; // 36d
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Age,sizeof(set_tasks_GPS.Age));
        ck_sum = set_tasks_GPS.Age ^ 0x0000 ^ 0x0000;
        memcpy(&GPS_Kompas_Buf[4],&ck_sum,sizeof(ck_sum));
        index_buf = 4;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x8F)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x8F; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x50)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x4F; // <Индентификатор>
        set_tasks_GPS.Max_Err_Height = 0x0005; //
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Max_Err_Height,sizeof(set_tasks_GPS.Max_Err_Height));
        ck_sum = set_tasks_GPS.Max_Err_Height ^ 0x0000 ^ 0x0000;
        memcpy(&GPS_Kompas_Buf[4],&ck_sum,sizeof(ck_sum));
        index_buf = 4;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x90)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x90; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x51)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x51; // <Индентификатор>
        set_tasks_GPS.Max_Err_plane = 0x0001;
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Max_Err_plane,sizeof(set_tasks_GPS.Max_Err_plane));
        ck_sum = set_tasks_GPS.Max_Err_plane ^ 0x0000 ^ 0x0000;
        index_buf = 4;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x91)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x91; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x52)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x52; // <Индентификатор>
        set_tasks_GPS.Max_Err_time = 0x0005;
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Max_Err_time,sizeof(set_tasks_GPS.Max_Err_time));
        ck_sum = set_tasks_GPS.Max_Err_time ^ 0x0000 ^ 0x0000;
        index_buf = 4;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x92)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x92; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x53)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x53; // <Индентификатор>
        set_tasks_GPS.Max_Time_Place = 0x0005;
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Max_Time_Place,sizeof(set_tasks_GPS.Max_Time_Place));
        ck_sum = set_tasks_GPS.Max_Time_Place ^ 0x0000 ^ 0x0000;
        index_buf = 4;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x93)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x93; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x54)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x54; // <Индентификатор>
        set_tasks_GPS.Mode_Work_Navig[0] = 0x0;// навтгатор переводится в режим диф станции
        set_tasks_GPS.Mode_Work_Navig[1] = 0x0001;//обычный режим навигатора
        set_tasks_GPS.Mode_Work_Navig[2] = 0x0002;//режим сохранения координат
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.State[0],sizeof(set_tasks_GPS.State[0])*3);
        ck_sum =  set_tasks_GPS.State[0]^set_tasks_GPS.State[1]^set_tasks_GPS.State[2];
        memcpy(&GPS_Kompas_Buf[8],&ck_sum,sizeof(ck_sum));
        index_buf = 8;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x94)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x94; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x55)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x55; // <Индентификатор>
        set_tasks_GPS.Navigate_Difinition[0] = 0x0;// режим автоматического выбора наилучшего режима
        set_tasks_GPS.Navigate_Difinition[1] = 0x0001;//режим только для выдачи временных меток, координаты навигатора постоянны
        set_tasks_GPS.Navigate_Difinition[2] = 0x0002;//определение плановых координат и выдача временных меток, высота постоянна
        set_tasks_GPS.Navigate_Difinition[3] = 0x0002;//режим определения всех составляющих координат

        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.State[0],sizeof(set_tasks_GPS.State[0])*4);
        ck_sum =  set_tasks_GPS.Navigate_Difinition[0]^set_tasks_GPS.Navigate_Difinition[1]^set_tasks_GPS.Navigate_Difinition[2]^set_tasks_GPS.Navigate_Difinition[3];
        memcpy(&GPS_Kompas_Buf[9],&ck_sum,sizeof(ck_sum));
        index_buf = 9;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x95)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x95; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x57)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x55; // <Индентификатор>
        set_tasks_GPS.Enable[0] = 0x0;//запрещает выдачу осредненных измерений
        set_tasks_GPS.Enable[1] = 0x0001;//разрешает выдачу осредненных измерений
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Enable[0],sizeof(set_tasks_GPS.Enable[0])*2);
        ck_sum = set_tasks_GPS.Enable[0] ^ set_tasks_GPS.Enable[1] ^ 0x0000;
        memcpy(&GPS_Kompas_Buf[6],&ck_sum,sizeof(ck_sum));
        index_buf = 7;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x97)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x97; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x58)
    {

        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x58; // <Индентификатор>
        /*локальное время с учётом часового пояса*/
        set_tasks_GPS.Set_Time[0] = 0x0006;   // HOUR
        set_tasks_GPS.Set_Time[1] = 0x0003;   // Minute
        set_tasks_GPS.Set_Time[2] = 0x000A;   // Sec
        set_tasks_GPS.Set_Time[3] = 0x0008;   // Day
        set_tasks_GPS.Set_Time[4] = 0x0006;   // Mounth
        set_tasks_GPS.Set_Time[5] = 0x07E5;   // Year
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Set_Time[0],sizeof(set_tasks_GPS.Set_Time[0])*6);
        ck_sum = set_tasks_GPS.Set_Time[0] ^ set_tasks_GPS.Set_Time[1] ^ set_tasks_GPS.Set_Time[2] ^ set_tasks_GPS.Set_Time[3]
               ^ set_tasks_GPS.Set_Time[4] ^ set_tasks_GPS.Set_Time[5];
        memcpy(&GPS_Kompas_Buf[15],&ck_sum,sizeof(ck_sum));
        index_buf = 15;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);


    }

    else if (iden == 0x98)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x98; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x59)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x59; // <Индентификатор>
        set_tasks_GPS.Magnetic = 10.5;
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Magnetic,sizeof(set_tasks_GPS.Magnetic));
        ck_sum_mas[0]     = (uint16_t)(GPS_Kompas_Buf[2] & 0x00000000000000FF);
        ck_sum_mas[0]     = (uint16_t)((GPS_Kompas_Buf[3] & 0x00000000000000FF)<<8)  | ck_sum_mas[0];
        ck_sum_mas[1]     = (uint16_t)(GPS_Kompas_Buf[4] & 0x00000000000000FF);
        ck_sum_mas[1]     = (uint16_t)((GPS_Kompas_Buf[5] & 0x00000000000000FF)<<8)  | ck_sum_mas[1];
        ck_sum = ck_sum_mas[0] ^ ck_sum_mas[1] ^ 0x0000;
        memcpy(&GPS_Kompas_Buf[6],&ck_sum,sizeof(ck_sum));
        index_buf = 6;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x99)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x98; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x5A)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x5A; // <Индентификатор>
        set_tasks_GPS.Delay = 7;
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Delay,sizeof(set_tasks_GPS.Delay));
        ck_sum_mas[0]     = (uint16_t)(GPS_Kompas_Buf[2] & 0x00000000000000FF);
        ck_sum_mas[0]     = (uint16_t)((GPS_Kompas_Buf[3] & 0x00000000000000FF)<<8)  | ck_sum_mas[0];
        ck_sum_mas[1]     = (uint16_t)(GPS_Kompas_Buf[4] & 0x00000000000000FF);
        ck_sum_mas[1]     = (uint16_t)((GPS_Kompas_Buf[5] & 0x00000000000000FF)<<8)  | ck_sum_mas[1];
        ck_sum = ck_sum_mas[0] ^ ck_sum_mas[1] ^0x0000;
        memcpy(&GPS_Kompas_Buf[6],&ck_sum,sizeof(ck_sum));
        index_buf = 6;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x9A)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x9A; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x5B)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x5B; // <Индентификатор>
        set_tasks_GPS.Shift = 45.0;
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Shift,sizeof(set_tasks_GPS.Shift));
        ck_sum_mas[0]     = (uint16_t)(GPS_Kompas_Buf[2] & 0x00000000000000FF);
        ck_sum_mas[0]     = (uint16_t)((GPS_Kompas_Buf[3] & 0x00000000000000FF)<<8)  | ck_sum_mas[0];
        ck_sum_mas[1]     = (uint16_t)(GPS_Kompas_Buf[4] & 0x00000000000000FF);
        ck_sum_mas[1]     = (uint16_t)((GPS_Kompas_Buf[5] & 0x00000000000000FF)<<8)  | ck_sum_mas[1];
        ck_sum = ck_sum_mas[0] ^ ck_sum_mas[1] ^0x0000;
        memcpy(&GPS_Kompas_Buf[6],&ck_sum,sizeof(ck_sum));
        index_buf = 6;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x9B)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x9B; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x5C)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x5C; // <Индентификатор>
        set_tasks_GPS.Elevation = 90;
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Elevation,sizeof(set_tasks_GPS.Elevation));
        ck_sum = set_tasks_GPS.Elevation ^ 0x0000 ^ 0x0000;
        memcpy(&GPS_Kompas_Buf[4],&ck_sum,sizeof(ck_sum));
        index_buf = 4;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x9C)
    {

        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x9C; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x5D)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x5D; // <Индентификатор>
        set_tasks_GPS.Phase = 0x03;
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Phase,sizeof(set_tasks_GPS.Phase));
        ck_sum_mas[0]     = (uint16_t)(GPS_Kompas_Buf[2] & 0x00000000000000FF);
        ck_sum_mas[0]     = (uint16_t)((GPS_Kompas_Buf[3] & 0x00000000000000FF)<<8)  | ck_sum_mas[0];
        ck_sum_mas[1]     = (uint16_t)(GPS_Kompas_Buf[4] & 0x00000000000000FF);
        ck_sum_mas[1]     = (uint16_t)((GPS_Kompas_Buf[5] & 0x00000000000000FF)<<8)  | ck_sum_mas[1];
        ck_sum = ck_sum_mas[0] ^ ck_sum_mas[1];
        memcpy(&GPS_Kompas_Buf[7],&ck_sum,sizeof(ck_sum));
        index_buf = 7;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);


    }

    else if (iden == 0x9D)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x9C; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x5E)
    {

        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x5E; // <Индентификатор>
        /*следует выбртать одну или несолько масок используя OR*/
        set_tasks_GPS.Mask_Bin[0]  = 0x00000001; //Эфемериды НКА системы "ГЛОНАСС"
        set_tasks_GPS.Mask_Bin[1]  = 0x00000002; //Эфемериды НКА системы "GPS"
        set_tasks_GPS.Mask_Bin[2]  = 0x00000004; //Альманахи НКА системы "ГЛОНАСС"
        set_tasks_GPS.Mask_Bin[3]  = 0x00000008; //Альманахи НКА системы "GPS"
        set_tasks_GPS.Mask_Bin[4]  = 0x00000020; //Параметры UTC
        set_tasks_GPS.Mask_Bin[5]  = 0x00000040; //Специальное сообщения системы "GPS"
        set_tasks_GPS.Mask_Bin[6]  = 0x00000080; //Ионосферные поправки системы "GPS"
        set_tasks_GPS.Mask_Bin[7]  = 0x00000100; //Содержимое 25 стр. 4 кадра системы "GPS"
        set_tasks_GPS.Mask_Bin[8]  = 0x00000200; //Содержимое 25 стр. 5 кадра системы "GPS"
        set_tasks_GPS.Mask_Bin[9]  = 0x00000400; //Навигационные определения
        set_tasks_GPS.Mask_Bin[10] = 0x00000800; //Состояние навигатора
        set_tasks_GPS.Mask_Bin[11] = 0x00001000; //Первичные измерения
        set_tasks_GPS.Mask_Bin[12] = 0x00002000; //Принятые диф. Поправки
        set_tasks_GPS.Mask_Bin[13] = 0x00004000; //Время и дата
        set_tasks_GPS.Mask_Bin[14] = 0x00008000; //Местоположение видимых спутников
        set_tasks_GPS.Mask_Bin[15] = 0x00010000; //Статистика
        set_tasks_GPS.Mask_Bin[16] = 0x00020000; //Первичные измерения (Новые)

        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Mask_Bin[0],sizeof(set_tasks_GPS.Mask_Bin[0]));
        ck_sum_mas[0]     = (uint16_t)(GPS_Kompas_Buf[2] & 0x00000000000000FF);
        ck_sum_mas[0]     = (uint16_t)((GPS_Kompas_Buf[3] & 0x00000000000000FF)<<8)  | ck_sum_mas[0];
        ck_sum_mas[1]     = (uint16_t)(GPS_Kompas_Buf[4] & 0x00000000000000FF);
        ck_sum_mas[1]     = (uint16_t)((GPS_Kompas_Buf[5] & 0x00000000000000FF)<<8)  | ck_sum_mas[1];

        ck_sum    = ck_sum_mas[0] ^ ck_sum_mas[1];
        memcpy(&GPS_Kompas_Buf[6],&ck_sum,sizeof(ck_sum));
        index_buf = 6;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x9E)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x9E; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x5F)
    {

        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x5F; // <Индентификатор>
        /*Ислкючающие маски*/
        set_tasks_GPS.Mask_Bin[0]  = 0x00000001; //Эфемериды НКА системы "ГЛОНАСС"
        set_tasks_GPS.Mask_Bin[1]  = 0x00000002; //Эфемериды НКА системы "GPS"
        set_tasks_GPS.Mask_Bin[2]  = 0x00000004; //Альманахи НКА системы "ГЛОНАСС"
        set_tasks_GPS.Mask_Bin[3]  = 0x00000008; //Альманахи НКА системы "GPS"
        set_tasks_GPS.Mask_Bin[4]  = 0x00000020; //Параметры UTC
        set_tasks_GPS.Mask_Bin[5]  = 0x00000040; //Специальное сообщения системы "GPS"
        set_tasks_GPS.Mask_Bin[6]  = 0x00000080; //Ионосферные поправки системы "GPS"
        set_tasks_GPS.Mask_Bin[7]  = 0x00000100; //Содержимое 25 стр. 4 кадра системы "GPS"
        set_tasks_GPS.Mask_Bin[8]  = 0x00000200; //Содержимое 25 стр. 5 кадра системы "GPS"
        set_tasks_GPS.Mask_Bin[9]  = 0x00000400; //Навигационные определения
        set_tasks_GPS.Mask_Bin[10] = 0x00000800; //Состояние навигатора
        set_tasks_GPS.Mask_Bin[11] = 0x00001000; //Первичные измерения
        set_tasks_GPS.Mask_Bin[12] = 0x00002000; //Принятые диф. Поправки
        set_tasks_GPS.Mask_Bin[13] = 0x00004000; //Время и дата
        set_tasks_GPS.Mask_Bin[14] = 0x00008000; //Местоположение видимых спутников
        set_tasks_GPS.Mask_Bin[15] = 0x00010000; //Статистика
        set_tasks_GPS.Mask_Bin[16] = 0x00020000; //Первичные измерения (Новые)

        memcpy(&GPS_Kompas_Buf,&set_tasks_GPS.Mask_Bin[0],sizeof(set_tasks_GPS.Mask_Bin[0]));
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Mask_Bin[0],sizeof(set_tasks_GPS.Mask_Bin[0]));
        ck_sum_mas[0]     = (uint16_t)(GPS_Kompas_Buf[2] & 0x00000000000000FF);
        ck_sum_mas[0]     = (uint16_t)((GPS_Kompas_Buf[3] & 0x00000000000000FF)<<8)  | ck_sum_mas[0];
        ck_sum_mas[1]     = (uint16_t)(GPS_Kompas_Buf[4] & 0x00000000000000FF);
        ck_sum_mas[1]     = (uint16_t)((GPS_Kompas_Buf[5] & 0x00000000000000FF)<<8)  | ck_sum_mas[1];

        ck_sum    = ck_sum_mas[0] ^ ck_sum_mas[1];
        memcpy(&GPS_Kompas_Buf[6],&ck_sum,sizeof(ck_sum));
        index_buf = 6;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x9F)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x9F; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x60)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x60; // <Индентификатор>
        set_tasks_GPS.Mask_NMEA[0] = 0x01; // GGA
        set_tasks_GPS.Mask_NMEA[1] = 0x02; // VTG
        set_tasks_GPS.Mask_NMEA[2] = 0x04; // ZDA
        set_tasks_GPS.Mask_NMEA[3] = 0x08; // GLL
        set_tasks_GPS.Mask_NMEA[4] = 0x10; // ALM
        set_tasks_GPS.Mask_NMEA[6] = 0x20; // GSA
        set_tasks_GPS.Mask_NMEA[7] = 0x40; // GSV
        set_tasks_GPS.Mask_NMEA[8] = 0x80; // RMC
        set_tasks_GPS.Mask_NMEA[9] = 0x8000; // PSDEN

        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Mask_NMEA[4],sizeof(set_tasks_GPS.Mask_NMEA[4]));

        ck_sum_mas[0]     = (uint16_t)(GPS_Kompas_Buf[2] & 0x00000000000000FF);
        ck_sum_mas[0]     = (uint16_t)((GPS_Kompas_Buf[3] & 0x00000000000000FF)<<8)  | ck_sum_mas[0];
        ck_sum_mas[1]     = (uint16_t)(GPS_Kompas_Buf[4] & 0x00000000000000FF);
        ck_sum_mas[1]     = (uint16_t)((GPS_Kompas_Buf[5] & 0x00000000000000FF)<<8)  | ck_sum_mas[1];

        ck_sum            = ck_sum_mas[0] ^ ck_sum_mas[1];
        memcpy(&GPS_Kompas_Buf[7],&ck_sum,sizeof(ck_sum));
        index_buf = 7;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0xA0)
    {

        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0xA0; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x61)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x61; // <Индентификатор>
        set_tasks_GPS.Mask_NMEA[0] = 0x01; // GGA
        set_tasks_GPS.Mask_NMEA[1] = 0x02; // VTG
        set_tasks_GPS.Mask_NMEA[2] = 0x04; // ZDA
        set_tasks_GPS.Mask_NMEA[3] = 0x08; // GLL
        set_tasks_GPS.Mask_NMEA[4] = 0x10; // ALM
        set_tasks_GPS.Mask_NMEA[6] = 0x20; // GSA
        set_tasks_GPS.Mask_NMEA[7] = 0x40; // GSV
        set_tasks_GPS.Mask_NMEA[8] = 0x80; // RMC
        set_tasks_GPS.Mask_NMEA[9] = 0x8000; // PSDEN

        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Mask_NMEA[4],sizeof(set_tasks_GPS.Mask_NMEA[4]));

        ck_sum_mas[0]     = (uint16_t)(GPS_Kompas_Buf[2] & 0x00000000000000FF);
        ck_sum_mas[0]     = (uint16_t)((GPS_Kompas_Buf[3] & 0x00000000000000FF)<<8)  | ck_sum_mas[0];
        ck_sum_mas[1]     = (uint16_t)(GPS_Kompas_Buf[4] & 0x00000000000000FF);
        ck_sum_mas[1]     = (uint16_t)((GPS_Kompas_Buf[5] & 0x00000000000000FF)<<8)  | ck_sum_mas[1];

        ck_sum            = ck_sum_mas[0] ^ ck_sum_mas[1];
        memcpy(&GPS_Kompas_Buf[7],&ck_sum,sizeof(ck_sum));
        index_buf = 7;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0xA1)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0xA0; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x62)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x61; // <Индентификатор>
        set_tasks_GPS.State_Information[0] = 0; // Выключить
        set_tasks_GPS.State_Information[1] = 1; // Включить
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.State_Information[0],sizeof(set_tasks_GPS.State_Information[0]));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0xA2)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0xA2; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x63)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x63; // <Индентификатор>
        set_tasks_GPS.Num = 0x0666; // номер для станции
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Num,sizeof(set_tasks_GPS.Num));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0xA3)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0xA3; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x64)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x64; // <Индентификатор>
        set_tasks_GPS.State_Filtering_Navig[0] = 0; // Выключить
        set_tasks_GPS.State_Filtering_Navig[1] = 1; // Включить
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.State_Filtering_Navig[0],sizeof(set_tasks_GPS.State_Filtering_Navig[0]));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0xA4)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0xA4; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x65)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x65; // <Индентификатор>
        set_tasks_GPS.Sigma = 0x01; // как считать параметр?
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Sigma,sizeof(set_tasks_GPS.Sigma));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0xA5)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0xA5; // <Индентификатор>
        ck_sum = 0x0000;
        memcpy(&GPS_Kompas_Buf[2],&ck_sum,sizeof(ck_sum));
        index_buf = 2;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);
    }

    else if (iden == 0x66)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x65; // <Индентификатор>
        set_tasks_GPS.Choose_NKA[0] = 0x01; // если используется НКА системы GPS
      //  set_tasks_GPS.Choose_NKA = 0x00; //если используется НКА системы ГЛОНАСС
        set_tasks_GPS.Number_NKA = 24;  // принимает значение от 1 до 24 для ГЛОНАСС, для НКА системы GPS
        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Choose_NKA[0],sizeof(set_tasks_GPS.Choose_NKA[0]));
        memcpy(&GPS_Kompas_Buf[4],&set_tasks_GPS.Number_NKA,sizeof(set_tasks_GPS.Number_NKA));
        index_buf = 4;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }


    else if (iden == 0x67)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x67; // <Индентификатор>
        set_tasks_GPS.number_packet_binary[0] = 0x0020; // эфемериды ГЛОНАСС
        set_tasks_GPS.number_packet_binary[1] = 0x0021; // эфемериды GPS
        set_tasks_GPS.number_packet_binary[2] = 0x0022; // альманах ГЛОНАСС
        set_tasks_GPS.number_packet_binary[3] = 0x0023; // альманах GPS
        set_tasks_GPS.number_packet_binary[4] = 0x0025; // параметры UTC
        set_tasks_GPS.number_packet_binary[5] = 0x0026; // специальное сообщение GPS
        set_tasks_GPS.number_packet_binary[6] = 0x0027; // ионосферные поправки GPS
        set_tasks_GPS.number_packet_binary[7] = 0x0028; // содержимое 25-ой стр. 4-го кадра альманаха GPS
        set_tasks_GPS.number_packet_binary[8] = 0x0029; // содержимое 25-ой стр. 5-го кадра альманаха GPS
        set_tasks_GPS.number_packet_binary[9] = 0x002A; // навигационные определения
        set_tasks_GPS.number_packet_binary[10]= 0x002B; // состояние навигатора
        set_tasks_GPS.number_packet_binary[11]= 0x002C; // первичные измерения
        set_tasks_GPS.number_packet_binary[12]= 0x002D; // принятые диф. поправки
        set_tasks_GPS.number_packet_binary[13]= 0x002E; // дата и время
        set_tasks_GPS.number_packet_binary[14]= 0x002F; // видимые НКА
        set_tasks_GPS.number_packet_binary[15]= 0x0030; // накопленная статистика
        set_tasks_GPS.number_packet_binary[16]= 0x0031; // первичные измерения (Новые)

        set_tasks_GPS.parametr = 0x1; // автоматическая выдача пакета разрешена

        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.number_packet_binary,sizeof(set_tasks_GPS.number_packet_binary));
        memcpy(&GPS_Kompas_Buf[4],&set_tasks_GPS.number_packet_binary,sizeof(set_tasks_GPS.number_packet_binary));
        index_buf = 4;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);


    }

    else if (iden == 0x68)
    {
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x67; // <Индентификатор>
        GPS_Kompas_Buf[0] = 0x10; // <Флаг>
        GPS_Kompas_Buf[1] = 0x61; // <Индентификатор>
        set_tasks_GPS.Mask_NMEA[0] = 0x01; // GGA
        set_tasks_GPS.Mask_NMEA[1] = 0x02; // VTG
        set_tasks_GPS.Mask_NMEA[2] = 0x04; // ZDA
        set_tasks_GPS.Mask_NMEA[3] = 0x08; // GLL
        set_tasks_GPS.Mask_NMEA[4] = 0x10; // ALM
        set_tasks_GPS.Mask_NMEA[6] = 0x20; // GSA
        set_tasks_GPS.Mask_NMEA[7] = 0x40; // GSV
        set_tasks_GPS.Mask_NMEA[8] = 0x80; // RMC
        set_tasks_GPS.Mask_NMEA[9] = 0x8000; // PSDEN

        memcpy(&GPS_Kompas_Buf[2],&set_tasks_GPS.Mask_NMEA[4],sizeof(set_tasks_GPS.Mask_NMEA[4]));

        ck_sum_mas[0]     = (uint16_t)(GPS_Kompas_Buf[2] & 0x00000000000000FF);
        ck_sum_mas[0]     = (uint16_t)((GPS_Kompas_Buf[3] & 0x00000000000000FF)<<8)  | ck_sum_mas[0];
        ck_sum_mas[1]     = (uint16_t)(GPS_Kompas_Buf[4] & 0x00000000000000FF);
        ck_sum_mas[1]     = (uint16_t)((GPS_Kompas_Buf[5] & 0x00000000000000FF)<<8)  | ck_sum_mas[1];

        ck_sum            = ck_sum_mas[0] ^ ck_sum_mas[1];
        memcpy(&GPS_Kompas_Buf[7],&ck_sum,sizeof(ck_sum));
        index_buf = 7;
        retindex  = search_repeat_data(&GPS_Kompas_Buf[2],index_buf,READ_OFF);
        retindex += 2; // учитываю элементы флаг и индентификатор
        GPS_Kompas_Buf[retindex] = 0x10; // <Флаг>
        retindex += 1;
        GPS_Kompas_Buf[retindex] = 0x03; // <Прк>
        retindex += 1;
        size_Buf = sizeof(GPS_Kompas_Buf[0])*retindex / sizeof(uint8_t);
        UART_write(uartHnd_GPS, (void *)&GPS_Kompas_Buf[0], size_Buf);

    }

    else if (iden == 0x69)
    {
        ;;
    }
}

unsigned int search_repeat_data(uint8_t *array_in,const int val_in_buf,uint8_t read_flag)
{
    size_t  size_array;
    unsigned int counter, index_incr;
    uint8_t  fuck_dat = 0x10; // значение для поиска во входном массиве
    uint8_t  mas_out[100];
    memset(&mas_out[0],0x0,sizeof(mas_out[0])*100);
    index_incr = 0; // возвращаем индекс последнего элемента преобразованного массива

    size_array = sizeof(array_in[0])*val_in_buf / sizeof(uint8_t);
    if (read_flag != 1)     // поиск во входном массиве 10h и дублирование 10h
    {
        for (counter = 0; counter < size_array; counter++)
        {
            mas_out[index_incr] = array_in[counter];
            if (array_in[counter] == fuck_dat)
            {
                index_incr++;
                mas_out[index_incr] = fuck_dat;
                index_incr++;
                mas_out[index_incr] = fuck_dat;
                index_incr++;
            }
            else
            {
                index_incr++;
            }
        }
        memcpy(&array_in[0],&mas_out, sizeof(&array_in[0])*index_incr);
    }
    else                  // поиск повторяющейся посылки 10h, дублирование отсекается
    {
        unsigned int ind2;
        index_incr = 0; ind2 = 0;

        for (counter = 0; counter < size_array; counter++)
        {

            if ((array_in[ind2] == fuck_dat)&&(array_in[ind2+1]==fuck_dat))
            {

                mas_out[index_incr] = array_in[ind2];
                ind2 = ind2 + 3;
                index_incr++;

            }
            else
            {
                mas_out[index_incr] = array_in[ind2];
                index_incr++;
                ind2++;

            }
        }
        memcpy(&array_in[0],&mas_out, sizeof(&array_in[0])*index_incr);
    }


    return index_incr;
}

