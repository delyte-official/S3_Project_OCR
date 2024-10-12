#ifndef EVENTS_MANAGER_H
#define EVENTS_MANAGER_H

#include <gtk/gtk.h>

void Standard_Signals(GtkWidget *window);
int _on_select_image_btn(GtkWidget*, gpointer);
void _on_auto_btn(GtkWidget* auto_btn, gpointer);

#endif
