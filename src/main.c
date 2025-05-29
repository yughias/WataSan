#include "SDL_MAINLOOP.h"
#include "watara.h"

SDL_Window* apu_win;

watara_t watara;

void setup(){
    if(getArgc() != 2){
        printf("<exe> <rom filename>");
        exit(EXIT_FAILURE);
    }

    setTitle(u8"ワタさん");
    size(160, 160);
    setScaleMode(NEAREST);
    frameRate(NMI_RATE);

    watara_init(&watara, getArgv(1));

    SDL_AudioSpec audioSpec;
    audioSpec.callback = NULL;
    audioSpec.channels = 2;
    audioSpec.format = AUDIO_S8;
    audioSpec.freq = AUDIO_FREQ;
    audioSpec.samples = AUDIO_BUFFER_SIZE;
    watara.apu.audio_dev = SDL_OpenAudioDevice(NULL, 0, &audioSpec, &audioSpec, 0);
    watara.apu.push_reload = AUDIO_CYCLES_PER_SAMPLE;

    SDL_PauseAudioDevice(watara.apu.audio_dev, 0);
}

void loop(){
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    if(keystate[SDL_SCANCODE_F3]){
        Uint32 win_id = SDL_GetWindowID(apu_win);
        if(!win_id)
            apu_win = SDL_CreateWindow("APU", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
    }

    watara_run_frame(&watara);

    apu_draw_waves(&watara.apu, &apu_win);
}