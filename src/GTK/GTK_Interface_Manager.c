/*

      ##############################################################
      #                                                            #
      #                  GTK_Interface_Manager.c                   #
      #                                                            #
      #            Create, modify and destroy GTK Widgets.         #
      #  Offer easy access to widgets and redirects their events.  #
      #                                                            #
      ##############################################################


List of all functions written in this file (and their type):
[See more description on their purpose and parameters down below]

void Build_Interface(GtkWidget*);
*/


////HEADERS Files
//Integrated C Libraries
#include <stdlib.h>
#include <stdio.h>

//Project Headers
#include "Events_Manager.h"
#include "Debug.h"

//Tools
#include <gtk/gtk.h>
////END HEADERS




void change_widget_color(GtkWidget *widget, const char *color) {
    GdkRGBA rgba;
    gdk_rgba_parse(&rgba, color);
    gtk_widget_override_background_color(widget, GTK_STATE_FLAG_NORMAL, &rgba);
}

void Build_Interface(GtkWidget *window) {
    ////Dividing interface into two spaces
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(window), hbox);
    ///Building left side of interface
    GtkWidget *box_left = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_box_pack_start(GTK_BOX(hbox), box_left, FALSE, FALSE, 0);
    gtk_widget_set_size_request(box_left, width / 2, -1);
    //Box for TITLE
    GtkWidget *box_title = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_box_pack_start(GTK_BOX(box_left), box_title, FALSE, FALSE, 0);
    gtk_widget_set_size_request(box_title, -1, height / 6);
    change_widget_color(box_title, "#1b4a45");

    //Box to center the button
    GtkWidget *box_center = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    gtk_box_pack_start(GTK_BOX(box_left), box_center, TRUE, TRUE, 0);
    gtk_widget_set_halign(box_center, GTK_ALIGN_CENTER);
    //Button for image selector
    GtkWidget *button = gtk_button_new_with_label("Select Image");
    gtk_box_pack_start(GTK_BOX(box_center), button, FALSE, FALSE, 0);
    g_signal_connect(button, "clicked", G_CALLBACK(file_selector), box_center);
    gtk_widget_set_valign(button, GTK_ALIGN_CENTER);

    ///Right side containing the interface
    //Vertical box for the right side
    GtkWidget *box_right = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_box_pack_start(GTK_BOX(hbox), box_right, TRUE, TRUE, 0);
    //gtk_widget_set_size_request(box_right, width / 2, -1);
    change_widget_color(box_right, "#32a852");
    //Header of the vertical section
    GtkWidget *box_header = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_box_pack_start(GTK_BOX(box_right), box_header, FALSE, FALSE, 0);
    gtk_widget_set_size_request(box_header, -1, height / 4);
    change_widget_color(box_header, "#f542ce");
    //Helper of vertical section
    GtkWidget *box_helper = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_box_pack_start(GTK_BOX(box_right), box_helper, FALSE, FALSE, 0);
    gtk_widget_set_size_request(box_helper, -1, height / 8);
    change_widget_color(box_helper, "#139485");

    //Show all the created widgets
    gtk_widget_show_all(window);
}
