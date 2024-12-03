/*
      ##############################################################
      #                                                            #
      #                         GTK_Window.c                       #
      #                                                            #
      #         Contains all functions related to handling,        #
      #   modifying and supervising memory of GTK windows system.  #
      #                                                            #
      ##############################################################
*/


////HEADERS
#include <err.h>
#include <gtk/gtk.h>


GtkWidget *create_window(GtkWindowType type, char* title, int width,
        int height) {
    GtkWidget *window = gtk_window_new(type);
    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
    //gtk_widget_set_size_request(window, width, height);
    gtk_window_maximize(GTK_WINDOW(window));
    return window;
}


void get_screen_size(GdkRectangle *geometry) {
    GdkDisplay *display = gdk_display_get_default();
    //0 => first screen
    GdkMonitor *monitor = gdk_display_get_monitor(display, 0);
    gdk_monitor_get_geometry(monitor, geometry);
}
