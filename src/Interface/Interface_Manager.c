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
        int percentage=atoi(value);
        *end_value = '<';
        //Calculating and writing the new value
        int new_value = (int)((percentage/100.0f)*(isWIDTH?width:height));
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
    AppState *state = get_appState();
    //Construct interface from XML file
    state->builder = gtk_builder_new_custom("main.glade",
            state->alloc_width, state->alloc_height);
    //Connect to window
    GtkWidget *toplvl = GETWIDGET("toplvl_id");
    gtk_container_add(GTK_CONTAINER(state->window), toplvl);
    gtk_builder_connect_signals(state->builder, NULL);

    /*//////Manual adjustements not possible with Glade
    //Image Background - Reordering
    GtkWidget *app_bg = image_new_from_file("src/assets/app_bg.png");
    GtkWidget *box = GTK_WIDGET(gtk_builder_get_object(state->builder,"divide_box_id"));
    //GETWIDGET("divide_box_id");
    gtk_container_remove(GTK_CONTAINER(toplvl), box);
    gtk_overlay_add_overlay(GTK_OVERLAY(toplvl),app_bg);
    gtk_overlay_add_overlay(GTK_OVERLAY(toplvl),box);
    gtk_widget_show(app_bg);*/
}
