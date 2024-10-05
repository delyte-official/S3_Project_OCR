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
////END HEADERS


/*  main():
  Official function to start the application properly: ~Main Canal.

Requisites assumed:
  No other instance of the program must be running.
  All other files must have proper defined header files.
*/
int main( //Parameters:
        int argc, //Size of argv
        char* argv[]) { //All parameters passed through command line

    //Category of command line parameters
    char* init_params[argc-1]; //Array of all initialitazion given parameters
    int init_size = argc-1; //Size of init_params

    char* gtk_params[argc-1]; //Array of given parameters for Run_Application()
    int gtk_size = argc-1; //Size of run_params

    //Handle and categorize every parameters given through the command line
    Filter_Params(argv, argc, init_params, &init_size, gtk_params, &gtk_size);


    //Calling the core function that will initialize the application,
    //then run, compute and link every events and actions to its
    //desired project systems.
    StartUp(init_params, init_size, gtk_params, gtk_size);

    //Closing the program entirely:
    //All used memory is assumed to be freed beforehand by the Core_Manager
    return EXIT_SUCCESS;
}
