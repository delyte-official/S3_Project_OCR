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
void get_controls(int,GtkWidget**);
void Build_Interface(GtkWidget*);
int file_selector();
int file_save();
*/


////HEADERS Files
//Integrated C Libraries
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
//Project Headers
#include "Events.h"
#include "../Debug.h"
#include "../Core_Manager.h"
#include "Interface_Manager.h"
//Tools
#include <gtk/gtk.h>
#include <pango/pango.h>
////END HEADERS
////DEFINE
static int display_height = 0;
static int display_width = 0;
////END DEFINE


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


/* resize():
    Resize a pixbuf to the maximum ratio possible while keeping
    the proportions of the given pixbuf.
*/
GdkPixbuf* resize(
        GdkPixbuf* pixbuf,
        int max_w,
        int max_h) {
    //Figuring out the size
    int origin_w = gdk_pixbuf_get_width(pixbuf);
    int origin_h = gdk_pixbuf_get_height(pixbuf);
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
        GtkWidget* container) {
    //Grab the size of container for max size
    GtkAllocation alloc;
    gtk_widget_get_allocation(container, &alloc);
    return resize(pixbuf, alloc.width, alloc.height);
}


/* get_controls():
    Returns pointers to the control buttons.
*/
void get_controls(
        int place,
        GtkWidget** widget) {
    //0: Prev    //2: Auto
    //1: Next    //3: Save
    static GtkWidget* controls[4] = {NULL};
    if (controls[place] == NULL)
        controls[place] = *widget;
    if (widget != NULL)
        *widget = controls[place];
}

/* image_load_from_pixbuf():
    Loads an image from a file and return its image widget.
*/
GtkWidget* image_load_from_pixbuf(
        char* filename) {
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
    GtkWidget* image = gtk_image_new_from_pixbuf(pixbuf);
    g_object_unref(pixbuf);
    return image;
}


