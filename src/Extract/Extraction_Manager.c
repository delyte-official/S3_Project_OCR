#include <gtk/gtk.h>
#include <stdio.h>
#include "Extract.h"


/* view_grid():
    Creates an image to visualize the grid.
*/
GdkPixbuf *view_grid(Cluster ***matrix, Size size, GdkPixbuf *pixbuf,
        char* filename, Position topL, Position botR) {
    GdkPixbuf *res = gdk_pixbuf_copy(pixbuf);
    guchar* pixels = gdk_pixbuf_get_pixels(res);
    int N = gdk_pixbuf_get_n_channels(res);
    int rowstride = gdk_pixbuf_get_rowstride(res);
    guchar *p;
    //Iterating over the clusters
    guchar r = 0, g = 255, b = 0;
    for (int xs = 0; xs < size.rows; xs++) {
        for (int ys = 0; ys < size.cols; ys++) {
            Cluster *first = matrix[xs][ys];
            //Coloring into red the whole cluster
            for (int x = first->minX; x <= first->maxX; x++) {
                p = pixels + first->minY*rowstride + x*N;
                p[0] = g;p[1]=r;p[2]=b;
                p = pixels + first->maxY*rowstride +x*N;
                p[0] = g;p[1]=r;p[2]=b;
            }
            for (int y = first->minY; y <= first->maxY; y++) {
                p = pixels + y*rowstride +first->minX*N;
                p[0] = g;p[1]=r;p[2]=b;
                p = pixels + y*rowstride +first->maxX*N;
                p[0] = g;p[1]=r;p[2]=b;
            }
        }
    }
    r = 0, g = 150, b = 0;
    for (int x = topL.x; x <= botR.x; x++) {
        p = pixels + topL.y*rowstride+x*N;
        p[0] = g; p[1] = r; p[2] = b;
        p = pixels + botR.y*rowstride+x*N;
        p[0] = g; p[1] = r; p[2] = b;
    }

    return res;
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



/* Extract_Data():
    Retrieves the grid and wordlist.
*/
int Extract_Data(GdkPixbuf *input, char* bin_filename) {
    char *grid_output;
    char *wordlist_output;
    if (asprintf(&grid_output,"%sgrid/",bin_filename) ==-1)
        return 0;
    if (asprintf(&wordlist_output,"%swordlist/",bin_filename) == -1)
        return 0;
    //Linked list of clusters
    Cluster *start = NULL;
    //STEP 1: retrieving EVERY clusters
    int median_size = 0;
    int count = retrieve_clusters(input, &start, &median_size);
    //STEP 2: Filter clusters
    count = threshold_filter(&start, count, median_size);
    //STEP 3: Align clusters
    Cluster ***matrixH, ***matrixV;
    Line *rows, *cols;
    Size sizeH, sizeV;
    align_clusters(start, &matrixH, &matrixV, &rows, &cols,&sizeH,&sizeV);
    free(rows);
    free(cols);
    //STEP 4: Classify clusters
    classify_clusters(&matrixH,&matrixV,&sizeH,&sizeV);

    //STEP 5: Cut the image into sub-images
    cut_grid(matrixH,sizeH, grid_output);
    cut_wordlist(matrixV, sizeV, input, wordlist_output);

    //Freeing memory
    free_matrix(matrixH,sizeH.rows);
    free_matrix(matrixV, sizeV.rows);
    while (start!=NULL) {
        Cluster *next = start->next;
        free_cluster(start);
        start = next;
    }
    return 1;
}
