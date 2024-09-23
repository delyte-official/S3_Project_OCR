#ifndef SDL_WINDOW_MANAGER_H
#define SDL_WINDOW_MANAGER_H

//Additional Headers requires for defining the imported types
#include <SDL2/SDL.h>


SDL_Window *create_window(char* title, unsigned int width, unsigned int height);
SDL_Renderer *create_renderer(SDL_Window *window);
void close_program(SDL_Window* window, SDL_Renderer *renderer);

#endif
