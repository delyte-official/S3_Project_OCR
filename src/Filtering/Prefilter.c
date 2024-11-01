#include <gtk/gtk.h>
#include <math.h>

GdkPixbuf *rotate_pixbuf(GdkPixbuf *original, double angle) {
    //Size of our original pixbuf
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
    //update && render
    gdk_cairo_set_source_pixbuf(cr, original, 0, 0);
    cairo_paint(cr);

    //Grab result
    GdkPixbuf *res = gdk_pixbuf_get_from_surface(surface,0,0,new_w,new_h);
    //clr
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    return res;
}
