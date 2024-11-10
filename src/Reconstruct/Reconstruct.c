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

typedef struct {
    int ini_row;
    int ini_col;
    int end_row;
    int end_col;
} Solution;

/* draw_line_from_data():
    Draw a line from specific data properties.
*/
void draw_line_from_data(GdkPixbuf* pixbuf,
        Position start_tl, Position start_br,
        Position end_tl, Position end_br, //Positions
        float thickness_ratio,
        double r, double g, double b, double a) {
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

void get_position_from_cluster(Cluster input, Position *p1, Position *p2) {
    *p1 = (Position) {.x=input.minX,.y=input.minY};
    *p2 = (Position) {.x=input.maxX,.y=input.maxY};
}

GdkPixbuf* reconstruct_from_data(GdkPixbuf* pixbuf, //Image to modify
        Cluster** grid, int size_x, int size_y,//Grid of letters (clusters)
        Cluster*** word_list, //List of words of letters (&clusters) with '\0'
        Solution** solutions, int len) { //To reconstruct graphically
    GdkPixbuf* res = gdk_pixbuf_copy(pixbuf);

    Position s1,s2,e1,e2;
    for (int i = 0; i < len; i++) {
        if (solutions[i]!=NULL) { //Word has been found
            //HIGHLIGHTING THE WORD IN GRID
            int sx = solutions[i]->ini_row;
            int sy = solutions[i]->ini_col;
            int ex = solutions[i]->end_row;
            int ey = solutions[i]->end_col;
            get_position_from_cluster(grid[sx][sy], &s1, &s2);
            get_position_from_cluster(grid[ex][ey], &e1, &e2);
            //Draw
            draw_line_from_data(pixbuf,s1,s2,e1,e2, 1.5f, //Thickness
                    1, 0, 0, 0.5f); //RGBA values

            //Crossing out the word in word list
            get_position_from_cluster(*word_list[i][0], &s1, &s2);
            int j = 0;
            while (word_list[i][j+1] != NULL)
                j++;
            get_position_from_cluster(*word_list[i][j], &e1, &e2);
            //Draw
            draw_line_from_data(pixbuf,s1,s2,e1,e2, 0.5f, //Thickness
                    1, 0, 0, 0.5f); //RGBA values
        }
    }
    return res;
}


int main(int argc, char* argv[]) {
    //INIT to bypass
    gtk_init(&argc, &argv);

    //Load pixbuf
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("input.png", NULL);

    //Creating test data
    int XSIZE = 12;
    int YSIZE = 14;
    int gminX[YSIZE] = {};
    int gminY[YSIZE] = {};
    int gmaxX[YSIZE] = {};
    int gmaxY[YSIZE] = {};

    int WSIZE = 13;
    int wminX[WSIZE] = {};
    int wminY[WSIZE] = {};
    int wmaxX[WSIZE] = {};
    int wmaxY[WSIZE] = {};

    //Call reconstruct
    GdkPixbuf* final = reconstruct_from_data(pixbuf, start);

    //Save
    gdk_pixbuf_save(final, "save.png", "png", NULL, NULL);
}
