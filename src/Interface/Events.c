////HEADERS
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
//Project Headers
#include "../Core_Manager.h"
#include "Events.h"
#include "Interface.h"


void Standard_Signals() {
    AppState *state = APPSTATE;
    g_signal_connect(state->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(state->window, "size-allocate",
            G_CALLBACK(_application_init), NULL);
}


void _application_init(GtkWidget *window, GtkAllocation*, gpointer) {
    AppState *state = APPSTATE;
    int width, height;
    gtk_window_get_size(GTK_WINDOW(window), &width, &height);
    if (width > state->width || height >= state->height)
        return;
    width = 1920; //Terminal blocks half the screen
    state->alloc_width = width;
    state->alloc_height = height;
    Build_Interface();
    g_signal_handlers_disconnect_matched(window, G_SIGNAL_MATCH_FUNC, 0, 0,
            NULL, G_CALLBACK(_application_init), NULL);
}


/* _on_auto_btn():
    Computes all the steps automatically.
*/
void _on_auto_btn(GtkWidget* auto_btn, gpointer) {
    AppState *state = APPSTATE;
    //Perform every step
    for (int i = state->step; i < STEP_END; i++) {
        if (!NextStep(NULL, NULL))
            return; //Error, stop
    }
    gtk_widget_set_sensitive(auto_btn, FALSE);
}


void _on_import_btn(GtkWidget*, gpointer) {
    if (GETSTEPDATA(STEP_LOAD)==NULL)
        NextStep(NULL,NULL);
    else if (confirm_dialog("Further steps will be deleted.")) {
        if (Load_Image()) {
            //Reset steps
            for (int i = STEP_FILTER; i < STEP_END; i++) {
                GtkWidget *stepdata = GETSTEPDATA(i);
                if (stepdata!=NULL)
                    g_object_unref(GETSTEPDATA(i));
                SETSTEPDATA(i,NULL);
            }
            ShowNext();
            APPSTATE->step++;
        }
    }
}


/* _log_handler():
    Redirects stdout and stderr messages from GTK libs to application.
*/
void _log_handler(const gchar* log_domain, GLogLevelFlags log_level,
        const gchar *message, gpointer) {
    const char *category;
    if (log_level & G_LOG_LEVEL_ERROR)
        category = "ERROR: ";
    else if (log_level & G_LOG_LEVEL_WARNING)
        category = "WARNING: ";
    else if (log_level & G_LOG_LEVEL_INFO)
        category = "INFO: ";
    else if (log_level & G_LOG_FLAG_RECURSION)
        category = "RECURSION: ";
    else if (log_level & G_LOG_FLAG_FATAL)
        category = "FATAL: ";
    else if (log_level & G_LOG_LEVEL_MESSAGE)
        category = "MESSAGE: ";
    else if (log_level & G_LOG_LEVEL_DEBUG)
        category = "DEBUG: ";
    else if (log_level & G_LOG_LEVEL_CRITICAL)
        category = "CRITICAL: ";
    else
        category = "";
    print("%s-%s%s\n",log_domain,category,message);
}


void _on_logs_change(GtkTextBuffer *buffer, GtkTextView *text_view) {
    GtkTextIter end_iter;
    GtkTextMark *end_mark;
    gtk_text_buffer_get_end_iter(buffer,&end_iter);
    end_mark = gtk_text_buffer_create_mark(buffer,NULL,&end_iter,FALSE);
    gtk_text_view_scroll_to_mark(text_view,end_mark,0.0,TRUE,0.0,1.0);
    gtk_text_buffer_delete_mark(buffer,end_mark);
}


void _save_curr_step(GtkWidget*, gpointer) {
    save_step(APPSTATE->step-1);
}
