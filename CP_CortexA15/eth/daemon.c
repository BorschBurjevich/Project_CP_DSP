/*
 * ======== daemon.c ========
 *
 * This file implements a simple network daemon.
 *
 */

#include <string.h>
#include <stdint.h>

#include <eth/netmain.h>
#include <_stack.h>
#include <xdc/std.h>

/**********************************************************************
 *************************** Local Definitions ************************
 **********************************************************************/

#define DAEMON_MAXRECORD  12

/**********************************************************************
 *************************** Local Structures *************************
 **********************************************************************/

/** 
 * @brief 
 *  The structure describes the Daemon Entry.
 */
typedef struct 
{
    /**
     * @brief   Type of socket. For V4 this could be SOCK_STREAM, 
     * SOCK_STREAMNC, SOCK_DGRAM But for V6 this can only be 
     * SOCK_STREAM and SOCK_DGRAM. 
     */
    uint32_t    Type;

    /**
     * @brief   Local V4 Address to which the socket is bound to
     *  For most cases this is passed as INADDR_ANY
     */
    uint32_t    LocalAddress;

#ifdef _INCLUDE_IPv6_CODE
    /**
     * @brief   Local V6 address to which the socket is bound to
     *  For most cases this is passed as IPV6_UNSPECIFIED_ADDRESS.
     */
    IP6N        LocalV6Address;
#endif

    /**
     * @brief   Local Port to which the daemon will be bound to
     * This cannot be NULL.
     */
    uint32_t    LocalPort;

    /**
     * @brief   Call back function which is invoked when data is
     * received on the registered daemon.
     */
    int         (*pCb)(SOCKET,uint32_t);
    
    /**
     * @brief   Priority of the child task.
     */
    uint32_t    Priority;

    /**
     * @brief   Stack size of the child task.
     */
    uint32_t    StackSize;

    /**
     * @brief   Argument which is to passed to the call back routine.
     */
    uint32_t    Argument;

    /**
     * @brief   Maximum number of tasks which can be spawned to handle
     * the daemon.
     */
    uint32_t    MaxSpawn;

    /**
     * @brief   Internal socket created by the daemon for use.
     */
    SOCKET      s;

    /**
     * @brief   Number of tasks spawned to handle the daemon.
     */    
    uint32_t    TasksSpawned;

    /**
     * @brief   Pointer to children.
     */
    struct _child *pC;
} DREC;

/** 
 * @brief 
 *  The structure describes the data instance for daemon child task.
 */
typedef struct _child 
{
    /**
     * @brief   Pointer to previous child or NULL
     */
    struct _child *pPrev;

    /**
     * @brief   Pointer to next child or NULL
     */    
    struct _child *pNext;

    /**
     * @brief   Handle to the child task
     */
    void     *hTask;

    /**
     * @brief   Socket on which the child task is operating.
     */
    SOCKET      s;

    /**
     * @brief   Flag which determine if the 'child' socket is to be closed
     * or not.
     */
    int         closeSock;
} CHILD;

/**********************************************************************
 *************************** GLOBAL Structures ************************
 **********************************************************************/

static DREC drec[DAEMON_MAXRECORD];
static FDPOLLITEM pollitem[DAEMON_MAXRECORD];

static void *hDTask = 0;       /* Handle to the main daemon task thread */
static void *hDSem = 0;        /* Handle our exclusion semaphore */
static uint32_t RequestedRecords; /* Number of initialized records in drec */

static void daemon();
static void dchild( DREC *pd, CHILD *pc );
extern void Vip_send(UArg arg0, UArg arg1);

/**********************************************************************
 *************************** DAEMON Functions *************************
 **********************************************************************/

/** 
 *  @b Description
 *  @n  
 *      The function creates a V4 Daemon.
 *
 *  @param[in]  Type
 *      This is the type of socket being opened through the daemon. In the case
 *      of IPv4 all socket types are supported i.e. SOCK_STREAM, SOCK_STREAMNC and
 *      SOCK_DGRAM.
 *  @param[in]  LocalAddress
 *      This is the Local Address to which the socket will be bound to.
 *      In most cases this is typically passed as NULL.
 *  @param[in]  LocalPort
 *      This is the Local Port to serve (cannot be NULL)
 *  @param[in]  pCb
 *      Call back function which is to be invoked.
 *  @param[in]  Priority
 *      Priority of new task to create for callback function 
 *  @param[in]  StackSize
 *      Stack size of new task to create for callback function
 *  @param[in]  Argument
 *      Argument (besides socket) to pass to callback function
 *  @param[in]  MaxSpawn
 *      Maximum number of callback function instances (must be 1 for UDP)
 *
 *  @retval
 *      Success - Handle to the spawned Daemon.
 *  @retval
 *      Error   - 0
 */
