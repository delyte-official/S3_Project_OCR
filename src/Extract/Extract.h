#ifndef EXTRACTION_H
#define EXTRACTION_H
#include <gtk/gtk.h>


#define FLAG_GRID 1
#define FLAG_WORD (1 << 1)
#define FLAG_UNUSED 0

typedef struct pixel {
    int x, y;
    struct pixel *next;
} Pixel;

typedef struct cluster {
    int size;
    int centerX, centerY;
    int maxX, maxY, minX, minY;
    struct cluster *next;
    Pixel *pixels;
    int flags;
} Cluster;

typedef struct {
    int pos, size, nbr;
} Line;

typedef struct {
    int x, y;
} Position;

typedef struct {
    int rows, cols;
} Size;

void free_matrix(Cluster ***matrix, int xs);
void free_cluster(Cluster *to_free);
int retrieve_clusters(GdkPixbuf *input, Cluster **last, int *median);
int threshold_filter(Cluster **clusters, int count, int median);
void align_clusters(Cluster *clusters,Cluster ****matrixH,Cluster ****matrixV,
        Line **rows_p, Line **cols_p, Size *sizeH, Size *sizeV);
void classify_clusters(Cluster ****grid, Cluster ****wordlist, Size *sizeH,
        Size *sizeV);
void cut_grid(Cluster ***grid, Size size, char *output);
void cut_wordlist(Cluster ***wordlist, Size size, GdkPixbuf *input,
        char *output);

#endif
