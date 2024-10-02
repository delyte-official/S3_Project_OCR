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
*/











////HEADERS Files
//Integrated C Libraries
#include <err.h>
#include <stdio.h>

//GTK Libraries
#include <gtk/gtk.h>

//Project Headers


////END HEADERS




/*  Standard_Signals():
  Links all events and signals to their designated functions.

Requisites assumed:
  It must be the first and last time that this function is called.
  Other intermediate event/signal linkage must be done through
  another function or canal.
*/
void Standard_Signals(GtkWidget *window) {
    //Closing window closes the program
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}



/*  file_selector():
    Opens a file selector dialog to choose an image for the project.
*/
void file_selector(GtkWidget *button, GtkWidget *box_left) {
    GtkWidget *dialog;
    
    //Creating the dialog window
    dialog = gtk_file_chooser_dialog_new("Select Image",
            GTK_WINDOW(gtk_widget_get_toplevel(button)),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Open", GTK_RESPONSE_ACCEPT,
            NULL);

    //Only accepting images
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pixbuf_formats(filter);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    //Run the dialog
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        //Get the selected image
        char* filename = gtk_file_chooser_get_filename(
                GTK_FILE_CHOOSER(dialog));
        
        printf("Filename: %s\n", filename);
        if (g_file_test(filename, G_FILE_TEST_EXISTS))
            printf("File does exist: %s\n", filename);
        //Load image
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
        printf("Loaded into pixbuf\n");
        if (pixbuf != NULL) {
            GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
            printf("Loaded as image\n");
            //Display image on given widget
            GList *children = gtk_container_get_children(
                    GTK_CONTAINER(box_left));
            if (children)
                gtk_widget_destroy(GTK_WIDGET(children->data));
            
            gtk_box_pack_start(GTK_BOX(box_left), image, TRUE, TRUE, 0);
            gtk_widget_show(image);

            //Free ressourece
            g_object_unref(pixbuf);
        }
        g_free(filename);
    }

    //Closing dialog wether cancel or accepted
    gtk_widget_destroy(dialog);
}
