#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <SDL2/SDL.h>

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int running;
} AppState;

extern AppState State;
void Memory_Init();

#endif
