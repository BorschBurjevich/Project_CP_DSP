#include "GPS_Kompas.h"

// установка приёмника навигации
int8_t set_recive_trs_GPS_Kompas(UART_Handle uartHnd_1)
{
    int8_t status = 0;
    uint16_t npcket;
    int count;

    npcket = 0x40;           // Установка параметров обмена по последовательным портам
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x80;           // Запрос параметров обмена
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x41;           // Установка системы координат и проекции
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x81;           // Запрос установленной системы координат
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x42;           // Установка используемой системы спутников
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x82;           // Запрос используемой системы спутников
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x43;           // Установка управляющего протокола
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x83;           // Запрос управляющего протокола
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x44;           // Установка информационного протокола
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x84;           // Запрос информационного протокола
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x45;           // Установка опорной точки
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x85;           // Запрос опорной точки
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x46;           // Установка алгоритма выдачи амплитуды сигнала
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x86;           // Запрос текущего алгоритма выдачи амплитуды сигнала
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x47;           // Установка алгоритма выдачи метки времени
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x87;           // Запрос алгоритма выдачи метки времени
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x48;           // Установка параметров системы координат пользователя
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x88;           // Запрос параметров системы координат пользователя
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x49;           // Установка алгоритма оценки погрешности навигационных определений
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x89;           // Запрос алгоритма оценки погрешности
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x4A;           // Установка поправки на локальное время
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x8A;           // Запрос поправки на локальное время
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x4B;           // Включить в разрешенные к использованию спутники
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x8B;           // Запрос разрешенных к использованию спутников
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x4C;           // Исключить из разрешенных к использованию спутников
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x8C;           // Запрос оставшихся разрешённых к использованию
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x4D;           // Установить статус использования дифференциальных поправок
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x8D;           // Запрос текущего статуса использования дифференциальных поправок
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x4E;           // Установить статус использования RAIM
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x8E;           // Запрос статуса использования RAIM
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x4F;           // Установить максимально допустимую старость дифференциальных поправок
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x8F;           // Запрос максимально допустимой старости диф поправок
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x50;           // Установить макс. допустимые ошибки по высоте
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x90;           // Запрос предельной ошибки определения высоты
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x51;           // Установить макс. допустимые ошибки в плане
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x91;           // запроса предельной ошибки планового решения
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x52;           // Установить макс. допустимые ошибки по времени
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x92;           // Запрос макс. допустимых ошибок по времени
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x53;           // Установить максимальное время счисление пути без навигационных решений
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x93;           // Запрос максимального времени счисления пути без навиг решений
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x54;           // Установить режим работы навигатора
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x94;           // Запрос режима работы навигатора
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x55;           // Установить режим навигационных определений
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x95;           // Запрос режима навигационных определений
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x57;           // Установить разрешение выдачи осредненных измерений
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x97;           // Запрос разрешения выдачи осреднённых измерений
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x58;           // Установить текущее время и дату
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x98;           // Запрос текущего времени и даты
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x59;           // Установить магнитное склонение
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x99;           // Запрос магнитного склонения
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x5A;           // Установить задержку в ВЧ тракте
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x9A;           // Запрос задержки в ВЧ тракте
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x5B;           //Установить сдвиг метки времени
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x9B;           // Запрос величины сдвига
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x5C;           // Установить минимальный угол места в градусах
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x9C;           // Запрорс минимального угла места
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x5D;           // устанавливает начальную фазу генератора ПСП
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x9D;           // Запрос текущей начальной фазы ПСП
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x5E;           // Включить в выдаваемые автоматически бинарные пакеты
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x9E;           // Запрос маски, разрешенных к выдаче бинарных пакетов
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x5F;           // Исключить из выдаваемых автоматически бинарных пакетов
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x9F;           // маска пакетов, разрешенных к выдаче
    set_tasks_GPS_Kompas(npcket,uartHnd_1);

    npcket = 0x60;           // Включить в выдаваемые автоматически NMEA-посылки
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0xA0;             // Запрос разрешенных к выдаче NMEA пакетов производится
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x61;           // Исключить из выдаваемых автоматически NMEA
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0xA1;           // маска разрешенных к выдаче NMEA
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x62;           // Включить или выключить накопление статистической информации
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0xA2;           // Запрос параметра накопления статистической информации
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x63;           // Установить номер станции
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0xA3;           // Запрос установленного номера станции
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x64;           // Включить или выключить фильтрацию навигационных определений
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0xA4;           // Запрос состояния фильтрации навигационных определений
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x65;           // Установить эквивалент среднеквадратической ошибки псевдодальности
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0xA5;           // Запрос среднеквадратической ошибки псевдодальности
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x66;           // Разрешить/запретить использование НКА
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x67;           // Разрешить/запретить автоматическую выдачу бинарного пакета
    set_tasks_GPS_Kompas(npcket,uartHnd_1);

    npcket = 0x68;           // Разрешить/запретить автоматическую выдачу NMEA-посылки
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x69;           // Передать в навигатор альманах Глонасса
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x6A;           // Передать в навигатор альманах GPS
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x6F;           // Установить поправку Глонасса
    set_tasks_GPS_Kompas(npcket,uartHnd_1);
    npcket = 0x72;           // Установить темп измерений
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
        // при первом включении, выполняется установка исп. спутниковой системы
        data[0] = 0x02; // совместно система ГЛОНАСС(потреб точность) и система GPS
        data[1] = 0x00;

        GPS_Kompas_Buf[cntr_Write] = 0x10; // <Флаг>
        cntr_Write++;
        GPS_Kompas_Buf[cntr_Write] = 0x42; // <Индентификатор>
        cntr_Write++;
        ck_sum = 0x0002; // сумма по модулю 2 (XOR)
        GPS_Kompas_Buf[cntr_Write] = data[0];
        cntr_Write++;
        GPS_Kompas_Buf[cntr_Write] = data[1];
        cntr_Write++;
        GPS_Kompas_Buf[cntr_Write] = (uint8_t)(ck_sum & 0x00ff);
        GPS_Kompas_Buf[cntr_Write] = (uint8_t)((ck_sum & 0xff00)>>8);
        GPS_Kompas_Buf[cntr_Write] = 0x10; // <Флаг>
        cntr_Write++;
        GPS_Kompas_Buf[cntr_Write] = 0x03; // <Прк>
        cntr_Write++;
        UART_write(RNM_handle, (void *)&GPS_Kompas_Buf[0], cntr_Write);
        cntr_Write = 0;
    }
    InitSP(RNM_handle);
}

