////HEADERS Files
//Integrated C Libraries


//SDL Libraries
#include <SDL2/SDL.h>




////ALL SERVICES FUNCTIONS: SDL_Window_Manager.c




/*  create_window();
Description:
  Creates a window with SDL, stores it inside the given
  windows tracking dock and returns the window.

Return: SDL_Window

Parameters:
  char* title: Title of the newly created window
  unsigned int width: Width dimension of the created window
  unsigned int height: Height dimension of the created window
  SDL_Window** dock: Dock tracking all created windows. Must be resizable.

  Dvelopper mode:
  force: Force initialization of SDL through the improper canal. Can lead to errors.
*/
SDL_Window *create_window(char* title, unsigned int width, unsigned int height, SDL_Window** dock, int force) {
	////Initializing variables:
	//window: element to return and add to the tracking dock
	SDL_Window *window = NULL;

	

	////Function code

	//Checking if SDL is initialized
	if (!SDL_WasInit(SDL_INIT_VIDEO)) {
		//Force through the Canal Error
		if (force) {
			if(SDL_Init(SDL_INIT_VIDEO)!=0) {
				//Exiting with error to avoid backtracking bugs
				errx(1, "SDL Initializing failed in: create_window.");
			}
		}
		else
			fprintf(stderr, "Error (create_window): SDL has not been initialized. Cancelling request of window creation. Make sure the proper initialization functions have been called beforehand.\nUse --force to force the initialization of SDL.\n\nRequest details:\ntitle: %s\nwidth:%d\nheight%d\n");
	}


}


SDL_Renderer *create_renderer(SDL_Window *window) {
	
}


void close_window(SDL_Window *window, SDL_Renderer *renderer) {
	
}


void close_program(SDL_Window** windows, SDL_Renderer** renderers) {
	
}