void *DaemonNew( uint32_t Type, uint32_t LocalAddress, uint32_t LocalPort,
                  int (*pCb)(SOCKET,uint32_t), uint32_t Priority,
                  uint32_t StackSize, uintptr_t Argument, uint32_t MaxSpawn )
{
    int i;
    DREC *pdr = 0;

    /* Sanity check the arguments */
    if( Type==SOCK_DGRAM )
        MaxSpawn=1;
    else if( Type!=SOCK_STREAM && Type!=SOCK_STREAMNC )
        return(0);

    if( !LocalPort || !pCb || Priority<1 || Priority>15 || !StackSize || !MaxSpawn )
        return(0);

    /* We'll borrow the stack's kernel mode for a temp exclusion method */
    llEnter();
    if( !hDSem )
    {
        hDSem = SemCreate( 1 );
        memset( drec, 0, sizeof(drec) );
        RequestedRecords = 0;
    }
    llExit();

    /* At this point we must have a semaphore */
    if( !hDSem )
        return(0);

    /* Enter our own lock */
    SemPend( hDSem, SEM_FOREVER );

    /* Scan the list for a free slot */
    for( i=0; i<DAEMON_MAXRECORD; i++ )
        if( !drec[i].Type && !drec[i].TasksSpawned )
            break;

    /* Break out if no free records */
    if(i==DAEMON_MAXRECORD)
        goto errorout;

    /* Build the new record */
    pdr = &drec[i];
    pdr->Type         = Type;
    pdr->LocalAddress = LocalAddress;
    pdr->LocalPort    = LocalPort;
    pdr->pCb          = pCb;
    pdr->Priority     = Priority;
    pdr->StackSize    = StackSize;
    pdr->Argument     = Argument;
    pdr->MaxSpawn     = MaxSpawn;
    pdr->s            = INVALID_SOCKET;

    /* If the Deamon task exists, ping it, otherwise create it */
    if( hDTask )
        fdSelectAbort( hDTask );
    else
    {
        hDTask = TaskCreate(daemon, "daemon", Priority, StackSize, 0, 0, 0);
        if( hDTask )
            fdOpenSession(hDTask);
        else
        {
            pdr->Type = 0;
            pdr = 0;
            goto errorout;
        }
    }

    RequestedRecords++;

errorout:
    /* Exit our lock */
    SemPost( hDSem );
    return( pdr );
}

/** 
 *  @b Description
 *  @n  
 *      The function is to used cleanup a previously created V4 daemon.
 *
 *  @param[in]  hEntry
 *      This is the handle to the daemon which was previously created 
 *      using DaemonNew
 *  @sa
 *      DaemonNew
 *
 *  @retval
 *      Not Applicable.
 */
void DaemonFree( void *hEntry )
{
    DREC *pdr = (DREC *)hEntry;
    CHILD *pc;

    /* At this point we must have a semaphore */
    if( !hDSem )
        return;

    /* Enter our own lock */
    SemPend( hDSem, SEM_FOREVER );

    /* Sanity check */
    if( pdr->Type!=SOCK_STREAM && pdr->Type!=SOCK_STREAMNC && pdr->Type!=SOCK_DGRAM )
        goto errorout;

    /* Clear the record */
    pdr->Type = 0;
    RequestedRecords--;

    /* Close the socket session of all children. This will */
    /* cause them to eventually fall out of their code and */
    /* close their sockets */
    pc = pdr->pC;
    while( pc )
    {
        if( pc->hTask )
        {
            fdCloseSession( pc->hTask );
            pc->hTask = 0;
        }
        pc = pc->pNext;
    }

    /* Close the socket (this will wake anyone who's using it) */
    if( pdr->s != INVALID_SOCKET )
    {
        fdClose( pdr->s );
        pdr->s = INVALID_SOCKET;
    }

    /* If there are no more records, close the daemon task's */
    /* file descriptor session. That will cause it to error */
    /* out and remove itself */
    if( !RequestedRecords )
    {
        fdCloseSession( hDTask );
        hDTask = 0;
    }

errorout:
    /* Exit our lock */
    SemPost( hDSem );
}

