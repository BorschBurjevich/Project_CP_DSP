#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/csl/soc.h>
#include <ti/csl/cslr_device.h>
#include <ti/ndk/inc/stkmain.h>
#include <ti/ndk/inc/netmain.h>
#include <ti/board/board.h>
#include <ti/drv/emac/emac_drv.h>
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/drv/emac/emac_drv.h>
#include <ti/drv/emac/src/v4/emac_drv_v4.h>
#include "gpio/gpio.h"
#include "delay/delay.h"
#include <ti/drv/gpio/GPIO.h>
#include <ti/drv/gpio/soc/GPIO_soc.h>

#include "Lorandit_functions/Structs.h"

//макросы
#define NIMU_log                printf
#define MAX_TABLE_ENTRIES   3U

//глобальные переменные
static int nimu_device_index = 0U;
NIMU_DEVICE_TABLE_ENTRY NIMUDeviceTable[MAX_TABLE_ENTRIES];
static char *VerStr = "\nTCP/IP Application\n\n";
extern Semaphore_Handle semen;
//static void* socet_1=0;
static void* socet_0=0;

/* Configuration */
static char *HostName    = "Lorandit";
static char *LocalIPAddr = "192.168.1.100";
static char *LocalIPMask = "255.255.255.0";    // Not used when using DHCP
static char *GatewayIP   = "192.168.1.1";     // Not used when using DHCP
static char *DomainName  = "demo.net";         // Not used when using DHCP
static char *DNSServer   = "0.0.0.0";          // Used when set to anything but zero

//объ€вление функций
extern int    arm_handler( SOCKET s, uint32_t unused );
//extern int    skl_handler( SOCKET s, uint32_t unused );

extern int    CpswEmacInit (STKEVENT_Handle hEvent);
static void   NetworkOpen();
static void   NetworkClose();
static void   NetworkIPAddr( uint32_t IPAddr, uint32_t IfIdx, uint32_t fAdd );
void          send_eth(SOCKET s, uint8_t *tx_buf,int size);
void          send_eth_ticket(SOCKET s, uint32_t com);

void ethernet()
{
    int            rc;
    void*          hCfg;
    CI_IPNET       NA;
    CI_ROUTE       RT;
    uint32_t       IPTmp;

    GPIO_init();
    GPIO_write(GPIO5_14, 0x0);
    mdelay(400);
    GPIO_write(GPIO5_14, 0x1);
    mdelay(1000);

    CSL_xbarMpuIrqConfigure(CSL_XBAR_INST_MPU_IRQ_92, CSL_XBAR_GMAC_SW_IRQ_RX_PULSE);
    CSL_xbarMpuIrqConfigure(CSL_XBAR_INST_MPU_IRQ_93, CSL_XBAR_GMAC_SW_IRQ_TX_PULSE);

    // Select RGMII 2 ports GMIIx_SEL = 2 for RGMII
    CSL_FINS (((CSL_control_coreRegs *) CSL_MPU_CTRL_MODULE_CORE_CORE_REGISTERS_REGS)->CONTROL_IO_1,
              CONTROL_CORE_CONTROL_IO_1_GMII1_SEL, 2U);
    CSL_FINS (((CSL_control_coreRegs *) CSL_MPU_CTRL_MODULE_CORE_CORE_REGISTERS_REGS)->CONTROL_IO_1,
              CONTROL_CORE_CONTROL_IO_1_GMII2_SEL, 2U);

    //GMAC RESET ISOLATION Enable
    CSL_FINS (((CSL_control_coreRegs *) CSL_MPU_CTRL_MODULE_CORE_CORE_REGISTERS_REGS)->CONTROL_IO_2,
              CONTROL_CORE_CONTROL_IO_2_GMAC_RESET_ISOLATION_ENABLE, 0U);
    CSL_FINS (((CSL_control_coreRegs *) CSL_MPU_CTRL_MODULE_CORE_CORE_REGISTERS_REGS)->CONTROL_IO_2,
              CONTROL_CORE_CONTROL_IO_2_GMAC_RESET_ISOLATION_ENABLE, 1U);

    NIMUDeviceTable[nimu_device_index++].init =  &CpswEmacInit;
    NIMUDeviceTable[nimu_device_index].init =  NULL;


 /*   EMAC_HwAttrs_V4 cfg;
    EMAC_socGetInitCfg(0, &cfg);

    //стойка
    cfg.port[0].phy_addr = 0x01;
    // cfg.port[1].phy_addr = 0x03;

    EMAC_socSetInitCfg(0, &cfg);*/
    // THIS MUST BE THE ABSOLUTE FIRST THING DONE IN AN APPLICATION before using the stack
    rc = NC_SystemOpen( NC_PRIORITY_LOW, NC_OPMODE_INTERRUPT );
    if( rc )
    {
        printf("NC_SystemOpen Failed (%d)\n",rc);
        for(;;);
    }

    hCfg = CfgNew();
    if( !hCfg )
    {
        printf("Unable to create configuration\n");
        goto main_exit;
    }
    UART_printf(VerStr);

    //Add our global hostname to hCfg (to be claimed in all connected domains)
    CfgAddEntry( hCfg, CFGTAG_SYSINFO, CFGITEM_DHCP_HOSTNAME, 0, strlen(HostName), (uint8_t *)HostName, 0 );

    printf("Task_Ethernet: using localIp\n");

    // Setup manual IP address
    memset(&NA, 0x00000000, sizeof(NA));
    NA.IPAddr  = inet_addr(LocalIPAddr);
    NA.IPMask  = inet_addr(LocalIPMask);
    strcpy( NA.Domain, DomainName );
    NA.NetType = 0;

    //Add the address to interface 1
    CfgAddEntry( hCfg, CFGTAG_IPNET, 1, 0, sizeof(CI_IPNET), (uint8_t *)&NA, 0 );

    memset(&NA, 0x00000000, sizeof(RT));
    RT.IPDestAddr = 0;
    RT.IPDestMask = 0;
    RT.IPGateAddr = inet_addr(GatewayIP);

    // Add the route
    CfgAddEntry( hCfg, CFGTAG_ROUTE, 0, 0, sizeof(CI_ROUTE), (uint8_t *)&RT, 0);

    // Manually add the DNS server when specified
    IPTmp = inet_addr(DNSServer);
    if( IPTmp )
    CfgAddEntry( hCfg, CFGTAG_SYSINFO, CFGITEM_DHCP_DOMAINNAMESERVER, 0, sizeof(IPTmp), (uint8_t *)&IPTmp, 0);

    // Configure IPStack/OS Options
    rc = DBG_WARN;
    CfgAddEntry( hCfg, CFGTAG_OS, CFGITEM_OS_DBGPRINTLEVEL, CFG_ADDMODE_UNIQUE, sizeof(uint32_t), (uint8_t *)&rc, 0 );


    // This code sets up the TCP and UDP buffer sizes
    //(Note 8192 is actually the default. This code is here to
    //illustrate how the buffer and limit sizes are configured.)

    // TCPIP Receive limit
    rc = 8192;
    CfgAddEntry( hCfg, CFGTAG_IP, CFGITEM_IP_SOCKTCPRXLIMIT, CFG_ADDMODE_UNIQUE, sizeof(uint32_t), (uint8_t *)&rc, 0 );

    // increase stack size
    rc = 8192;
    CfgAddEntry(hCfg, CFGTAG_OS, CFGITEM_OS_TASKSTKBOOT, CFG_ADDMODE_UNIQUE, sizeof(uint32_t), (uint8_t *)&rc, 0 );


    // Boot the system using this configuration
    // We keep booting until the function returns 0. This allows
    // us to have a "reboot" command.
    do
    {
        rc = NC_NetStart( hCfg, NetworkOpen, NetworkClose, NetworkIPAddr );
    } while( rc > 0 );

    // Delete Configuration
    CfgFree( hCfg );

main_exit:
    printf("Task_Ethernet: exiting\n");
    NC_SystemClose();
}


