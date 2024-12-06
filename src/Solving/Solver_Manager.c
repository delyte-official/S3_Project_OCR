#include <err.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <gtk/gtk.h>
#include "../Core_Manager.h"
#include "Solver_Manager.h"


/* is_pint():
    Returns TRUE if the string represents a positive integer.
*/
int is_pint(char* str) {
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        if (str[i] < '0' || str[i] > '9')
            return 0;
    }
    return 1;
}

int stoi(char* str, int size, int *output) {
    char check[size+1];
    memcpy(check, str, size);
    check[size] = '\0';
    if (!is_pint(check)) {
        return EXIT_FAILURE; }
    long res = strtol(check, NULL, 10);
    if (res > INT_MAX)
        return EXIT_FAILURE;
    *output = (int)res;
    return EXIT_SUCCESS;
}


int read_output(Solution **store, char* output) {
    if (strcmp("Not found", output) == 0)
        *store = NULL;
    else {
        *store = malloc(sizeof(Solution));
        if (*store == NULL)
            return EXIT_FAILURE;
        **store = (Solution) {
            .ini_row = 0,
            .ini_col = 0,
            .end_row = 0,
            .end_col = 0
        };
        char *separ1 = strchr(output, ',');
        char *separ2 = strchr(separ1, '(');
        char *separ3 = strchr(separ2, ',');
        if (stoi(output+1, separ1-output - 1, &((*store)->ini_col))
            || stoi(separ1+1, separ2-separ1 - 2, &((*store)->ini_row))
            || stoi(separ2+1, separ3 - separ2 - 1, &((*store)->end_col))
            || stoi(separ3+1, output + strlen(output) -separ3 - 2,
                    &((*store)->end_row)))
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


int check_error(int count, int* pids) {
    for (int i = 0; i < count; i++) {
        int status;
        if (g_spawn_check_wait_status(waitpid(pids[i], &status, 0), NULL)) {
            if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE)
                return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}


int readall(Solution** store, int* fds, size_t len) {
    for (size_t i = 0; i < len; i++) {
        char buffer[16];
        char* output = malloc(sizeof(char));
        output[0] = '\0';
        size_t out_len = 0;
        ssize_t r_bytes;
        while ((r_bytes = read(fds[i], buffer, sizeof(buffer))) > 0) {
            buffer[r_bytes-1] = '\0'; //-1 to remoe \n
            size_t new_l = out_len + r_bytes;
            char* new_out = realloc(output, (new_l + 1) * sizeof(char));
            if (new_out == NULL) {
                if (output != NULL)
                    free(output);
                return EXIT_FAILURE;
            }
            output = new_out;
            //Append to output
            strncat(output, buffer, r_bytes);
            out_len = new_l;
        }
        if (r_bytes == -1)
            return EXIT_FAILURE;
        if (read_output(store+i, output)) {
            free(output);
            return EXIT_FAILURE;
        }
        free(output);
    }
    return EXIT_SUCCESS;
}


void set_result(Solution** p, int count) {
    GtkBuilder *builder = gtk_builder_new_from_file(
            "src/assets/solver_tile.glade");
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder,
                "solutions"));
    for (int i = 0; i < count; i++) {
        GtkTextIter end;
        gtk_text_buffer_get_end_iter(buffer, &end);
        char buffering[100];
        if (p[i] != NULL)
            snprintf(buffering, sizeof(buffering),
                    "(%d,%d) to (%d,%d)\n",
                    p[i]->ini_col,p[i]->ini_row,p[i]->end_col,p[i]->end_row);
        else
            snprintf(buffering, sizeof(buffering), "NULL\n");
        gtk_text_buffer_insert(buffer, &end, buffering, -1);
    }
    GtkWidget *data = GTK_WIDGET(gtk_builder_get_object(builder,"toplvl"));
    g_object_set_data(G_OBJECT(data), "solutions", p);
    g_object_set_data(G_OBJECT(data), "buffer",buffer);
    FREESTEPDATA(STEP_SOLVE);
    SETSTEPDATA(STEP_SOLVE,data);
}


int Solve(char* grid_file, char* words_file, int words_count) {
    //Allocate enough memory space to store the solution
    Solution** p = malloc(words_count * sizeof(Solution*));
    if (p == NULL) //Error malloc
        return 0;
    //Allocate enough meomery for the file descriptors
    int* fds = malloc(words_count * sizeof(int));
    if (fds == NULL) {
        free(p);
        return 0;
    } //Allocate enough memory to store child IDs
    int* pids = malloc(words_count * sizeof(int));
    if (pids == NULL) {
        free(p);
        free(fds);
        return 0;
    }
    //Start reading the file line by line to get each word lonely
    FILE *file = fopen(words_file, "r");
    if (file == NULL) {
        free(p);
        free(fds);
        free(pids);
        return 0;
    }
    char* line = NULL; 
    size_t len = 0;
    ssize_t r_bytes;
    int w_index = 0;
    while ((r_bytes = getline(&line, &len, file)) != -1) {
        line[r_bytes-1] = 0;
        //Forking --> Child process replaced by solver
        //Cant fork manually with gtk => use g_spawn()
        char* argv[] = {"./src/Solving/solver", grid_file, line, NULL};
        if (!g_spawn_async_with_pipes(NULL, argv, NULL,
                    G_SPAWN_DO_NOT_REAP_CHILD, NULL, NULL, &pids[w_index],
                    NULL, &fds[w_index], //Redirect output of children
                    NULL, NULL)) {
            free(p);
            free(fds);
            free(pids);
            return 0;
        }
        //Parent Process
        w_index++;
    }
    fclose(file);
    //Wait for every children to finish computer and check for errors
    if (check_error(w_index, pids)) {
        free(p);
        free(fds);
        free(pids);
        return 0;
    }
    free(pids);
    //Reading and computing output of every pipes
    if (readall(p, fds, words_count)) {
        free(p);
        free(fds);
        return 0;
    }
    //Store the result for the application to use
    set_result(p, w_index);
    free(fds);
    return 1;
}
