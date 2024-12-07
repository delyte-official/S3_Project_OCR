#ifndef EVENTS_H
#define EVENTS_H

#include <gtk/gtk.h>
#include "../Core_Manager.h"

void Standard_Signals();
void _application_init(GtkWidget*, GtkAllocation*, gpointer);
void _on_auto_btn(GtkWidget *auto_btn, gpointer);
void _on_import_btn(GtkWidget*,gpointer);
void _log_handler(const gchar*, GLogLevelFlags log_level,
        const gchar *message, gpointer);
void _on_change_rotate(GtkWidget*, GtkStack *stack);
void _on_save_rotate(GtkWidget*, GtkStack *stack);
void _on_cancel_rotate(GtkWidget*, GtkStack *stack);
void _on_rotate_value(GtkWidget *range, gpointer);

#endif
