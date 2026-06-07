////HEADERS
#define _GNU_SOURCE
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
//Project Headers
#include "../Core_Manager.h"
#include "Events.h"
#include "Interface.h"
#include "../Filter/Filter.h"


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
            free_all_steps(STEP_FILTER, STEP_RECONSTRUCT);
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

void _on_change_rotate(GtkWidget*, GtkStack *stack) {
    //Show page
    gtk_stack_set_visible_child_name(stack,"INPUT_ROTATE");
    gtk_stack_set_transition_type(GTK_STACK(DISPLAY),
            GTK_STACK_TRANSITION_TYPE_NONE);
    GtkWidget *original_img = GETSTEPDATA(APPSTATE->step-1);
    GdkPixbuf *original_pix = g_object_get_data(G_OBJECT(original_img),
                "pixbuf");
    GdkPixbuf *modified_pix = gdk_pixbuf_copy(original_pix);
    GtkWidget *modified_img = gtk_image_new_from_pixbuf(
            gtk_image_get_pixbuf(GTK_IMAGE(original_img)));
    g_object_set_data(G_OBJECT(modified_img),"pixbuf",modified_pix);
    g_object_ref(modified_pix);
    AddPage("MODIFY",modified_img);
    ShowPage("MODIFY");
    //Reset value
    GtkWidget *scale = GETWIDGET("scale_rotate");
    gulong handler_id = g_signal_handler_find(scale, G_SIGNAL_MATCH_FUNC,
            0, 0, NULL, (GCallback)_on_rotate_value, NULL);
    g_signal_handler_block(scale, handler_id);
    gtk_range_set_value(GTK_RANGE(scale),0);
    g_signal_handler_unblock(scale,handler_id);
}

