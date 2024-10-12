#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat *format)
{
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);
    
    // Calculate the grayscale value
    Uint8 gray = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);
    
    return SDL_MapRGB(format, gray, gray, gray);
}

Uint32 pixel_to_black_and_white(Uint32 pixel_color, SDL_PixelFormat *format, Uint8 threshold)
{
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);
    
    // Use the grayscale value for thresholding
    Uint8 gray = r;  // After conversion to grayscale, all channels are equal

    // Set threshold for black and white conversion
    if (gray > threshold)
        return SDL_MapRGB(format, 255, 255, 255); // White
    else
        return SDL_MapRGB(format, 0, 0, 0); // Black
}

void surface_to_black_and_white(SDL_Surface *surface, Uint8 threshold)
{
    if (SDL_LockSurface(surface) != 0)
    {
        fprintf(stderr, "SDL_LockSurface Error: %s\n", SDL_GetError());
        return;
    }

    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int width = surface->w;
    int height = surface->h;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Uint32 pixel_color = pixels[(y * width) + x];
            // First convert to grayscale
            pixel_color = pixel_to_grayscale(pixel_color, format);
            // Then convert to black and white
            pixels[(y * width) + x] = pixel_to_black_and_white(pixel_color, format, threshold);
        }
    }

    SDL_UnlockSurface(surface);
}

int count_neighbors(Uint32 *pixels, int width, int height, int x, int y, Uint32 target_color)
{
    int count = 0;
    int neighbors[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1}, {1, 0}, {1, 1}
    };

    for (int i = 0; i < 8; i++)
    {
        int nx = x + neighbors[i][0];
        int ny = y + neighbors[i][1];

        // Check if neighbor is within bounds
        if (nx >= 0 && nx < width && ny >= 0 && ny < height)
        {
            if (pixels[ny * width + nx] == target_color)
            {
                count++;
            }
        }
    }

    return count;
}

void clean_isolated_pixels(SDL_Surface *surface)
{
    if (SDL_LockSurface(surface) != 0)
    {
        fprintf(stderr, "SDL_LockSurface Error: %s\n", SDL_GetError());
        return;
    }

    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int width = surface->w;
    int height = surface->h;

    // Define the colors for black and white
    Uint32 black_color = SDL_MapRGB(format, 0, 0, 0);
    Uint32 white_color = SDL_MapRGB(format, 255, 255, 255);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // Check if the pixel is black
            if (pixels[y * width + x] == black_color)
            {
                // Count the number of white neighbors
                int white_neighbors = count_neighbors(pixels, width, height, x, y, white_color);
                
                // If the majority of neighbors are white, change the pixel to white
                if (white_neighbors >= 5)  // Example threshold for majority
                {
                    pixels[y * width + x] = white_color;
                }
            }
        }
    }

    SDL_UnlockSurface(surface);
}

int main(int argc, char** argv)
{
    if (argc != 3)
        errx(EXIT_FAILURE, "Usage: image-file threshold");

    // Parse the threshold value from the command-line arguments
    Uint8 threshold = (Uint8)atoi(argv[2]);
    if (threshold > 255)  // Ensure threshold is between 0 and 255
        errx(EXIT_FAILURE, "Threshold must be between 0 and 255");

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Window* window = SDL_CreateWindow("Black and White Converter", 0, 0, 0, 0, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* t = IMG_Load(argv[1]);
    if (t == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface *surface = SDL_ConvertSurfaceFormat(t, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(t);
    if (surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_SetWindowSize(window, surface->w, surface->h);

    surface_to_black_and_white(surface, threshold);
    clean_isolated_pixels(surface);  // Clean isolated pixels

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    while (1)
    {
        SDL_WaitEvent(&event);
        switch (event.type)
        {
            case SDL_QUIT:
                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return EXIT_SUCCESS;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);
                }
                break;
        }
    }
    return EXIT_SUCCESS;
}