struct sockaddr_in sin1;

static void daemon()
{
    int                i,closeSock;
    SOCKET             tsock;
    CHILD              *pc;
    //struct sockaddr_in sin1;
    /* Enter our lock */
    SemPend( hDSem, SEM_FOREVER );

    for(;;)
    {
        /* Create any socket that needs to be created */
        for( i=0; i<DAEMON_MAXRECORD; i++ )
        {
            if( drec[i].Type && drec[i].s == INVALID_SOCKET )
            {
                /* Create UDP or TCP as needed */
                if( drec[i].Type == SOCK_DGRAM )
                    drec[i].s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                else
                    drec[i].s = socket(AF_INET, drec[i].Type, IPPROTO_TCP);

                /* If the socket was created, bind it */
                if( drec[i].s != INVALID_SOCKET )
                {
                    /* Bind to the specified Server port */
                    memset( &sin1, 0, sizeof(struct sockaddr_in) );
                    sin1.sin_family      = AF_INET;
                    sin1.sin_addr.s_addr = drec[i].LocalAddress;
                    sin1.sin_port        = NDK_htons(drec[i].LocalPort);

                    if( bind( drec[i].s,(struct sockaddr *)&sin1, sizeof(sin1) ) < 0 )
                    {
                        fdClose( drec[i].s );
                        drec[i].s = INVALID_SOCKET;
                    }
                }

                /* If the socket is bound and TCP, start listening */
                if( drec[i].s != INVALID_SOCKET && drec[i].Type != SOCK_DGRAM )
                {
                    if( listen( drec[i].s, drec[i].MaxSpawn ) < 0 )
                    {
                        fdClose( drec[i].s );
                        drec[i].s = INVALID_SOCKET;
                    }
                }
            }

            /* Update the fdPoll array */
            pollitem[i].fd = drec[i].s;
            if( drec[i].Type && drec[i].TasksSpawned < drec[i].MaxSpawn )
                pollitem[i].eventsRequested = POLLIN;
            else
                pollitem[i].eventsRequested = 0;
        }

        /* Leave our lock */
        SemPost( hDSem );

        /* Poll with a timeout of 10 second - to try and catch */
        /* synchronization error */
        if( fdPoll( pollitem, DAEMON_MAXRECORD, 10000 ) == SOCKET_ERROR )
            break;

        /* Enter our lock */
        SemPend( hDSem, SEM_FOREVER );

        /* Spawn tasks for any active sockets */
        for( i=0; i<DAEMON_MAXRECORD; i++ )
        {
            /* If no poll results or the drec has been freed, skip it */
            if( !pollitem[i].eventsDetected || !drec[i].Type )
                continue;

            /* If the socket is invalid, close it */
            if( pollitem[i].eventsDetected & POLLNVAL )
            {
                fdClose( drec[i].s);
                drec[i].s = INVALID_SOCKET;
                continue;
            }

            if( pollitem[i].eventsDetected & POLLIN )
            {
                if( drec[i].Type == SOCK_DGRAM )
                {
                    tsock = drec[i].s;
                    closeSock = 0;
                }
                else
                {
                    tsock = accept( drec[i].s, 0, 0 );
                    closeSock = 1;
                }

                if( tsock != INVALID_SOCKET )
                {
                    /* Create a record to track this task */
                    pc = mmAlloc( sizeof(CHILD) );
                    if( !pc )
                        goto spawnComplete;

                    /* Create the task */
                    pc->hTask = TaskCreate( dchild, "dchild",
                            drec[i].Priority, drec[i].StackSize,
                            (uintptr_t)&drec[i], (uintptr_t)pc, 0);

                    if( !pc->hTask )
                    {
                        mmFree( pc );
                        goto spawnComplete;
                    }

                    /* Open a socket session for the child task */
                    fdOpenSession( pc->hTask );

                    /* Fill in the rest of the child record */
                    pc->closeSock = closeSock;
                    pc->s = tsock;

                    /* Now we won't close the socket here */
                    closeSock = 0;

                    /* Link this record onto the DREC */
                    drec[i].TasksSpawned++;
                    pc->pPrev = 0;
                    pc->pNext = drec[i].pC;
                    drec[i].pC = pc;
                    if( pc->pNext )
                        pc->pNext->pPrev = pc;

spawnComplete:
                    /* If there was an error, we may need to close the socket */
                    if( closeSock )
                        fdClose( tsock );
                }
            }
        }
    }

    TaskExit();
}

