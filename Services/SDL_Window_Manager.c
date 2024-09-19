/*

      ##############################################################
      #                                                            #
      #                    SDL_Window_Manager.c                    #
      #                                                            #
      #         Contains all functions related to handling,        #
      #   modifying and supervising memory of SDL windows system.  #
      #
      ##############################################################


*/











////HEADERS Files
//Integrated C Libraries


//SDL Libraries
#include <SDL2/SDL.h>


//Project Headers





////ALL SERVICES FUNCTIONS: SDL_Window_Manager.c




/*  create_window();
  Creates a window with SDL and returns the window.

Requisites assumed:
  No other window must be running by the program.
  Returning window is the main project window.
  SDL and its sub-systems must be initialized beforehand.
*/
SDL_Window *create_window( //Parameters:
      char* title, //Title of the window
      int x, //Horizontal starting position or corresponding flags
      int y, //Vertical starting position or corresponding flags
      unsigned int width, //Width of the window
      unsigned int height, //Height of the window
      Uint32 flags) { //General flags of the window

  //window: element to return
  SDL_Window *window = NULL;

  //Creating the window
  window = SDL_CreateWindow(title, x, y, width, height, flags);

  //Checking for success
  if (!window)
    errx(EXIT_FAILURE, "create_window: Window creation request failed.");

  //Returning the created window
  return window;
}


SDL_Renderer *create_renderer(SDL_Window *window, Uint32 flags) {
  //renderer: element to return
  SDL_Renderer *renderer = NULL;

  //Creating the renderer
  renderer = SDL_CreateRenderer(window, -1, flags);

  //Checking for success
  if (!renderer)
    errx(EXIT_FAILURE, "create_renderer: Renderer creation request failed.");

  //Returning the created renderer
  return renderer;
}



void close_program(SDL_Window* window, SDL_Renderer* renderer) {
  //Destroy the renderer
  SDL_DestroyRenderer(renderer);
  //Destroy the main window (assumed to be the only one created)
  SDL_DestroyWindow(window);
  //Close the program (assumed all memory usage has been freed)
  SDL_QUIT();
}
