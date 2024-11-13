 /*
      ##############################################################
      #                                                            #
      #                  GTK_Interface_Manager.c                   #
      #                                                            #
      #            Create, modify and destroy GTK Widgets.         #
      #  Offer easy access to widgets and redirects their events.  #
      #                                                            #
      ##############################################################
*/


////HEADERS Files
//Integrated C Libraries
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
//Project Headers
#include "Events.h"
#include "../Core_Manager.h"
#include "Interface_Manager.h"
//Tools
#include <gtk/gtk.h>
#include <pango/pango.h>
////END HEADERS


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
    Returns the specified target button.
*/
GtkWidget *get_controls(char* id) {
    AppState *state = get_app_state();
    return GTK_WIDGET(gtk_builder_get_object(state->builder, id));
}

/* image_load_from_file():
    Loads an image from a file and return its image widget.
*/
GtkWidget* image_load_from_file(
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
/*GtkWidget *get_resized_step(STEP step, int width, int height) {
    GtkWidget *widget = step_widget(step+1, NULL);
    if (GTK_IS_IMAGE(widget)) {
        GdkPixbuf *old_pixbuf = g_object_get_data(G_OBJECT(widget), "pixbuf");
        GdkPixbuf *pixbuf = resize(old_pixbuf, width, height);
        widget = gtk_image_new_from_pixbuf(pixbuf);
    } else if (step == STEP_SOLVE || step == STEP_EXTRACT) {
        GtkWidget *new_textview = gtk_text_view_new();
        //Transpose text or wtv
        GtkTextBuffer *obuffer = g_object_get_data(G_OBJECT(widget), "buffer");
        GtkTextBuffer *nbuffer = gtk_text_buffer_new(NULL);
        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(obuffer, &start);
        gtk_text_buffer_get_end_iter(obuffer, &end);
        gchar *text = gtk_text_buffer_get_text(obuffer, &start, &end, FALSE);
        gtk_text_buffer_set_text(nbuffer, text, -1);
        g_free(text);
        gtk_text_view_set_buffer(GTK_TEXT_VIEW(new_textview), nbuffer);
        
        //Size
        GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
        gtk_widget_set_size_request(scroll, width, height);
        gtk_widget_set_size_request(new_textview, width, height);
        gtk_container_add(GTK_CONTAINER(scroll), new_textview);
        widget = scroll;
    }
    return widget;
}*/


/* get_history():
    Returns the history scroll container.
*/
GtkWidget* get_history(GtkWidget *widget) {
    static GtkWidget* history = NULL;
    if (history == NULL)
        history = widget;
    return history;
}


/* clear_history_from():
    Clears every step of the history.
*/
/*void clear_history_from(STEP step) {
    GList *child = gtk_container_get_children(
            GTK_CONTAINER(get_history(NULL)));
    //Skipping header of history
    //child = child->next;
    //Searching for step
    int i = 0;
    while (i < (int)step && child != NULL) {
        child = child->next;
        i+=1;
    }
    while (child != NULL) {
        gtk_widget_destroy(GTK_WIDGET(child->data));
        child = child->next;
    }
}*/


/* add_history_step():
    Add a tile to the history scroll container, representing a step done.
*/
/*void add_history_step(STEP step) {
    //Creating the tile
    GtkWidget *tile_o = gtk_overlay_new();
    //Add image
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(
            "src/assets/history_tile.png", NULL);
    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
    gtk_overlay_add_overlay(GTK_OVERLAY(tile_o), image);
    gtk_widget_set_size_request(tile_o,(float)(global_width * 3) / 8,
            global_height / 6);
    center_widget(tile_o);

    //Content
    GtkWidget *container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_overlay_add_overlay(GTK_OVERLAY(tile_o), container);

    ///Preview
    //Spacing
    auto_pack_box(GTK_ORIENTATION_HORIZONTAL, 0, container, FALSE, FALSE, 0,
            1, (float)(global_width * 3) / 160, -1);
    //Converting
    GtkWidget* preview = get_resized_step(step,
            (float)(global_width*593) / 10000,
            (float)(global_height * 7)/ 60);
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
    change_label_color(title, 255, 255, 255);
    change_label_color(desc, 255, 255, 255);

    ///Buttons
    GtkWidget *buttons = auto_pack_box(GTK_ORIENTATION_VERTICAL,5,container,
            FALSE,FALSE,30,0,(float)(global_width * 3) / 40,-1);
    auto_pack_box(GTK_ORIENTATION_VERTICAL,0,buttons,FALSE,FALSE,0,1,-1,50);
    //Save
    GtkWidget *save_btn = gtk_button_new_with_label("Save output");
    gtk_box_pack_start(GTK_BOX(buttons), save_btn, FALSE, FALSE, 0);
    g_signal_connect(save_btn, "clicked", G_CALLBACK(_on_step_save_history),
            GINT_TO_POINTER((int)step));
    GtkWidget *jumpto_btn = gtk_button_new_with_label("Jump to step");
    gtk_box_pack_start(GTK_BOX(buttons), jumpto_btn, FALSE, FALSE, 0);
    g_signal_connect(jumpto_btn, "clicked", G_CALLBACK(_on_jumpto_step),
            GINT_TO_POINTER((int)step));

    //Add to history
    gtk_box_pack_start(GTK_BOX(get_history(NULL)), tile_o, FALSE, FALSE, 0);
    gtk_widget_show_all(tile_o);
}*/


/* gtk_builder_new_custom():
    Creates a gtk builder from an XML file with PERCENTAGE format for
    the sizes of the "width-request" and "height-request" properties.
*/
GtkBuilder *gtk_builder_new_custom(char* filename, int width, int height) {
    FILE *file = fopen(filename,"r");
    if (!file)
        errx(EXIT_FAILURE,"fopen()"); //ERROR - should NOT happen.
    //Finding size to allocate
    fseek(file,0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file,0,SEEK_SET);
    char* xml_content = malloc(file_size+1); //include \0
    if (!xml_content)
        errx(EXIT_FAILURE,"malloc()"); //error malloc
    fread(xml_content, 1, file_size, file);
    xml_content[file_size] = '\0'; //End string
    fclose(file);

    //Modifying the content
    size_t new_size = file_size+1;
    char* xml_modified = malloc(new_size);
    if (!xml_modified)
        errx(EXIT_FAILURE,"malloc()"); //error malloc ag-in
    //Pointer to where we are reading/writing
    char* toread = xml_content;
    char* towrite = xml_modified;
    //Modify every properties "width-request" and "height-request":
    while (*toread) {
        char* width_prop = strstr(toread, "width-request");
        char *height_prop = strstr(toread, "height-request");
        //Dealing with only one of them
        char *prop = NULL;
        int isWIDTH = 0;
        if (width_prop && (!height_prop || width_prop<height_prop)) {
            prop = width_prop;
            isWIDTH=1;
        } else if (height_prop)
            prop = height_prop;
        if (!prop) { //no more props to modify
            //Copying any other character
            strcpy(towrite, toread);
            break;
        }
        //Dealing with the property- assume NO errors (= XML error)
        char *value = strchr(prop,'>')+1;
        size_t prefix_l = value-toread;
        strncpy(towrite, toread, prefix_l);
        towrite+=prefix_l;
        char *end_value = strchr(value,'<');
        *end_value = '\0';
        int percentage=atoi(value);
        *end_value = '<';
        //Calculating and writing the new value
        int new_value = (int)((percentage/100.0f)*(isWIDTH?width:height));
        int expand = snprintf(NULL,0,"%d",new_value) - (end_value-value);
        //Changing size
        new_size+=expand;
        size_t curr_len = towrite-xml_modified;
        char* tmp = realloc(xml_modified,new_size);
        if (!tmp)
            errx(EXIT_FAILURE,"realloc()");
        xml_modified = tmp;
        towrite = xml_modified + curr_len;
        towrite += sprintf(towrite, "%d", new_value);
        toread=end_value; //To not read the same item twice
    }
    xml_modified[new_size-1]='\0';
    GtkBuilder *builder = gtk_builder_new_from_string(xml_modified,-1);
    free(xml_content);
    free(xml_modified);
    return builder;
}


/* Build_Interface():
    Creates every widget and the structure of the starting project menu.
    Creates every needed signals for the application to run properly.
*/
void Build_Interface(GtkWidget *window, AppState *state) {
    //Construct the interface from an XML file with GTKBuilder
    state->builder = gtk_builder_new_custom("src/assets/main.glade",1920,1041);
    //The "MAIN.glade" only builds the toplevel widget, not window, thus:
    GtkWidget *toplvl = GETWIDGET("toplvl_id");
    gtk_container_add(GTK_CONTAINER(window), toplvl);

    //Automatically link all the signals from the builder
    gtk_builder_connect_signals(state->builder, NULL);

    //////Manual adjustements not possible with Glade
    //Image Background - Reordering
    GtkWidget *app_bg = image_load_from_file("src/assets/app_bg.png");
    GtkWidget *box = GETWIDGET("divide_box_id");
    gtk_container_remove(GTK_CONTAINER(toplvl), box);
    gtk_overlay_add_overlay(GTK_OVERLAY(toplvl),app_bg);
    gtk_overlay_add_overlay(GTK_OVERLAY(toplvl),box);
}


/*  file_selector():
    Opens a file selector dialog to choose an image for the project.
*/
/*int file_selector() {
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
            GdkPixbuf *resized = resize_from_container(pixbuf, display_b);
            //Creating the image
            GtkWidget *image = gtk_image_new_from_pixbuf(resized);
            //Storing the original pixbuf inside the image widget
            //to keep as much quality as possible while displaying "resized"
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
}*/


/* file_save():
    Opens a file dialog to save a file for the user.
*/
/*int file_save(void* data, EXTENSION type) {
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
            //Checking extension
            if (!g_str_has_suffix(filename, ".png") && !strchr(filename,'.')) {
                char *newfile = g_strconcat(filename, ".png", NULL);
                g_free(filename);
                filename = newfile;
            }
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
}*/


/* save_step():
    Save the given - finished - step into file(s).
*/
/*void save_step(STEP step) {
    if (step == STEP_LOAD || step == STEP_FILTER || step == STEP_RECONSTRUCT) {
        GtkWidget *image = step_widget(step+1, NULL);
        GdkPixbuf *pixbuf = g_object_get_data(G_OBJECT(image), "pixbuf");
        file_save(&pixbuf, EXT_PNG);
    } else if (step == STEP_EXTRACT) {
        GtkWidget *extract = step_widget(step+1, NULL);
        //Grab content of text view or smth
        GtkTextBuffer *b_grid = g_object_get_data(G_OBJECT(extract), "buffer");
        file_save(&b_grid, EXT_TXT);
        GtkTextBuffer *b_wordlist = g_object_get_data(G_OBJECT(extract),
                "buffer:words");
        file_save(&b_wordlist, EXT_TXT);
    } else if (step == STEP_SOLVE) {
        GtkWidget *textview = step_widget(step+1, NULL);
        //Grab content of text view
        GtkTextBuffer *buffer = g_object_get_data(G_OBJECT(textview),
                "buffer");
        file_save(&buffer, EXT_TXT);
    }
}*/


/* confirm_dialog():
    Pops up a confirmation dialog on the window.
*/
/*int confirm_dialog(
        char* text) {
    GtkWidget *display = get_display(NULL);
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Confirm?",
            GTK_WINDOW(gtk_widget_get_toplevel(display)), //Main window
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            "Confirm", GTK_RESPONSE_OK,
            "Cancel", GTK_RESPONSE_CANCEL,
            NULL);

    //Add confirmation text
    GtkWidget *area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *label = gtk_label_new(text);
    GtkWidget *label_c = gtk_label_new("Are you sure you want to continue?");
    gtk_container_add(GTK_CONTAINER(area), label);
    auto_pack_box(GTK_ORIENTATION_VERTICAL,0,area,FALSE,FALSE, 0, 1, -1, 30);
    gtk_container_add(GTK_CONTAINER(area), label_c);
    //Run the dialog
    gtk_widget_show_all(dialog);
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return response == GTK_RESPONSE_OK;
}*/


/* modify_image():
    Modifies the input image from user input such as rotation angle.
*/
/*int modify_image() {
    GtkWidget *display = get_display(NULL);
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Modify Image",
            GTK_WINDOW(gtk_widget_get_toplevel(display)), //Main window ag
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            "Confirm", GTK_RESPONSE_OK,
            "Cancel", GTK_RESPONSE_CANCEL,
            NULL); //End of sentinel
    //Modify image
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *area = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(content), scroll);
    gtk_container_add(GTK_CONTAINER(scroll), area);
    gtk_widget_set_size_request(scroll, 900, 900);

    GObject *curr_step = G_OBJECT(step_widget(STEP_LOAD+1, NULL));
    GdkPixbuf *original_pixbuf = g_object_get_data(curr_step, "pixbuf");
    GtkWidget *image = gtk_image_new_from_pixbuf(original_pixbuf);
    g_object_set_data(G_OBJECT(image), "pixbuf", original_pixbuf);
    gtk_box_pack_start(GTK_BOX(area), image, TRUE, TRUE, 0);

    GtkWidget *entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(area), entry, TRUE, TRUE, 0);

    GtkWidget *apply = gtk_button_new_with_label("Apply");
    gtk_box_pack_start(GTK_BOX(area), apply, TRUE, TRUE, 0);
    GtkWidget *error_show = gtk_label_new("Invalid float integer input.");
    gtk_box_pack_start(GTK_BOX(area), error_show, TRUE, TRUE, 0);

    GtkWidget *table[4] = {entry , image, error_show, area};
    g_signal_connect(apply, "clicked",
            G_CALLBACK(_on_apply_rotation), table);
    //Run the dialog
    gtk_widget_show_all(dialog);
    gtk_widget_hide(error_show);
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) { //Save modifications
        //Grab result
        GdkPixbuf *result = g_object_get_data(G_OBJECT(image), "pixbuf");
        //Update display + stockage
        GtkWidget *displayed = step_widget(STEP_LOAD+1,NULL);
        //Resized version
        GdkPixbuf *resized = resize_from_container(result, get_display(NULL));
        gtk_image_set_from_pixbuf(GTK_IMAGE(displayed), resized);
        g_object_set_data(G_OBJECT(displayed), "pixbuf", result);
        g_object_ref(result);
        gtk_widget_destroy(dialog);
        //Updating history
        clear_history_from(STEP_LOAD);
        add_history_step(STEP_LOAD);
        return 1;
    }
    gtk_widget_destroy(dialog);
    return 0;
}*/
