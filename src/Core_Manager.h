#ifndef CORE_MANAGER_H
#define CORE_MANAGER_H
#define _GNU_SOURCE
#include <gtk/gtk.h>

#define APPSTATE get_appState()
#define GETWIDGET(id) GTK_WIDGET(gtk_builder_get_object(\
            APPSTATE->builder, id))
#define DISPLAY GETWIDGET("display_section")
#define WINDOW APPSTATE->window
#define GETSTEPDATA(step) APPSTATE->steps_tracker[step]
#define SETSTEPDATA(step,widget) set_step_data(step,widget)
#define FREESTEPDATA(step) free_step_data(step)


typedef enum {
    STEP_LOAD = 0,
    STEP_FILTER = 1,
    STEP_EXTRACT = 2,
    STEP_OCR = 3,
    STEP_SOLVE = 4,
    STEP_RECONSTRUCT = 5,
    STEP_END = 6
} STEP;

typedef struct {
    gboolean unsaved_changes;
} Settings;

typedef struct {
    GtkBuilder *builder; //App builder
    GtkWidget *window;
    GtkWidget *display;
    Settings settings;
    STEP step; //Current step
    int width, height; //Window SIZE
    int alloc_width, alloc_height; //Content area SIZE
    GtkWidget* steps_tracker[STEP_END]; //Steps Storage
} AppState;

AppState *get_appState();
void set_step_data(STEP step, GtkWidget *data);
void free_all_steps(STEP from, STEP to);
void free_step_data(STEP step);
void Filter_Params(char** all_params, int len, char* *init_params,
        int *init_size, char* *gtk_params, int *gtk_size);
void StartUp(char** gtk_params, int gtk_len,
        char** init_params, int init_len);
int NextStep(GtkWidget*, gpointer);
void ShowNext();
const char* STEPtoSTR(STEP step);
void print(const char *format, ...);

#endif
