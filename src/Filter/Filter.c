#include <gtk/gtk.h>
#include <math.h>
#include "../Core_Manager.h"
#include "../Interface/Interface.h"
#include "Filter.h"

int Filter_Image(GdkPixbuf *input) {
    GdkPixbuf *filtered = grayscale_pixbuf(input, 180);
    //Saving widget
    GdkPixbuf *resized = resize_from_container(filtered, DISPLAY);
    GtkWidget *image = gtk_image_new_from_pixbuf(resized);
    g_object_set_data(G_OBJECT(image), "pixbuf", filtered);
    g_object_ref(filtered);
    FREESTEPDATA(STEP_FILTER);
    SETSTEPDATA(STEP_FILTER,image);
    return 1;
}

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
