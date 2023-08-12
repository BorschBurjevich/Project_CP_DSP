/*
 *  ======== App.c ========
 */

// package header files
#include <xdc/std.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>

#include <ti/ipc/Ipc.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/SharedRegion.h>

#include <ti/sysbios/heaps/HeapBuf.h>
#include <ti/sysbios/knl/Task.h>

// local header files
#include "../IPC/shared/AppCommon.h"
#include "App.h"
#include <stdlib.h>

// round up the value 'size' to the next 'align' boundary
#define ROUNDUP(size, align) \
    (UInt32)(((UInt32)(size) + ((UInt32)(align) - 1)) & ~((UInt32)(align) - 1))

#define NUM_MSGS 1

//объ€вление глобальные переменных
extern UInt32 rx_buff[N_DOTS_IPC];
int counter;

//тела функций
Int App_create(UInt16 remoteProcId, UInt16 App_MsgHeapId, UInt32 slaveQue, App_Module *core)
{
    Int                 status = 0;
    Int                 align;
    Error_Block         eb;
    IHeap_Handle        srHeap;
    HeapBuf_Params      heapParams;
    MessageQ_Params     msgqParams;
    char                msgqName[32];

    Log_print0(Diags_INFO, "App_create: -->");

    // setting default values
    core->hostQue = NULL;
    core->slaveQue = slaveQue;//MessageQ_INVALIDMESSAGEQ;
    core->heapId = App_MsgHeapId;

    // compute message size to fill entire cache lines
    align = SharedRegion_getCacheLineSize(0);
    core->msgSize = ROUNDUP(sizeof(App_Msg), align);
    // compute message pool size
    core->poolSize = core->msgSize * NUM_MSGS;

    // acquire message pool memory
    srHeap = (IHeap_Handle)SharedRegion_getHeap(0);
    core->store = Memory_alloc(srHeap, core->poolSize, align, NULL);

    // create a heap in shared memory for message pool
    HeapBuf_Params_init(&heapParams);
    heapParams.blockSize = core->msgSize;
    heapParams.numBlocks = NUM_MSGS;
    heapParams.bufSize = core->poolSize;
    heapParams.align = align;
    heapParams.buf = core->store;
    Error_init(&eb);

    core->heap = HeapBuf_create(&heapParams, &eb);

    if (core->heap == NULL) {
        Log_error0("App_create: failed creating message pool");
        status = -1;
        goto leave;
    }

    // bind message pool to heapId
    MessageQ_registerHeap((Ptr)(core->heap), App_MsgHeapId);

    // create local message queue (inbound messages)
    MessageQ_Params_init(&msgqParams);

    core->hostQue = MessageQ_create(NULL, &msgqParams);

    if (core->hostQue == NULL) {
        Log_error0("App_create: failed creating MessageQ");
        status = -1;
        goto leave;
    }

    // open the remote message queue
    System_sprintf(msgqName, App_SlaveMsgQueName,
            MultiProc_getName(remoteProcId));

    do {
        status = MessageQ_open(msgqName, &core->slaveQue);
        Task_sleep(1);
    } while (status == MessageQ_E_NOTFOUND);

    if (status < 0) {
        Log_error0("App_create: failed opening MessageQ");
        goto leave;
    }

    Log_print0(Diags_INFO, "App_create: Host is ready");

leave:
    Log_print1(Diags_INFO, "App_create: <-- status=%d", (IArg)status);
    return(status);
}


Int App_delete(UInt16 App_MsgHeapId, App_Module *core)
{
    Int             status;
    IHeap_Handle    srHeap;

    Log_print0(Diags_INFO, "App_delete: -->");

    // close remote resources
    status = MessageQ_close(&core->slaveQue);

    if (status < 0) {
        goto leave;
    }

    // delete the host message queue
    status = MessageQ_delete(&core->hostQue);

    if (status < 0) {
        goto leave;
    }

    // unregister message pool heap
    MessageQ_unregisterHeap(App_MsgHeapId);

    // delete the message pool heap
    HeapBuf_delete(&core->heap);

    // release message pool memory
    srHeap = (IHeap_Handle)SharedRegion_getHeap(0);
    Memory_free(srHeap, core->store, core->poolSize);

leave:
    Log_print1(Diags_INFO, "App_delete: <-- status=%d", (IArg)status);
    return(status);
}

Int send_mess(UInt32 *mess, App_Module *core)
{
    Int         status;
    App_Msg *   msg;
    Int         i;

    // allocate message
    msg = (App_Msg *)MessageQ_alloc(core->heapId, core->msgSize);

    // fill in message payload
    for (i = 0; i < N_DOTS_IPC; ++i)
    {
        msg->cmd[i] = mess[i];
    }

   // send message
    MessageQ_setReplyQueue(core->hostQue, (MessageQ_Msg)msg);
    status =  MessageQ_put(core->slaveQue, (MessageQ_Msg)msg);

    return (status);
}

Int receive_mess(UInt32 *mess, App_Module *core)
{
    Int         status, i;
    App_Msg *   msg;
    //uint32_t    cmd[N_DOTS_IPC];

    status = MessageQ_get(core->hostQue, (MessageQ_Msg *)&msg, MessageQ_FOREVER);

    for (i = 0; i < N_DOTS_IPC; ++i)
    {
        mess[i] = msg->cmd[i];
    }

    //counter = msg->reserved.seqNum;
    status = MessageQ_free((MessageQ_Msg)msg);


    return (status);
}

Int send_test_mess(App_Module *core)
{
    Int         status;
    App_Msg *   msg;
    uint32_t    cmd = 0xaaaa5555;

    // allocate message
    msg = (App_Msg *)MessageQ_alloc(core->heapId, core->msgSize);

    // fill in message payload
    msg->cmd[0] = cmd;

    // send message
    MessageQ_setReplyQueue(core->hostQue, (MessageQ_Msg)msg);
    status =  MessageQ_put(core->slaveQue, (MessageQ_Msg)msg);

    return (status);
}

Int receive_test_mess(App_Module *core)
{
    Int         status;
    App_Msg *   msg;
    uint32_t    cmd;

    MessageQ_get(core->hostQue, (MessageQ_Msg *)&msg, MessageQ_FOREVER);

    cmd = msg->cmd[0];

    if (cmd == 0xaaaa5555)
    {
        status = 0x1;
    }
    MessageQ_free((MessageQ_Msg)msg);
    return (status);
}

Int test_msg(App_Module *core_0, App_Module *core_1)
{
    uint32_t status_0, status_1;
    Int      rtn;

    send_test_mess(core_0);
    send_test_mess(core_1);

    status_0 = receive_test_mess(core_0);
    status_1 = receive_test_mess(core_1);

    if((status_0 = 0x1) && (status_1 = 0x1))
    {
        rtn = 0x1;
    }

    else
    {
        rtn = 0x0;
    }

    return(rtn);
}


