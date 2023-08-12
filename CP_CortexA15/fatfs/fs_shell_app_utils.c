/**
 *  \file  fs_shell_app_utils.c
 *
 *  \brief This file implements file system shell interface.
 *
 *  \copyright Copyright (C) 2013-2018 Texas Instruments Incorporated -
 *             http://www.ti.com/
 */

/*

 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/csl/tistdtypes.h>
#include "string.h"
#include "stdio.h"
#include <ti/csl/csl_error.h>
#include <ti/fs/fatfs/ff.h>
#include "FATFS_log.h"
#include "fs_shell_app_utils.h"
#include "Lorandit_functions/Cod_com.h"
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/*****************************************************************************
Defines the size of the buffers that hold the path, or temporary data from
the memory card.  There are two buffers allocated of this size.  The buffer
size must be large enough to hold the longest expected full path name,
including the file name, and a trailing null character.
******************************************************************************/
#define FS_SHELL_APP_UTILS_PATH_BUF_SIZE   512

/* Defines size of the buffers that hold temporary data. */
#define FS_SHELL_APP_UTILS_DATA_BUF_SIZE   512
/*****************************************************************************
Defines the size of the buffer that holds the command line.
******************************************************************************/
#define FS_SHELL_APP_UTILS_CMD_CMD_BUF_SIZE    512

/*****************************************************************************
Defines the help message for cat.
******************************************************************************/
#define FS_SHELL_APP_UTILS_CMD_INFO_CAT                                            \
        "  : Show contents of a text file : cat <FILENAME> \n"                 \
                  "       Write to a file : cat <INPUTFILE> > <OUTPUTFILE> \n" \
                   "       Read from UART : cat dev.UART \n"                   \
                  "       Write from UART : cat dev.UART > <OUTPUTFILE>"
#define FS_SHELL_APP_UTILS_CMD_INFO_HELP  " : Display list of commands"
#define FS_SHELL_APP_UTILS_CMD_INFO_LS  "   : Display list of files"
#define FS_SHELL_APP_UTILS_CMD_INFO_CD     ": Change directory"
#define FS_SHELL_APP_UTILS_CMD_INFO_MKDIR  ": Create directory"
#define FS_SHELL_APP_UTILS_CMD_INFO_PWD  "  : Show current working directory"
#define FS_SHELL_APP_UTILS_CMD_INFO_RM  "   : Delete a file or an empty directory"


int32_t FSShellAppUtilsCmdExecute(uint8_t *pCmdLine,
                                         fsShellAppUtilsCmdEntry_t *pCmdList);
int32_t FSShellAppUtilsCmdLs(int32_t argc, char *argv[]);
int32_t FSShellAppUtilsCmdCd(int32_t argc, char *argv[]);
int32_t FSShellAppUtilsCmdPwd(int32_t argc, char *argv[]);
int32_t FSShellAppUtilsCmdMkDir(int32_t argc, char *argv[]);
int32_t FSShellAppUtilsCmdRm(int32_t argc, char *argv[]);
int32_t FSShellAppUtilsCmdCat(int32_t argc, char *argv[]);
int32_t FSShellAppUtilsCmdHelp(int32_t argc, char *argv[]);

uint32_t size = 0;//размер файла
uint32_t length = 0;//число блоков по 511
/* ========================================================================== */
/*                            Global Variables Declarations                   */
/* ========================================================================== */

static DIR gFsShellAppUtilsDirObj;
static FILINFO gFsShellAppUtilsFileInfo;
volatile uint32_t gFsShellAppUtilsCurState = FS_SHELL_APP_UTILS_STATE_MAX;
static FIL gFsShellAppUtilsReadFileObj  __attribute__ ((aligned (SOC_CACHELINE_SIZE)));

static FIL gFsShellAppUtilsWriteFileObj  __attribute__ ((aligned (SOC_CACHELINE_SIZE)));

static char gFsShellAppUtilsTempPath[FS_SHELL_APP_UTILS_PATH_BUF_SIZE]
                      __attribute__ ((aligned (SOC_CACHELINE_SIZE)));

static char gFsShellAppUtilsCwd[FS_SHELL_APP_UTILS_DATA_BUF_SIZE]
                             __attribute__ ((aligned (SOC_CACHELINE_SIZE)));

static char gFsShellAppUtilsDataBuf[FS_SHELL_APP_UTILS_DATA_BUF_SIZE]
                             __attribute__ ((aligned (SOC_CACHELINE_SIZE)));

