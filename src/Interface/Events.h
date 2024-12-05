#ifndef EVENTS_H
#define EVENTS_H

#include <gtk/gtk.h>
#include "../Core_Manager.h"

void Standard_Signals();
void _application_init(GtkWidget*, GtkAllocation*, gpointer);
void _on_auto_btn(GtkWidget *auto_btn, gpointer);
void _on_import_btn(GtkWidget*,gpointer);

#endif
