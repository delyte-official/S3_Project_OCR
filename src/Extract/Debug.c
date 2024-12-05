#include <gtk/gtk.h>
#include <stdio.h>
#include "Extraction.h"

//JUST TO TEST - TO REMOVE
void testing(Cluster *first, GdkPixbuf *pixbuf, char* filename) {
    GdkPixbuf *res = gdk_pixbuf_copy(pixbuf);
    
    guchar* pixels = gdk_pixbuf_get_pixels(res);
    int N = gdk_pixbuf_get_n_channels(res);
    int rowstride = gdk_pixbuf_get_rowstride(res);

    guchar *p;
    //Iterating over the clusters
    while (first!=NULL) {
        //Coloring into red the whole cluster
        for (int x = first->minX; x <= first->maxX; x++) {
            p = pixels + first->minY*rowstride + x*N;
            p[0] = 255;p[1]=0;p[2]=0;
            p = pixels + first->maxY*rowstride +x*N;
            p[0] = 255;p[1]=0;p[2]=0;
        }
        for (int y = first->minY; y <= first->maxY; y++) {
            p = pixels + y*rowstride +first->minX*N;
            p[0] = 255;p[1]=0;p[2]=0;
            p = pixels + y*rowstride +first->maxX*N;
            p[0] = 255;p[1]=0;p[2]=0;
        }
        first=first->next;
    }
    gdk_pixbuf_save(res, filename, "png", NULL, NULL);
    g_object_unref(res);
}



//JUST DO TEST - //TODO: to REMOVE
void print_testing(Cluster ***rows, Cluster ***cols, int rows_x, int rows_y,
        int cols_x, int cols_y, Line* rows_inf, Line* cols_inf) {
    printf("\nPRINT HORIZONTAL ALIGNMENT:\n");
    for (int x = 0; x < rows_x; x++) {
        printf("%3d %3d ",rows_inf[x].pos,rows_inf[x].size);
        for (int y = 0; y < rows_y; y++)
            printf("%2c ",rows[x][y] != NULL ? 'X' : ' ');
        printf("\n");
    }
    printf("\nPRINT VERTICAL ALIGNMENT:\n");
    for (int x = 0; x < cols_x; x++)
        printf("%3d ",cols_inf[x].pos);
    printf("\n");
    for (int x = 0; x < cols_x; x++)
        printf("%3d ",cols_inf[x].size);
    printf("\n");
    for (int y = 0; y < cols_y; y++) {
        for (int x = 0; x < cols_x; x++) {
            printf("%3c ",cols[x][y] != NULL ? 'X' : ' ');
        }
        printf("\n");
    }
}


//TEST FUNCTION
void print_matrix(Cluster ***matrix, Size size) {
    printf("\nPRINT MATRIX(%d;%d):\n",size.rows,size.cols);
    for (int y = 0; y < size.cols; y++) {
        for (int x = 0; x < size.rows; x++)
            printf("%c ", matrix[x][y]!=NULL ? 'X' : '\\');
        printf("\n");
    }
    printf("END\n\n");
}


void grid_testing(Cluster ***matrix, Size size, GdkPixbuf *pixbuf, char* filename) {
    GdkPixbuf *res = gdk_pixbuf_copy(pixbuf);

    guchar* pixels = gdk_pixbuf_get_pixels(res);
    int N = gdk_pixbuf_get_n_channels(res);
    int rowstride = gdk_pixbuf_get_rowstride(res);

    guchar *p;
    //Iterating over the clusters
    for (int x = 0; x < size.rows; x++) {
        for (int y = 0; y < size.cols; y++) {
            Cluster *first = matrix[x][y];
            //Coloring into red the whole cluster
            for (int x = first->minX; x <= first->maxX; x++) {
                p = pixels + first->minY*rowstride + x*N;
                p[0] = 0;p[1]=255;p[2]=0;
                p = pixels + first->maxY*rowstride +x*N;
                p[0] = 0;p[1]=255;p[2]=0;
            }
            for (int y = first->minY; y <= first->maxY; y++) {
                p = pixels + y*rowstride +first->minX*N;
                p[0] = 0;p[1]=255;p[2]=0;
                p = pixels + y*rowstride +first->maxX*N;
                p[0] = 0;p[1]=255;p[2]=0;
            }
        }
    }
    gdk_pixbuf_save(res, filename, "png", NULL, NULL);
    g_object_unref(res);
}


void wordList_testing(Cluster ***matrix, Size size, GdkPixbuf *pixbuf,
        char* filename) {
    GdkPixbuf *res = gdk_pixbuf_copy(pixbuf);

    guchar* pixels = gdk_pixbuf_get_pixels(res);
    int N = gdk_pixbuf_get_n_channels(res);
    int rowstride = gdk_pixbuf_get_rowstride(res);

    guchar *p;
    //Iterating over the clusters
    for (int x = 0; x < size.rows; x++) {
        //New word
        int r = (x*40)%256, g = (255-x*40)%256, b = 255;
        for (int y = 0; y < size.cols; y++) {
            Cluster *first = matrix[x][y];
            if (first == NULL)
                break; //Skip row
            //Coloring into red the whole cluster
            for (int x = first->minX; x <= first->maxX; x++) {
                p = pixels + first->minY*rowstride + x*N;
                p[0] = r;p[1]=g;p[2]=b;
                p = pixels + first->maxY*rowstride +x*N;
                p[0] = r;p[1]=g;p[2]=b;
            }
            for (int y = first->minY; y <= first->maxY; y++) {
                p = pixels + y*rowstride +first->minX*N;
                p[0] = r;p[1]=g;p[2]=b;
                p = pixels + y*rowstride +first->maxX*N;
                p[0] = r;p[1]=g;p[2]=b;
            }
        }
    }
    gdk_pixbuf_save(res, filename, "png", NULL, NULL);
    g_object_unref(res);
}


int main(int argc, char* argv[]) {
    if (argc!=2)
        return 1;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(argv[1], NULL);
    extract_information(pixbuf, "grid/", "wordlist/");
    g_object_unref(pixbuf);
}
