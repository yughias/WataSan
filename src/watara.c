#include "watara.h"

#include "SDL_MAINLOOP.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WATARA_STEP(func) \
int cycles = cpu->cycles; \
func (cpu); \
int elapsed = cpu->cycles - cycles; \
tmr_tick(tmr, w->ctrl, elapsed); \
apu_step(apu, elapsed); \
apu_push_sample(apu, elapsed); \
if(w65c02_interrupt_enabled(cpu) && ((tmr->irq | (apu->ch3.irq << 1)) & w->ctrl)) \
    w65c02_irq(cpu);

static u8 watara_get_controller();

void watara_init(watara_t* w, const char* filename){
    memset(w, 0, sizeof(watara_t));

    FILE* fptr = fopen(filename, "rb");
    if(!fptr){
        printf("cannot open rom...\n");
        exit(EXIT_FAILURE);
    }

    fseek(fptr, 0, SEEK_END);
    w->rom_size = ftell(fptr);
    rewind(fptr);

    w->rom = (u8*)malloc(w->rom_size);
    fread(w->rom, 1, w->rom_size, fptr);   

    w->cpu.ctx = (void*)w;
    w->cpu.read = watara_read;
    w->cpu.write = watara_write;
    w65c02_init(&w->cpu);
    w65c02_reset(&w->cpu);

    w->apu.noise.lfsr = 0x7FFF;
    w->apu.read = watara_read;
    w->apu.ctx = (void*)w;

    w->dma.read = watara_read;
    w->dma.write = watara_write;
    w->dma.ctx = (void*)w;
}

void watara_run_frame(watara_t* w){
    w65c02_t* cpu = &w->cpu;
    tmr_t* tmr = &w->tmr;
    apu_t* apu = &w->apu;

    while(cpu->cycles < CYCLES_BEFORE_NMI){
        WATARA_STEP(w65c02_step);
    }

    cpu->cycles -= CYCLES_BEFORE_NMI;

    if(w->ctrl & 1){
        WATARA_STEP(w65c02_nmi);
    }

    screen_update(&w->lcd);
}

u8 watara_read(void* ctx, u16 addr){
    watara_t* w = (watara_t*)ctx;

    if(addr < 0x2000)
        return w->ram[addr];

    if(addr == 0x2020){
        return watara_get_controller();
    }

    if(addr == 0x2024){
        w->tmr.irq = false;
        return 0xFF;
    }

    if(addr == 0x2025){
        w->apu.ch3.irq = false;
        return 0xFF;
    }

    if(addr == 0x2027){
        u8 irq_status = 0;
        irq_status |= w->tmr.irq | (w->apu.ch3.irq << 1);
        return irq_status;
    }

    if(addr >= 0x4000 && addr < 0x6000)
        return w->lcd.vram[addr - 0x4000];

    if(addr >= 0x8000){
        u8 bank = addr & (1 << 14) ? 0b111 : (w->apu.ch3.dma_active ? (w->apu.ch3.ctrl >> 4) & 0b111 :w->ctrl >> 5);
        u32 rom_addr = addr & ((1 << 14) - 1);
        rom_addr |= bank << 14;
        return w->rom[rom_addr % w->rom_size];
    }

    //printf("invalid read %X\n", addr);
    //exit(EXIT_FAILURE);
    return 0xFF;
}

void watara_write(void* ctx, u16 addr, u8 byte){
    watara_t* w = (watara_t*)ctx;

    if(addr < 0x2000){
        w->ram[addr] = byte;
        return;
    }

    if(addr >= 0x4000 && addr < 0x6000){
        w->lcd.vram[addr - 0x4000] = byte;
        return;
    }

    if(addr == 0x2000){
        w->lcd.x_size = byte;
        return;
    }

    
    if(addr == 0x2001){
        w->lcd.y_size = byte;
        return;
    }

    
    if(addr == 0x2002){
        w->lcd.x_scroll = byte;
        return;
    }
    
    if(addr == 0x2003){
        w->lcd.y_scroll = byte;
        return;
    }

    if(addr == 0x2008){
        w->dma.src_lo = byte;
        return;
    }

    if(addr == 0x2009){
        w->dma.src_hi = byte;
        return;
    }

    if(addr == 0x200A){
        w->dma.dst_lo = byte;
        return;
    }

    if(addr == 0x200B){
        w->dma.dst_hi = byte;
        return;
    }

    if(addr == 0x200C){
        w->dma.len = byte;
        return;
    }

    if(addr == 0x200D){
        if(byte & 0x80)
            dma_trigger(&w->dma);
        return;
    }

    if(addr)

    if(addr == 0x2010){
        w->apu.waves[0].flow = byte;
        return;
    }

    if(addr == 0x2011){
        w->apu.waves[0].fhi = byte;
        return;
    }
    
    if(addr == 0x2012){
        w->apu.waves[0].vol_duty = byte;
        return;
    }

    if(addr == 0x2013){
        w->apu.waves[0].length = byte;
        return;
    }
    
    if(addr == 0x2014){
        w->apu.waves[1].flow = byte;
        return;
    }

    if(addr == 0x2015){
        w->apu.waves[1].fhi = byte;
        return;
    }
    
    if(addr == 0x2016){
        w->apu.waves[1].vol_duty = byte;
        return;
    }

    if(addr == 0x2017){
        w->apu.waves[1].length = byte;
        return;
    }

    if(addr == 0x2018){
        w->apu.ch3.addr_lo = byte;
        return;
    }

    if(addr == 0x2019){
        w->apu.ch3.addr_hi = byte;
        return;
    }

    if(addr == 0x201A){
        w->apu.ch3.length = byte;
        return;
    }

    if(addr == 0x201B){
        w->apu.ch3.ctrl = byte;
        return;
    }

    if(addr == 0x201C){
        w->apu.ch3.trigger = true;
        return;
    }

    if(addr == 0x2021){
        w->ddr = byte & 0xF;
        return;
    }

    if(addr == 0x2022){
        w->link = byte & 0xF;
        return;
    }

    if(addr == 0x2023){
        w->tmr.ctr = byte;
        w->tmr.divider = 0;
        return;
    }

    if(addr == 0x2026){
        w->ctrl = byte;
        return;
    }

    if(addr == 0x2028){
        w->apu.noise.freq_vol = byte;
        return;
    }

    if(addr == 0x2029){
        w->apu.noise.length = byte;
        return;
    }

    if(addr == 0x202A){
        w->apu.noise.lfsr = 0x7FFF;
        w->apu.noise.ctrl = byte;
        return;
    }

    //printf("invalid write %02X:%X\n", addr, byte);
    //exit(EXIT_FAILURE);
}

static u8 watara_get_controller(){
    const SDL_Scancode keys[8] = {
        SDL_SCANCODE_RIGHT, SDL_SCANCODE_LEFT, SDL_SCANCODE_DOWN, SDL_SCANCODE_UP,
        SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_RSHIFT, SDL_SCANCODE_RETURN
    };

    u8 out = 0xFF;
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    for(int i = 0; i < 8; i++)
        out &= ~( ((bool)keystate[keys[i]]) << i );

    return out;
}