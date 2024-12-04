/*
      ##############################################################
      #                                                            #
      #                           Events.c                         #
      #                                                            #
      #       Contains all functions receiving, handling and       #
      #    sending signals and events to other declared systems.   #
      #                                                            #
      ##############################################################
*/

////HEADERS
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
//Project Headers
#include "../Core_Manager.h"
#include "Events.h"
#include "Interface_Manager.h"


void Standard_Signals() {
    AppState *state = APPSTATE;
    g_signal_connect(state->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(state->window, "size-allocate",
            G_CALLBACK(_application_init), NULL);
}


void _application_init(GtkWidget *window, GtkAllocation*, gpointer) {
    AppState *state = APPSTATE;
    int width, height;
    gtk_window_get_size(GTK_WINDOW(window), &width, &height);
    if (width > state->width || height >= state->height)
        return;
    width = 1920; //Terminal blocks half the screen
    state->alloc_width = width;
    state->alloc_height = height;
    Build_Interface();
    g_signal_handlers_disconnect_matched(window, G_SIGNAL_MATCH_FUNC, 0, 0,
            NULL, G_CALLBACK(_application_init), NULL);
}
