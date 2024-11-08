/*
      ##############################################################
      #                                                            #
      #                     Solver_Manager.c                       #
      #                                                            #
      #     Manages the "Solving" branch: send requests to the     #
      #  solver, read its output and convert it into usable data.  #
      #                                                            #
      ##############################################################

List of all functions written in this file (and their type):
[See more description on their purpose and parameters down below]

int Solver_Run(char*,char*);
*/


////HEADERS Files
//Integrated C Libraries
#include <err.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
//Tools
#include <gtk/gtk.h>
//Project Headers
#include "../Core_Manager.h"
#include "Solver_Manager.h"
////END HEADERS


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

/* stoi():
    Takes a substring of a str and convert it into an int
*/
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


/* read_output():
    Read a string and creates the associated SOLUTION struct
    Output given is assumed to be in correct format.
*/
int read_output(solution **store, char* output) {
    if (strcmp("Not found", output) == 0)
        *store = NULL;
    else {
        *store = malloc(sizeof(solution));
        if (*store == NULL)
            return EXIT_FAILURE;
        **store = (solution) {
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


/* check_error():
    Check if any children has returned an error.
*/
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


/* readall():
    Reads every pipes and computes their output, then stores it.
*/
int readall(
        solution** store,
        int* fds,
        size_t len) {
    for (size_t i = 0; i < len; i++) {
        char buffer[16];
        char* output = malloc(sizeof(char));
        output[0] = '\0';
        size_t out_len = 0;
        ssize_t r_bytes;

        //Read while theres smth to read
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


/* set_result():
    Finalize the result of the Solving branch by creating the widget and
    linking it to the application, with its associated data.
*/
void set_result(solution** p, int count) {
    GtkWidget *widget = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *text_view = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(widget), text_view);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    
    //Setting the text
    for (int i = 0; i < count; i++) {
        GtkTextIter end;
        gtk_text_buffer_get_end_iter(buffer, &end);
        //Creating the string
        char buffering[100];
        if (p[i] != NULL)
            snprintf(buffering, sizeof(buffering),
                    "(%d,%d) to (%d,%d)\n",
                    p[i]->ini_col,p[i]->ini_row,p[i]->end_col,p[i]->end_row);
        else
            snprintf(buffering, sizeof(buffering), "NULL\n");

        gtk_text_buffer_insert(buffer, &end, buffering, -1);
    }

    //Store result
    g_object_set_data(G_OBJECT(widget), "data", p);
    g_object_set_data(G_OBJECT(widget), "buffer", buffer);
    step_widget(STEP_SOLVE+1, widget);
}


/* Solver_Run:
    Call the solver on every word of the words_file and store their result.
*/
int Solver_Run(
        char* grid_file,
        char* words_file,
        int words_count) {
    //Allocate enough memory space to store the solution
    solution** p = malloc(words_count * sizeof(solution*));
    if (p == NULL) //Error malloc
        return EXIT_FAILURE;

    //Allocate enough meomery for the file descriptors
    int* fds = malloc(words_count * sizeof(int));
    if (fds == NULL) {
        free(p);
        return EXIT_FAILURE;
    } //Allocate enough memory to store child IDs
    int* pids = malloc(words_count * sizeof(int));
    if (pids == NULL) {
        free(p);
        free(fds);
        return EXIT_FAILURE;
    }

    //Start reading the file line by line to get each word lonely
    FILE *file = fopen(words_file, "r");
    if (file == NULL) {
        free(p);
        free(fds);
        free(pids);
        return EXIT_FAILURE;
    }

    char* line = NULL; //Buffer to read a line, managed by getline()
    size_t len = 0; //Buffer size
    ssize_t r_bytes;
    int w_index = 0; //Word count
    while ((r_bytes = getline(&line, &len, file)) != -1) {
        line[r_bytes-1] = 0; //Removing the \n

        //Forking --> Child process replaced by solver
        //Cant fork manually with gtk => use g_spawn()
        char* argv[] = {"./src/Solving/solver", grid_file, line, NULL};
        if (!g_spawn_async_with_pipes(NULL, argv, //Arguments
                    NULL, G_SPAWN_DO_NOT_REAP_CHILD, //Properties
                    NULL, NULL, &pids[w_index], //Save pids ID for wait()
                    NULL, &fds[w_index], //Redirect output of children
                    NULL, NULL)) {
            free(p);
            free(fds);
            free(pids);
            return EXIT_FAILURE;
        }

        ////Parent Process
        w_index++;
    }
    fclose(file);

    //Wait for every children to finish computer and check for errors
    if (check_error(w_index, pids)) {
        free(p);
        free(fds);
        free(pids);
        return EXIT_FAILURE;
    }
    free(pids);
    
    //Reading and computing output of every pipes
    if (readall(p, fds, words_count)) {
        free(p);
        free(fds);
        return EXIT_FAILURE;
    }

    //Store the result for the application to use
    set_result(p, w_index);

    free(fds);
    return EXIT_SUCCESS;
}