void _on_save_rotate(GtkWidget*, GtkStack *stack) {
    if (!APPSTATE->settings.unsaved_changes) {
        _on_cancel_rotate(NULL,stack);
        return;
    }
    gtk_stack_set_visible_child_name(stack, "OUTPUT");
    APPSTATE->settings.unsaved_changes = FALSE;
    GtkWidget *page = gtk_stack_get_child_by_name(GTK_STACK(DISPLAY),
            "MODIFY");
    //Update original image
    GtkWidget *official = GETSTEPDATA(APPSTATE->step-1);
    g_object_set_data(G_OBJECT(official), "pixbuf",
            GDK_PIXBUF(g_object_get_data(G_OBJECT(page),"pixbuf")));
    gtk_image_set_from_pixbuf(GTK_IMAGE(official),
            gtk_image_get_pixbuf(GTK_IMAGE(page)));
    //Render results
    ShowPage(STEPtoSTR(APPSTATE->step-1));
    gtk_widget_destroy(page);
    gtk_stack_set_transition_type(GTK_STACK(DISPLAY),
            GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    free_all_steps(APPSTATE->step,STEP_RECONSTRUCT);
}

void _on_cancel_rotate(GtkWidget*, GtkStack *stack) {
    ShowPage(STEPtoSTR(APPSTATE->step-1));
    GtkWidget *page = gtk_stack_get_child_by_name(GTK_STACK(DISPLAY),
            "MODIFY");
    if (page)
        gtk_widget_destroy(page);
    gtk_stack_set_visible_child_name(stack, "OUTPUT");
    APPSTATE->settings.unsaved_changes = FALSE;
    gtk_stack_set_transition_type(GTK_STACK(DISPLAY),
            GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
}

void _on_rotate_value(GtkWidget* range, gpointer) {
    double value = gtk_range_get_value(GTK_RANGE(range));
    if (value!=0)
        APPSTATE->settings.unsaved_changes = TRUE;
    else
        APPSTATE->settings.unsaved_changes = FALSE;
    GtkWidget *data = GETSTEPDATA(APPSTATE->step-1);
    GdkPixbuf *pixbuf = g_object_get_data(G_OBJECT(data),"pixbuf");
    GdkPixbuf *new_pix = resize_from_container(rotate_pixbuf(pixbuf,value),
            DISPLAY);
    GtkWidget *page = gtk_stack_get_child_by_name(GTK_STACK(DISPLAY),"MODIFY");
    gtk_image_set_from_pixbuf(GTK_IMAGE(page), new_pix);
    g_object_set_data(G_OBJECT(page),"pixbuf",new_pix);
}


void _on_change_ocr(GtkWidget*, GtkStack *stack) {
    //Show page
    gtk_stack_set_visible_child_name(stack,"INPUT_BUFFERS");
    gtk_stack_set_transition_type(GTK_STACK(DISPLAY),
            GTK_STACK_TRANSITION_TYPE_NONE);
    GtkWidget *original_img = GETSTEPDATA(STEP_LOAD);
    GdkPixbuf *original_pix = g_object_get_data(G_OBJECT(original_img),
                "pixbuf");
    GdkPixbuf *modified_pix = gdk_pixbuf_copy(original_pix);
    GtkWidget *modified_img = gtk_image_new_from_pixbuf(
            gtk_image_get_pixbuf(GTK_IMAGE(original_img)));
    g_object_set_data(G_OBJECT(modified_img),"pixbuf",modified_pix);
    g_object_ref(modified_pix);
    AddPage("MODIFY",modified_img);
    ShowPage("MODIFY");
    //Write in the buffers
    GtkWidget *data = GETSTEPDATA(STEP_OCR);
    GtkBuilder *builder = GTK_BUILDER(g_object_get_data(G_OBJECT(data),
                "builder"));
    GtkTextBuffer *buffer_g = GTK_TEXT_BUFFER(
            gtk_builder_get_object(builder,"grid"));
    gchar *text = read_from_buffer(buffer_g);
    GtkTextBuffer *buffer_mg=GTK_TEXT_BUFFER(
            gtk_builder_get_object(APPSTATE->builder,"buffer_grid_modify"));
    gtk_text_buffer_set_text(buffer_mg,text,-1);
    g_free(text);
    GtkTextBuffer *buffer_w = GTK_TEXT_BUFFER(
            gtk_builder_get_object(builder,"wordlist"));
    text = read_from_buffer(buffer_w);
    GtkTextBuffer *buffer_mw=GTK_TEXT_BUFFER(gtk_builder_get_object(
                APPSTATE->builder,"buffer_wordlist_modify"));
    gtk_text_buffer_set_text(buffer_mw,text,-1);
    g_free(text);
}


void _on_save_ocr(GtkWidget*, GtkStack *stack) {
    if (!APPSTATE->settings.unsaved_changes) {
        _on_cancel_ocr(NULL,stack);
        return;
    }
    gtk_stack_set_visible_child_name(stack, "OUTPUT");
    APPSTATE->settings.unsaved_changes = FALSE;
    GtkWidget *page = gtk_stack_get_child_by_name(GTK_STACK(DISPLAY),
            "MODIFY");
    //Update original image
    GtkTextBuffer *buffer_og=GTK_TEXT_BUFFER(gtk_builder_get_object(
                APPSTATE->builder,"buffer_grid_modify"));
    GtkTextBuffer *buffer_ow = GTK_TEXT_BUFFER(gtk_builder_get_object(
                APPSTATE->builder,"buffer_wordlist_modify"));
    gchar *text = read_from_buffer(buffer_og);
    GtkWidget *data = GETSTEPDATA(STEP_OCR);
    GtkBuilder *builder = GTK_BUILDER(g_object_get_data(G_OBJECT(data),
                "builder"));
    GtkTextBuffer *buffer_g = GTK_TEXT_BUFFER(
            gtk_builder_get_object(builder,"grid"));
    gtk_text_buffer_set_text(buffer_g,text,-1);
    //Rewriting grid file
    if (!g_file_set_contents("src/bin/grid",text,-1,NULL)) {
        g_log("GLib",G_LOG_LEVEL_ERROR,"Impossible to write to "
                "'src/bin/grid'.");
        return;
    }
    //Wordlist
    text = read_from_buffer(buffer_ow);
    GtkTextBuffer *buffer_w = GTK_TEXT_BUFFER(
            gtk_builder_get_object(builder,"wordlist"));
    gtk_text_buffer_set_text(buffer_w,text,-1);
    //Rewriting wordlist file
    if (!g_file_set_contents("src/bin/wordlist",text,-1,NULL)) {
        g_log("GLib",G_LOG_LEVEL_ERROR,"Impossible to write to "
                "'src/bin/wordlist'.");
        return;
    }
    //Render results
    ShowPage(STEPtoSTR(APPSTATE->step-1));
    gtk_widget_destroy(page);
    gtk_stack_set_transition_type(GTK_STACK(DISPLAY),
            GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    free_all_steps(APPSTATE->step,STEP_RECONSTRUCT);
    g_free(text);
}


void _on_cancel_ocr(GtkWidget*, GtkStack *stack) {
        ShowPage("STEP_OCR");
        GtkWidget *page = gtk_stack_get_child_by_name(GTK_STACK(DISPLAY),
                "MODIFY");
        if (page) {
            gtk_container_remove(GTK_CONTAINER(DISPLAY),page);
            gtk_widget_destroy(page);
        }
        gtk_stack_set_visible_child_name(stack, "OUTPUT");
        APPSTATE->settings.unsaved_changes = FALSE;
        gtk_stack_set_transition_type(GTK_STACK(DISPLAY),
                GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
}


void _on_ocr_value(GtkWidget*, gpointer) {
    APPSTATE->settings.unsaved_changes = TRUE;
}
