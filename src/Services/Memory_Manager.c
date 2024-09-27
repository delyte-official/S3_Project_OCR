/*

      ##############################################################
      #                                                            #
      #                     Memory_Manager.c                       #
      #                                                            #
      #        Handles any memory management, from creation        #
      #     to freeing memory, to linking it to other systems.     #
      #                                                            #
      ##############################################################


List of all functions written in this file (and their type):
[See more description on their purpose and parameters down below]

Memory_Init();
*/











////HEADERS Files
//Integrated C Libraries


//Project Headers
#include "Memory_Manager.h"

////END HEADERS

////DEFINING
/*Application Singletone Structure (defined in the header)
typedef struct {
  SDL_Window *window; //Main Project Window
  SDL_Renderer *renderer; //Main Project Window's Renderer
  int running; //Switch to turn on/off the Application
} AppState

*///Global Application Singletone
AppState State;

////END DEFINING





/*  Memory_Init():
  Initialize the memory usage of the project and setup the Global
  singletone that will be used for easier linkage between project systems.

Requisites assumed:
  Any previous information written in the singletone will be overwritten.
  Every memory tracking is initialized in this function.
  Any linkage to the singletone State must be before its initialization.
*/
void Memory_Init() {
  //Initializing elements of AppState to NULL or DEFAULT values
  State.window = NULL;
  State.renderer = NULL;
  State.running = 0;

  //State is considered to be initialized from now on.
}
