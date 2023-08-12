#include "Func_Loopback.h"
#include "gpio/gpio.h"
#include "Lorandit_functions/Cod_com.h"
#include "Shared_mem_communication/shared.h"
#include "delay/delay.h"
#include "Lorandit_functions/Structs.h"
#define MAX_CONTI_OF_INTERRUPTS     (2U)

#define LPBK_APP_NUM_TESTS              (sizeof (gLpbkAppTestPrms) / \
                                         sizeof (gLpbkAppTestPrms[0U]))

volatile UInt32     gExitApp;
extern uint8_t      vip_flag;
LpbkApp_Obj         *appObj;
LpbkApp_InstObj     *instObj;
LpbkApp_TestParams  *testPrms;

BspOsal_SemHandle overflowSem;

LpbkApp_TestParams gLpbkAppTestPrms =
{
    "MY TEST            -> VIP1_S1_PORTA (800x480@30FPS)   -> BGR888     -> DSS Video1 -> DPI1 LCD",
    1U,                                         /* numCaptHandles */
    0U,                                         /* numDispHandles */
    /* captInstId[] */
    {
        VPS_CAPT_VIP_MAKE_INST_ID(VPS_VIP1, VPS_VIP_S0, VPS_VIP_PORTA),
        //VPS_CAPT_VIP_MAKE_INST_ID(VPS_VIP1, VPS_VIP_S1, VPS_VIP_PORTA),
        //VPS_CAPT_VIP_MAKE_INST_ID(VPS_VIP1, VPS_VIP_S0, VPS_VIP_PORTA),
    },
    /* dispInstId[] */
    {
        VPS_DISP_INST_DSS_VID1,
        //VPS_DISP_INST_DSS_VID1,
    },

    /* decDrvId[] */
    {
     0U,
    },
    /* encDrvId[] */
    {
     0U,
    },
    /* vencId */
    {
        VPS_DCTRL_DSS_VENC_LCD1
    },
    FALSE,                                      /* isSdVenc */
    /* captStandard */
    FVID2_STD_AUTO_DETECT,//FVID2_STD_720P_60,//FVID2_STD_WVGA_60,//FVID2_STD_D1,
    /* dispStandard */
    {
        FVID2_STD_CUSTOM,
    },

    FVID2_VIFM_SCH_DS_HSYNC_VSYNC, /* captVideoIfMode */
    FVID2_VIFW_16BIT,              /* captVideoIfWidth */
    FVID2_SF_PROGRESSIVE,          /* captScanFormat */
    FVID2_DF_RAW16,                /* captInDataFmt */
    FVID2_DF_RAW16,                /* captOutDataFmt */
    LPBK_APP_SC_ENABLE_ALL,        /* captScEnable */
    LPBK_APP_BUF_WIDTH,            /* captInWidth */
    LPBK_APP_BUF_HEIGHT,           /* captInHeight */
    LPBK_APP_BUF_WIDTH,            /* captOutWidth */
    LPBK_APP_BUF_HEIGHT,           /* captOutHeight */
    LPBK_APP_BUF_WIDTH,            /* dispInWidth */
    LPBK_APP_BUF_HEIGHT,           /* dispInHeight */
    LPBK_APP_BUF_WIDTH,            /* Target width */
    LPBK_APP_BUF_HEIGHT,           /* Target Height */
    LPBK_APP_RUN_COUNT,            /* runCount */
    BSP_BOARD_MODE_VIDEO_12BIT,    /* captBoardMode */
    BSP_BOARD_MODE_VIDEO_12BIT,    /* dispBoardMode */
    /* boardId */
    (BSP_BOARD_UNKNOWN | BSP_BOARD_GP_AM572X)
};

void init_and_start_vip()
{
    Int32            retVal = FVID2_SOK;
    UInt32           instCnt;

    LpbkApp_InstObj *instObj = NULL;

    BspUtils_memcpy(&appObj->testPrms, testPrms, sizeof (appObj->testPrms));

    /* Create driver */
    LpbkApp_initParams(appObj);
    LpbkApp_captCreate(appObj);

    appObj->startTime = BspOsal_getCurTimeInMsec();

    /* Start the load calculation */
    BspUtils_prfLoadCalcStart();

    if (FVID2_SOK == retVal)
    {
        for (instCnt = 0U; instCnt < appObj->testPrms.numCaptHandles; instCnt++)
        {
            instObj = &appObj->instObj[instCnt];

            retVal = Fvid2_start(instObj->captDrvHandle, NULL);
            if (retVal != FVID2_SOK)
            {
                GT_0trace(BspAppTrace, GT_ERR, APP_NAME ": Capture Start Failed!!!\r\n");
                break;
            }
        }
    }

    return;
}

