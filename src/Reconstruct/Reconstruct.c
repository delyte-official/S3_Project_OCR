#include <gtk/gtk.h>
#include "../Core_Manager.h"
#include "../Extract/Extract.h"
#include "../Solving/Solver_Manager.h"
#include "../Interface/Interface.h"


/* draw_line_from_data():
    Draw a line from specific data properties (Thickness, Color...).
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


void get_position_from_cluster(Cluster *input, Position *p1, Position *p2) {
    *p1 = (Position) {.x=input->minX,.y=input->minY};
    *p2 = (Position) {.x=input->maxX,.y=input->maxY};
}


void refine_letter(GdkPixbuf *original, GdkPixbuf *input, Cluster *letter) {
    guchar *pixels_o = gdk_pixbuf_get_pixels(original);
    guchar *pixels_n = gdk_pixbuf_get_pixels(input);
    int n = gdk_pixbuf_get_n_channels(original);
    int rowstride = gdk_pixbuf_get_rowstride(original);
    Pixel *curr = letter->pixels;
    while (curr!=NULL) {
        guchar *p_o = pixels_o+curr->y*rowstride+curr->x*n;
        guchar *p_n = pixels_n+curr->y*rowstride+curr->x*n;
        p_n[0] = p_o[0]; p_n[1] = p_o[1]; p_n[2] =p_o[2];
        curr=curr->next;
    }
}


int clamp(int value, int min, int max) {
    if (value<min) return min;
    if (value>max) return max;
    return value;
}


void refine_letters(GdkPixbuf *original, GdkPixbuf *input, Cluster* **grid,
        Solution* solution) {
    int dx = clamp(solution->end_col-solution->ini_col,-1,1);
    int dy =clamp(solution->end_row-solution->ini_row,-1,1);
    for (int x = solution->ini_col, y = solution->ini_row;
            x!=solution->end_col+dx || y!=solution->end_row+dy; x+=dx,y+=dy) {
        refine_letter(original, input, grid[x][y]);
    }
}


int Reconstruct(GdkPixbuf* input, Cluster* **grid, Cluster* **word_list,
        Solution** solutions, int word_count) {
    GdkPixbuf* res = gdk_pixbuf_copy(input);
    Position s1,s2,e1,e2;
    for (int i = 0; i < word_count; i++) {
        if (solutions[i]!=NULL) { //Word has been found
            //HIGHLIGHTING THE WORD IN GRID
            int sx = solutions[i]->ini_col;
            int sy = solutions[i]->ini_row;
            int ex = solutions[i]->end_col;
            int ey = solutions[i]->end_row;
            get_position_from_cluster(grid[sx][sy], &s1, &s2);
            get_position_from_cluster(grid[ex][ey], &e1, &e2);
            //Draw
            float r = (i*40)%256, g = (255-i*40)%256, b = 255;
            draw_line_from_data(res,s1,s2,e1,e2, 1.5f, //Thickness
                    r/255, g/255, b/255, 0.5f); //RGBA values
            refine_letters(input,res,grid,solutions[i]);
            //Crossing out the word in word list
            get_position_from_cluster(word_list[i][0], &s1, &s2);
            int j = 0;
            while (word_list[i][j+1] != NULL)
                j++;
            get_position_from_cluster(word_list[i][j], &e1, &e2);
            //Draw
            draw_line_from_data(res,s1,s2,e1,e2, 0.2f, //Thickness
                    1, 0, 0, 1); //RGBA values
        }
    }
    GdkPixbuf *resized = resize_from_container(res, DISPLAY);
    GtkWidget *image = gtk_image_new_from_pixbuf(resized);
    g_object_set_data(G_OBJECT(image), "pixbuf", res);
    g_object_ref(res);
    FREESTEPDATA(STEP_RECONSTRUCT);
    SETSTEPDATA(STEP_RECONSTRUCT,image);
    return 1;
}