fsShellAppUtilsCmdEntry_t gFsShellAppUtilsCmdTable[] =
{
    { "help",     &FSShellAppUtilsCmdHelp,  FS_SHELL_APP_UTILS_CMD_INFO_HELP},
    { "ls",       &FSShellAppUtilsCmdLs,    FS_SHELL_APP_UTILS_CMD_INFO_LS},
    { "cd",       &FSShellAppUtilsCmdCd,    FS_SHELL_APP_UTILS_CMD_INFO_CD},
    { "mkdir",    &FSShellAppUtilsCmdMkDir, FS_SHELL_APP_UTILS_CMD_INFO_MKDIR},
    { "rm",       &FSShellAppUtilsCmdRm,    FS_SHELL_APP_UTILS_CMD_INFO_RM},
    { "pwd",      &FSShellAppUtilsCmdPwd,   FS_SHELL_APP_UTILS_CMD_INFO_PWD},
    { "cat",      &FSShellAppUtilsCmdCat,   FS_SHELL_APP_UTILS_CMD_INFO_CAT},
    { 0, 0, 0 }
};

uint8_t mode = 0;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

int32_t FSShellAppUtilsInit(void)
{

    return(FS_SHELL_APP_UTILS_CMD_STATUS_INVALID);
}


int32_t FSShellAppUtilsProcess(uint32_t command)
{
    int32_t retStat = CSL_ESYS_FAIL;
    uint8_t gFsShellAppUtilsRxBuf[15] = {0};
//////////////////////////////////
    strcpy(gFsShellAppUtilsCwd, "0:");

   if(FR_OK == f_opendir(&gFsShellAppUtilsDirObj, gFsShellAppUtilsCwd))
   {
       // ls
       if(command == 0)
       {
           gFsShellAppUtilsRxBuf[0]=0x6c;
           gFsShellAppUtilsRxBuf[1]=0x73;

       }

       //чтение app
       if(command == 1)
       {
           gFsShellAppUtilsRxBuf[0]=0x63;
           gFsShellAppUtilsRxBuf[1]=0x61;
           gFsShellAppUtilsRxBuf[2]=0x74;
           gFsShellAppUtilsRxBuf[3]=0x20;

           gFsShellAppUtilsRxBuf[4]=0x61;
           gFsShellAppUtilsRxBuf[5]=0x70;
           gFsShellAppUtilsRxBuf[6]=0x70;

           mode = command;
       }

       //чтение MLO
       if(command == 2)
       {
           gFsShellAppUtilsRxBuf[0]=0x63;
           gFsShellAppUtilsRxBuf[1]=0x61;
           gFsShellAppUtilsRxBuf[2]=0x74;
           gFsShellAppUtilsRxBuf[3]=0x20;

           gFsShellAppUtilsRxBuf[4]=0x4d;
           gFsShellAppUtilsRxBuf[5]=0x4c;
           gFsShellAppUtilsRxBuf[6]=0x4f;

           mode = command;
       }

       //запись app
       if(command == 3)
       {
           gFsShellAppUtilsRxBuf[0]=0x63;
           gFsShellAppUtilsRxBuf[1]=0x61;
           gFsShellAppUtilsRxBuf[2]=0x74;
           gFsShellAppUtilsRxBuf[3]=0x20;

           gFsShellAppUtilsRxBuf[4]=0x61;
           gFsShellAppUtilsRxBuf[5]=0x70;
           gFsShellAppUtilsRxBuf[6]=0x70;
           gFsShellAppUtilsRxBuf[7]=0x20;
           gFsShellAppUtilsRxBuf[8]=0x3e;
           gFsShellAppUtilsRxBuf[9]=0x20;
           gFsShellAppUtilsRxBuf[10]=0x61;
           gFsShellAppUtilsRxBuf[11]=0x70;
           gFsShellAppUtilsRxBuf[12]=0x70;

           mode = command;
       }

       //запись mlo
       if(command == 4)
       {
           gFsShellAppUtilsRxBuf[0]=0x63;
           gFsShellAppUtilsRxBuf[1]=0x61;
           gFsShellAppUtilsRxBuf[2]=0x74;
           gFsShellAppUtilsRxBuf[3]=0x20;

           gFsShellAppUtilsRxBuf[4]=0x4d;
           gFsShellAppUtilsRxBuf[5]=0x4c;
           gFsShellAppUtilsRxBuf[6]=0x4f;
           gFsShellAppUtilsRxBuf[7]=0x20;
           gFsShellAppUtilsRxBuf[8]=0x3e;
           gFsShellAppUtilsRxBuf[9]=0x20;
           gFsShellAppUtilsRxBuf[10]=0x4d;
           gFsShellAppUtilsRxBuf[11]=0x4c;
           gFsShellAppUtilsRxBuf[12]=0x4f;

           mode = command;
       }
       ////////////////////////////////////
       gFsShellAppUtilsCurState = FS_SHELL_APP_UTILS_STATE_EXECUTE_COMMAND;
       retStat = CSL_SOK;
   }
   else
   {
       printf("oshibka\n");
   }
   if(FR_OK == f_opendir(&gFsShellAppUtilsDirObj, gFsShellAppUtilsCwd))
   {
       FSShellAppUtilsCmdExecute(gFsShellAppUtilsRxBuf, gFsShellAppUtilsCmdTable);
       gFsShellAppUtilsCurState = FS_SHELL_APP_UTILS_STATE_HELP;
       retStat = CSL_SOK;
   }
    return retStat;
}

