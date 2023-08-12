/*
 * Shared.h
 *
 *  Created on: 10 сент. 2020 г.
 *
 */

#ifndef SHARED_MEM_COMMUNICATION_SHARED_H_
#define SHARED_MEM_COMMUNICATION_SHARED_H_

#include <xdc/std.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/family/arm/a15/Cache.h>

#define base_addr  (0xA3000000)

#define word00      (*((uint32_t *) (base_addr + 0x00000000))) // cmd       /all items for DSP0/
#define word01      (*((uint32_t *) (base_addr + 0x00000004))) // data in
#define word02      (*((uint32_t *) (base_addr + 0x00000008))) // data out
#define word03      (*((uint32_t *) (base_addr + 0x0000000C))) // N fft
#define word04      (*((uint32_t *) (base_addr + 0x00000010)))
#define word05      (*((uint32_t *) (base_addr + 0x00000014)))
#define word06      (*((uint32_t *) (base_addr + 0x00000018)))
#define word07      (*((uint32_t *) (base_addr + 0x0000001C)))
#define word08      (*((uint32_t *) (base_addr + 0x00000020)))
#define word09      (*((uint32_t *) (base_addr + 0x00000024)))
#define word0a      (*((uint32_t *) (base_addr + 0x00000028)))
#define word0b      (*((uint32_t *) (base_addr + 0x0000002C)))
#define word0c      (*((uint32_t *) (base_addr + 0x00000030)))
#define word0d      (*((uint32_t *) (base_addr + 0x00000034)))
#define word0e      (*((uint32_t *) (base_addr + 0x00000038)))
#define word0f      (*((uint32_t *) (base_addr + 0x0000003C)))

#define word10      (*((uint32_t *) (base_addr + 0x00000040)))  // cmd  / all items for DSP1 /
#define word11      (*((uint32_t *) (base_addr + 0x00000044)))  // data in
#define word12      (*((uint32_t *) (base_addr + 0x00000048))) // data out
#define word13      (*((uint32_t *) (base_addr + 0x0000004C)))  // N fft
#define word14      (*((uint32_t *) (base_addr + 0x00000050)))
#define word15      (*((uint32_t *) (base_addr + 0x00000054)))
#define word16      (*((uint32_t *) (base_addr + 0x00000058)))
#define word17      (*((uint32_t *) (base_addr + 0x0000005C)))
#define word18      (*((uint32_t *) (base_addr + 0x00000060)))
#define word19      (*((uint32_t *) (base_addr + 0x00000064)))
#define word1a      (*((uint32_t *) (base_addr + 0x00000068)))
#define word1b      (*((uint32_t *) (base_addr + 0x0000006C)))
#define word1c      (*((uint32_t *) (base_addr + 0x00000070)))
#define word1d      (*((uint32_t *) (base_addr + 0x00000074)))
#define word1e      (*((uint32_t *) (base_addr + 0x00000078)))
#define word1f      (*((uint32_t *) (base_addr + 0x0000007C)))


extern float     rxDSPBuf0[2048];
extern float     rxDSPBuf1[2048];

extern float     txDSPBuf0[2048];
extern float     txDSPBuf1[2048];
extern float     txDSPBuf2[2048];
extern float     txDSPBuf3[2048];
extern float     txDSPBuf4[2048];
extern float     txDSPBuf5[2048];

extern float     BearingBuf01[25000];
extern float     BearingBuf00[25000];
extern float     BearingBuf02[25000];
extern float     BearingBuf03[25000];
extern float     BearingBuf04[25000];


extern float     BearingBuf10[25000];
extern float     BearingBuf11[25000];
extern float     BearingBuf12[25000];
extern float     BearingBuf13[25000];
extern float     BearingBuf14[25000];


typedef enum {

    SPECTRUM_POWER_DSP0 = 0x00010001,
    SPECTRUM_DSP0 = 0x00010002,
    BEARING_DSP0        = 0x00010003,
    BEARING_SW_ANT_1_DSP0 = 0x00010004,
    BEARING_USW_ANT_2_3_DSP0 = 0x00010005,
    SPECTRUM_POWER_DSP1 = 0x00020001,
    SPECTRUM_DSP1 = 0x00020002,
    BEARING_DSP1        = 0x00020003,
    BEARING_SW_ANT_1_DSP1 = 0x00020004,
    BEARING_USW_ANT_2_3_DSP1 = 0x00020005,
}calculation_mode;

typedef enum {
    DSP0_ = 0x0,
    DSP1_ = 0x1,
}core_;

void sh_mem_send_time_seq(calculation_mode mode, uint32_t *buf_in, uint32_t *buf_out, uint32_t N_dots);
void sh_mem_bearing(calculation_mode mode, uint32_t N_dots);
void sh_mem_recieve(core_ core);


#endif /* SHARED_MEM_COMMUNICATION_SHARED_H_ */
