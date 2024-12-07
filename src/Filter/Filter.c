#include <gtk/gtk.h>
#include <math.h>
#include "../Core_Manager.h"
#include "../Interface/Interface.h"
#include "Filter.h"

int Filter_Image(GdkPixbuf *input) {
    GdkPixbuf *filtered = gdk_pixbuf_copy(input);
    pixbuf_to_grayscale(filtered);
    binary_threshold(filtered,180);
    //Saving widget
    GdkPixbuf *resized = resize_from_container(filtered, DISPLAY);
    GtkWidget *image = gtk_image_new_from_pixbuf(resized);
    g_object_set_data(G_OBJECT(image), "pixbuf", filtered);
    g_object_ref(filtered);
    FREESTEPDATA(STEP_FILTER);
    SETSTEPDATA(STEP_FILTER,image);
    return 1;
}


void get_info_from_pixbuf(GdkPixbuf *input, int *width, int *height,
        int *rowstride, int *channels, guchar **pixels) {
    *width = gdk_pixbuf_get_width(input);
    *height = gdk_pixbuf_get_height(input);
    *rowstride = gdk_pixbuf_get_rowstride(input);
    *channels = gdk_pixbuf_get_n_channels(input);
    *pixels = gdk_pixbuf_get_pixels(input);
}


void pixbuf_to_grayscale(GdkPixbuf *input) {
    int width, height, rowstride, N;
    guchar *pixels;
    get_info_from_pixbuf(input, &width, &height, &rowstride, &N, &pixels);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            guchar *p = pixels + y * rowstride + x*N;
            //The RGB values
            guchar r = p[0];guchar g = p[1];guchar b = p[2];
            //=>to gray
            guchar gray = (guchar)(0.3 * r + 0.59 * g + 0.11 * b);
            p[0] = gray;p[1] = gray;p[2] = gray;
        }
    }
}


/* binary_threshold():
    Returns the same pixbuf but in black and white pixels.
*/
void binary_threshold(GdkPixbuf *input, int threshold) {
    int width, height, rowstride, N;
    guchar *pixels;
    get_info_from_pixbuf(input, &width, &height, &rowstride, &N, &pixels);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            guchar *p = pixels + y*rowstride + x*N;
            unsigned char final;
            if (p[0] > threshold)
                final = 255;
            else
                final = 0;
            p[0] = final;p[1]=final;p[2]=final;
        }
    }
}


GdkPixbuf *rotate_pixbuf(GdkPixbuf *original, double angle) {
    int width = gdk_pixbuf_get_width(original);
    int height = gdk_pixbuf_get_height(original);
    //Now we have new size which depends on angle, and we wrap it in a rect
    double rads = angle * (M_PI / 180.0);
    //fabs = abs() for float/doubles
    int new_w = fabs(width * cos(rads)) + fabs(height * sin(rads));
    int new_h = fabs(width * sin(rads)) + fabs(height * cos(rads));
    //cairo is a tool to modify images fast
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
            new_w, new_h);
    cairo_t *cr = cairo_create(surface);
    //Fill background with white
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    //go to mid and rotate, then move back
    cairo_translate(cr, new_w/ 2, new_h /2);
    cairo_rotate(cr, rads);
    cairo_translate(cr, -width/ 2, -height/ 2);
    gdk_cairo_set_source_pixbuf(cr, original, 0, 0);
    cairo_paint(cr);
    GdkPixbuf *res = gdk_pixbuf_get_from_surface(surface,0,0,new_w,new_h);
    //clr/free data
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    return res;
}