/* -------------------------------------------------------------------------- */
/*                 Internal Function Definitions                              */
/* -------------------------------------------------------------------------- */

int32_t FSShellAppUtilsCmdExecute(uint8_t *pCmdLine,
                                         fsShellAppUtilsCmdEntry_t *pCmdList)
{
    int32_t retStatus = CSL_ESYS_FAIL;
    static uint8_t *argv[FSSHELLAPPUTILS_CMDLINE_MAX_ARGS + 1U];
    uint8_t *pChar;
    int32_t argc;
    uint32_t findArg = TRUE;

    /*
	 * Initialize the argument counter, and point to the beginning of the
     * command line string.
	 */
    argc = 0U;
    pChar = pCmdLine;

    /* Advance through the command line until a zero character is found. */
    while(*pChar)
    {
        /*
		 * If there is a space, then replace it with a zero, and set the flag
         * to search for the next argument.
		 */
        if(*pChar == ' ')
        {
            *pChar = 0;
            findArg = TRUE;
        }

        /*
         * Otherwise it is not a space, so it must be a character that is part
         * of an argument.
		 */
        else
        {
            /*
			 * If findArg is set, then that means we are looking for the start
             * of the next argument.
			 */
            if(TRUE == findArg)
            {
                /*
				 * As long as the maximum number of arguments has not been
                 * reached, then save the pointer to the start of this new arg
                 * in the argv array, and increment the count of args, argc.
				 */
                if(argc < FSSHELLAPPUTILS_CMDLINE_MAX_ARGS)
                {
                    argv[argc] = pChar;
                    argc++;
                    findArg = FALSE;
                    retStatus = CSL_SOK;
                }

                /*
				 * The maximum number of arguments has been reached so return
                 * the error.
				 */
                else
                {
                    FATFS_log("Too many arguments for command processor!\n");
                    retStatus = CSL_ESYS_FAIL;
                    break;
                }
            }
        }

        /* Advance to the next character in the command line. */
        pChar++;
    }

    /* If one or more arguments was found, then process the command. */
    if((CSL_SOK == retStatus) && (0U != argc))
    {
        /*
		 * Search through the command table until a null command string is
         * found, which marks the end of the table.
		 */
        while(pCmdList->pCmd)
        {
            /*
			 * If this command entry command string matches argv[0], then call
             * the function for this command, passing the command line
             * arguments.
			 */
            if(!strcmp((const char *)argv[0], (const char *)pCmdList->pCmd))
            {
                retStatus = pCmdList->pfnCmd(argc, (char **)argv);
            }

            /* Not found, so advance to the next entry. */
            pCmdList++;
        }
    }

    /*
	 * Fall through to here means that no matching command was found, so return
     * an error.
	 */
    return retStatus;
}

