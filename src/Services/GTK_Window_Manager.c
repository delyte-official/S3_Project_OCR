/*

      ##############################################################
      #                                                            #
      #                    GTK_Window_Manager.c                    #
      #                                                            #
      #         Contains all functions related to handling,        #
      #   modifying and supervising memory of GTK windows system.  #
      #                                                            #
      ##############################################################


List of all functions written in this file (and their type):
[See more description on their purpose and parameters down below]

GtkWidget *create_window();
SDL_Renderer *renderer(int32);
void close_program(SDL_Window*,SDL_Renderer*);
*/











////HEADERS Files
//Integrated C Libraries
#include <err.h>

//GTK Libraries
#include <gtk/gtk.h>

//Project Headers


////END HEADERS


GtkWidget *create_window(
        GtkWindowType type,
        char* title,
        int width,
        int height) {

    GtkWidget *window;

    //Creating the window
    window = gtk_window_new(type);
    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);

    return window;
}