/*
 *  Callback function, recieved when overflow occues in VIP
 */
Int32 LpbkApp_overflowCbFxn()
{
    Int32 retVal = FVID2_SOK;
    /* Most of the time VIP recovers automatically. This is to give
     * some time to VIP to recover automatically
     */
    GT_assert(BspAppTrace, (instObj != NULL));

    instObj->overflowCount++;
    if (instObj->overflowCount > MAX_CONTI_OF_INTERRUPTS)
    {
        retVal = Fvid2_control(instObj->captDrvHandle, IOCTL_VPS_CAPT_RESET_VIP_PORT, NULL, NULL);
        if (retVal != FVID2_SOK)
        {
            GT_0trace(BspAppTrace, GT_ERR, APP_NAME ": VIP Reset Failed!!!\r\n");
        }
    }

    BspOsal_semPost(overflowSem);

    return retVal;
}

void LpbkApp_captCreate()
{
    Int32  retVal = FVID2_SOK;
    UInt32 instCnt;
    UInt32 streamId, chId;
    Vps_VpdmaMaxSizeParams vipMaxSizePrms;
    Vps_CaptFrameSkip      frmSkipPrms;

    for (instCnt = 0U; instCnt < appObj->testPrms.numCaptHandles; instCnt++)
    {
        instObj = &appObj->instObj[instCnt];
        instObj->captDrvHandle = Fvid2_create(FVID2_VPS_CAPT_VID_DRV, instObj->captInstId,
                                              &instObj->captCreatePrms, &instObj->captCreateStatus, &instObj->captCbPrms);
        if ((NULL == instObj->captDrvHandle) || (instObj->captCreateStatus.retVal != FVID2_SOK))
        {
            GT_0trace(BspAppTrace, GT_ERR, APP_NAME ": Capture Create Failed!!!\r\n");
            retVal = instObj->captCreateStatus.retVal;
        }
        if (FVID2_SOK == retVal)
        {
            retVal = Fvid2_control(instObj->captDrvHandle, IOCTL_VPS_CAPT_SET_VIP_PARAMS, &instObj->vipPrms, NULL);
            if (retVal != FVID2_SOK)
            {
                GT_0trace(BspAppTrace, GT_ERR, APP_NAME ": VIP Set Params IOCTL Failed!!!\r\n");
            }
        }

        if (FVID2_SOK == retVal)
        {
            /* Set frame skip for each channel */
            VpsCaptFrameSkip_init(&frmSkipPrms);
            for (streamId = 0U; streamId < instObj->captCreatePrms.numStream;
                 streamId++)
            {
                for (chId = 0U; chId < instObj->captCreatePrms.numCh; chId++)
                {
                    /* This IOCTL is supported only for frame drop mode */
                    if (instObj->captCreatePrms.bufCaptMode == VPS_CAPT_BCM_FRM_DROP)
                    {
                        frmSkipPrms.chNum = Vps_captMakeChNum(instObj->captInstId, streamId, chId);
                        frmSkipPrms.frmSkipMask = LPBK_APP_FRAME_SKIP_PATTERN;
                        retVal = Fvid2_control(instObj->captDrvHandle, IOCTL_VPS_CAPT_SET_FRAME_SKIP, &frmSkipPrms, NULL);
                        if (retVal != FVID2_SOK)
                        {
                            GT_0trace(BspAppTrace, GT_ERR, APP_NAME ": VIP Set frame skip IOCTL Failed!!\r\n");
                            break;
                        }
                    }
                }
            }
        }

        if (FVID2_SOK == retVal)
        {
            VpsVpdmaMaxSizeParams_init(&vipMaxSizePrms);
            vipMaxSizePrms.instId = Vps_captGetVipId(appObj->testPrms.captInstId[instCnt]);
            vipMaxSizePrms.maxOutWidth[0U]  = instObj->maxOutWidth[0U];
            vipMaxSizePrms.maxOutHeight[0U] = instObj->maxOutHeight[0U];
            vipMaxSizePrms.maxOutWidth[1U]  = instObj->maxOutWidth[1U];
            vipMaxSizePrms.maxOutHeight[1U] = instObj->maxOutHeight[1U];
            vipMaxSizePrms.maxOutWidth[2U]  = instObj->maxOutWidth[2U];
            vipMaxSizePrms.maxOutHeight[2U] = instObj->maxOutHeight[2U];

            retVal = Fvid2_control(appObj->fvidHandleAll, IOCTL_VPS_CAPT_SET_VIP_MAX_SIZE, &vipMaxSizePrms, NULL);
            if (retVal != FVID2_SOK)
            {
                GT_0trace(BspAppTrace, GT_ERR,APP_NAME ": VIP Set Max Frame Size Params IOCTL Failed!!!\r\n");
            }
        }
        if (FVID2_SOK == retVal)
        {
            retVal = LpbkApp_captAllocQueueFrm(FALSE);
            if (retVal != FVID2_SOK)
            {
                GT_0trace(BspAppTrace, GT_ERR, APP_NAME ": Capture Alloc and Queue Failed!!!\r\n");
            }
        }

        if (FVID2_SOK == retVal)
        {
            /*retVal = Bsp_boardSetPinMux(FVID2_VPS_CAPT_VID_DRV, instObj->captInstId, appObj->testPrms.captBoardMode);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(BspAppTrace, GT_ERR, "Pin Muxing Failed \r\n");
            }*/
        }

        if (retVal == FVID2_SOK)
        {
            Vps_CaptOverflowParams overflowParams;
            overflowParams.overflowCbFxn = (Vps_CaptVipOverflowFxn) & LpbkApp_overflowCbFxn;
            overflowParams.retParams = (void *) instObj;
            retVal = Fvid2_control(instObj->captDrvHandle, IOCTL_VPS_CAPT_REGISTER_OVF_INTR, &overflowParams, NULL);
            if (retVal != FVID2_SOK)
            {
                GT_0trace(BspAppTrace, GT_ERR, APP_NAME ": VIP Set Params IOCTL Failed!!!\r\n");
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        //GT_0trace(BspAppTrace, GT_INFO,
          //        APP_NAME ": Capture create complete!!\r\n");
    }

    return;
}
uint8_t flajek = 0;
Int32 LpbkApp_captCbFxn(Fvid2_Handle handle, Ptr appData, Ptr reserved)
{
    Int32              retVal = FVID2_SOK;
    UInt32             streamId;
    LpbkApp_InstObj   *instObj = (LpbkApp_InstObj *) appData;
    //LpbkApp_Obj       *appObj  = instObj->appObj;
    Fvid2_FrameList    frmList;

    //t2 = Timestamp_get32();
    //delta22 = t2 - t1;

    for (streamId = 0U; streamId < instObj->captCreatePrms.numStream; streamId++)
    {
        retVal = Fvid2_dequeue(instObj->captDrvHandle, &frmList, streamId, FVID2_TIMEOUT_NONE);
        if (FVID2_SOK == retVal)
        {
            retVal = Fvid2_queue(instObj->captDrvHandle, &frmList, streamId);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(BspAppTrace, GT_ERR, APP_NAME ": Queue Failed!!!\r\n");
            }
        }
    }


    if ( flajek == 0 )
    {
        uint32_t *p;
        p = (uint32_t)(frmList.drvData + 76);
        *p = &rxvipBuf[0];
        flajek++;
    }


    return (retVal);
}

/**
 *  \brief Allocate and queue frames to driver
 */
Int32 LpbkApp_captAllocQueueFrm(UInt32 fieldMerged)
{
    Int32               retVal = FVID2_SOK;
    UInt32              streamId, chId, frmId, idx;
    UInt32              bufSize;
    UInt32              yFld1Offset, cbCrFld0Offset, cbCrFld1Offset;
    Fvid2_Format       *fmt;
    Fvid2_Frame        *frm;
    Fvid2_FrameList     frmList;
    Vps_CaptVipOutInfo *outInfo;
    Char fileStr[30U];

    /* for every stream and channel in a capture handle */
    Fvid2FrameList_init(&frmList);
    for (streamId = 0U; streamId < instObj->captCreatePrms.numStream;
         streamId++)
    {
        for (chId = 0U; chId < instObj->captCreatePrms.numCh; chId++)
        {
            outInfo = &instObj->vipPrms.outStreamInfo[streamId];
            fmt     = &instObj->captAllocFmt;
            Fvid2Format_init(fmt);

            /* base index for instObj->captFrames[] */
            idx = (VPS_CAPT_CH_PER_PORT_MAX * LPBK_APP_FRAMES_PER_CH * streamId) + (LPBK_APP_FRAMES_PER_CH * chId);

            if (idx >= LPBK_APP_CAPT_MAX_FRM)
            {
                idx = 0U;
            }
            frm = &instObj->captFrames[idx];

            /* fill format with channel specific values  */
            fmt->chNum = Vps_captMakeChNum(instObj->captInstId, streamId, chId);
            fmt->width  = appObj->maxWidth;
            fmt->height = appObj->maxHeight + LPBK_APP_PADDING_LINES;
            if (fieldMerged)
            {
                fmt->height *= 2;
            }
            fmt->pitch[0U] = outInfo->outFmt.pitch[0U];
            fmt->pitch[1U] = outInfo->outFmt.pitch[1U];
            fmt->pitch[2U] = outInfo->outFmt.pitch[2U];
            //if (FVID2_SF_INTERLACED == appObj->testPrms.captScanFormat)
            //{
                /* Field merge capture - pitch is already doubled */
                //fmt->pitch[0U] /= 2U;
               // fmt->pitch[1U] /= 2U;
                //fmt->pitch[2U] /= 2U;
            //}
            fmt->fieldMerged[0U] = FALSE;
            fmt->fieldMerged[1U] = FALSE;
            fmt->fieldMerged[2U] = FALSE;
            fmt->dataFormat      = outInfo->outFmt.dataFormat;
            fmt->scanFormat      = FVID2_SF_PROGRESSIVE;
            fmt->bpp = FVID2_BPP_BITS16; /* ignored */

            Fvid2Frame_init(&instObj->captDropFrame);
            instObj->captDropFrame.chNum = fmt->chNum;
            instObj->isDropFrmWithDriver = FALSE;

            /*
             * alloc memory based on 'format'
             * Allocated frame info is put in frames[]
             * LPBK_APP_FRAMES_PER_CH is the number of buffers per channel to
             * allocate
             */
            retVal = BspUtils_memFrameAlloc(fmt, frm, LPBK_APP_FRAMES_PER_CH);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(BspAppTrace, GT_ERR, APP_NAME ": Capture Frame Alloc Failed!!!\r\n");
                break;
            }

            snprintf(fileStr, 30U, "loopbackOption%uInst%uCh%uStr%u",
                     (unsigned int) appObj->testPrms.testId,
                     (unsigned int) instObj->captInstId,
                     (unsigned int) chId,
                     (unsigned int) streamId);
            BspUtils_memFrameGetSize(fmt, &bufSize, NULL);
            BspOsal_cacheInv(frm[0U].addr[0U][0U], (LPBK_APP_FRAMES_PER_CH * bufSize), BSP_OSAL_CT_ALL, BSP_OSAL_WAIT_FOREVER);
            //BspUtils_appPrintSaveRawCmd(
                //fileStr,
                //frm[0U].addr[0U][0U],
                //outInfo->outFmt.dataFormat,
                //fmt->width,
                //fmt->height,
                //(LPBK_APP_FRAMES_PER_CH * bufSize));


                /* Fill with background color */
 // 12.08.2019 и зменил на заполнение буфера значением 0x00
                BspUtils_memset( frm[0U].addr[0U][0U], 0x00U, (LPBK_APP_FRAMES_PER_CH * bufSize));

                /* Flush and invalidate the CPU write */
                BspOsal_cacheWbInv(frm[0U].addr[0U][0U], (LPBK_APP_FRAMES_PER_CH * bufSize), BSP_OSAL_CT_ALL, BSP_OSAL_WAIT_FOREVER);

            for (frmId = 0U; frmId < LPBK_APP_FRAMES_PER_CH; frmId++)
            {
                /* Fill frame info */
                instObj->captFrameInfo[frmId].bufFid  = FVID2_FID_TOP;
                instObj->captFrameInfo[frmId].bufPair = &instObj->captFramesOdd[frmId];
                instObj->captFrameInfo[frmId].instObj = instObj;

                frm[frmId].fid     = FVID2_FID_FRAME;
                frm[frmId].appData = &instObj->captFrameInfo[frmId];

                /* Since BspUtils_memFrameAlloc is setting the address
                 * for only top field, set addresses for bottom fields. */
                if (Fvid2_isDataFmtYuv422I(fmt->dataFormat))
                {
                    yFld1Offset = (UInt32) frm[frmId].addr[0U][0U] + fmt->pitch[0U];
                    frm[frmId].addr[1U][0U] = (Ptr) yFld1Offset;
                }
                if (Fvid2_isDataFmtSemiPlanar(fmt->dataFormat))
                {
                    yFld1Offset = (UInt32) frm[frmId].addr[0U][0U] + fmt->pitch[0U];
                    cbCrFld0Offset = (UInt32) ((UInt32) frm[frmId].addr[0U][0U] + (fmt->pitch[0U] * appObj->maxHeight));
                    cbCrFld1Offset = (UInt32) (cbCrFld0Offset + fmt->pitch[0U]);
                    frm[frmId].addr[0U][1U] = (Ptr) cbCrFld0Offset;
                    frm[frmId].addr[1U][0U] = (Ptr) yFld1Offset;
                    frm[frmId].addr[1U][1U] = (Ptr) cbCrFld1Offset;
                }

                /* Set number of frame in frame list */
                frmList.frames[0U] = &frm[frmId];
                frmList.numFrames  = 1U;

                /* In case of field capture give odd field frame */
                if (FVID2_SF_INTERLACED == appObj->testPrms.captScanFormat)
                {
                    BspUtils_memcpy(
                        &instObj->captFramesOdd[frmId],
                        &frm[frmId],
                        sizeof (Fvid2_Frame));

                    /* Fill frame info */
                    instObj->captFrameInfoOdd[frmId].bufFid  = FVID2_FID_BOTTOM;
                    instObj->captFrameInfoOdd[frmId].bufPair = &instObj->captFrames[frmId];
                    instObj->captFrameInfoOdd[frmId].instObj = instObj;

                    /* Set the odd address and app data */
                    instObj->captFramesOdd[frmId].appData = &instObj->captFrameInfoOdd[frmId];
                    instObj->captFramesOdd[frmId].addr[0U][0U] = instObj->captFramesOdd[frmId].addr[1U][0U];
                    instObj->captFramesOdd[frmId].addr[0U][1U] = instObj->captFramesOdd[frmId].addr[1U][1U];
                    instObj->captFramesOdd[frmId].addr[0U][2U] = instObj->captFramesOdd[frmId].addr[1U][2U];

                    /* Queue the odd frame */
                    frmList.frames[1U] = &instObj->captFramesOdd[frmId];
                    frmList.numFrames++;
                }

                /*
                 * queue the frames in frmList
                 * All allocate frames are queued here as an example.
                 * In general atleast 2 frames per channel need to queued
                 * before starting capture,
                 * else frame will get dropped until frames are queued
                 */
                retVal =
                    Fvid2_queue(instObj->captDrvHandle, &frmList, streamId);
                if (FVID2_SOK != retVal)
                {
                    GT_0trace(BspAppTrace, GT_ERR, APP_NAME ": Capture Queue Failed!!!\r\n");
                    break;
                }
            }

            if (FVID2_SOK != retVal)
            {
                break;
            }
        }

        if (FVID2_SOK != retVal)
        {
            break;
        }
    }

    return (retVal);
}