static int32_t FSShellAppUtilsFrmtPath(char* inputPath, char* outputPath)
{
    int32_t retStat = CSL_ESYS_FAIL;
    uint32_t drvLen = 0U; /* relative path */

    /* Append the input path. */
    strcpy(outputPath, "");

    /* Look for drive separator. */
    drvLen = strcspn(inputPath, ":");

    /*
     * If path contains driver name.
     */
    if (drvLen < strlen(inputPath))
    {
        /* Append the input path. */
        strcpy(outputPath, inputPath);

        retStat = CSL_SOK;
    }
    /*
     * If the first character is /, then this is a fully specified path, and it
     * should just be used as-is.
     */
    else
    {
        /* Look for drive separator. */
        drvLen = strcspn(gFsShellAppUtilsCwd, ":");

        /*
         * If path contains driver name.
         */
        if (drvLen < strlen(gFsShellAppUtilsCwd))
        {
            if('/' == *inputPath)
            {
                /*
                 * Make sure the new path with drive number [X:] is not bigger than the
                 * cwd buffer. It need to include a drive number, separator and a
                 * trailing null character.
                 */
                if((strlen(inputPath) + drvLen + 1U + 1U) <= sizeof(gFsShellAppUtilsCwd))
                {
                    if(0U != strlen(gFsShellAppUtilsCwd))
                    {
                        /* Copy the current drive into the output buffer. */
                        strncpy(outputPath, gFsShellAppUtilsCwd, drvLen + 1U);

                        outputPath[drvLen + 1U] = '\0';

                        if(strlen(inputPath) > 1U)
                        {
                            /* Append the input path. */
                            strcat(outputPath, inputPath);
                        }

                        retStat = CSL_SOK;
                    }
                }
                else
                {
                    retStat = CSL_ESYS_FAIL;
                }
            }
            else
            {
                /*
                 * Test to make sure that when the new additional path is added on to
                 * the current path, there is room in the buffer for the full new path.
                 * It needs to include a new separator, and a trailing null character.
                 */
                if((strlen(gFsShellAppUtilsCwd) + strlen(inputPath) + 1U + 1U) <= sizeof(gFsShellAppUtilsCwd))
                {
                    /*
                     * Append the current working directory to relative path.
                     */
                    strcat(outputPath, gFsShellAppUtilsCwd);
                    strcat(outputPath, "/");
                    strcat(outputPath, inputPath);

                    retStat = CSL_SOK;
                }
                else
                {
                    retStat = CSL_ESYS_FAIL;
                }
            }
        }
        else
        {
            retStat = FALSE;
        }
    }

    return retStat;
}

int32_t FSShellAppUtilsCmdLs(int32_t argc, char *argv[])
{
    uint32_t totalSize = 0U;
    uint32_t fileCount = 0U;
    uint32_t dirCount = 0U;
    FRESULT fresult;
    FATFS *pFatFs;

    /* Open the current directory for access. */
    fresult = f_opendir(&gFsShellAppUtilsDirObj, gFsShellAppUtilsCwd);

    /* Enter loop to enumerate through all directory entries. */
    while(FR_OK == fresult)
    {
        /* Read an entry from the directory. */
        fresult = f_readdir(&gFsShellAppUtilsDirObj, &gFsShellAppUtilsFileInfo);

        /* Check for error and return if there is a problem. */
        if(FR_OK == fresult)
        {
            /* If the file name is blank, then this is the end of the listing. */
            if('\0' == gFsShellAppUtilsFileInfo.fname[0])
            {
                break;
            }

            /* If the attribute is directory, then increment the directory count. */
            if(AM_DIR == (gFsShellAppUtilsFileInfo.fattrib & AM_DIR))
            {
                dirCount++;
            }

            /*
             * Otherwise, it is a file.  Increment the file count, and add in the
             * file size to the total.
             */
            else
            {
                fileCount++;
                totalSize += gFsShellAppUtilsFileInfo.fsize;
            }

            /*
             * Print the entry information on a single line with formatting to show
             * the attributes, date, time, size, and name.
             */
            FATFS_log("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9u  %s\n",
                               (gFsShellAppUtilsFileInfo.fattrib & AM_DIR) ? 'D' : '-',
                               (gFsShellAppUtilsFileInfo.fattrib & AM_RDO) ? 'R' : '-',
                               (gFsShellAppUtilsFileInfo.fattrib & AM_HID) ? 'H' : '-',
                               (gFsShellAppUtilsFileInfo.fattrib & AM_SYS) ? 'S' : '-',
                               (gFsShellAppUtilsFileInfo.fattrib & AM_ARC) ? 'A' : '-',
                               (gFsShellAppUtilsFileInfo.fdate >> 9) + 1980,
                               (gFsShellAppUtilsFileInfo.fdate >> 5) & 15,
                                gFsShellAppUtilsFileInfo.fdate & 31,
                               (gFsShellAppUtilsFileInfo.ftime >> 11),
                               (gFsShellAppUtilsFileInfo.ftime >> 5) & 63,
                                gFsShellAppUtilsFileInfo.fsize,
                                gFsShellAppUtilsFileInfo.fname);
        }
    }

    /* Check for error and return if there is a problem. */
    if(FR_OK == fresult)
    {
        /* Print summary lines showing the file, dir, and size totals. */
        FATFS_log("\n%4u File(s),%10u bytes total\n%4u Dir(s)",
                           fileCount, totalSize, dirCount);

        /* Get the free space. */
        fresult = f_getfree("/", (DWORD *)&totalSize, &pFatFs);

        /* Check for error and return if there is a problem. */
        if(FR_OK == fresult)
        {
            /* Display the amount of free space that was calculated. */
            FATFS_log(", %10uK bytes free\n", totalSize * pFatFs->csize / 2);
        }
    }

    /* Return status. */
    return fresult;
}

