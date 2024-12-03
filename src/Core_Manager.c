/*
      ##############################################################
      #                                                            #
      #                       Core_Manager.c                       #
      #                                                            #
      #         Handles, supervises and links all resources        #
      #      between every systems of the Project Application.     #
      #                                                            #
      ##############################################################
*/


////HEADERS
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <gtk/gtk.h>
//Project Headers
#include "Core_Manager.h"
#include "Interface/GTK_Window.h"
#include "Interface/Events.h"
////DEFINING
#define ID_INIT_SIZE 1
static const char* ID_INIT_PARAMS[ID_INIT_SIZE] =  {"--force"};


AppState *get_appState() {
    static AppState state = (AppState) {0};
    return &state;
}


int check_param(const char** PARAMS, int len, char* to_check) {
    for (int i = 0; i < len; i++) {
        if (to_check == PARAMS[i])
            return 1; //Found
    }
    return 0; //Not found
}


void Filter_Params(char** all_params, int len, char* *init_params,
        int *init_size, char* *gtk_params, int *gtk_size) {
    for (char** curr = all_params; curr < all_params + len; curr++) {
        size_t length = strlen(*curr);
        if (length < 2) //Wrong format
            continue;
        if (check_param(ID_INIT_PARAMS, ID_INIT_SIZE, *curr)) { //Init param
            *init_params = *curr;
            init_params++;
            (*init_size)++;
        } else { //Sent to GTK
            *gtk_params = *curr;
            gtk_params++;
            (*gtk_size)++;
        }
    }
}


void StartUp(char** gtk_params, int gtk_len,
        char** init_params, int init_len) {
    (void) init_params; //No possible init parameters.
    (void) init_len;

    gtk_init(&gtk_len, &gtk_params);
    AppState *state = get_appState();

    //Creation of the window
    char* title = "#Free Galane - Word Search Solver";
    GdkRectangle geometry;
    get_screen_size(&geometry);
    state->width = geometry.width;
    state->height = geometry.height;
    int type = GTK_WINDOW_TOPLEVEL;
    state->window = create_window(type, title, state->width, state->height);

    //Run the application
    Standard_Signals();
    gtk_widget_show_all(state->window);
    gtk_main();
}
