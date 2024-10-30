#ifndef EVENTS_H
#define EVENTS_H

#include <gtk/gtk.h>
#include "../Core_Manager.h"

void Standard_Signals(GtkWidget *window);
void _application_init(GtkWidget *window, GtkAllocation *alloc, gpointer);
void _null_event(GtkWidget*, gpointer);
void _on_select_image_btn(GtkWidget*, gpointer);
void _on_auto_btn(GtkWidget* auto_btn, gpointer);
void _on_save_btn(GtkWidget*, gpointer);
void _on_step_save_history(GtkWidget*, STEP step);
void _on_jumpto_step(GtkWidget*, STEP step);

#endif
