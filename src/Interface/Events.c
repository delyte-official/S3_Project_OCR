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
    } else if (confirm_dialog("This will erase any steps that have not been"
                " saved.")) {//Ask dialog to continue
        if (file_selector(NULL, NULL)) {
            //Reset work
            for (int i = -2; i > -6; i--) {
                step_widget(i, NULL);
            }
            ShowNext();
            STEP* curr_step = get_step();
            (*curr_step)++;
            //Reset history
            clear_history();
            //Add the first step again
            add_history_step(STEP_LOAD);
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
    save_step(*curr_step-1);
}

/* _on_step_save_history():
    Saves the step of the clicked history tile.
*/
void _on_step_save_history(GtkWidget*, STEP step) {
    save_step(step);
}


/* _on_jumpto_step():
    Jump to the specified step.
*/
void _on_jumpto_step(GtkWidget*, STEP step) {
    int dst = (int)step+1;
    STEP *curr_step = get_step();
    int src = (int)(*curr_step);
    if (dst-src > 0) {
        for (int i = src; i < dst; i++) {
            //We know the steps have already been computed.
            NextStep(NULL, NULL);
        }
    } else if (dst-src < 0) {
        for (int i = src; i > dst; i--) {
            ShowPrevious(NULL, NULL);
        }
    }
}
