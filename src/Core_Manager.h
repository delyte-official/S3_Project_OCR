#ifndef CORE_MANAGER_H
#define CORE_MANAGER_H

#include <gtk/gtk.h>

typedef enum {
    STEP_LOAD = 0,
    STEP_FILTER = 1,
    STEP_EXTRACT = 2,
    STEP_SOLVE = 3,
    STEP_RECONSTRUCT = 4,
    STEP_END = 5
} STEP;

STEP* get_step();
GtkWidget* get_display(GtkWidget** widget);
void set_display(GtkWidget* widget);
GtkWidget* step_widget(int step, GtkWidget* set);
void Filter_Params(char** all_params, int len, char* *init_params,
        int *init_size, char* *gtk_params, int *gtk_size);
void StartUp(char** gtk_params, int gtk_len,
        char** init_params, int init_len);
void ShowNext();
void NextStep(GtkWidget* next_btn, int* show);
void ShowPrevious(GtkWidget* prev_btn, int* show);

#endif
