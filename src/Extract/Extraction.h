#ifndef EXTRACTION_H
#define EXTRACTION_H
#include <gtk/gtk.h>


#define FLAG_GRID 1
#define FLAG_WORD (1 << 1)
#define FLAG_UNUSED 0

typedef struct cluster {
    int size;
    int centerX, centerY;
    int maxX, maxY, minX, minY;
    struct cluster *next;
    int flags;
} Cluster;

typedef struct {
    int pos, size, nbr;
} Line;

typedef struct {
    int x, y;
} Size;

void extract_information(GdkPixbuf *input, char *grid_output,
        char *wordlist_output);

#endif
