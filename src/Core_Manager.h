#ifndef CORE_MANAGER_H
#define CORE_MANAGER_H

typedef enum {
    STEP_START = 0,
    STEP_LOAD = 1,
    STEP_FILTER = 2,
    STEP_EXTRACT = 3,
    STEP_SOLVE = 4,
    STEP_RECONSTRUCT = 5,
    STEP_END = 6
} STEP;

STEP* get_step();
void Filter_Params(char** all_params, int len, char* *init_params,
        int *init_size, char* *gtk_params, int *gtk_size);
void StartUp(char** gtk_params, int gtk_len,
        char** init_params, int init_len);
void NextStep();

#endif
