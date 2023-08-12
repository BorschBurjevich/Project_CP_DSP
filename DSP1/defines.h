/*
 * defines.h
 *
 *  Created on: 23 θών. 2021 γ.
 *
 */

#ifndef DEFINES_H_
#define DEFINES_H_

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





#endif /* DEFINES_H_ */
