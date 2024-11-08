#include <err.h>
#include <stdio.h>

//Cluster struct
typedef struct {
    int nb_pixels;

    int maxX, maxY, minX, minY;
} Cluster


/* retrieve_clusters():
    First step of the extraction.
    Retrieves all clusters of an image.
*/
int retrieve_clusters(GdkPixbuf *input, Cluster* *clusters) {
    //Grab the size of the pixbuf
    int width = gdk_pixbuf_get_width(input);
    int height = gdk_pixbuf_get_height(input);

    //Array to mark pixels as visisted
    gboolean** visited = malloc(height * sizeof(gboolean*));
    if (visisted == NULL)
        errx(EXIT_FAILURE, "malloc()");
    for (int i = 0; i < height; i++) {
        visited[i] = malloc(width * sizeof(gboolean));
        if (visited[i] == NULL) {
            free(visited);
            errx(EXIT_FAILURE, "malloc()");
        }
    }

    int N = gdk_pixbuf_get_n_channels(input);
    int rowstride = gdk_pixbuf_get_rowstride(input);
    guchar* pixels = gdk_pixbuf_get_pixels(input);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            //new cluster?
            guchar *pixel = pixels + y * rowstride + x * N;
            if (*pixel == 0 && !visited[y][x]) {//0 means a black pixel
                initCluster(&clusters[clusterCount]);
                dfs(x, y, &clusters[clusterCount], visited, input);
                clusterCount++;
            }
        }
    }
}


/* extract_information():
    Performs all the extraction from an image.
*/
void extract_information(GdkPixbuf *input) {
    //Grab the size
    int width = gdk_pixbuf_get_width(input);
    int height = gdk_pixbuf_get_height(input);

    //Dynamic array of clusters
    Cluster* clusters = NULL;
    
    //FIRST STEP 1: retrieving EVERY clusters
    retrieve_clusters(input, &clusters);

}
