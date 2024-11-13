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
*/
int main(int argc, char* argv[]) {
    //Categorize the command line parameters
    char* init_params[argc-1];
    int init_size = argc-1;
    char* gtk_params[argc-1];
    int gtk_size = argc-1;
    Filter_Params(argv, argc, init_params, &init_size, gtk_params, &gtk_size);

    //Run the application
    StartUp(init_params, init_size, gtk_params, gtk_size);

    //Closing the program entirely.
    return EXIT_SUCCESS;
}
