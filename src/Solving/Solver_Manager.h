#ifndef SOLVER_MANAGER_H
#define SOLVER_MANAGER_H
typedef struct {
    int ini_row;
    int ini_col;
    int end_row;
    int end_col;
} solution;

int check_error();
int Solver_Run(char* grid_file, char* words_file, int words_count);

#endif
