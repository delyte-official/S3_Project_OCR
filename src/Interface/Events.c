/*
      ##############################################################
      #                                                            #
      #                           Events.c                         #
      #                                                            #
      #       Contains all functions receiving, handling and       #
      #    sending signals and events to other declared systems.   #
      #                                                            #
      ##############################################################

List of all functions written in this file (and their type):
[See more description on their purpose and parameters down below]

void Standard_Signals(GtkWidget);
void _on_select_image_btn(GtkWidget*, gpointer);
void _on_auto_btn(GtkWidget*, gpointer);
void _on_save_btn(GtkWidget*, gpointer);
*/

////HEADERS Files
//Integrated C Libraries
#include <err.h>
#include <stdio.h>
//GTK Libraries
#include <gtk/gtk.h>
//Project Headers
#include "Interface_Manager.h"
#include "../Core_Manager.h"
#include "Events.h"
////END HEADERS


/*  Standard_Signals():
  Links all events and signals to their designated functions.
*/
void Standard_Signals(GtkWidget *window) {
    //Closing window closes the program
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "size-allocate",
            G_CALLBACK(_application_init), NULL);
}


/* _application_init():
    Initialize all the content of the window of the application as soon
    as the windowh as been realized.
*/
void _application_init(
        GtkWidget *window,
        GtkAllocation*,
        gpointer) {
    int width, height;
    gtk_window_get_size(GTK_WINDOW(window), &width, &height);
    if (width > global_width || height >= global_height)
        return;
    printf("Size: %d;%d\n", width, height);
    Build_Interface(window, width, height);
    g_signal_handlers_disconnect_matched(window, G_SIGNAL_MATCH_FUNC, 0, 0,
            NULL, G_CALLBACK(_application_init), NULL);
}


/* _null_event():
    Deactivates any events connected to it.
*/
void _null_event(GtkWidget*, gpointer) {}


/* _on_select_image_btn():
    Handles the event of "clicked" from the select image button.
*/
void _on_select_image_btn(GtkWidget*, gpointer) {
    GtkWidget* curr_widget = step_widget(1, NULL);
    if (curr_widget == NULL) {
        NextStep(NULL, NULL);
    } else if (1) {//Ask dialog to continue
        if (file_selector(NULL, NULL)) {
            //Reset work
            for (int i = -2; i > -6; i--) {
                step_widget(i, NULL);
            }
            ShowNext();
            STEP* curr_step = get_step();
            (*curr_step)++;
        }
    }
}


/* _on_auto_btn():
    Handles the event of "clicked" from the auto complete button.
*/
void _on_auto_btn(GtkWidget* auto_btn, gpointer) {
    STEP* curr_step = get_step();
    //Perform every step
    for (int i = *curr_step; i < 5; i++) {
        if (!NextStep(NULL, NULL))
            return; //Error, stop
    }
    gtk_widget_set_sensitive(auto_btn, FALSE);
}


/* _on_save_btn():
    Handles the event of "clicked" from the save step button.
*/
void _on_save_btn(GtkWidget*, gpointer) {
    STEP* curr_step = get_step();
    switch (*curr_step) {
        case STEP_FILTER:
            GtkWidget *image1 = step_widget(1, NULL);
            GdkPixbuf *pixbuf1 = g_object_get_data(G_OBJECT(image1), "pixbuf");
            file_save(&pixbuf1, EXT_PNG);
            break;
        case STEP_EXTRACT:
            GtkWidget *image2= step_widget(2, NULL);
            GdkPixbuf *pixbuf2 = g_object_get_data(G_OBJECT(image2), "pixbuf");
            file_save(&pixbuf2, EXT_PNG);
            break;
        case STEP_SOLVE:
            break;
        case STEP_RECONSTRUCT:
            break;
        case STEP_END:
            break;
        default: //Nothing to save
            return;
    }
}
