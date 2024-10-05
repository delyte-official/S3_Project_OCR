/*

      ##############################################################
      #                                                            #
      #                    GTK_Window_Manager.c                    #
      #                                                            #
      #         Contains all functions related to handling,        #
      #   modifying and supervising memory of GTK windows system.  #
      #                                                            #
      ##############################################################


List of all functions written in this file (and their type):
[See more description on their purpose and parameters down below]

GtkWidget *create_window();
*/











////HEADERS Files
//Integrated C Libraries
#include <err.h>

//GTK Libraries
#include <gtk/gtk.h>

//Project Headers


////END HEADERS


/* create_window():
    Create and returns a GTK with the given arguments.
*/
GtkWidget *create_window(
        GtkWindowType type,
        char* title,
        int width,
        int height) {

    GtkWidget *window;

    //Creating the window
    window = gtk_window_new(type);
    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
    //gtk_widget_set_size_request(window, width, height);
    gtk_window_maximize(GTK_WINDOW(window));

    return window;
}




/*  get_screen_details():
    Requests and returns the screen details.
    Also stores the width and height in the given pointers for direct access.
*/
void get_screen_size(GdkRectangle *geometry) {
    //Get monitor details
    GdkDisplay *display = gdk_display_get_default();
    GdkMonitor *monitor = gdk_display_get_monitor(display, 0); // 0 for the primary monitor

    // Get the monitor geometry
    gdk_monitor_get_geometry(monitor, geometry);
}
