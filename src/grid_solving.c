#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_image.h>
#include <err.h>

SDL_Window *create_window(char *title, unsigned int width, unsigned int height)
{
	int init = SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (init != 0 || win == NULL)
	{
		return NULL;
	}
	else
	{
		return win;
	}
}


SDL_Renderer *create_renderer(SDL_Window *window)
{
	return SDL_CreateRenderer(window, 0, 0);
}


void close_program(SDL_Window *window, SDL_Renderer *renderer)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
}


int process_events()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			return 0;
		}
	}

	return 1;
}

int main()
{
    SDL_Window *window = create_window("grid_detect", 800, 500);

    if (!window)
        errx(EXIT_FAILURE, "Failed to create SDL window: %s", SDL_GetError());

    SDL_Renderer *renderer = create_renderer(window);

    if (!renderer)
        errx(EXIT_FAILURE, "Failed to create SDL renderer: %s", SDL_GetError());

    int keep_alive = 1;
    while (keep_alive)
    {
        keep_alive = process_events();
        SDL_RenderPresent(renderer);
    }

    close_program(window, renderer);
    return EXIT_SUCCESS;
}