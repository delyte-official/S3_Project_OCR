#ifndef CORE_MANAGER_H
#define CORE_MANAGER_H

void Filter_Params(char** all_params, size_t len, char* *init_params,
        size_t *init_size, char* *gtk_params, size_t *gtk_size);
void StartUp(char** gtk_params, size_t gtk_len,
        char** init_params, size_t init_len);

#endif
