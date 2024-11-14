#ifndef CORE_MANAGER_H
#define CORE_MANAGER_H

#include <gtk/gtk.h>

#define GETWIDGET(id) GTK_WIDGET(gtk_builder_get_object(\
            get_app_state()->builder, id))
#define DISPLAY GETWIDGET("display_section_id")

typedef enum {
    STEP_LOAD = 0,
    STEP_FILTER = 1,
    STEP_EXTRACT = 2,
    STEP_SOLVE = 3,
    STEP_RECONSTRUCT = 4,
    STEP_END = 5
} STEP;

typedef struct {
    GtkBuilder* builder; //App builder
    STEP step; //Current step
    int width, height; //Window SIZE
    int alloc_width, alloc_height; //Content area SIZE
    GtkWidget* steps_tracker[6]; //Steps Storage
} AppState;

AppState *get_app_state();
void set_display(GtkWidget* widget);
GtkWidget* step_widget(int step, GtkWidget* set);
void Filter_Params(char** all_params, int len, char* *init_params,
        int *init_size, char* *gtk_params, int *gtk_size);
void StartUp(char** gtk_params, int gtk_len,
        char** init_params, int init_len);
void ShowNext();
int NextStep(GtkWidget*, gpointer);
void ShowPrevious(GtkWidget*, gpointer);

#endif