static void NetworkOpen()
{
    socet_0 = DaemonNew( SOCK_STREAMNC, 0, 707, arm_handler, OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3);//TCPIP
  //  socet_1 = DaemonNew( SOCK_STREAMNC, 0, 404, skl_handler, OS_TASKPRINORM, OS_TASKSTKNORM, 0, 3);//TCPIP
}

static void NetworkClose()
{
    DaemonFree( socet_0 );
 //   DaemonFree( socet_1 );
}

static void NetworkIPAddr( uint32_t IPAddr, uint32_t IfIdx, uint32_t fAdd )
{
    uint32_t IPTmp;

    if( fAdd )
        printf("Network Added: ");
    else
        printf("Network Removed: ");

    // Print a message
    IPTmp = NDK_ntohl( IPAddr );
    printf("If-%d:%d.%d.%d.%d\n", (uint8_t)IfIdx, (uint8_t)(IPTmp>>24)&0xFF, (uint8_t)(IPTmp>>16)&0xFF, (uint8_t)(IPTmp>>8)&0xFF, (uint8_t)IPTmp&0xFF );
}

void send_eth(SOCKET s, uint8_t *tx_buf,int size)
{
    fdOpenSession(TaskSelf());
    send( s, tx_buf, size, 0);
    fdCloseSession(TaskSelf());
}

void send_eth_ticket(SOCKET s, uint32_t com)
{

    uint8_t buf[12];
    uint32_t k = 0;

    hat.n_bytes = sizeof(hat.command)+sizeof(hat.command);
    memcpy(&buf[k], &hat.n_bytes, sizeof(hat.n_bytes));
    k += sizeof(hat.n_bytes);

    hat.command = 0x0001;                                   // квитирование от €чейки
    memcpy(&buf[k], &hat.command, sizeof(hat.command));
    k += sizeof(hat.command);

    memcpy(&buf[k], &com, sizeof(hat.command));
    k += sizeof(hat.command);

    fdOpenSession(TaskSelf());
    send( s, buf, k, 0);
    fdCloseSession(TaskSelf());
}
void send_eth_error(SOCKET s, uint32_t error)
{
    uint8_t buf[10]={0x00, 0x00, 0x00, 0x06, 0x00, 0x02};
    memcpy(&buf[6], &error, 4);

    fdOpenSession(TaskSelf());
    send( s, buf, 10, 0);
    fdCloseSession(TaskSelf());
}