int32_t FSShellAppUtilsCmdRm(int32_t argc, char *argv[])
{
    int32_t retStat = CSL_ESYS_FAIL;
    FRESULT fresult;

    /*
     * Format the path of file/folder to be removed with drive and absolute path.
     */
    retStat = FSShellAppUtilsFrmtPath(argv[1U], gFsShellAppUtilsTempPath);

    if (CSL_SOK == retStat)
    {
        /* Remove file or directory path is in gFsShellAppUtilsTempPath. */
        fresult = f_unlink(gFsShellAppUtilsTempPath);

        /* Check status. Inform user and return. */
        if(fresult != FR_OK)
        {
            retStat = CSL_ESYS_FAIL;
        }
    }

    /* Return status. */
    return retStat;
}

int32_t FSShellAppUtilsCmdMkDir(int32_t argc, char *argv[])
{
    int32_t retStat = CSL_ESYS_FAIL;
    FRESULT fresult;

    /*
     * Format the path of folder to be created with drive and absolute path.
     */
    retStat = FSShellAppUtilsFrmtPath(argv[1U], gFsShellAppUtilsTempPath);

    if (CSL_SOK == retStat)
    {
        /* Create new directory at the path in chTmpBuf. */
        fresult = f_mkdir(gFsShellAppUtilsTempPath);

        /* Check for the status of create directory.  Inform user and return. */
        if(FR_OK != fresult)
        {
            FATFS_log("mkdir: %s\n", gFsShellAppUtilsTempPath);
            retStat = CSL_ESYS_FAIL;
        }
    }

    /* Return status. */
    return retStat;
}

int32_t FSShellAppUtilsCmdCd(int32_t argc, char *argv[])
{
    int32_t retStat = CSL_ESYS_FAIL;
    FRESULT fresult;

    /*
     * Format the path of folder to be created with drive and absolute path.
     */
    retStat = FSShellAppUtilsFrmtPath(argv[1U], gFsShellAppUtilsTempPath);

    if (CSL_SOK == retStat)
    {
        /*
         * At this point, a candidate new directory path is in chTmpBuf.  Try to
         * open it to make sure it is valid.
         */
        fresult = f_opendir(&gFsShellAppUtilsDirObj, gFsShellAppUtilsTempPath);

        /* If it can't be opened, then it is a bad path.  Inform user and return. */
        if(FR_OK != fresult)
        {
            FATFS_log("cd: %s\n", gFsShellAppUtilsTempPath);
            retStat = CSL_ESYS_FAIL;
        }

        /* Otherwise, it is a valid new path, so copy it into the CWD. */
        else
        {
            strncpy(gFsShellAppUtilsCwd, gFsShellAppUtilsTempPath, sizeof(gFsShellAppUtilsCwd));
        }
    }

    /* Return status. */
    return retStat;
}