static void dchild( DREC *pdr, CHILD *pc )
{
    int rc;

    /* Get the lock to synchronize with parent */
    SemPend( hDSem, SEM_FOREVER );
    SemPost( hDSem );

    /* This function returns "1" if the socket is still open, */
    /* and "0" is the socket has been closed. */
    rc = pdr->pCb( pc->s,  pdr->Argument );

    /* Close the socket if we need to */
    /* We do this before we get the lock so if the socket */
    /* uses LINGER, we don't hold everyone up */
    if( rc && pc->closeSock )
        fdClose( pc->s );

    /* Get our lock */
    SemPend( hDSem, SEM_FOREVER );

    /* Close the socket session (if open) */
    if( pc->hTask )
        fdCloseSession( pc->hTask );

    /* Remove our record from the DREC */
    if( pc->pNext )
        pc->pNext->pPrev = pc->pPrev;
    if( !pc->pPrev )
        pdr->pC = pc->pNext;
    else
        pc->pPrev->pNext = pc->pNext;
    pdr->TasksSpawned--;

    /* Free our record */
    mmFree( pc );

    /* Kick the parent thread */
    if( hDTask )
        fdSelectAbort( hDTask );

    /* Release the lock */
    SemPost( hDSem );

    TaskExit();
}

#ifdef _INCLUDE_IPv6_CODE

static DREC       drec6[DAEMON_MAXRECORD];
static FDPOLLITEM pollitem6[DAEMON_MAXRECORD];
static void   *hDTask6 = 0;     /* Handle to the main V6 daemon task thread */
static void   *hDSem6 = 0;      /* Handle to V6 exclusion semaphore */
static uint32_t  RequestedRecords6; /* Number of initialized records in drec6 */

static void dchild6( DREC *pdr, CHILD *pc )
{
    int rc;

    /* Get the lock to synchronize with parent */
    SemPend( hDSem6, SEM_FOREVER );
    SemPost( hDSem6 );

    /* This function returns "1" if the socket is still open, */
    /* and "0" is the socket has been closed. */
    rc = pdr->pCb( pc->s,  pdr->Argument );

    /* Close the socket if we need to */
    /* We do this before we get the lock so if the socket */
    /* uses LINGER, we don't hold everyone up */
    if( rc && pc->closeSock )
        fdClose( pc->s );

    /* Get our lock */
    SemPend( hDSem6, SEM_FOREVER );

    /* Close the socket session (if open) */
    if( pc->hTask )
        fdCloseSession( pc->hTask );

    /* Remove our record from the DREC */
    if( pc->pNext )
        pc->pNext->pPrev = pc->pPrev;
    if( !pc->pPrev )
        pdr->pC = pc->pNext;
    else
        pc->pPrev->pNext = pc->pNext;
    pdr->TasksSpawned--;

    /* Free our record */
    mmFree( pc );

    /* Kick the parent thread */
    if( hDTask6 )
        fdSelectAbort( hDTask6 );

    /* Release the lock */
    SemPost( hDSem6 );

    TaskExit();
}

