#ifndef __WATARA_H__
#define __WATARA_H__

#include "w65c02.h"
#include "lcd.h"
#include "tmr.h"
#include "apu.h"
#include "dma.h"

#define CYCLES_BEFORE_NMI 65536
#define NMI_RATE 61.04
#define RAM_SIZE 0x2000

typedef struct watara_t {
    w65c02_t cpu;
    lcd_t lcd;
    tmr_t tmr;
    apu_t apu;
    dma_t dma;

    u8* rom;
    size_t rom_size;

    u8 ram[RAM_SIZE];

    // regs 
    u8 ctrl;
    u8 ddr;
    u8 link;
} watara_t;

void watara_init(watara_t* w, const char* filename);
void watara_run_frame(watara_t* w);

u8 watara_read(void* ctx, u16 addr);
void watara_write(void* ctx, u16 addr, u8 byte);

#endif