/**
 *  LpbkApp_initParams
 *  Initialize the global variables and frame pointers.
 */
void LpbkApp_initParams()
{
    UInt32 streamId, chId, instCnt, plId;
    Vps_CaptCreateParams *createPrms;
    Vps_CaptVipParams    *vipPrms;
    Vps_CaptVipScParams  *scPrms;
    Vps_CaptVipOutInfo   *outInfo;
    UInt32 maxHandles;

    appObj->maxWidth          = 1024;//64
    appObj->maxHeight         = 2;
    appObj->totalCaptFldCount = 0U;
    appObj->totalDispCount    = 0U;
    appObj->totalCpuLoad      = 0U;
    appObj->cpuLoadCount      = 0U;
    appObj->callBackCount     = 0U;


    maxHandles = appObj->testPrms.numCaptHandles;
    if (maxHandles < appObj->testPrms.numDispHandles)
    {
        maxHandles = appObj->testPrms.numDispHandles;
    }
    for (instCnt = 0U; instCnt < maxHandles; instCnt++)
    {
        instObj = &appObj->instObj[instCnt];
        instObj->captInstId    = appObj->testPrms.captInstId[instCnt];
        instObj->dispInstId    = appObj->testPrms.dispInstId[instCnt];
        instObj->decDrvId      = appObj->testPrms.decDrvId[instCnt];
        instObj->encDrvId      = appObj->testPrms.encDrvId[instCnt];
        instObj->captDrvHandle = NULL;
        instObj->dispDrvHandle = NULL;
        Fvid2CbParams_init(&instObj->captCbPrms);
        instObj->captCbPrms.cbFxn   = &LpbkApp_captCbFxn;
        instObj->captCbPrms.appData = instObj;

        /*
        Fvid2CbParams_init(&instObj->dispCbPrms);
        instObj->dispCbPrms.cbFxn   = &LpbkApp_dispCbFxn;
        instObj->dispCbPrms.appData = instObj;
        */

        instObj->appObj = appObj;

        createPrms = &instObj->captCreatePrms;
        VpsCaptCreateParams_init(createPrms);
        createPrms->videoIfMode  = appObj->testPrms.captVideoIfMode;
        createPrms->videoIfWidth = appObj->testPrms.captVideoIfWidth;
        createPrms->bufCaptMode  = VPS_CAPT_BCM_LAST_FRM_REPEAT;
        createPrms->numCh        = LPBK_APP_NUM_CAPT_CH;
        createPrms->numStream    = LPBK_APP_NUM_CAPT_STREAM;

        vipPrms = &instObj->vipPrms;
        VpsCaptVipParams_init(vipPrms);
        vipPrms->inFmt.chNum           = 0U;
        vipPrms->inFmt.width           = appObj->testPrms.captInWidth;
        vipPrms->inFmt.height          = appObj->testPrms.captInHeight;
        vipPrms->inFmt.pitch[0U]       = 0U;
        vipPrms->inFmt.pitch[1U]       = 0U;
        vipPrms->inFmt.pitch[2U]       = 0U;
        vipPrms->inFmt.fieldMerged[0U] = FALSE;
        vipPrms->inFmt.fieldMerged[1U] = FALSE;
        vipPrms->inFmt.fieldMerged[2U] = FALSE;
        vipPrms->inFmt.dataFormat      = appObj->testPrms.captInDataFmt;
        vipPrms->inFmt.scanFormat      = FVID2_SF_PROGRESSIVE;
        vipPrms->inFmt.bpp      = FVID2_BPP_BITS16;
        vipPrms->inFmt.reserved = NULL;

        scPrms          = &instObj->vipScPrms;
        vipPrms->scPrms = scPrms;
        VpsCaptVipScParams_init(scPrms);
        scPrms->inCropCfg.cropStartX   = 0U;
        scPrms->inCropCfg.cropStartY   = 0U;
        scPrms->inCropCfg.cropWidth    = vipPrms->inFmt.width;
        scPrms->inCropCfg.cropHeight   = vipPrms->inFmt.height;
        scPrms->scCfg.bypass           = FALSE;
        scPrms->scCfg.nonLinear        = FALSE;
        scPrms->scCfg.stripSize        = 0U;
        scPrms->scCfg.enableEdgeDetect = TRUE;
        scPrms->scCfg.enablePeaking    = TRUE;
        scPrms->scCfg.advCfg           = NULL;
        scPrms->scCoeffCfg      = NULL;
        scPrms->enableCoeffLoad = FALSE;
        vipPrms->vipPortCfg     = NULL;

        Bsp_platformSetVipClkInversion(instObj->captInstId, FALSE);


        //22.07.2019
         // Настройки полярности добавил
        vipPrms->vipPortCfg = &instObj->vipPortCfg;
        VpsVipPortConfig_init(&instObj->vipPortCfg);
        instObj->vipPortCfg.disCfg.actvidPol     = FVID2_POL_HIGH;
        instObj->vipPortCfg.disCfg.vsyncPol      = FVID2_POL_HIGH;//FVID2_POL_LOW;
        instObj->vipPortCfg.disCfg.hsyncPol      = FVID2_POL_HIGH;//FVID2_POL_LOW;
        instObj->vipPortCfg.comCfg.pixClkEdgePol = FVID2_EDGE_POL_RISING;

        /* возможно понадобится*/
        /* Disable VIP cropping */
        /*
        instObj->vipPortCfg.actCropEnable     = FALSE;
        instObj->vipPortCfg.actCropCfg.srcNum = 0U;
        instObj->vipPortCfg.actCropCfg.cropCfg.cropStartX = 0U;
        instObj->vipPortCfg.actCropCfg.cropCfg.cropStartY = 0U;
        instObj->vipPortCfg.actCropCfg.cropCfg.cropWidth  = 1920U;
        instObj->vipPortCfg.actCropCfg.cropCfg.cropHeight = 1080U;
        */
        /* Enable VIP cropping */
        /*
        instObj->vipPortCfg.actCropEnable     = TRUE;
        instObj->vipPortCfg.actCropCfg.srcNum = 0U;
        instObj->vipPortCfg.actCropCfg.cropCfg.cropStartX = 20U;
        instObj->vipPortCfg.actCropCfg.cropCfg.cropStartY = 11U;
        instObj->vipPortCfg.actCropCfg.cropCfg.cropWidth  = 1920U;
        instObj->vipPortCfg.actCropCfg.cropCfg.cropHeight = 1080U;
        */

        vipPrms->cscCfg = NULL;

        for (streamId = 0U; streamId < LPBK_APP_NUM_CAPT_STREAM; streamId++)
        {
            for (chId = 0U; chId < VPS_CAPT_CH_PER_PORT_MAX; chId++)
            {
                createPrms->chNumMap[streamId][chId] = Vps_captMakeChNum(instObj->captInstId, streamId, chId);
            }
            outInfo = &vipPrms->outStreamInfo[streamId];
            outInfo->outFmt.chNum           = 0U;
            outInfo->outFmt.width           = appObj->testPrms.captOutWidth;
            outInfo->outFmt.height          = appObj->testPrms.captOutHeight;
            outInfo->outFmt.fieldMerged[0U] = FALSE;
            outInfo->outFmt.fieldMerged[1U] = FALSE;
            outInfo->outFmt.fieldMerged[2U] = FALSE;
            outInfo->outFmt.dataFormat      = appObj->testPrms.captOutDataFmt;
            outInfo->outFmt.scanFormat      = FVID2_SF_PROGRESSIVE;
            outInfo->outFmt.bpp       = FVID2_BPP_BITS16;
            outInfo->outFmt.pitch[0U] = 0U;
            outInfo->outFmt.pitch[1U] = 0U;
            outInfo->outFmt.pitch[2U] = 0U;
            if (Fvid2_isDataFmtSemiPlanar(outInfo->outFmt.dataFormat))
            {
                outInfo->outFmt.pitch[FVID2_YUV_SP_Y_ADDR_IDX] = VpsUtils_align(appObj->maxWidth, VPS_BUFFER_ALIGNMENT);
                outInfo->outFmt.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] = outInfo->outFmt.pitch[FVID2_YUV_SP_Y_ADDR_IDX];
            }
            else if (Fvid2_isDataFmtYuv422I(outInfo->outFmt.dataFormat) || (FVID2_DF_RAW16 == outInfo->outFmt.dataFormat))
            {
                outInfo->outFmt.pitch[FVID2_YUV_INT_ADDR_IDX] = VpsUtils_align(appObj->maxWidth * 2U, VPS_BUFFER_ALIGNMENT);
            }
            else if (FVID2_DF_RAW08 == outInfo->outFmt.dataFormat)
            {
                outInfo->outFmt.pitch[FVID2_YUV_INT_ADDR_IDX] = VpsUtils_align(appObj->maxWidth, VPS_BUFFER_ALIGNMENT);
            }
            else
            {
                /* Align the pitch to BPP boundary as well since the pitch
                 * aligined to VPS_BUFFER_ALIGNMENT may not be multiple of 3
                 * bytes (1 pixel) */
                outInfo->outFmt.pitch[FVID2_RGB_ADDR_IDX] = VpsUtils_align(appObj->maxWidth * 3U, (VPS_BUFFER_ALIGNMENT * 3U));
            }

            if (FVID2_SF_INTERLACED == appObj->testPrms.captScanFormat)
            {
                /* Field merge capture - double the pitch */
                outInfo->outFmt.pitch[0U] *= 2U;
                outInfo->outFmt.pitch[1U] *= 2U;
                outInfo->outFmt.pitch[2U] *= 2U;
            }

            outInfo->bufFmt  = FVID2_BUF_FMT_FRAME;
            outInfo->memType = VPS_VPDMA_MT_NONTILEDMEM;
if (!Bsp_platformIsTI814xFamilyBuild())
            {
            outInfo->maxOutWidth[0U]  = VPS_VPDMA_MAX_OUT_WIDTH_REG1;
            outInfo->maxOutHeight[0U] = VPS_VPDMA_MAX_OUT_HEIGHT_REG1;
            for (plId = 1U; plId < FVID2_MAX_PLANES; plId++)
            {
                outInfo->maxOutWidth[plId]  = VPS_VPDMA_MAX_OUT_WIDTH_REG2;
                outInfo->maxOutHeight[plId] = VPS_VPDMA_MAX_OUT_HEIGHT_REG2;
            }
}
            else
            {
                for (plId = 0U; plId < FVID2_MAX_PLANES; plId++)
                {
                    outInfo->maxOutWidth[plId] = VPS_VPDMA_MAX_OUT_WIDTH_1920_PIXELS;
                    outInfo->maxOutHeight[plId] = VPS_VPDMA_MAX_OUT_HEIGHT_1080_LINES;
                }
            }

            outInfo->scEnable = FALSE;
            if (LPBK_APP_SC_ENABLE_ALL == appObj->testPrms.captScEnable)
            {
                outInfo->scEnable = TRUE;
            }
            if ((0U == streamId) &&
                (LPBK_APP_SC_ENABLE_0 == appObj->testPrms.captScEnable))
            {
                outInfo->scEnable = TRUE;
            }
            if ((1U == streamId) &&
                (LPBK_APP_SC_ENABLE_1 == appObj->testPrms.captScEnable))
            {
                outInfo->scEnable = TRUE;
            }
            outInfo->subFrmPrms.subFrameEnable      = FALSE;
            outInfo->subFrmPrms.numLinesPerSubFrame = 0U;
            outInfo->subFrmPrms.subFrameCb          = NULL;
        }

        //Установка махсимальной ширины, все что более будет отброшено DMA
        instObj->maxOutWidth[0U]  = LPBK_APP_MAXSIZE_1_WIDTH; // 12.08.2019
        //Установка махсимальной высоты, все что более будет отброшено DMA
        instObj->maxOutHeight[0U] = LPBK_APP_MAXSIZE_1_HEIGHT; // 12.08.2019

        instObj->maxOutWidth[1U]  = LPBK_APP_MAXSIZE_2_WIDTH;
        instObj->maxOutHeight[1U] = LPBK_APP_MAXSIZE_2_HEIGHT;
        instObj->maxOutWidth[2U]  = LPBK_APP_MAXSIZE_3_WIDTH;
        instObj->maxOutHeight[2U] = LPBK_APP_MAXSIZE_3_HEIGHT;

    }

    return;
}

