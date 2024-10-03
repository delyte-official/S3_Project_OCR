/*

      ##############################################################
      #                                                            #
      #                           solver.c                         #
      #                                                            #
      #         This program is independant from the main.c.       #
      #     It executes from command line and returns in stdout.   #
      #                                                            #
      ##############################################################


This program solves a word search given a word
and the file containing a grid.
The given file and word is assumed to be in a correct format.
*/



////HEADERS Files
#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>

////END HEADERS


//Prototypes
char** read_file(char* filename, size_t* width, size_t* height);


int main(int argc, char* argv[]) {
    //Verify that we have the correct number of arguments
    if (argc != 3) {
        errx(EXIT_FAILURE, "Incorrect number of arguments.");
    }


    //Get the dynamic memory array representing the grid:
    size_t width, height;
    char** grid = read_file(argv[1], &width, &height);

    //Printing the grid
    for (size_t i = 0; i < height; i++) {
        printf("%s\n", grid[i]);
    }
}

void get_size(char* filename, size_t* width, size_t* height) {
    //Open file to read
    FILE* file = fopen(filename, "r");
    if (file == NULL) //Error in opening the file
        errx(EXIT_FAILURE, "fopen()");

    char* line = NULL;
    ssize_t read;
    size_t len = 0;
    *width = 0;
    *height = 0;

    while ((read = getline(&line, &len, file)) != -1) {
        line[strcspn(line, "\n")] = '\0';

        if (*height == 0)
            *width = strlen(line);

        (*height)++;
    }
    free(line);
    fclose(file);
}

char** read_file(char* filename, size_t* width, size_t* height) {
    //Get size of grid
    get_size(filename, width, height);

    //Open the file in reading mode
    FILE* file = fopen(filename, "r");
    if (file == NULL) //Error in opening the file
        errx(EXIT_FAILURE, "fopen()");

    //Allocating memory for the grid
    char** grid = (char**)malloc(*height  * sizeof(char*));
    if (grid == NULL)
        errx(EXIT_FAILURE, "malloc()");
    for (size_t i = 0; i < *height; i++) {
        grid[i] = (char*)malloc(*width * sizeof(char));
        if (grid[i] == NULL)
            errx(EXIT_FAILURE, "malloc()");
    }
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    size_t index = 0;

    //Reading the file
    while ((read = getline(&line, &len, file)) != -1) {
        //To remove the new line character, strcspn just find the index
        line[strcspn(line, "\n")] = '\0';

        strcpy(grid[index], line);
        
        index++;
    }

    free(line);
    fclose(file);

    return grid;
}
