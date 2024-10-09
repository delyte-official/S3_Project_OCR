/*
      ##############################################################
      #                                                            #
      #                       Core_Manager.c                       #
      #                                                            #
      #         Handles, supervises and links all resources        #
      #      between every systems of the Project Application.     #
      #                                                            #
      ##############################################################

List of all functions written in this file (and their type):
[See more description on their purpose and parameters down below]

int recognized(char**,size_t,char*);
void Filter_Params(char**,size_t,char***,size_t*,char***,size_t*);
void StartUp(char**,size_t,char**,size_t);
void NextStep(GtkWidget,void*);
*/


////HEADERS Files
//Integrated C Libraries
#include <stdlib.h>
#include <stdio.h>

//Project Headers
#include "GTK/Window_Manager.h"
#include "Services/Events_Manager.h"
#include "Services/Debug.h"
#include "Core_Manager.h"
//Tools
#include <gtk/gtk.h>
////END HEADERS
////DEFINING
//Constants
#define ID_INIT_SIZE 1
static const char* ID_INIT_PARAMS[ID_INIT_SIZE] = {"--force"};
////END DEFINING


/* get_step():
    Returns a pointer to the static variable representing the current
    step that the program is at.
*/
STEP* get_step() {
    static STEP curr_step = 0;
    return &curr_step;
}


/*  recognized():
  Returns if an element is present or not in the given parameters list.

Requisites assumed:
  The given parameters list is correct.
  The parameter to check is in correct format.
*/
int recognized(const char** PARAMS, int len, char* to_check) {
    for (int i = 0; i < len; i++) {
        if (to_check == PARAMS[i]) //Found
            return 1;
    }
    return 0; //Not found
}


/*  Filter_Params():
  Filters every given parameters into two categories:
  - GTK parameters
  - Application parameters
  Returns them in given arrays, and their sizes.
  Every unrecognized parameters will be thrown away.

Requisites assumed:
  The pointed arrays have enough space to hold every parameters by themselves.
  The pointed arrays and size_t are already initialized and empty.
*/
void Filter_Params( //Parameters
        char** all_params, //The given command-line parameters
        int len, //The size of all_parameters
        char* *init_params, //Array to write initialization parameters
        int *init_size, //To store size of init_params
        char* *gtk_params, //Array to write Application parameters
        int *gtk_size) { //To store size of run_params
    //Iterating through the given parameters
    for (char** curr = all_params; curr < all_params + len; curr++) {
        size_t length = strlen(*curr);
        if (length < 2) //Skipping unformalized parameters
            continue;

        //Checking if it is a recognized parameter
        if (recognized(ID_INIT_PARAMS, ID_INIT_SIZE, *curr)) {
            //Putting the parameter into its corresponding category
            *init_params = *curr;
            init_params++;
            (*init_size)++;
        } else { //Is a garbage parameter or a GTK Parameter
                 //We let GTK treat them however it wants
            *gtk_params = *curr;
            gtk_params++;
            (*gtk_size)++;
        }
    }
}


/*  StartUp():
  Initialize all systems necessary for the projects:
  - C Libraries
  - Signals Tracking
  - SDL/GTK Systems
  - Others
  Then run the application.

Requisites assumed:
  It must be the first and last time that this function is called.
  Any error in this function must terminate the entire processus.
  The given parameters are valid, and their number correct.
*/
void StartUp( //Parameters:
        char** gtk_params, //Given parameters for gtk_initialization
        int gtk_len, //Size of gtk_params
        char** init_params, //Given parameters for developper testing
        int init_len) { //Size of init_params
    //Initialize GTK3 and its sub-systems
    gtk_init(&gtk_len, &gtk_params);

    //Defining necessary variables for Window_Init
    GtkWidget *window;
    char* title = "OCR Word Search Solver";
    GdkRectangle geometry;
    get_screen_size(&geometry);
    int width = geometry.width;
    int height = geometry.height;
    int type = GTK_WINDOW_TOPLEVEL;

    //Initialize GTK Main Project Window
    window = create_window(type,title,width,height);

    //Building the interface and setup the associated signals and events
    Build_Interface(window, width, height, title);
    
    //Link all standard signals and events of the window
    Standard_Signals(window);

    //Running the application
    gtk_main();
}


/* NextStep():
    Performs the next operation of the OCR Word Search program.
*/
void NextStep(GtkWidget* next_btn, int* curr_step) {
    //Performing operation according to curr_step
    switch (*curr_step) {
        case STEP_START:
            printf("STARTING\n");
            break;
        case STEP_LOAD:
            break;
        case STEP_FILTER:
            break;
        case STEP_EXTRACT:
            break;
        case STEP_SOLVE:
            break;
        case STEP_RECONSTRUCT:
            break;
        case STEP_END:
            break;
        default:
            errx(EXIT_FAILURE, "STEP is in incorrect form.");
    }

    //Advancing the current step
    (*curr_step)++;
    printf("New step: %d\n", *curr_step);
}