/* center_widget();
    Center the given widget in its parent.
*/
void center_widget(GtkWidget* widget) {
    gtk_widget_set_valign(widget, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(widget, GTK_ALIGN_CENTER);
}


/* get_resized_step():
    Returns the content of the step pointed to, resized, taking into
    account the type of content (Pixbuf, text, etc).
*/
GtkWidget *get_resized_step(STEP step, int width, int height) {
    GtkWidget *widget = step_widget(step+1, NULL);
    if (GTK_IS_IMAGE(widget)) {
        GdkPixbuf *old_pixbuf = g_object_get_data(G_OBJECT(widget), "pixbuf");
        GdkPixbuf *pixbuf = resize(old_pixbuf, width, height);
        widget = gtk_image_new_from_pixbuf(pixbuf);
    } else if (GTK_IS_TEXT_VIEW(widget)) {
        GtkWidget *new_textview = gtk_text_view_new();
        //Transpose text or wtv
        GtkTextBuffer *obuffer = gtk_text_view_get_buffer(
                GTK_TEXT_VIEW(widget));
        GtkTextBuffer *nbuffer = gtk_text_buffer_new(NULL);
        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(obuffer, &start);
        gtk_text_buffer_get_end_iter(obuffer, &end);
        gchar *text = gtk_text_buffer_get_text(obuffer, &start, &end, FALSE);
        gtk_text_buffer_set_text(nbuffer, text, -1);
        g_free(text);
        gtk_text_view_set_buffer(GTK_TEXT_VIEW(new_textview), nbuffer);
        
        //Size
        gtk_widget_set_size_request(new_textview, width, height);
        widget = new_textview;
    }
    return widget;
}


/* get_history():
    Returns the history scroll container.
*/
GtkWidget* get_history(GtkWidget *widget) {
    static GtkWidget* history = NULL;
    if (history == NULL)
        history = widget;
    return history;
}


/* clear_history():
    Clears every step of the history.
*/
void clear_history() {
    GList *child = gtk_container_get_children(
            GTK_CONTAINER(get_history(NULL)));
    //Skipping header of history
    //child = child->next;
    //Deleting steps
    while (child != NULL) {
        GtkWidget *step = GTK_WIDGET(child->data);
        gtk_widget_destroy(step);
        child = child->next;
    }
}


/* add_history_step():
    Add a tile to the history scroll container, representing a step done.
*/
void add_history_step(STEP step) {
    //Creating the tile
    GtkWidget *tile_o = gtk_overlay_new();
    //Add image
    //gtk_overlay_add_overlay(GTK_OVERLAY(tile_o), image);
    gtk_widget_set_size_request(tile_o,(float)(global_width * 3) / 8,
            global_height / 6);
    center_widget(tile_o);

    //Content
    GtkWidget *container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_overlay_add_overlay(GTK_OVERLAY(tile_o), container);
    change_widget_color(container, "#000000");

    ///Preview
    //Spacing
    auto_pack_box(GTK_ORIENTATION_HORIZONTAL, 0, container, FALSE, FALSE, 0,
            1, (float)(global_width * 3) / 160, -1);
    //Converting
    GtkWidget* preview = get_resized_step(step,
            (float)(global_width*7656) / 100000,
            (float)(global_height * 9)/ 60);
    center_widget(preview);
    gtk_box_pack_start(GTK_BOX(container), preview, FALSE, FALSE, 0);
    ///Description
    GtkWidget *info = auto_pack_box(GTK_ORIENTATION_VERTICAL, 10, container,
            FALSE, FALSE, 10, 1, (float)(global_width * 3) / 40, -1);
    //Label AND desc
    GtkWidget *title = NULL;
    GtkWidget *desc = NULL;
    switch (step) {
        case STEP_LOAD:
            title = gtk_label_new("Loaded Image");
            desc = gtk_label_new("The selected image\nhas been loaded into\n"
                    "memory and displayed.");
            break;
        case STEP_FILTER:
            title = gtk_label_new("Filtered Image");
            desc = gtk_label_new("The image has been\nfiltered to help the\n"
                    "next operations of extraction.");
            break;
        case STEP_EXTRACT:
            title = gtk_label_new("Extracted Data");
            desc = gtk_label_new("The grid details and\nthe word list has "
                    "been\nextracted and saved in\ncache files.");
            break;
        case STEP_SOLVE:
            title = gtk_label_new("Solved Word Search");
            desc = gtk_label_new("The word search, given\nby the grid and word"
                    "\nlist, has been solved.");
            break;
        case STEP_RECONSTRUCT:
            title = gtk_label_new("Reconstructed Image");
            desc = gtk_label_new("Solution has been\ndisplayed graphically\n"
                    "on the original image.");
            break;
        default:
            errx(EXIT_FAILURE, "Step impossible to add to history.");
    }
    auto_pack_box(GTK_ORIENTATION_VERTICAL,0,info,FALSE,FALSE,0,1,-1,30);
    gtk_box_pack_start(GTK_BOX(info), title, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(info), desc, FALSE, FALSE, 0);

    //Add to history
    gtk_box_pack_start(GTK_BOX(get_history(NULL)), tile_o, FALSE, FALSE, 0);
    gtk_widget_show_all(tile_o);
}


/* Build_Interface():
    Creates every widget and the structure of the starting project menu.
    Creates every needed signals for the application to run properly.
*/
void Build_Interface(
        GtkWidget *window,
        int width,
        int height) {
    //Setting static variables
    display_width = (float)(width * 17) / 40;
    display_height = (float)(height * 7) / 10;
    ////MAIN CONTAINER
    GtkWidget *main_o = gtk_overlay_new();
    gtk_container_add(GTK_CONTAINER(window), main_o);
    gtk_overlay_add_overlay(GTK_OVERLAY(main_o),
            image_load_from_pixbuf("src/assets/app_bg.png"));

    ////Dividing interface into two spaces
    GtkWidget *divide_b = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_overlay_add_overlay(GTK_OVERLAY(main_o), divide_b);

    ////Building left side of interface
    GtkWidget *left_b = auto_pack_box(GTK_ORIENTATION_VERTICAL,
            0, divide_b, FALSE, FALSE, 0, TRUE, width / 2, -1);
    //Holder of display
    GtkWidget *holder_o = gtk_overlay_new();
    gtk_widget_set_size_request(holder_o, -1, (float)(height * 9) / 10);
    gtk_box_pack_end(GTK_BOX(left_b), holder_o, FALSE, FALSE, 0);
    gtk_overlay_add_overlay(GTK_OVERLAY(holder_o),
        image_load_from_pixbuf("src/assets/display_section.png"));
    ///Display screen
    GtkWidget* display_o = gtk_overlay_new();
    gtk_widget_set_size_request(display_o, display_width, display_height);
    gtk_overlay_add_overlay(GTK_OVERLAY(holder_o), display_o);
    center_widget(display_o);
    gtk_widget_set_hexpand(display_o, FALSE);
    gtk_widget_set_vexpand(display_o, FALSE);
    ///Initializing the static getter of the Application's display section
    get_display(&display_o);
    ///Button for image selector
    GtkWidget *select_btn = gtk_button_new();
    gtk_button_set_image(GTK_BUTTON(select_btn),
            image_load_from_pixbuf("src/assets/button_import.png"));
    gtk_button_set_relief(GTK_BUTTON(select_btn), GTK_RELIEF_NONE);
    gtk_widget_set_size_request(select_btn, display_width, display_height);
    g_signal_connect(select_btn, "button-press-event",
            G_CALLBACK(_on_select_image_btn), NULL);
    g_signal_connect(select_btn, "enter-notify-event",
            G_CALLBACK(_null_event), NULL);
    center_widget(select_btn);
    step_widget(0, select_btn);
    set_display(select_btn);
    ///Box for TITLE
    GtkWidget *title_b = auto_pack_box(GTK_ORIENTATION_VERTICAL,
            0, left_b, TRUE, TRUE, 0, FALSE, -1, -1);
    change_widget_color(title_b, "#1b4a45");
    //<Title
    GtkWidget *title_lbl = gtk_label_new("Dashboard");
    gtk_box_pack_start(GTK_BOX(title_b), title_lbl, TRUE, TRUE, 0);
    gtk_widget_set_halign(title_lbl, GTK_ALIGN_CENTER);

    ////Right side containing the interface
    ///Vertical box for the right side
    GtkWidget *right_b = auto_pack_box(GTK_ORIENTATION_VERTICAL,
            0, divide_b, TRUE, TRUE, 0, TRUE, -1, -1);
    //change_widget_color(right_b, "#32a852");

    ///Header of the vertical section
    GtkWidget *header_b = auto_pack_box(GTK_ORIENTATION_VERTICAL,
            0, right_b, FALSE, FALSE, 0, TRUE, -1, height / 4);
    //change_widget_color(header_b, "#f542ce");
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
    GtkWidget *auto_btn = gtk_button_new_with_label("Auto Complete");
    gtk_box_pack_start(GTK_BOX(control_b), auto_btn, FALSE, FALSE, 0);
    gtk_widget_set_size_request(GTK_WIDGET(auto_btn), width / 8, -1);
    g_signal_connect(auto_btn, "clicked", G_CALLBACK(_on_auto_btn), NULL);
    get_controls(2, &auto_btn);

    //Button Control: Save
    GtkWidget* save_btn = gtk_button_new_with_label("Save Step");
    center_widget(save_btn);
    gtk_box_pack_end(GTK_BOX(header_b), save_btn, FALSE, FALSE, 0);
    gtk_widget_set_size_request(GTK_WIDGET(save_btn), -1, height / 24);
    g_signal_connect(save_btn, "clicked", G_CALLBACK(_on_save_btn), NULL);
    gtk_widget_set_sensitive(save_btn, FALSE);
    get_controls(3, &save_btn);

    //Helper of vertical section
    GtkWidget *helper_b = auto_pack_box(GTK_ORIENTATION_VERTICAL,
            0, right_b, FALSE, FALSE, 0, TRUE, -1, height / 8);
    //change_widget_color(helper_b, "#139485");
    
    ////History Scrollable Container
    GtkWidget *fill_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(fill_scroll, -1, (float)(height*5) / 8);
    gtk_box_pack_end(GTK_BOX(right_b), fill_scroll, TRUE, TRUE, 0);
    GtkWidget *history_b = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    get_history(history_b);
    gtk_container_add(GTK_CONTAINER(fill_scroll), history_b);

    ////Show all the created widgets
    gtk_widget_show_all(window);
}


/*  file_selector():
    Opens a file selector dialog to choose an image for the project.
*/
int file_selector() {
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
            pixbuf = resize_from_container(pixbuf, display_b);
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


/* file_save():
    Opens a file dialog to save a file for the user.
*/
int file_save(void* data, EXTENSION type) {
    //Retrieve the display section
    GtkWidget* display_b = get_display(NULL);
    //Creating the dialog window
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Save File",
            GTK_WINDOW(gtk_widget_get_toplevel(display_b)),
            GTK_FILE_CHOOSER_ACTION_SAVE,
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Save", GTK_RESPONSE_ACCEPT,
            NULL);
    GtkFileChooser* chooser = GTK_FILE_CHOOSER(dialog);
    //Filter
    GtkFileFilter *filter = gtk_file_filter_new();
    if (type == EXT_PNG) {
        gtk_file_filter_set_name(filter, "PNG files");
        gtk_file_filter_add_mime_type(filter, "image/png");
        gtk_file_chooser_set_current_name(chooser, "untilted.png");
    } else if (type == EXT_TXT) {
        gtk_file_filter_set_name(filter, "Text files");
        gtk_file_filter_add_pattern(filter, "*.txt");
        gtk_file_chooser_set_current_name(chooser, "untitlted.txt");
    }
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    //Run the dialog
    int response = gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT;
    if (response) {
        //Get the selected image
        char* filename = gtk_file_chooser_get_filename(chooser);

        if (type == EXT_PNG) {
            //Data is a pixbuf
            GdkPixbuf *pixbuf = *(GdkPixbuf**)data;
            //Save to file
            if (!gdk_pixbuf_save(pixbuf, filename, "png", NULL, NULL))
                response = 0;
        } else if (type == EXT_TXT) {
            //Data is a buffer
            GtkTextBuffer *buffer = *(GtkTextBuffer**)data;
            //Grab content
            GtkTextIter start, end;
            gtk_text_buffer_get_start_iter(buffer, &start);
            gtk_text_buffer_get_end_iter(buffer, &end);
            char* text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
            //Save to File
            FILE *file =fopen(filename, "w");
            if (file == NULL) {
                response = 0;
            } else {
                fprintf(file, "%s",text);
                fclose(file);
            }
        }
        g_free(filename);
    }

    //Closing dialog wether cancel or accepted
    gtk_widget_destroy(dialog);
    return response;
}


/* save_step():
    Save the given - finished - step into file(s).
*/
void save_step(STEP step) {
    if (step == STEP_LOAD || step == STEP_FILTER || step == STEP_RECONSTRUCT) {
        GtkWidget *image = step_widget(step+1, NULL);
        GdkPixbuf *pixbuf = g_object_get_data(G_OBJECT(image), "pixbuf");
        file_save(&pixbuf, EXT_PNG);
    } else if (step == STEP_EXTRACT) {
        GtkWidget *extract = step_widget(step+1, NULL);
        //Grab content of text view or smth
        GtkTextBuffer *b_grid = g_object_get_data(G_OBJECT(extract), "grid");
        file_save(&b_grid, EXT_TXT);
        GtkTextBuffer *b_wordlist = g_object_get_data(G_OBJECT(extract),
                "wordlist");
        file_save(&b_wordlist, EXT_TXT);
    } else if (step == STEP_SOLVE) {
        GtkWidget *textview = step_widget(step+1, NULL);
        //Grab content of text view
        GtkTextBuffer *buffer = g_object_get_data(G_OBJECT(textview),
                "buffer");
        file_save(&buffer, EXT_TXT);
    }
}
