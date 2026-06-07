#ifndef FILTER_H
#define FILTER_H

int Filter_Image(GdkPixbuf *input);
void pixbuf_to_grayscale(GdkPixbuf *origin);
void binary_threshold(GdkPixbuf *input, int threshold);
GdkPixbuf *rotate_pixbuf(GdkPixbuf *original, double angle);

#endif
