#include <gtk/gtk.h>

void testing(Cluster *first, GdkPixbuf *pixbuf, char* filename);
void print_testing(Cluster ***rows, Cluster ***cols, int rows_x, int rows_y,
        int cols_x, int cols_y, Line* rows_inf, Line* cols_inf);

void print_matrix(Cluster ***matrix, Size size);

void grid_testing(Cluster ***matrix, Size size, GdkPixbuf *pixbuf, char* filename);

void wordList_testing(Cluster ***matrix, Size size, GdkPixbuf *pixbuf,
        char* filename);
