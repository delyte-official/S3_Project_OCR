#ifndef CUT_H
#define CUT_H

#include <gtk/gtk.h>

typedef struct {
    int x, y;
} Position;

typedef struct {
    Position top_left, bottom_right;
} BoundingBox;

GdkPixbuf *cut_from_pixbuf(GdkPixbuf *original, BoundingBox bounds);

#endif