///  функц-ия разрешение  автоматической выдачи бинарного пакета
void InitSP(UART_Handle local_RNM)
{

    uint32_t cntr_Write = 0;
/// разрешает автоматическую выдачу даты и времени///
    GPS_Kompas_Buf[cntr_Write] = 0x10; // <Флаг>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x67; // <Индентификатор>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x2E; // дата и время
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x01;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x2F; // кс
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x10;// <Флаг>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x03;// <Прк>
    cntr_Write++;
    UART_write(local_RNM, (void *)&GPS_Kompas_Buf[0], cntr_Write);
    cntr_Write = 0;

    /////  разрешает автоматическую выдачу навигационных определений
    GPS_Kompas_Buf[cntr_Write] = 0x10; // <Флаг>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x67; // <Индентификатор>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x2A; // навигационные определения
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x01; // разрешение автоматич выдачи
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x2B; // контрольная сумма
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x10;// <Флаг>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x03;// <Прк>
    cntr_Write++;
    UART_write(local_RNM, (void *)&GPS_Kompas_Buf[0], cntr_Write);
    cntr_Write = 0;

    /// возможно потребуется пакет для установки сситемы координат и проекции

    GPS_Kompas_Buf[cntr_Write] = 0x10; // <Флаг>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x41; // <Индентификатор>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x01; // номер системы координат (СК-42)
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00; // выбор прямоугольный проекции
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x01; // контрольная сумма
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x10;// <Флаг>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x03;// <Прк>
    cntr_Write++;
    UART_write(local_RNM, (void *)&GPS_Kompas_Buf[0], cntr_Write);
    cntr_Write = 0;

}

void InitSSys(UART_Handle local_RNM)
{
    ;;
}

// команда сохранения настройки приёмника навигации, в ответ навигатор возвращает принятый пакет
void Save_changes(UART_Handle local_RNM)
{
    uint32_t cntr_Write = 0;

    GPS_Kompas_Buf[cntr_Write] = 0x10; // <Флаг>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0xD0; // <Индентификатор>
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x01;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x01; // КС
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x00;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x10;
    cntr_Write++;
    GPS_Kompas_Buf[cntr_Write] = 0x03;
    cntr_Write++;

}