void LpbkApp_init()
{
    Int32  retVal;
    UInt32 isI2cInitReq;

    /* System init */
    isI2cInitReq = FALSE;//12.08.2019// //TRUE - Р±С‹Р»Рѕ;
    retVal       = BspUtils_appDefaultInit(isI2cInitReq);
    if (retVal != FVID2_SOK)
    {
        GT_0trace(BspAppTrace, GT_ERR, APP_NAME ": System Init Failed!!!\r\n");
    }
    if (FVID2_SOK == retVal)
    {
        /* Create global capture handle, used for common driver configuration */
        appObj->fvidHandleAll = Fvid2_create(FVID2_VPS_CAPT_VID_DRV, VPS_CAPT_INST_ALL,
                                                NULL,  /* NULL for VPS_LPBK_INST_ALL */
                                                NULL,  /* NULL for VPS_LPBK_INST_ALL */
                                                NULL); /* NULL for VPS_LPBK_INST_ALL */
        if (NULL == appObj->fvidHandleAll)
        {
            GT_0trace(BspAppTrace, GT_ERR, APP_NAME ": Global Handle Create Failed!!!\r\n");
            retVal = FVID2_EBADARGS;
        }
    }
    if (FVID2_SOK == retVal)
    {
        /* Create DCTRL handle, used for common driver configuration */
        appObj->dctrlHandle = Fvid2_create(FVID2_VPS_DCTRL_DRV, VPS_DCTRL_INST_0,
                                            NULL, /* NULL for VPS_DCTRL_INST_0 */
                                            NULL, /* NULL for VPS_DCTRL_INST_0 */
                                            NULL);/* NULL for VPS_DCTRL_INST_0 */
        if (NULL == appObj->dctrlHandle)
        {
            GT_0trace(BspAppTrace, GT_ERR, APP_NAME ": DCTRL Create Failed!!!\r\n");
            retVal = FVID2_EBADARGS;
        }
    }


    if (FVID2_SOK == retVal)
    {
        //GT_0trace(BspAppTrace, GT_INFO,
                  //APP_NAME ": LpbkApp_init() - DONE !!!\r\n");
    }

    return;
}

