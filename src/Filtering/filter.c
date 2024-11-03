#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

GdkPixbuf *grayscale_pixbuf(GdkPixbuf *origin, int threshold) {
    //Creating a deep copy to not modify our original input
    GdkPixbuf *res = gdk_pixbuf_copy(origin);

    //Get back up* the information
    int width = gdk_pixbuf_get_width(res);
    int height = gdk_pixbuf_get_height(res);
    int rowstride = gdk_pixbuf_get_rowstride(res);
    int channels = gdk_pixbuf_get_n_channels(res);
    guchar *pixels = gdk_pixbuf_get_pixels(res);

    //Iterating over the pixels of pixbuf
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            guchar *pixel = pixels + y * rowstride + x * channels;
            //The RGB values
            guchar r = pixel[0];
            guchar g = pixel[1];
            guchar b = pixel[2];
            guchar gray = (guchar)(0.3 * r + 0.59 * g + 0.11 * b);
            //threshomd
            if (gray > threshold)
                gray = 255;
            else
                gray = 0;
            pixel[0] = gray;
            pixel[1] = gray;
            pixel[2] = gray;
        }
    }
    return res;
}
