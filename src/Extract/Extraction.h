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

void extract_information(GdkPixbuf *input, char *grid_output,
        char *wordlist_output);

#endif
