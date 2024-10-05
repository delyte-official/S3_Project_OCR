#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <gtk/gtk.h>

GtkWidget *create_window(GtkWindowType type, char* title, int width,
        int height);
void get_screen_size(GdkRectangle *geometry);

#endif
