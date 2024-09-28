#ifndef CORE_MANAGER_H
#define CORE_MANAGER_H

void Filter_Params(char** all_params, int len, char* *init_params,
        int *init_size, char* *gtk_params, int *gtk_size);
void StartUp(char** gtk_params, int gtk_len,
        char** init_params, int init_len);

#endif