static void daemon6()
{
    int                   i,closeSock;
    struct sockaddr_in6   sin1;
    SOCKET                tsock;
    CHILD*                pc;

    /* Enter our lock */
    SemPend( hDSem6, SEM_FOREVER );

    for(;;)
    {
        /* Create any socket that needs to be created */
        for( i=0; i<DAEMON_MAXRECORD; i++ )
        {
            if( drec6[i].Type && drec6[i].s == INVALID_SOCKET )
            {
                /* Create UDP or TCP as needed */
                if( drec6[i].Type == SOCK_DGRAM )
                    drec6[i].s = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
                else
                    drec6[i].s = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

                /* If the socket was created, bind it */
                if( drec6[i].s != INVALID_SOCKET )
                {
                    /* Bind to the specified Server port */
                    memset( &sin1, 0, sizeof(struct sockaddr_in) );
                    sin1.sin6_family     = AF_INET6;
                    memcpy((void *)&sin1.sin6_addr,(void *)&drec6[i].LocalAddress, sizeof(struct in6_addr));
                    sin1.sin6_port       = NDK_htons(drec6[i].LocalPort);

                    if( bind( drec6[i].s,(struct sockaddr *)&sin1, sizeof(sin1) ) < 0 )
                    {
                        fdClose( drec6[i].s );
                        drec6[i].s = INVALID_SOCKET;
                    }
                }

                /* If the socket is bound and TCP, start listening */
                if( drec6[i].s != INVALID_SOCKET && drec6[i].Type != SOCK_DGRAM )
                {
                    if( listen( drec6[i].s, drec6[i].MaxSpawn ) < 0 )
                    {
                        fdClose( drec6[i].s );
                        drec6[i].s = INVALID_SOCKET;
                    }
                }
            }

            /* Update the fdPoll array */
            pollitem6[i].fd = drec6[i].s;
            if( drec6[i].Type && drec6[i].TasksSpawned < drec6[i].MaxSpawn )
                pollitem6[i].eventsRequested = POLLIN;
            else
                pollitem6[i].eventsRequested = 0;
        }

        /* Leave our lock */
        SemPost( hDSem6 );

        /* Poll with a timeout of 10 second - to try and catch */
        /* synchronization error */
        if( fdPoll( pollitem6, DAEMON_MAXRECORD, 10000 ) == SOCKET_ERROR )
            break;

        /* Enter our lock */
        SemPend( hDSem6, SEM_FOREVER );

        /* Spawn tasks for any active sockets */
        for( i=0; i<DAEMON_MAXRECORD; i++ )
        {
            /* If no poll results or the drec6 has been freed, skip it */
            if( !pollitem6[i].eventsDetected || !drec6[i].Type )
                continue;

            /* If the socket is invalid, close it */
            if( pollitem6[i].eventsDetected & POLLNVAL )
            {
                fdClose( drec6[i].s);
                drec6[i].s = INVALID_SOCKET;
                continue;
            }

            if( pollitem6[i].eventsDetected & POLLIN )
            {
                if( drec6[i].Type == SOCK_DGRAM )
                {
                    tsock = drec6[i].s;
                    closeSock = 0;
                }
                else
                {
                    tsock = accept( drec6[i].s, 0, 0 );
                    closeSock = 1;
                }

                if( tsock != INVALID_SOCKET )
                {
                    /* Create a record to track this task */
                    pc = mmAlloc( sizeof(CHILD) );
                    if( !pc )
                        goto spawnComplete;

                    /* Create the task */
                    pc->hTask = TaskCreate( dchild6, "dchild6",
                            drec6[i].Priority, drec6[i].StackSize,
                            (uintptr_t)&drec6[i], (uintptr_t)pc, 0);

                    if( !pc->hTask )
                    {
                        mmFree( pc );
                        goto spawnComplete;
                    }

                    /* Open a socket session for the child task */
                    fdOpenSession( pc->hTask );

                    /* Fill in the rest of the child record */
                    pc->closeSock = closeSock;
                    pc->s = tsock;

                    /* Now we won't close the socket here */
                    closeSock = 0;

                    /* Link this record onto the DREC */
                    drec6[i].TasksSpawned++;
                    pc->pPrev = 0;
                    pc->pNext = drec6[i].pC;
                    drec6[i].pC = pc;
                    if( pc->pNext )
                        pc->pNext->pPrev = pc;

spawnComplete:
                    /* If there was an error, we may need to close the socket */
                    if( closeSock )
                        fdClose( tsock );
                }
            }
        }
    }

    TaskExit();
}

/** 
 *  @b Description
 *  @n  
 *      The function creates a V6 Daemon.
 *
 *  @param[in]  Type
 *      This is the type of socket being opened through the daemon. In
 *      the case of IPv6 the following types are supported
 *          - SOCK_STREAM
 *              Use this for TCP Sockets.
 *          - SOCK_DGRAM
 *              Use this for UDP Sockets.
 *  @param[in]  LocalAddress
 *      This is the Local Address to which the socket will be bound to.
 *      In most cases this is typically passed as IPV6_UNSPECIFIED_ADDRESS
 *  @param[in]  LocalPort
 *      This is the Local Port to serve (cannot be NULL)
 *  @param[in]  pCb
 *      Call back function which is to be invoked.
 *  @param[in]  Priority
 *      Priority of new task to create for callback function 
 *  @param[in]  StackSize
 *      Stack size of new task to create for callback function
 *  @param[in]  Argument
 *      Argument (besides socket) to pass to callback function
 *  @param[in]  MaxSpawn
 *      Maximum number of callback function instances (must be 1 for UDP)
 *
 *  @retval
 *      Success - Handle to the spawned Daemon.
 *  @retval
 *      Error   - 0
 */
