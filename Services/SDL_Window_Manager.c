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
  int x: Window's horizontal starting position or corresponding flags
  int y: Window's vertical starting position or corresponding flags
  unsigned int width: Width dimension of the created window
  unsigned int height: Height dimension of the created window
  Uint32 flags: Window's general flags with supported sub-systems
  SDL_Window** dock: Dock tracking all created windows. Must be resizable.

  Dvelopper mode:
  force: Force initialization of SDL through the improper canal. Can lead to errors.
*/
SDL_Window *create_window(char* title, int x, int y, unsigned int width, unsigned int height, Uint32 flags, SDL_Window** dock, int force) {
	
	////CHECKING REQUIREMENTS
	//Checking if SDL is initialized
	if (!SDL_WasInit(SDL_INIT_VIDEO)) {
		if (!force) {
			fprintf(stderr, "Error (create_window): SDL has not been initialized. Cancelling request of window creation. Make sure the proper initialization functions have been called beforehand.\nUse --force to force the initialization of SDL.\n\nRequest details:\ntitle: %s\nwidth:%d\nheight%d\n");
		}
		//FORCE: Dump the main canal and uses force canal instead.
		else {
			if(SDL_Init(SDL_INIT_VIDEO)!=0) {
				//Exiting with error to avoid backtracking bugs
				errx(1, "SDL Initializing failed in: create_window.");
			}
		}


	////Initializing variables:
	//window: element to return and add to the tracking dock
	SDL_Window *window = NULL;

	
	//Creating the window
	window = SDL_CreateWindow(title, x, y, width, height, flags);

	//Tracking the window for future memory management: adding it to the dock
	add_to(dock);

	//Returning the currently created window
	return window;
}


SDL_Renderer *create_renderer(SDL_Window *window) {
	
}


void close_window(SDL_Window *window, SDL_Renderer *renderer) {
	
}


void close_program(SDL_Window** windows, SDL_Renderer** renderers) {
	
}
