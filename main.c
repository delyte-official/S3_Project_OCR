/*

      #################################################################
      #                                                               #
      #                             Main.c                            #
      #                                                               #
      #              This file represents the Main Canal.             #
      #    The final version of the project must be run from here.    #
      #                                                               #
      #################################################################

*/

////HEADER Files
//C Libraries
#include <stdlib.h>

//Project Libraries
#include "src/Core_Manager.h"


/*  main():
  Official function to start the application properly ~Main Canal.

Requisites assumed:
  No other instance of the program must be running.
  All other files must have proper defined header files.
*/
int main( //Parameters:
      int argc, //Size of argv
      char *argv[]) { //All parameters passed through command line

  //Category of command line parameters
  char* init_params[argc-1]; //Array of all initialitazion given parameters
  size_t init_size; //Size of init_params

  char* run_params[argc-1]; //Array of given parameters for Run_Application()
  size_t run_size; //Size of run_params

  //Handle and categorize every parameters given through the command line
  Filter_Params(argv, argc, init_params, &init_size, run_params, &run_size);

  //Initializing all the systems and their sub-systems required for
  //the project to run properly. This is done through the main canal.
  Initialize(init_params, init_size);


  //Calling the core function that will run, compute and links
  //every events and actions to its desired project systems.
  Run_Application(run_params, run_size);

  //Closing the program entirely:
  //All used memory is assumed to be freed beforehand by the Core_Manager
  return EXIT_SUCCESS;
}
