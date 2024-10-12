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
#include "Events_Manager.h"
#include "../Debug.h"
#include "../Core_Manager.h"
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
        int origin,
        int width_req,
        int height_req) {
    //Creating the box
    GtkWidget *new_b = gtk_box_new(orientation, child_padd);
    //Putting box in parent box
    if (origin)
        gtk_box_pack_start(GTK_BOX(parent_box), new_b, expand, fill,
                extra_padd);
    else
        gtk_box_pack_end(GTK_BOX(parent_box), new_b, expand, fill,
                extra_padd);
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


/* get_controls():
    Returns pointers to the control buttons.
*/
void get_controls(
        int place,
        GtkWidget** widget) {
    //0: Prev    //2: Auto
    //1: Next    //3: Save
    static GtkWidget* controls[3] = {NULL};
    if (controls[place] == NULL)
        controls[place] = *widget;
    if (widget != NULL)
        *widget = controls[place];
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
            0, main_b, FALSE, FALSE, 0, TRUE, width / 2, -1);
    ///Box for TITLE
    GtkWidget *title_b = auto_pack_box(GTK_ORIENTATION_VERTICAL,
            0, left_b, FALSE, FALSE, 0, TRUE, -1, height /6);
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
            0, left_b, TRUE, TRUE, 0, TRUE, -1, -1);
    change_widget_color(display_b, "#42f593");
    ///Initializing the static getter of the Application's display section
    get_display(&display_b);
    ///Button for image selector
    GtkWidget *select_btn = gtk_button_new_with_label("Select Image");
    g_signal_connect(select_btn, "clicked",
            G_CALLBACK(_on_select_image_btn), NULL);
    GtkWidget *center_b = center_new(select_btn);
    step_widget(0, center_b);
    set_display(center_b);

    ////Right side containing the interface
    ///Vertical box for the right side
    GtkWidget *right_b = auto_pack_box(GTK_ORIENTATION_VERTICAL,
            0, main_b, TRUE, TRUE, 0, TRUE, -1, -1);
    change_widget_color(right_b, "#32a852");

    ///Header of the vertical section
    GtkWidget *header_b = auto_pack_box(GTK_ORIENTATION_VERTICAL,
            0, right_b, FALSE, FALSE, 0, TRUE, -1, height / 4);
    change_widget_color(header_b, "#f542ce");
    //Spacing header_b
    auto_pack_box(GTK_ORIENTATION_HORIZONTAL, 0, header_b,
            FALSE, FALSE, 0, FALSE, -1, height / 32);
    //Controls Button
    GtkWidget *control_b = auto_pack_box(GTK_ORIENTATION_HORIZONTAL,
            0, header_b, FALSE, FALSE, 0, FALSE, -1, height / 24);
    //Spacing control_b
    auto_pack_box(GTK_ORIENTATION_VERTICAL, 0, control_b, FALSE, FALSE, 0,
            TRUE, width / 16, -1);
    auto_pack_box(GTK_ORIENTATION_VERTICAL, 0, control_b, FALSE, FALSE, 0,
            FALSE, width / 16, -1);
    //Button Control: Next
    GtkWidget *next_btn = gtk_button_new_with_label("Next");
    gtk_box_pack_end(GTK_BOX(control_b), next_btn, FALSE, FALSE, 0);
    gtk_widget_set_size_request(GTK_WIDGET(next_btn), width / 16, -1);
    g_signal_connect(next_btn, "clicked", G_CALLBACK(NextStep), NULL);
    get_controls(1, &next_btn);
    //Button Control: Prev
    GtkWidget *prev_btn = gtk_button_new_with_label("Previous");
    gtk_box_pack_start(GTK_BOX(control_b), prev_btn, FALSE, FALSE, 0);
    gtk_widget_set_size_request(GTK_WIDGET(prev_btn), width / 16, -1);
    g_signal_connect(prev_btn, "clicked", G_CALLBACK(ShowPrevious), NULL);
    get_controls(0, &prev_btn);
    gtk_widget_set_sensitive(prev_btn, FALSE);
    //Spacing
    auto_pack_box(GTK_ORIENTATION_VERTICAL, 0, control_b, FALSE, FALSE, 0,
            TRUE, width / 16, -1);
    //Button Control: Auto
    GtkWidget *auto_btn = gtk_button_new_with_label("Auto");
    gtk_box_pack_start(GTK_BOX(control_b), auto_btn, FALSE, FALSE, 0);
    gtk_widget_set_size_request(GTK_WIDGET(auto_btn), width / 16, -1);
    g_signal_connect(auto_btn, "clicked", G_CALLBACK(_on_auto_btn), NULL);
    get_controls(2, &auto_btn);

    //Helper of vertical section
    GtkWidget *helper_b = auto_pack_box(GTK_ORIENTATION_VERTICAL,
            0, right_b, FALSE, FALSE, 0, TRUE, -1, height / 8);
    change_widget_color(helper_b, "#139485");

    ////Show all the created widgets
    gtk_widget_show_all(window);
}


/*  file_selector():
    Opens a file selector dialog to choose an image for the project.
*/
int file_selector(GtkWidget, gpointer) {
    //Retrieve the display section
    GtkWidget* display_b = get_display(NULL);
    //Creating the dialog window
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Select Image",
            GTK_WINDOW(gtk_widget_get_toplevel(display_b)),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Open", GTK_RESPONSE_ACCEPT,
            NULL);
    //Only accepting images
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pixbuf_formats(filter);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    //Run the dialog
    int response = gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT;
    if (response) {
        //Get the selected image
        char* filename = gtk_file_chooser_get_filename(
                GTK_FILE_CHOOSER(dialog));

        //Load image
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
        if (pixbuf != NULL) {
            //Resizing the image to fit the display section
            int width = gdk_pixbuf_get_width(pixbuf);
            int height = gdk_pixbuf_get_height(pixbuf);
            pixbuf = resize_from_container(pixbuf,width, height, display_b);
            //Creating the image
            GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
            //Storing the pixbuf inside the image widget
            g_object_set_data(G_OBJECT(image), "pixbuf", pixbuf);
            g_object_ref(pixbuf);
            //Saving that widget as a STEP WIDGET
            step_widget(1, image);
        }
        g_free(filename);
    }

    //Closing dialog wether cancel or accepted
    gtk_widget_destroy(dialog);
    return response;
}
