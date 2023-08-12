/*
 *  ======== ipc.cfg.xs ========
 */

/* root of the configuration object model */
var Program = xdc.useModule('xdc.cfg.Program');
var cfgArgs = Program.build.cfgArgs;

/* configure processor names */
var procNameAry = ["HOST","DSP1","DSP2"];
var MultiProc = xdc.useModule('ti.sdo.utils.MultiProc');
MultiProc.setConfig(xdc.global.procName, procNameAry);

/* ipc configuration */
var Ipc = xdc.useModule('ti.sdo.ipc.Ipc');
//Ipc.procSync = Ipc.ProcSync_PAIR;
Ipc.procSync = Ipc.ProcSync_ALL;
Ipc.sr0MemorySetup = true;

/* shared region configuration */
var SharedRegion = xdc.useModule('ti.sdo.ipc.SharedRegion');

/* configure SharedRegion #0 (IPC) */
var SR0Mem = Program.cpu.memoryMap["SR_0"];

SharedRegion.setEntryMeta(0,
    new SharedRegion.Entry({
        name:           "SR_0",
        base:           SR0Mem.base,
        len:            SR0Mem.len,
        ownerProcId:    0,
        isValid:        true,
        cacheEnable:    false,
    })
);
