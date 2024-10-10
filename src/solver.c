/*
      ##############################################################
      #                                                            #
      #                           solver.c                         #
      #                                                            #
      #       This program is independant from the Main Canal.     #
      #     It executes from command line and returns in stdout.   #
      #                                                            #
      ##############################################################

This program solves a word search given a word as a string
and the file containing the grid.
The given file and word is assumed to be in the same format
as seen in the Moodle Documentations.
*/


////HEADERS Files
#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
////END HEADERS


////DEFINING
typedef struct {
    int row;
    int col;
} coords;

void toupper_str(char* str);
char** read_file(char* filename, int* rows, int* cols);
int search_word(char** grid, int rows, int cols, const char* word,
        coords* start, coords* end);
//END DEFINING


/* main():
    Handles all the logic and errors.
*/
int main(int argc, char* argv[]) {
    //Verify that we have the correct number of arguments
    if (argc != 3) {
        errx(EXIT_FAILURE, "Incorrect number of arguments.");
    }


    //Get the dynamic memory array representing the grid:
    int rows, cols;
    char** grid = read_file(argv[1], &rows, &cols);

    char* word = argv[2];
    toupper_str(word);
    coords start, end;
    if (search_word(grid, rows, cols, word, &start, &end))
        printf("(%d,%d)(%d,%d)\n",start.col,start.row,end.col,end.row);
    else
        printf("Not found\n");
    return EXIT_SUCCESS;
}


/* toupper_str():
    Transform every letter of a string into an uppercase letter.
*/
void toupper_str(char* str) {
    for (size_t i = 0; i < strlen(str); i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}


/* get_size():
    Returns through the pointers the size of the given grid.
*/
void get_size(FILE* file, int* rows, int* cols) {
    char* line = NULL;
    ssize_t read;
    size_t len = 0;
    *rows = 0;
    *cols = 0;

    while ((read = getline(&line, &len, file)) != -1) {
        if (*rows == 0) {
            *cols = strlen(line) - 1;
        }
        
        (*rows)++;
    }
    free(line);
}


/* read_file():
    Reads and writes the entire content of the file into a dynamic array.
*/
char** read_file(char* filename, int* rows, int* cols) {
    //Get size of grid
    FILE* file = fopen(filename, "r");
    if (file == NULL)
        errx(EXIT_FAILURE,"fopen()");
    get_size(file, rows, cols);

    //Return to start of file
    fseek(file, 0, SEEK_SET);
    //If size is not minimal
    if (*rows < 5 || *cols < 6)
        errx(EXIT_FAILURE, "Grid size too small.");
    
    //Allocating memory for the grid
    char** grid = (char**)malloc(*rows  * sizeof(char*));
    if (grid == NULL)
        errx(EXIT_FAILURE, "malloc()");
    for (size_t i = 0; i < *rows; i++) {
        grid[i] = (char*)malloc(*cols * sizeof(char));
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

        //To uppercase to avoid confusion or whatever
        toupper_str(line);
        strcpy(grid[index], line);
        
        index++;
    }

    free(line);
    fclose(file);

    return grid;
}


/* inbounds():
    Checks if a position is in bounds.
*/
int inbounds(int y, int x, int rows, int cols) {
    return x >= 0 && x < cols && y >= 0 && y < rows;
}


/* search_direction():
    Search for a match of a word in a specified direction.
*/
int search_direction(char** grid, int rows, int cols, const char* word,
        coords *startSearch, coords *dir, coords *start, coords *end) {
    int word_len = strlen(word);
    int row = (*startSearch).row + (*dir).row;
    int col = (*startSearch).col + (*dir).col;

    for (int i = 0; i < word_len - 1; i++) {
        if (inbounds(row, col, rows, cols)) {
            if (grid[row][col] != word[i+1])
                return 0;
        } else
            return 0;
        row+=(*dir).row;
        col+=(*dir).col;
    }

    //Word found
    start->row = startSearch->row;
    start->col = startSearch->col;
    end->row = row - dir->row;
    end->col = col - dir->col;

    return 1;
}


/* search_word():
    Search the given word in the grid in all 8 2-Dimensional directions.
*/
int search_word(char** grid, int rows, int cols, const char* word,
        coords* start, coords* end) {
    int dirs[8][2] = {{0,1},{0,-1},{1,0},{-1,0},{1,1},{1,-1},{-1,1},{-1,-1}};

    //Iterate through the grid
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            //If letter, try searching in all direction
            if (grid[row][col] == word[0]) {
                coords startSearch = {
                    .row = row,
                    .col = col
                };
                for (int d = 0; d < 8; d++) {
                    coords dir = {
                        .row = dirs[d][0],
                        .col = dirs[d][1]
                    };
                    if (search_direction(grid, rows, cols, word,
                                &startSearch, &dir, start, end))
                        return 1;
                }
            }
        }
    }

    //Not found
    return 0;
}
