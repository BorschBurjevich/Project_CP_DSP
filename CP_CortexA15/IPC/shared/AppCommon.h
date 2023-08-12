/*
 *  ======== AppCommon.h ========
 *
 */

#ifndef AppCommon__include
#define AppCommon__include
#if defined (__cplusplus)
extern "C" {
#endif

/*
 *  ======== Application Configuration ========
 */

/* notify commands 00 - FF */
#define App_CMD_MASK            0xFF000000
#define App_CMD_NOP             0x00000000  /* cc------ */
#define App_CMD_SHUTDOWN        0x02000000  /* cc------ */
#define N_DOTS_IPC              32 //число элементов в массиве IPC

typedef struct {
    MessageQ_MsgHeader  reserved;
    UInt32              cmd[N_DOTS_IPC];
} App_Msg;

//#define App_MsgHeapId           0
#define App_HostMsgQueName      "HOST:MsgQ:01"
#define App_SlaveMsgQueName     "%s:MsgQ:01"  /* %s is each slave's Proc Name */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
#endif /* AppCommon__include */
