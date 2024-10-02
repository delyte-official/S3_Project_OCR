/*

      ##############################################################
      #                                                            #
      #                           Debug.c                          #
      #                                                            #
      #        Contains functions for debugging the other files    #
      #     quickly without having to write damp code everywhere.  #
      #                                                            #
      ##############################################################


No list is given for this file as it contains temporary functions.
*/





////HEADERS Files
#include <err.h>
#include <stdio.h>
#include <gtk/gtk.h>

////END HEADERS

void change_widget_color(GtkWidget *widget, const char *color) {
    GdkRGBA rgba;
    gdk_rgba_parse(&rgba, color);
    gtk_widget_override_background_color(widget, GTK_STATE_FLAG_NORMAL, &rgba);
}
