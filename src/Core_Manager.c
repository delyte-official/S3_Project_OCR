/*
      ##############################################################
      #                                                            #
      #                       Core_Manager.c                       #
      #                                                            #
      #         Handles, supervises and links all resources        #
      #      between every systems of the Project Application.     #
      #                                                            #
      ##############################################################
*/


////HEADERS
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <gtk/gtk.h>
//Project Headers
#include "Core_Manager.h"
#include "Interface/GTK_Window.h"
#include "Interface/Events.h"
#include "Interface/Interface.h"
#include "Filter/Filter.h"
////DEFINING
#define ID_INIT_SIZE 1
static const char* ID_INIT_PARAMS[ID_INIT_SIZE] =  {"--force"};


AppState *get_appState() {
    static AppState state = (AppState) {0};
    return &state;
}


int check_param(const char** PARAMS, int len, char* to_check) {
    for (int i = 0; i < len; i++) {
        if (to_check == PARAMS[i])
            return 1; //Found
    }
    return 0; //Not found
}


void Filter_Params(char** all_params, int len, char* *init_params,
        int *init_size, char* *gtk_params, int *gtk_size) {
    for (char** curr = all_params; curr < all_params + len; curr++) {
        size_t length = strlen(*curr);
        if (length < 2) //Wrong format
            continue;
        if (check_param(ID_INIT_PARAMS, ID_INIT_SIZE, *curr)) { //Init param
            *init_params = *curr;
            init_params++;
            (*init_size)++;
        } else { //Sent to GTK
            *gtk_params = *curr;
            gtk_params++;
            (*gtk_size)++;
        }
    }
}


void StartUp(char** gtk_params, int gtk_len,
        char** init_params, int init_len) {
    (void) init_params; //No possible init parameters.
    (void) init_len;

    gtk_init(&gtk_len, &gtk_params);
    AppState *state = APPSTATE;

    //Creation of the window
    char* title = "#Free Galane - Word Search Solver";
    GdkRectangle geometry;
    get_screen_size(&geometry);
    state->width = geometry.width;
    state->height = geometry.height;
    int type = GTK_WINDOW_TOPLEVEL;
    WINDOW = create_window(type, title, state->width, state->height);

    //Run the application
    Standard_Signals();
    gtk_widget_show_all(WINDOW);
    gtk_main();
}


int NextStep(GtkWidget*, gpointer) {
    AppState *state = APPSTATE;
    if (state->steps_tracker[state->step]!=NULL) {
        ShowNext();
        state->step++;
        return 1;
    }
    switch (state->step) {
        case STEP_LOAD:
            if (!Load_Image())
                return 0;
            break;
        case STEP_FILTER:
            GtkWidget *data = GETSTEPDATA(STEP_LOAD);
            if (!Filter_Image(g_object_get_data(G_OBJECT(data),"pixbuf")))
                return 0;
            break;
        case STEP_EXTRACT:
            break;
        case STEP_OCR:
            break;
        case STEP_SOLVE:
            break;
        case STEP_RECONSTRUCT:
            break;
        default:
            errx(EXIT_FAILURE, "Step format error.");
    }
    print("Step: %d\n",(int)state->step);
    ShowNext();
    state->step++;
    return 1;
}


void ShowNext() {
    AppState *state=APPSTATE;
    switch (state->step) {
        case STEP_LOAD:
            gtk_widget_set_sensitive(GETWIDGET("previous_btn"), TRUE);
            ShowWidget(GETSTEPDATA(STEP_LOAD));
            break;
        case STEP_FILTER:
            ShowWidget(GETSTEPDATA(STEP_FILTER));
            break;
        case STEP_EXTRACT:
            break;
        case STEP_OCR:
            break;
        case STEP_SOLVE:
            break;
        case STEP_RECONSTRUCT:
            gtk_widget_set_sensitive(GETWIDGET("next_btn"), FALSE);
            gtk_widget_set_sensitive(GETWIDGET("auto_complete"), FALSE);
            break;
        default:
            errx(EXIT_FAILURE, "STEP is in incorrect format.");
    }
}


void ShowPrevious(GtkWidget*, gpointer) {
    AppState *state = APPSTATE;
    state->step--;
    switch (state->step) {
        case STEP_RECONSTRUCT:
            gtk_widget_set_sensitive(GETWIDGET("next_btn"), TRUE);
            gtk_widget_set_sensitive(GETWIDGET("auto_complete"), TRUE);
            break;
        case STEP_SOLVE:
            break;
        case STEP_OCR:
            break;
        case STEP_EXTRACT:
            break;
        case STEP_FILTER:
            ShowWidget(GETSTEPDATA(STEP_LOAD));
            break;
        case STEP_LOAD:
            gtk_widget_set_sensitive(GETWIDGET("previous_btn"), FALSE);
            ShowWidget(GETWIDGET("import_btn"));
            break;
        default:
            errx(EXIT_FAILURE, "STEP is in incorrect form.");
    }
}


/* print():
    Custom print function compatible with the application.
*/
void print(const char *format, ...) {
    va_list args;
    va_start(args,format);
    char *result;
    if (vasprintf(&result,format,args)==-1)
        return;
    GtkTextIter end;
    GtkTextView *textview = GTK_TEXT_VIEW(GETWIDGET("logs"));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(textview);
    gtk_text_buffer_get_end_iter(buffer,&end);
    gtk_text_buffer_insert(buffer,&end,result,-1);
    va_end(args);
}
