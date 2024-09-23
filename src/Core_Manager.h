#ifndef CORE_MANAGER_H
#define CORE_MANAGER_H

void Filter_Params(char** all_params, size_t len, char* *init_params,
      size_t *init_size, char* *run_params, size_t *run_size);
void Initialize(char** params, size_t len);
void Run_Application(char** params, size_t len);

#endif
