#include <gtk/gtk.h>
#include "Cut.h"
#include <stdio.h>

GdkPixbuf *cut_from_pixbuf(GdkPixbuf *original, BoundingBox bounds) {
    int sub_w = bounds.bottom_right.x - bounds.top_left.x;
    int sub_h = bounds.bottom_right.y - bounds.top_left.y;
    printf("before cut\n");
    GdkPixbuf *subpix = gdk_pixbuf_new_subpixbuf(original, //Source
            bounds.top_left.x, bounds.top_left.y,//Top_left pos
            sub_w, sub_h); //Top_right pos relative to top_left pos
    printf("after cut\n");
    //Create copy bc "subpix" shares data with "original"
    GdkPixbuf *res = gdk_pixbuf_copy(subpix);
    return res;
}
