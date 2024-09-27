#ifndef SDL_WINDOW_MANAGER_H
#define SDL_WINDOW_MANAGER_H

//Additional Headers requires for defining the imported types
#include <SDL2/SDL.h>


SDL_DisplayMode get_display_mode();
SDL_Window *create_window(char* title,
        int x,
        int y,
        unsigned int width,
        unsigned int height,
        Uint32 flags);
SDL_Renderer *create_renderer(
        SDL_Window *window,
        Uint32 flags);
void close_program(SDL_Window* window,
        SDL_Renderer *renderer);

#endif
