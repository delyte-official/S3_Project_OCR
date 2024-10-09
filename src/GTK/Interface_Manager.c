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

GtkWidget* auto_pack_box(GtkOrientation,int,GtkWidget*,int,int,int,int,int);
GtkWidget* center_new(GtkWidget*);
GdkPixbuf* resize(GdkPixbuf*,int,int,int,int);
GdkPixbuf* resize_from_container(GdkPixbuf*,int,int,GtkWidget*);
void Build_Interface(GtkWidget*);
*/


////HEADERS Files
//Integrated C Libraries
#include <stdlib.h>
#include <stdio.h>

//Project Headers
#include "../Services/Events_Manager.h"
#include "../Services/Debug.h"

//Tools
#include <gtk/gtk.h>
#include <pango/pango.h>
////END HEADERS


/* auto_pack_box():
    Shortcut function to automatically create a box, pack it inside
    the pointed parent and set its requested size.
*/
GtkWidget* auto_pack_box(
        GtkOrientation orientation,
        int child_padd,
        GtkWidget *parent_box,
        int expand,
        int fill,
        int extra_padd,
        int width_req,
        int height_req) {
    //Creating the box
    GtkWidget *new_b = gtk_box_new(orientation, child_padd);
    //Putting box in parent box
    gtk_box_pack_start(GTK_BOX(parent_box), new_b, expand, fill, extra_padd);
    //Request size
    gtk_widget_set_size_request(new_b, width_req, height_req);
    return new_b;
}


/* center_new():
    Shortcut function that creates a custom widget centering the pointed
    child in it. Is an alternative to the deprecated GTK Widget alignment.
*/
GtkWidget* center_new(GtkWidget* child) {
    //Creating vertical box
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    //Creating horizontal box
    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
    gtk_widget_set_halign(hbox, GTK_ALIGN_CENTER);
    //Attaching child
    gtk_box_pack_start(GTK_BOX(hbox), child, FALSE, FALSE, 0);
    gtk_widget_set_valign(child, GTK_ALIGN_CENTER);
    return vbox;
}


/* resize():
    Resize a pixbuf to the maximum ratio possible while keeping
    the proportions of the given pixbuf.
*/
GdkPixbuf* resize(
        GdkPixbuf* pixbuf,
        int origin_w,
        int origin_h,
        int max_w,
        int max_h) {
    //Figuring out the size
    float w_ratio = (float)max_w/origin_w;
    float h_ratio = (float)max_h/origin_h;
    float scale = w_ratio < h_ratio ? w_ratio : h_ratio;
    int width = (int)(origin_w * scale);
    int height = (int)(origin_h *scale);
    //Creating the scaled pixbuf
    GdkPixbuf *scaled = gdk_pixbuf_scale_simple(pixbuf, width, height,
            GDK_INTERP_BILINEAR);
    return scaled;
}


/* resize_from_container():
    Shortcut to resize() by automatically getting the maximum size possible.
*/
GdkPixbuf* resize_from_container(
        GdkPixbuf* pixbuf,
        int origin_w,
        int origin_h,
        GtkWidget* container) {
    //Grab the size of container for max size
    GtkAllocation alloc;
    gtk_widget_get_allocation(container, &alloc);
    return resize(pixbuf, origin_w, origin_h, alloc.width, alloc.height);
}


/* Build_Interface():
    Creates every widget and the structure of the starting project menu.
    Creates every needed signals for the application to run properly.
*/
void Build_Interface(
        GtkWidget *window,
        int width,
        int height,
        char* title) {
    ////Dividing interface into two spaces
    GtkWidget *main_b = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(window), main_b);

    ////Building left side of interface
    GtkWidget *left_b = auto_pack_box(GTK_ORIENTATION_VERTICAL,
            0, main_b, FALSE, FALSE, 0, width / 2, -1);
    ///Box for TITLE
    GtkWidget *title_b = auto_pack_box(GTK_ORIENTATION_VERTICAL,
            0, left_b, FALSE, FALSE, 0, -1, height /6);
    change_widget_color(title_b, "#1b4a45");
    //<Title
    GtkWidget *title_lbl = gtk_label_new(title);
    gtk_box_pack_start(GTK_BOX(title_b), title_lbl, TRUE, TRUE, 0);
    gtk_widget_set_halign(title_lbl, GTK_ALIGN_CENTER);
    PangoFontDescription *font_desc = pango_font_description_new();
    pango_font_description_set_size(font_desc, 24 * PANGO_SCALE);
    gtk_widget_override_font(title_lbl, font_desc);
    ///Display screen
    GtkWidget *display_b = auto_pack_box(GTK_ORIENTATION_HORIZONTAL,
            0, left_b, TRUE, TRUE, 0, -1, -1);
    change_widget_color(display_b, "#42f593");
    ///Button for image selector
    GtkWidget *select_btn = gtk_button_new_with_label("Select Image");
    g_signal_connect(select_btn, "clicked", G_CALLBACK(file_selector),
            display_b);
    GtkWidget *center_b = center_new(select_btn);
    gtk_box_pack_start(GTK_BOX(display_b), center_b, TRUE, TRUE, 0);

    ////Right side containing the interface
    ///Vertical box for the right side
    GtkWidget *right_b = auto_pack_box(GTK_ORIENTATION_VERTICAL,
            0, main_b, TRUE, TRUE, 0, -1, -1);
    change_widget_color(right_b, "#32a852");
    ///Header of the vertical section
    GtkWidget *header_b = auto_pack_box(GTK_ORIENTATION_VERTICAL,
            0, right_b, FALSE, FALSE, 0, -1, height /4);
    change_widget_color(header_b, "#f542ce");
    ///Helper of vertical section
    GtkWidget *helper_b = auto_pack_box(GTK_ORIENTATION_VERTICAL,
            0, right_b, FALSE, FALSE, 0, -1, height / 8);
    change_widget_color(helper_b, "#139485");

    ////Show all the created widgets
    gtk_widget_show_all(window);
}
