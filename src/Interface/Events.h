#ifndef EVENTS_H
#define EVENTS_H

#include <gtk/gtk.h>

void Standard_Signals(GtkWidget *window);
void _application_init(GtkWidget *window, GtkAllocation *alloc, gpointer);
void _null_event(GtkWidget*, gpointer);
void _on_select_image_btn(GtkWidget*, gpointer);
void _on_auto_btn(GtkWidget* auto_btn, gpointer);
void _on_save_btn(GtkWidget*, gpointer);

#endif
