#include <ti/sysbios/knl/Semaphore.h>
#include <ti/ndk/inc/netmain.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/BIOS.h>
#include "Lorandit_functions/Cod_com.h"
#include "Lorandit_functions/Structs.h"

SOCKET              arm;
SOCKET              skl;
extern              Semaphore_Handle semen;
extern struct       sockaddr_in sin1;


int arm_handler( SOCKET s, uint32_t unused )
{
    struct timeval to;
    int I;
    //void *hBuffer;
    (void)unused;

    int tmp = sizeof( sin1 );
    uint8_t  *data = pBuf;
    uint32_t *data_file = file_boot_loader;
    uint32_t Len = 0;                           // длина сообщения в байтах
    uint32_t iCount = 0;

    // Configure our socket timeout to be 2 seconds
    to.tv_sec = 2;
    to.tv_usec = 0;
    NDK_setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof(to) );
    NDK_setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to) );

    I = 1;
    NDK_setsockopt( s, IPPROTO_TCP, NDK_TCP_NOPUSH, &I, 4 );
    arm = s;

    while(1)
    {
        //прием длины сообщения и кода комманды
        //I = (int)NDK_recvnc( s, (void **)&data, 0, &hBuffer );
        I = (int)NDK_recvfrom( s, data, (sizeof(hat.n_bytes)+sizeof(hat.command)), 0, (struct sockaddr *)&sin1, &tmp);
        if(I > 0)
        {
            memcpy(&hat.n_bytes, &data[0], sizeof(hat.n_bytes));//длина сообщения
            memcpy(&hat.command, &data[sizeof(hat.n_bytes)], sizeof(hat.command));//код команды
            Len = hat.n_bytes - sizeof(hat.command);

            if (hat.command == boot_)
            {
                while(Len > 0)
                {
                    iCount = (int)NDK_recvfrom( s, data_file, Len, 0, (struct sockaddr *)&sin1, &tmp);
                    Len -= iCount;

                }
            }

            else
            {
                while(Len > 0)
                {
                    iCount = (int)NDK_recvfrom( s, data, Len, 0, (struct sockaddr *)&sin1, &tmp);
                    Len -= iCount;

                }
            }

        }

        if(I == 0)
        {
            break;
        }

    }
    fdClose(s);

    // Return "0" since we closed the socket
    return(1);
}



