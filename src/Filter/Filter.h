#ifndef FILTER_H
#define FILTER_H

int Filter_Image(GdkPixbuf *input);
GdkPixbuf *grayscale_pixbuf(GdkPixbuf *origin, int threshold);
GdkPixbuf *rotate_pixbuf(GdkPixbuf *original, double angle);

#endif
