/*

      ##############################################################
      #                                                            #
      #                      Events_Manager.c                      #
      #                                                            #
      #       Contains all functions receiving, handling and       #
      #    sending signals and events to other declared systems.   #
      #                                                            #
      ##############################################################


List of all functions written in this file (and their type):
[See more description on their purpose and parameters down below]

void on_window_realize(GtkWidget,gpointer);
void Standard_Signals(GtkWidget);
*/











////HEADERS Files
//Integrated C Libraries
#include <err.h>

//GTK Libraries
#include <gtk/gtk.h>

//Project Headers


////END HEADERS


/* on_window_realise():
    Forbids the window from being resized once it is ready to display.
*/
void on_window_realize(GtkWidget *widget, gpointer data) {
    //gtk_window_set_resizable(GTK_WINDOW(widget), FALSE);
}



/*  Standard_Signals():
  Links all events and signals to their designated functions.

Requisites assumed:
  It must be the first and last time that this function is called.
  Other intermediate event/signal linkage must be done through
  another function or canal.
*/
void Standard_Signals(GtkWidget *window) {
    //Closing window closes the program
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    //When window can be rendered, freeze its size in its current state
    g_signal_connect(window, "realize", G_CALLBACK(on_window_realize), NULL);
}
