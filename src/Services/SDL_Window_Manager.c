/*

      ##############################################################
      #                                                            #
      #                    SDL_Window_Manager.c                    #
      #                                                            #
      #         Contains all functions related to handling,        #
      #   modifying and supervising memory of SDL windows system.  #
      #                                                            #
      ##############################################################


List of all functions written in this file (and their type):
[See more description on their purpose and parameters down below]

SDL_Window *create_window(char*,int,int,unsigned int,unsigned int,Uint32);
SDL_Renderer *renderer(SDL_Window*, Uint32);
void close_program(SDL_Window*,SDL_Renderer*);
*/











////HEADERS Files
//Integrated C Libraries


//SDL Libraries
#include <SDL2/SDL.h>


//Project Headers


////END HEADERS





/*  create_window():
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





/*  create_renderer():
  Creates a renderer with SDL and returns the renderer.

Requisites assumed:
  No other renderer must be associated with the given window.
  Returning renderer is the renderer of the main project window.
  SDL and its sub-systems must be initialized beforehand.
*/
SDL_Renderer *create_renderer( //Parameters:
      SDL_Window *window, //Associated window
      Uint32 flags) { //Flags for the renderer

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




/*  close_program():
  Destroy the main window and its associated renderer, then closes
  SDL and its systems altogether before allowing to exit the program.

Requisites assumed:
  No other windows or renderer have been created.
  Given window and renderer must have been correctly tracked at their creation.
  Any other memory management must have been freed beforehand.
  Closing the final program must be done through the main.c file afterwards.
*/
void close_program(
      SDL_Window* window, //Main project window
      SDL_Renderer* renderer) { //Renderer associated to the given window.

  //Destroy the renderer
  SDL_DestroyRenderer(renderer);
  //Destroy the main window (assumed to be the only one created)
  SDL_DestroyWindow(window);
  //Close the program (assumed all memory usage has been freed)
  SDL_QUIT();
}