void *Daemon6New (uint32_t Type, IP6N LocalAddress, uint32_t LocalPort,
                   int (*pCb)(SOCKET,uint32_t), uint32_t Priority,
                   uint32_t StackSize, uintptr_t Argument, uint32_t MaxSpawn )
{
    int i;
    DREC *pdr = 0;

    /* Sanity check the arguments */
    if( Type==SOCK_DGRAM )
        MaxSpawn=1;
    else if( Type!=SOCK_STREAM)
        return(0);

    if( !LocalPort || !pCb || Priority<1 || Priority>15 || !StackSize || !MaxSpawn )
        return(0);

    /* We'll borrow the stack's kernel mode for a temp exclusion method */
    llEnter();
    if( !hDSem6 )
    {
        hDSem6 = SemCreate( 1 );
        memset( drec6, 0, sizeof(drec6) );
        RequestedRecords6 = 0;
    }
    llExit();

    /* At this point we must have a semaphore */
    if( !hDSem6 )
        return(0);

    /* Enter our own lock */
    SemPend( hDSem6, SEM_FOREVER );

    /* Scan the list for a free slot */
    for( i=0; i<DAEMON_MAXRECORD; i++ )
        if( !drec6[i].Type && !drec6[i].TasksSpawned )
            break;

    /* Break out if no free records */
    if(i==DAEMON_MAXRECORD)
        goto errorout;

    /* Build the new record */
    pdr = &drec6[i];
    pdr->Type           = Type;
    pdr->LocalV6Address = LocalAddress;
    pdr->LocalPort      = LocalPort;
    pdr->pCb            = pCb;
    pdr->Priority       = Priority;
    pdr->StackSize      = StackSize;
    pdr->Argument       = Argument;
    pdr->MaxSpawn       = MaxSpawn;
    pdr->s              = INVALID_SOCKET;

    /* If the Deamon task exists, ping it, otherwise create it */
    if( hDTask6 )
        fdSelectAbort( hDTask6 );
    else
    {
        hDTask6 = TaskCreate(daemon6,"daemon6",OS_TASKPRINORM,OS_TASKSTKLOW,0,0,0);
        if( hDTask6 )
            fdOpenSession(hDTask6);
        else
        {
            pdr->Type = 0;
            pdr = 0;
            goto errorout;
        }
    }

    RequestedRecords6++;

errorout:
    /* Exit our lock */
    SemPost( hDSem6 );
    return( pdr );
}

/** 
 *  @b Description
 *  @n  
 *      The function is to used cleanup a previously created V6 daemon.
 *
 *  @param[in]  hEntry
 *      This is the handle to the daemon which was previously created 
 *      using Daemon6New
 *  @sa
 *      Daemon6New
 *
 *  @retval
 *      Not Applicable.
 */
void Daemon6Free( void *hEntry )
{
    DREC *pdr = (DREC *)hEntry;
    CHILD *pc;

    /* At this point we must have a semaphore */
    if( !hDSem6 )
        return;

    /* Enter our own lock */
    SemPend( hDSem6, SEM_FOREVER );

    /* Sanity check */
    if( pdr->Type!=SOCK_STREAM && pdr->Type!=SOCK_DGRAM )
        goto errorout;

    /* Clear the record */
    pdr->Type = 0;
    RequestedRecords6--;

    /* Close the socket session of all children. This will */
    /* cause them to eventually fall out of their code and */
    /* close their sockets */
    pc = pdr->pC;
    while( pc )
    {
        if( pc->hTask )
        {
            fdCloseSession( pc->hTask );
            pc->hTask = 0;
        }
        pc = pc->pNext;
    }

    /* Close the socket (this will wake anyone who's using it) */
    if( pdr->s != INVALID_SOCKET )
    {
        fdClose( pdr->s );
        pdr->s = INVALID_SOCKET;
    }

    /* If there are no more records, close the daemon task's */
    /* file descriptor session. That will cause it to error */
    /* out and remove itself */
    if( !RequestedRecords6 )
    {
        fdCloseSession( hDTask6 );
        hDTask6 = 0;
    }

errorout:
    /* Exit our lock */
    SemPost( hDSem6 );
}

#endif /* _INCLUDE_IPv6_CODE */

