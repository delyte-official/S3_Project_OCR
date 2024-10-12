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

void Standard_Signals(GtkWidget);
void file_selector(GtkWidget*,GtkWidget*);
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
////END HEADERS


/*  Standard_Signals():
  Links all events and signals to their designated functions.
*/
void Standard_Signals(GtkWidget *window) {
    //Closing window closes the program
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}


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


/**/
void _on_auto_btn(GtkWidget* auto_btn, gpointer) {
    STEP* curr_step = get_step();
    //Perform every step
    for (int i = *curr_step; i < 5; i++) {
        if (!NextStep(NULL, NULL))
            return; //Error, stop
    }
    gtk_widget_set_sensitive(auto_btn, FALSE);
}
