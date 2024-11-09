#include <gtk/gtk.h>

typedef struct cluster {
    int size;
    int centerX, centerY;
    int maxX, maxY, minX, minY;
    struct cluster *next;
} Cluster;

typedef struct {
    int x, y;
} Position;

/* draw_line_from_data():
    Draw a line from specific data properties.
*/
void draw_line_from_data(GdkPixbuf* pixbuf,
        Position start_tl, Position start_br,
        Position end_tl, Position end_br, //Positions
        float thickness_ratio,
        double r, double g, double b, double a) {
    //Calculating data
    //Calculating data
    Position start_c = (Position) {
        .x = (start_tl.x + start_br.x) /2,
        .y = (start_tl.y + start_br.y) /2,
    };
    Position end_c = (Position) {
        .x = (end_tl.x + end_br.x)/2,
        .y = (end_tl.y + end_br.y)/2
    };
    int thickness = thickness_ratio * MAX(MAX(start_br.x-start_tl.x,
                start_br.y-start_tl.y), MAX(end_br.x-end_tl.x,
                    end_br.y-end_tl.y));

    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    //Create a Cairo surface from the pixbuf data
    cairo_surface_t *surface = cairo_image_surface_create_for_data(
        pixels, CAIRO_FORMAT_RGB24, width, height, rowstride);
    cairo_t *cr = cairo_create(surface);

    //Draw line
    cairo_set_source_rgba(cr, b, g, r, a);
    cairo_set_line_width(cr, thickness);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

    cairo_move_to(cr, start_c.x, start_c.y);
    cairo_line_to(cr, end_c.x, end_c.y);
    cairo_stroke(cr);

    //Freeing
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}


/*GdkPixbuf* reconstruct_from_data(GdkPixbuf* pixbuf,) {
    GdkPixbuf* res = gdk_pixbuf_copy(pixbuf);

    //Highlight every word found
    while (cluster != NULL) {
        //draw_highlight(res, cluster);
        cluster = cluster->next;
    }
    return res;
}*/


int main(int argc, char* argv[]) {
    //INIT to bypass
    gtk_init(&argc, &argv);

    //Load pixbuf
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("input.png", NULL);

    //Cluster tables for test

    //Call reconstruct
    //GdkPixbuf* final = reconstruct_from_data(pixbuf, start);
    Position s1,s2,e1,e2;
    s1 = (Position) {.x=338,.y=409};
    s2 = (Position) {.x=355,.y=429};
    e1 = (Position) {.x=600,.y=649};
    e2 = (Position) {.x=614,.y=669};

    //Call
    draw_line_from_data(pixbuf, s1, s2, e1, e2, 0.4f, 1, 0, 0, 1);

    //Save
    gdk_pixbuf_save(pixbuf, "save.png", "png", NULL, NULL);
}
