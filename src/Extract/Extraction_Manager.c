#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <stdio.h>
#include "../Core_Manager.h"
#include "../Interface/Interface.h"
#include "Extract.h"


/* view_grid():
    Creates an image to visualize the grid.
*/
GdkPixbuf *view_grid(Cluster ***matrix, Size size, GdkPixbuf *pixbuf) {
    //Finding out grid bounds
    Position topL = (Position) {.x=matrix[0][0]->minX,.y=matrix[0][0]->minY};
    Position botR = (Position) {.x=matrix[0][0]->maxX,.y=matrix[0][0]->maxY};
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
            if (first->minX < topL.x)
                topL.x=first->minX;
            else if (first->maxX > botR.x)
                botR.x=first->maxX;
            if (first->minY < topL.y)
                topL.y=first->minY;
            else if (first->maxY > botR.y)
                botR.y=first->maxY;
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
    r = 255, g = 165, b = 0;
    for (int x = topL.x; x <= botR.x; x++) {
        p = pixels + topL.y*rowstride+x*N;
        p[0] = r; p[1] = g; p[2] = b;
        p = pixels + botR.y*rowstride+x*N;
        p[0] = r; p[1] = g; p[2] = b;
    }
    for (int y = topL.y; y <= botR.y; y++) {
        p = pixels + y*rowstride+topL.x*N;
        p[0] = r; p[1] = g; p[2] = b;
        p = pixels + y*rowstride+botR.x*N;
        p[0] = r; p[1] = g; p[2] = b;
    }

    return res;
}


/* view_wordlist():
    Visualize the word list.
*/
GdkPixbuf *view_wordlist(Cluster ***matrix, Size size, GdkPixbuf *pixbuf) {
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
    return res;
}



/* Extract_Data():
    Retrieves the grid and wordlist.
*/
int Extract_Data(GdkPixbuf *input, char* bin_filename) {
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
    cut_grid(matrixH,sizeH, bin_filename);
    cut_wordlist(matrixV, sizeV, input, bin_filename);
    //STEP 6: Saving result
    GdkPixbuf *grid_img = view_grid(matrixH,sizeH,input);
    GdkPixbuf *final_img = view_wordlist(matrixV,sizeV,grid_img);
    GdkPixbuf *resized = resize_from_container(final_img, DISPLAY);
    GtkWidget *image = gtk_image_new_from_pixbuf(resized);
    g_object_set_data(G_OBJECT(image), "pixbuf", final_img);
    g_object_ref(final_img);
    FREESTEPDATA(STEP_EXTRACT);
    SETSTEPDATA(STEP_EXTRACT,image);
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