void LpbkApp_deInit()
{
    Int32  retVal;
    UInt32 isI2cDeInitReq;

    /* Delete DCTRL handle */
    retVal = Fvid2_delete(appObj->dctrlHandle, NULL);
    if (retVal != FVID2_SOK)
    {
        GT_0trace(BspAppTrace, GT_ERR, APP_NAME ": DCTRL handle delete failed!!!\r\n");
    }

    if (FVID2_SOK == retVal)
    {
        /* Delete global VIP capture handle */
        retVal = Fvid2_delete(appObj->fvidHandleAll, NULL);
        if (retVal != FVID2_SOK)
        {
            GT_0trace(BspAppTrace, GT_ERR, APP_NAME ": Global handle delete failed!!!\r\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        if (BSP_BOARD_MULTIDES == Bsp_boardGetId())
        {
            retVal =
                BspUtils_appDeInitSerDeSer();
            if (retVal != FVID2_SOK)
            {
                GT_0trace(BspAppTrace, GT_ERR, APP_NAME ": MultiDes Board DeInit failed!!!\r\n");
            }
        }
    }
    if (FVID2_SOK == retVal)
    {
        /* System de-init */
        isI2cDeInitReq = FALSE;//12.08.2019 // TRUE; - СЂР°РЅРµРµ
        retVal         = BspUtils_appDefaultDeInit(isI2cDeInitReq);
        if (retVal != FVID2_SOK)
        {
            GT_0trace(BspAppTrace, GT_ERR, APP_NAME ": System De-Init Failed!!!\r\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Needed onlt for TDA2xx platform. Other platform will do nothing
         * and return OK */
        retVal = Bsp_platformEnableHdmiPll(FALSE);
        if (retVal != FVID2_SOK)
        {
            GT_0trace(BspAppTrace, GT_ERR,
                      APP_NAME ": Disabling HDMI PLL Failed!!!\r\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        GT_0trace(BspAppTrace, GT_INFO,
                  APP_NAME ": LpbkApp_deInit() - DONE !!!\r\n");
    }

    return;
}
extern uint8_t count_Start_data;
extern uint8_t data_flow;
void Start_data()
{

    vip_flag = 0;


    GPIO8 |= 0x00000004;
    udelay(1);
    GPIO8 &= 0xFFFFFFFB;





    ///// добавить сигнал синхростарта, для того чтобы показать гтовность принимать данные с AD ////

    ///////////////////////////////////////////////////////////////////////////////////////////////


}
