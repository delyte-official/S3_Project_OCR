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
#include <string.h>
//GTK Libraries
#include <gtk/gtk.h>
//Project Headers
#include "Interface_Manager.h"
#include "../Core_Manager.h"
#include "../Filter/Prefilter.h"
#include "Events.h"
////END HEADERS


/*  Standard_Signals():
  Links all events and signals to their designated functions.
*/
void Standard_Signals(GtkWidget *window) {
    //Closing window closes the program
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}


/* _null_event():
    Deactivates any events connected to it.
*/
//void _null_event(GtkWidget*, gpointer) {}


/* _on_select_image_btn():
    Handles the event of "clicked" from the select image button.
*/
/*void _on_select_image_btn(GtkWidget*, gpointer) {
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
            STEP* curr_step = &(get_app_state()->step);
            (*curr_step)++;
            //Reset history
            clear_history_from(STEP_LOAD);
            //Add the first step again
            add_history_step(STEP_LOAD);
        }
    }
}*/


/* _on_auto_btn():
    Handles the event of "clicked" from the auto complete button.
*/
/*void _on_auto_btn(GtkWidget* auto_btn, gpointer) {
    STEP curr_step = get_app_state()->step;
    //Perform every step
    for (int i = curr_step; i < 5; i++) {
        if (!NextStep(NULL, NULL))
            return; //Error, stop
    }
    gtk_widget_set_sensitive(auto_btn, FALSE);
}*/


/* _on_save_btn():
    Handles the event of "clicked" from the save step button.
*/
/*void _on_save_btn(GtkWidget*, gpointer) {
    save_step(get_app_state()->step-1);
}*/


/* _on_modify_btn():
    Modifies the input image by user input such as rotation.
*/
/*void _on_modify_btn(GtkWidget*, gpointer) {
    GtkWidget* next = step_widget(STEP_FILTER+1, NULL);
    if (next != NULL) { //Already have steps
        if (confirm_dialog("This will delete any further steps not saved.")) {
            if (modify_image()) {
                //Deleting everything behind this
                for (int i = -1-STEP_FILTER; i > -6; i--) {
                    step_widget(i, NULL);
                }
            }
        }
        return;
    }
    modify_image();
}*/


/* _on_step_save_history():
    Saves the step of the clicked history tile.
*/
/*void _on_step_save_history(GtkWidget*, STEP step) {
    save_step(step);
}*/


/* _on_jumpto_step():
    Jump to the specified step.
*/
/*void _on_jumpto_step(GtkWidget*, STEP step) {
    int dst = (int)step+1;
    int src = (int)(get_app_state()->step);
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
}*/


/* _on_apply_rotation():
    Apply the rotation from the user input.
*/
/*void _on_apply_rotation(GtkWidget*, GtkWidget *table[4]) {
    GtkEntry *entry = GTK_ENTRY(table[0]);
    const char *angle_txt = gtk_entry_get_text(entry);
    //Convert to double
    char* end;
    errno = 0;
    double angle = strtod(angle_txt, &end);
    if (end == angle_txt || errno == ERANGE || *end != '\0') {
        //Error in string
        gtk_widget_show(table[2]);
        return;
    }
    //Clamping angle
    while (angle < -360.0)
        angle += 360.0;
    while (angle > 360.0)
        angle -= 360.0;
    //Rotate
    GtkImage *dis_image = GTK_IMAGE(table[1]);
    GObject *curr_image = G_OBJECT(step_widget(STEP_LOAD+1, NULL));
    GdkPixbuf *pixbuf = g_object_get_data(curr_image, "pixbuf");
    GdkPixbuf *rotated = rotate_pixbuf(pixbuf, angle);
    gtk_image_set_from_pixbuf(dis_image, rotated);
    g_object_set_data(G_OBJECT(dis_image), "pixbuf", rotated);
}*/
