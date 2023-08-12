/*
 *  ======== App.h ========
 *
 */

#ifndef App__include
#define App__include
#if defined (__cplusplus)
extern "C" {
#endif



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

// module structure
typedef struct {
    MessageQ_Handle     hostQue;    // created locally
    MessageQ_QueueId    slaveQue;   // opened remotely
    UInt16              heapId;     // MessageQ heapId
    HeapBuf_Handle      heap;       // message heap
    Int                 msgSize;    // aligned size of message
    Int                 poolSize;   // size of message pool
    Ptr                 store;      // memory store for message pool
} App_Module;

App_Module Module;

//объявление функций
Int App_create(UInt16 remoteProcId, UInt16 App_MsgHeapId, UInt32 slaveQue, App_Module *core);
Int App_delete(UInt16 App_MsgHeapId, App_Module *core);
Int send_mess(UInt32 *mess, App_Module *core);
Int send_test_mess(App_Module *core);
Int receive_mess(UInt32 *mess, App_Module *core);
Int receive_test_mess(App_Module *core);
Int test_msg(App_Module *core_0, App_Module *core_1);

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
#endif /* App__include */
