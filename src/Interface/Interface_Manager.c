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


////HEADERS
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <gtk/gtk.h>
#include <pango/pango.h>
//Project Headers
#include "Events.h"
#include "../Core_Manager.h"
#include "Interface_Manager.h"


GtkWidget* image_new_from_file(
        char* filename) {
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
    GtkWidget* image = gtk_image_new_from_pixbuf(pixbuf);
    g_object_unref(pixbuf);
    return image;
}


/* gtk_builder_new_custom():
    Load XML file and modify its size into percentage of given dimensions.
*/
GtkBuilder *gtk_builder_new_custom(char* filename, int width, int height) {
    FILE *file = fopen(filename,"r");
    if (!file)
        errx(EXIT_FAILURE,"fopen()"); //Can't load application

    //Read content of XML file
    fseek(file,0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file,0,SEEK_SET);
    char* xml_content = malloc(file_size+1); //include \0
    if (!xml_content)
        errx(EXIT_FAILURE,"malloc()");
    fread(xml_content, 1, file_size, file);
    xml_content[file_size] = '\0';
    fclose(file);

    //Modifying the content
    size_t new_size = file_size+1;
    char* xml_modified = malloc(new_size);
    if (!xml_modified)
        errx(EXIT_FAILURE,"malloc()");
    char* toread = xml_content;
    char* towrite = xml_modified;
    //Modify every properties "width-request" and "height-request":
    while (*toread) {
        char* width_prop = strstr(toread, "width-request");
        char *height_prop = strstr(toread, "height-request");
        char *prop = NULL;
        int isWIDTH = 0;
        if (width_prop && (!height_prop || width_prop<height_prop)) {
            prop = width_prop;
            isWIDTH=1;
        } else if (height_prop)
            prop = height_prop;
        if (!prop) { //no more properties to modify
            strcpy(towrite, toread);
            break;
        }
        //Changing size of property
        char *value = strchr(prop,'>')+1;
        size_t prefix_l = value-toread;
        strncpy(towrite, toread, prefix_l);
        towrite+=prefix_l;
        char *end_value = strchr(value,'<');
        *end_value = '\0';
        int read_value=atoi(value);
        *end_value = '<';
        //Calculating and writing the new value
        int new_value = read_value;
        if (read_value>=0&&read_value<=100)
            new_value= (int)((read_value/100.0f)*(isWIDTH?width:height));
        else if (read_value>100)
            new_value=read_value-100;
        int expand = snprintf(NULL,0,"%d",new_value) - (end_value-value);
        new_size+=expand;
        size_t curr_len = towrite-xml_modified;
        char* tmp = realloc(xml_modified,new_size);
        if (!tmp)
            errx(EXIT_FAILURE,"realloc()");
        xml_modified = tmp;
        towrite = xml_modified + curr_len;
        towrite += sprintf(towrite, "%d", new_value);
        toread=end_value; //Prevent loops
    }
    xml_modified[new_size-1]='\0';
    //Load interface
    GtkBuilder *builder = gtk_builder_new_from_string(xml_modified,-1);
    free(xml_content);
    free(xml_modified);
    return builder;
}


void Build_Interface() {
    AppState *state = APPSTATE;
    //Construct interface from XML file
    state->builder = gtk_builder_new_custom("main.glade",
            state->alloc_width, state->alloc_height);
    //Connect to window
    GtkWidget *toplvl = GETWIDGET("toplvl_id");
    gtk_container_add(GTK_CONTAINER(state->window), toplvl);
    gtk_builder_connect_signals(state->builder, NULL);
    //Manual data
    state->display = GETWIDGET("display_section");
}


void Show_Widget(GtkWidget *widget) {
    GtkWidget* display = DISPLAY;
    GList *children = gtk_container_get_children(GTK_CONTAINER(display));
    if (children) {
        g_object_ref(children->data);
        gtk_container_remove(GTK_CONTAINER(display), children->data);
    }
    if (widget != NULL) {
        gtk_overlay_add_overlay(GTK_OVERLAY(display), widget);
        gtk_widget_show_all(widget);
    }
}


GdkPixbuf* resize_pixbuf(GdkPixbuf* pixbuf, int max_w, int max_h) {
    int origin_w = gdk_pixbuf_get_width(pixbuf);
    int origin_h = gdk_pixbuf_get_height(pixbuf);
    float w_ratio = (float)max_w/origin_w;
    float h_ratio = (float)max_h/origin_h;
    float scale = w_ratio < h_ratio ? w_ratio : h_ratio;
    int width = (int)(origin_w * scale);
    int height = (int)(origin_h *scale);
    GdkPixbuf *scaled = gdk_pixbuf_scale_simple(pixbuf, width, height,
            GDK_INTERP_HYPER);
    return scaled;
}


GdkPixbuf* resize_from_container(GdkPixbuf* pixbuf, GtkWidget* container) {
    GtkAllocation alloc;
    gtk_widget_get_allocation(container, &alloc);
    return resize_pixbuf(pixbuf, alloc.width, alloc.height);
}


int Load_Image() {
    AppState *state = APPSTATE;
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Select Image",
            GTK_WINDOW(gtk_widget_get_toplevel(state->window)),
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
        char* filename = gtk_file_chooser_get_filename(
                GTK_FILE_CHOOSER(dialog));
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
        if (pixbuf != NULL) {
            GdkPixbuf *resized = resize_from_container(pixbuf, DISPLAY);
            GtkWidget *image = gtk_image_new_from_pixbuf(resized);
            g_object_set_data(G_OBJECT(image), "pixbuf", pixbuf);
            g_object_ref(pixbuf);
            state->steps_tracker[0]=image;
        }
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
    return response;
}