int32_t FSShellAppUtilsCmdPwd(int32_t argc, char *argv[])
{
    /* Print the CWD to the console. */
    FATFS_log("%s\n", gFsShellAppUtilsCwd);

    return CSL_SOK;
}
int32_t FSShellAppUtilsCmdCat(int32_t argc, char *argv[])
{
    uint32_t bytesWrite = 0;
    uint32_t usBytesRead = 0;
    int32_t retStat = CSL_ESYS_FAIL;
    strcpy(gFsShellAppUtilsTempPath, "");

    retStat = FSShellAppUtilsFrmtPath(argv[1U], gFsShellAppUtilsTempPath);

    if ((mode == 1)||(mode == 2))
    {
        f_open(&gFsShellAppUtilsReadFileObj, gFsShellAppUtilsTempPath, FA_READ);
    }


    if ((mode == 3)||(mode == 4))
    {
        if(argc >= 4)
        {
            if(0U == strcmp(argv[2], ">"))
            {
                strcpy(gFsShellAppUtilsTempPath, "");
                retStat = FSShellAppUtilsFrmtPath(argv[3U], gFsShellAppUtilsTempPath);

                if (CSL_SOK == retStat)
                {
                    f_open(&gFsShellAppUtilsWriteFileObj, gFsShellAppUtilsTempPath, FA_WRITE|FA_OPEN_ALWAYS);
                }
                gFsShellAppUtilsWriteFileObj.obj.objsize = size;
            }
        }
    }

    int ind = 0;
    while(1)
    {
        //чтение указанного файла
        if (mode == 1 || mode == 2)
        {
            f_read(&gFsShellAppUtilsReadFileObj, gFsShellAppUtilsDataBuf, sizeof(gFsShellAppUtilsDataBuf) - 1, &usBytesRead);

            for(int j = 0; (j < sizeof(gFsShellAppUtilsDataBuf) - 1) ; j++)
            {
                file_boot_loader[ind++] = gFsShellAppUtilsDataBuf[j];
            }

            if(gFsShellAppUtilsReadFileObj.obj.objsize == gFsShellAppUtilsReadFileObj.fptr)
            {
                if (mode ==  1)
                {
                    printf("\n app read ok, size = %d bites; \n", (int)gFsShellAppUtilsReadFileObj.obj.objsize);
                    size = gFsShellAppUtilsReadFileObj.obj.objsize;
                    length = (uint32_t)(size/511);
                }

                if (mode ==  2)
                {
                    printf("\n MLO read ok, size = %d bites; \n", (int)gFsShellAppUtilsReadFileObj.obj.objsize);
                    size = gFsShellAppUtilsReadFileObj.obj.objsize;
                    length = (uint32_t)(size/511);
                }
                f_close(&gFsShellAppUtilsReadFileObj);
                break;
            }
        }

        //запись указанного файла
        if(mode == 3 || mode == 4)
        {

            if (length == 0)
            {
                for(int j = 0; j < size ; j++)
                {
                    gFsShellAppUtilsDataBuf[j] = file_boot_loader[ind++];
                }
                f_write(&gFsShellAppUtilsWriteFileObj, gFsShellAppUtilsDataBuf, size, &bytesWrite);
            }
            else
            {
                for(int j = 0; j < 511 ; j++)
                {
                    gFsShellAppUtilsDataBuf[j] = file_boot_loader[ind++];
                }
                f_write(&gFsShellAppUtilsWriteFileObj, gFsShellAppUtilsDataBuf, 511, &bytesWrite);
                size -= 511;
                length -= 1;
            }

            //if(gFsShellAppUtilsWriteFileObj.obj.objsize == gFsShellAppUtilsWriteFileObj.fptr)
            if(length == 0)
            {
                if (mode ==  3)
                {
                    printf("\n app write ok, size = %d bites\n", (int)gFsShellAppUtilsWriteFileObj.obj.objsize);
                }

                if (mode ==  4)
                {
                    printf("\n MLO write ok, size = %d bites\n", (int)gFsShellAppUtilsWriteFileObj.obj.objsize);
                }

                f_close(&gFsShellAppUtilsWriteFileObj);
                break;
            }
        }
    }
    return retStat;
}


int32_t FSShellAppUtilsCmdHelp(int32_t argc, char *argv[])
{
    fsShellAppUtilsCmdEntry_t *pEntry;

    /*
    ** Print some header text.
    */
    FATFS_log("\nAvailable commands\n");
    FATFS_log("------------------\n");

    /*
    ** Point at the beginning of the command table.
    */
    pEntry = &gFsShellAppUtilsCmdTable[0];

    /*
    ** Enter a loop to read each entry from the command table.  The end of the
    ** table has been reached when the command name is NULL.
    */
    while(pEntry->pCmd)
    {
        /*
        ** Print the command name and the brief description.
        */
        FATFS_log("%s%s\n", pEntry->pCmd, pEntry->pHelp);

        /*
        ** Advance to the next entry in the table.
        */
        pEntry++;
    }

    /* Return success. */
    return(0);
}

