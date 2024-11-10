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
        Cluster grid[14][12], int size_x, int size_y,//Grid of letters (clusters)
        Cluster* word_list[13][11], //List of words of letters (&clusters) with '\0'
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
            draw_line_from_data(res,s1,s2,e1,e2, 1.5f, //Thickness
                    1, 0, 0, 0.5f); //RGBA values

            //Crossing out the word in word list
            get_position_from_cluster(*(word_list[i][0]), &s1, &s2);
            int j = 0;
            while (word_list[i][j+1] != NULL)
                j++;
            get_position_from_cluster(*word_list[i][j], &e1, &e2);
            //Draw
            draw_line_from_data(res,s1,s2,e1,e2, 0.5f, //Thickness
                    1, 0, 0, 1); //RGBA values
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
    Cluster grid[14][12] = {
        {(Cluster) {.minX=25,.maxX=43,.minY=25,.maxY=45},
(Cluster) {.minX=78,.maxX=93,.minY=25,.maxY=45},
(Cluster) {.minX=128,.maxX=148,.minY=25,.maxY=45},
(Cluster) {.minX=182,.maxX=199,.minY=25,.maxY=45},
(Cluster) {.minX=235,.maxX=249,.minY=25,.maxY=45},
(Cluster) {.minX=288,.maxX=302,.minY=25,.maxY=45},
(Cluster) {.minX=339,.maxX=354,.minY=25,.maxY=45},
(Cluster) {.minX=389,.maxX=407,.minY=25,.maxY=45},
(Cluster) {.minX=444,.maxX=458,.minY=25,.maxY=45},
(Cluster) {.minX=496,.maxX=509,.minY=25,.maxY=45},
(Cluster) {.minX=547,.maxX=562,.minY=25,.maxY=45},
(Cluster) {.minX=598,.maxX=613,.minY=25,.maxY=45}},
        {(Cluster) {.minX=26,.maxX=42,.minY=73,.maxY=93},
(Cluster) {.minX=79,.maxX=93,.minY=73,.maxY=93},
(Cluster) {.minX=131,.maxX=146,.minY=73,.maxY=93},
(Cluster) {.minX=182,.maxX=197,.minY=73,.maxY=93},
(Cluster) {.minX=236,.maxX=250,.minY=73,.maxY=93},
(Cluster) {.minX=287,.maxX=302,.minY=73,.maxY=93},
(Cluster) {.minX=340,.maxX=354,.minY=73,.maxY=93},
(Cluster) {.minX=388,.maxX=408,.minY=73,.maxY=93},
(Cluster) {.minX=443,.maxX=458,.minY=73,.maxY=93},
(Cluster) {.minX=493,.maxX=511,.minY=73,.maxY=93},
(Cluster) {.minX=546,.maxX=563,.minY=73,.maxY=93},
(Cluster) {.minX=600,.maxX=614,.minY=73,.maxY=93}},
        {(Cluster) {.minX=27,.maxX=42,.minY=121,.maxY=141},
(Cluster) {.minX=79,.maxX=94,.minY=121,.maxY=141},
(Cluster) {.minX=132,.maxX=145,.minY=121,.maxY=141},
(Cluster) {.minX=180,.maxX=200,.minY=121,.maxY=141},
(Cluster) {.minX=235,.maxX=250,.minY=121,.maxY=141},
(Cluster) {.minX=286,.maxX=303,.minY=121,.maxY=141},
(Cluster) {.minX=339,.maxX=354,.minY=121,.maxY=141},
(Cluster) {.minX=390,.maxX=407,.minY=121,.maxY=141},
(Cluster) {.minX=442,.maxX=458,.minY=121,.maxY=141},
(Cluster) {.minX=494,.maxX=511,.minY=121,.maxY=141},
(Cluster) {.minX=546,.maxX=561,.minY=121,.maxY=141},
(Cluster) {.minX=598,.maxX=615,.minY=121,.maxY=141}},
        {(Cluster) {.minX=27,.maxX=42,.minY=169,.maxY=189},
(Cluster) {.minX=78,.maxX=95,.minY=169,.maxY=189},
(Cluster) {.minX=130,.maxX=145,.minY=169,.maxY=189},
(Cluster) {.minX=184,.maxX=197,.minY=169,.maxY=189},
(Cluster) {.minX=236,.maxX=250,.minY=169,.maxY=189},
(Cluster) {.minX=285,.maxX=303,.minY=169,.maxY=189},
(Cluster) {.minX=339,.maxX=354,.minY=169,.maxY=189},
(Cluster) {.minX=390,.maxX=405,.minY=169,.maxY=189},
(Cluster) {.minX=442,.maxX=459,.minY=169,.maxY=189},
(Cluster) {.minX=494,.maxX=509,.minY=169,.maxY=189},
(Cluster) {.minX=548,.maxX=562,.minY=169,.maxY=189},
(Cluster) {.minX=599,.maxX=614,.minY=169,.maxY=189}},
        {(Cluster) {.minX=28,.maxX=42,.minY=217,.maxY=237},
(Cluster) {.minX=76,.maxX=96,.minY=217,.maxY=237},
(Cluster) {.minX=132,.maxX=145,.minY=217,.maxY=237},
(Cluster) {.minX=184,.maxX=198,.minY=217,.maxY=237},
(Cluster) {.minX=234,.maxX=251,.minY=217,.maxY=237},
(Cluster) {.minX=287,.maxX=302,.minY=217,.maxY=237},
(Cluster) {.minX=339,.maxX=354,.minY=217,.maxY=237},
(Cluster) {.minX=396,.maxX=399,.minY=217,.maxY=237},
(Cluster) {.minX=442,.maxX=459,.minY=217,.maxY=237},
(Cluster) {.minX=495,.maxX=510,.minY=217,.maxY=237},
(Cluster) {.minX=547,.maxX=562,.minY=217,.maxY=237},
(Cluster) {.minX=599,.maxX=614,.minY=217,.maxY=237}},
        {(Cluster) {.minX=27,.maxX=42,.minY=265,.maxY=285},
(Cluster) {.minX=79,.maxX=94,.minY=265,.maxY=285},
(Cluster) {.minX=136,.maxX=139,.minY=265,.maxY=285},
(Cluster) {.minX=184,.maxX=197,.minY=265,.maxY=285},
(Cluster) {.minX=235,.maxX=250,.minY=265,.maxY=285},
(Cluster) {.minX=287,.maxX=302,.minY=265,.maxY=285},
(Cluster) {.minX=336,.maxX=356,.minY=265,.maxY=285},
(Cluster) {.minX=391,.maxX=406,.minY=265,.maxY=285},
(Cluster) {.minX=443,.maxX=458,.minY=265,.maxY=285},
(Cluster) {.minX=496,.maxX=509,.minY=265,.maxY=285},
(Cluster) {.minX=546,.maxX=563,.minY=265,.maxY=285},
(Cluster) {.minX=598,.maxX=614,.minY=265,.maxY=285}},
        {(Cluster) {.minX=28,.maxX=42,.minY=313,.maxY=333},
(Cluster) {.minX=80,.maxX=94,.minY=313,.maxY=333},
(Cluster) {.minX=129,.maxX=147,.minY=313,.maxY=333},
(Cluster) {.minX=183,.maxX=198,.minY=313,.maxY=333},
(Cluster) {.minX=233,.maxX=251,.minY=313,.maxY=333},
(Cluster) {.minX=286,.maxX=303,.minY=313,.maxY=333},
(Cluster) {.minX=336,.maxX=356,.minY=313,.maxY=333},
(Cluster) {.minX=392,.maxX=405,.minY=313,.maxY=333},
(Cluster) {.minX=443,.maxX=458,.minY=313,.maxY=333},
(Cluster) {.minX=494,.maxX=511,.minY=313,.maxY=333},
(Cluster) {.minX=547,.maxX=562,.minY=313,.maxY=333},
(Cluster) {.minX=599,.maxX=614,.minY=313,.maxY=333}},
        {(Cluster) {.minX=27,.maxX=42,.minY=361,.maxY=381},
(Cluster) {.minX=79,.maxX=94,.minY=361,.maxY=381},
(Cluster) {.minX=132,.maxX=146,.minY=361,.maxY=381},
(Cluster) {.minX=183,.maxX=198,.minY=361,.maxY=381},
(Cluster) {.minX=235,.maxX=250,.minY=361,.maxY=381},
(Cluster) {.minX=286,.maxX=301,.minY=361,.maxY=381},
(Cluster) {.minX=339,.maxX=354,.minY=361,.maxY=381},
(Cluster) {.minX=392,.maxX=406,.minY=361,.maxY=381},
(Cluster) {.minX=443,.maxX=458,.minY=361,.maxY=381},
(Cluster) {.minX=495,.maxX=510,.minY=361,.maxY=381},
(Cluster) {.minX=546,.maxX=562,.minY=361,.maxY=381},
(Cluster) {.minX=599,.maxX=614,.minY=361,.maxY=381}},
        {(Cluster) {.minX=26,.maxX=43,.minY=409,.maxY=429},
(Cluster) {.minX=79,.maxX=94,.minY=409,.maxY=429},
(Cluster) {.minX=130,.maxX=146,.minY=409,.maxY=429},
(Cluster) {.minX=182,.maxX=199,.minY=409,.maxY=429},
(Cluster) {.minX=234,.maxX=250,.minY=409,.maxY=429},
(Cluster) {.minX=286,.maxX=303,.minY=409,.maxY=429},
(Cluster) {.minX=339,.maxX=354,.minY=409,.maxY=429},
(Cluster) {.minX=390,.maxX=407,.minY=409,.maxY=429},
(Cluster) {.minX=442,.maxX=457,.minY=409,.maxY=429},
(Cluster) {.minX=496,.maxX=509,.minY=409,.maxY=429},
(Cluster) {.minX=546,.maxX=563,.minY=409,.maxY=429},
(Cluster) {.minX=597,.maxX=615,.minY=409,.maxY=429}},
        {(Cluster) {.minX=28,.maxX=41,.minY=457,.maxY=477},
(Cluster) {.minX=78,.maxX=94,.minY=457,.maxY=477},
(Cluster) {.minX=130,.maxX=146,.minY=457,.maxY=477},
(Cluster) {.minX=182,.maxX=199,.minY=457,.maxY=477},
(Cluster) {.minX=235,.maxX=250,.minY=457,.maxY=477},
(Cluster) {.minX=286,.maxX=301,.minY=457,.maxY=477},
(Cluster) {.minX=340,.maxX=354,.minY=457,.maxY=477},
(Cluster) {.minX=391,.maxX=406,.minY=457,.maxY=477},
(Cluster) {.minX=444,.maxX=458,.minY=457,.maxY=477},
(Cluster) {.minX=500,.maxX=503,.minY=457,.maxY=477},
(Cluster) {.minX=544,.maxX=564,.minY=457,.maxY=477},
(Cluster) {.minX=604,.maxX=607,.minY=457,.maxY=477}},
        {(Cluster) {.minX=27,.maxX=42,.minY=505,.maxY=525},
(Cluster) {.minX=80,.maxX=94,.minY=505,.maxY=525},
(Cluster) {.minX=131,.maxX=146,.minY=505,.maxY=525},
(Cluster) {.minX=182,.maxX=199,.minY=505,.maxY=525},
(Cluster) {.minX=234,.maxX=251,.minY=505,.maxY=525},
(Cluster) {.minX=286,.maxX=303,.minY=505,.maxY=525},
(Cluster) {.minX=338,.maxX=353,.minY=505,.maxY=525},
(Cluster) {.minX=390,.maxX=407,.minY=505,.maxY=525},
(Cluster) {.minX=442,.maxX=459,.minY=505,.maxY=525},
(Cluster) {.minX=495,.maxX=510,.minY=505,.maxY=525},
(Cluster) {.minX=547,.maxX=562,.minY=505,.maxY=525},
(Cluster) {.minX=599,.maxX=613,.minY=505,.maxY=525}},
        {(Cluster) {.minX=26,.maxX=42,.minY=553,.maxY=573},
(Cluster) {.minX=79,.maxX=94,.minY=553,.maxY=573},
(Cluster) {.minX=131,.maxX=146,.minY=553,.maxY=573},
(Cluster) {.minX=184,.maxX=198,.minY=553,.maxY=573},
(Cluster) {.minX=235,.maxX=250,.minY=553,.maxY=573},
(Cluster) {.minX=287,.maxX=302,.minY=553,.maxY=573},
(Cluster) {.minX=338,.maxX=353,.minY=553,.maxY=573},
(Cluster) {.minX=390,.maxX=407,.minY=553,.maxY=573},
(Cluster) {.minX=443,.maxX=458,.minY=553,.maxY=573},
(Cluster) {.minX=494,.maxX=509,.minY=553,.maxY=573},
(Cluster) {.minX=546,.maxX=561,.minY=553,.maxY=573},
(Cluster) {.minX=596,.maxX=616,.minY=553,.maxY=573}},
        {(Cluster) {.minX=26,.maxX=42,.minY=601,.maxY=621},
(Cluster) {.minX=79,.maxX=94,.minY=601,.maxY=621},
(Cluster) {.minX=131,.maxX=146,.minY=601,.maxY=621},
(Cluster) {.minX=184,.maxX=198,.minY=601,.maxY=621},
(Cluster) {.minX=235,.maxX=250,.minY=601,.maxY=621},
(Cluster) {.minX=288,.maxX=302,.minY=601,.maxY=621},
(Cluster) {.minX=338,.maxX=353,.minY=601,.maxY=621},
(Cluster) {.minX=392,.maxX=405,.minY=601,.maxY=621},
(Cluster) {.minX=443,.maxX=458,.minY=601,.maxY=621},
(Cluster) {.minX=496,.maxX=509,.minY=601,.maxY=621},
(Cluster) {.minX=546,.maxX=562,.minY=601,.maxY=621},
(Cluster) {.minX=604,.maxX=607,.minY=601,.maxY=621}},
        {(Cluster) {.minX=27,.maxX=41,.minY=649,.maxY=669},
(Cluster) {.minX=78,.maxX=94,.minY=649,.maxY=669},
(Cluster) {.minX=131,.maxX=146,.minY=649,.maxY=669},
(Cluster) {.minX=182,.maxX=199,.minY=649,.maxY=669},
(Cluster) {.minX=235,.maxX=249,.minY=649,.maxY=669},
(Cluster) {.minX=288,.maxX=302,.minY=649,.maxY=669},
(Cluster) {.minX=338,.maxX=355,.minY=649,.maxY=669},
(Cluster) {.minX=392,.maxX=406,.minY=649,.maxY=669},
(Cluster) {.minX=443,.maxX=458,.minY=649,.maxY=669},
(Cluster) {.minX=494,.maxX=511,.minY=649,.maxY=669},
(Cluster) {.minX=547,.maxX=562,.minY=649,.maxY=669},
(Cluster) {.minX=600,.maxX=614,.minY=649,.maxY=669}}};

    Cluster* word_list[13][11] = {
        {&(Cluster) {.minX=653,.maxX=663,.minY=33,.maxY=44},
&(Cluster) {.minX=666,.maxX=674,.minY=33,.maxY=44},
&(Cluster) {.minX=676,.maxX=684,.minY=33,.maxY=44},
&(Cluster) {.minX=687,.maxX=694,.minY=33,.maxY=44},
&(Cluster) {.minX=697,.maxX=704,.minY=33,.maxY=44}, NULL},
        {&(Cluster) {.minX=654,.maxX=661,.minY=60,.maxY=71},
&(Cluster) {.minX=664,.maxX=671,.minY=60,.maxY=71},
&(Cluster) {.minX=674,.maxX=684,.minY=60,.maxY=71},
&(Cluster) {.minX=687,.maxX=696,.minY=60,.maxY=71},
&(Cluster) {.minX=699,.maxX=707,.minY=60,.maxY=71}, NULL},
        {&(Cluster) {.minX=654,.maxX=662,.minY=87,.maxY=98},
&(Cluster) {.minX=665,.maxX=674,.minY=87,.maxY=98},
&(Cluster) {.minX=677,.maxX=685,.minY=87,.maxY=98},
&(Cluster) {.minX=687,.maxX=697,.minY=87,.maxY=98},
&(Cluster) {.minX=700,.maxX=708,.minY=87,.maxY=98},
&(Cluster) {.minX=710,.maxX=720,.minY=87,.maxY=98}, NULL},
        {&(Cluster) {.minX=654,.maxX=661,.minY=114,.maxY=125},
&(Cluster) {.minX=664,.maxX=665,.minY=114,.maxY=125},
&(Cluster) {.minX=669,.maxX=678,.minY=114,.maxY=125},
&(Cluster) {.minX=682,.maxX=689,.minY=114,.maxY=125}, NULL},
        {&(Cluster) {.minX=654,.maxX=663,.minY=141,.maxY=152},
&(Cluster) {.minX=666,.maxX=674,.minY=141,.maxY=152},
&(Cluster) {.minX=676,.maxX=686,.minY=141,.maxY=152},
&(Cluster) {.minX=688,.maxX=696,.minY=141,.maxY=152},
&(Cluster) {.minX=699,.maxX=708,.minY=141,.maxY=152},
&(Cluster) {.minX=711,.maxX=718,.minY=141,.maxY=152}, NULL},
        {&(Cluster) {.minX=654,.maxX=665,.minY=168,.maxY=179},
&(Cluster) {.minX=666,.maxX=676,.minY=168,.maxY=179},
&(Cluster) {.minX=676,.maxX=685,.minY=168,.maxY=179},
&(Cluster) {.minX=687,.maxX=694,.minY=168,.maxY=179},
&(Cluster) {.minX=697,.maxX=705,.minY=168,.maxY=179},
&(Cluster) {.minX=708,.maxX=718,.minY=168,.maxY=179},
&(Cluster) {.minX=722,.maxX=729,.minY=168,.maxY=179},
&(Cluster) {.minX=732,.maxX=739,.minY=168,.maxY=179},
&(Cluster) {.minX=741,.maxX=750,.minY=168,.maxY=179},
&(Cluster) {.minX=753,.maxX=761,.minY=168,.maxY=179}, NULL},
        {&(Cluster) {.minX=654,.maxX=663,.minY=195,.maxY=206},
&(Cluster) {.minX=666,.maxX=674,.minY=195,.maxY=206},
&(Cluster) {.minX=676,.maxX=686,.minY=195,.maxY=206},
&(Cluster) {.minX=688,.maxX=696,.minY=195,.maxY=206},
&(Cluster) {.minX=699,.maxX=706,.minY=195,.maxY=206}, NULL},
        {&(Cluster) {.minX=654,.maxX=662,.minY=222,.maxY=233},
&(Cluster) {.minX=664,.maxX=665,.minY=222,.maxY=233},
&(Cluster) {.minX=669,.maxX=680,.minY=222,.maxY=233},
&(Cluster) {.minX=683,.maxX=684,.minY=222,.maxY=233}, NULL},
        {&(Cluster) {.minX=654,.maxX=662,.minY=249,.maxY=260},
&(Cluster) {.minX=664,.maxX=672,.minY=249,.maxY=260},
&(Cluster) {.minX=674,.maxX=682,.minY=249,.maxY=260},
&(Cluster) {.minX=685,.maxX=694,.minY=249,.maxY=260},
&(Cluster) {.minX=696,.maxX=707,.minY=249,.maxY=260},
&(Cluster) {.minX=710,.maxX=718,.minY=249,.maxY=260},
&(Cluster) {.minX=721,.maxX=728,.minY=249,.maxY=260},
&(Cluster) {.minX=731,.maxX=739,.minY=249,.maxY=260},
&(Cluster) {.minX=742,.maxX=750,.minY=249,.maxY=260},
&(Cluster) {.minX=752,.maxX=761,.minY=249,.maxY=260}, NULL},
        {&(Cluster) {.minX=654,.maxX=662,.minY=276,.maxY=287},
&(Cluster) {.minX=664,.maxX=673,.minY=276,.maxY=287},
&(Cluster) {.minX=676,.maxX=684,.minY=276,.maxY=287},
&(Cluster) {.minX=685,.maxX=695,.minY=276,.maxY=287},
&(Cluster) {.minX=695,.maxX=704,.minY=276,.maxY=287},
&(Cluster) {.minX=705,.maxX=715,.minY=276,.maxY=287}, NULL},
    {&(Cluster) {.minX=654,.maxX=662,.minY=303,.maxY=314},
&(Cluster) {.minX=666,.maxX=673,.minY=303,.maxY=314},
&(Cluster) {.minX=675,.maxX=683,.minY=303,.maxY=314},
&(Cluster) {.minX=687,.maxX=694,.minY=303,.maxY=314},
&(Cluster) {.minX=697,.maxX=705,.minY=303,.maxY=314},
&(Cluster) {.minX=708,.maxX=715,.minY=303,.maxY=314},
&(Cluster) {.minX=718,.maxX=726,.minY=303,.maxY=314},
&(Cluster) {.minX=729,.maxX=737,.minY=303,.maxY=314},
&(Cluster) {.minX=739,.maxX=748,.minY=303,.maxY=314}, NULL},
    {&(Cluster) {.minX=654,.maxX=662,.minY=330,.maxY=341},
&(Cluster) {.minX=666,.maxX=672,.minY=330,.maxY=341},
&(Cluster) {.minX=674,.maxX=684,.minY=330,.maxY=341},
&(Cluster) {.minX=686,.maxX=694,.minY=330,.maxY=341},
&(Cluster) {.minX=697,.maxX=705,.minY=330,.maxY=341},
&(Cluster) {.minX=708,.maxX=716,.minY=330,.maxY=341},
&(Cluster) {.minX=719,.maxX=726,.minY=330,.maxY=341},
&(Cluster) {.minX=729,.maxX=737,.minY=330,.maxY=341},
&(Cluster) {.minX=740,.maxX=748,.minY=330,.maxY=341},
&(Cluster) {.minX=750,.maxX=759,.minY=330,.maxY=341}, NULL},
    {&(Cluster) {.minX=654,.maxX=662,.minY=357,.maxY=368},
&(Cluster) {.minX=664,.maxX=674,.minY=357,.maxY=368},
&(Cluster) {.minX=676,.maxX=684,.minY=357,.maxY=368},
&(Cluster) {.minX=687,.maxX=695,.minY=357,.maxY=368},
&(Cluster) {.minX=698,.maxX=706,.minY=357,.maxY=368},
&(Cluster) {.minX=709,.maxX=716,.minY=357,.maxY=368},
&(Cluster) {.minX=719,.maxX=727,.minY=357,.maxY=368},
&(Cluster) {.minX=730,.maxX=738,.minY=357,.maxY=368},
&(Cluster) {.minX=740,.maxX=749,.minY=357,.maxY=368}, NULL}};

    Solution* solutions[13] = {
        &(Solution) {.ini_row=8,.ini_col=3,.end_row=4,.end_col=3},
        &(Solution) {.ini_row=3,.ini_col=3,.end_row=3,.end_col=7},
        &(Solution) {.ini_row=5,.ini_col=5,.end_row=10,.end_col=5},
        &(Solution) {.ini_row=4,.ini_col=2,.end_row=7,.end_col=2},
        &(Solution) {.ini_row=8,.ini_col=6,.end_row=13,.end_col=11},
        &(Solution) {.ini_row=0,.ini_col=2,.end_row=0,.end_col=11},
        &(Solution) {.ini_row=7,.ini_col=10,.end_row=3,.end_col=10},
        &(Solution) {.ini_row=9,.ini_col=8,.end_row=9,.end_col=11},
        &(Solution) {.ini_row=0,.ini_col=1,.end_row=9,.end_col=1},
        &(Solution) {.ini_row=2,.ini_col=4,.end_row=2,.end_col=9},
        &(Solution) {.ini_row=12,.ini_col=8,.end_row=12,.end_col=0},
        &(Solution) {.ini_row=10,.ini_col=0,.end_row=1,.end_col=0},
        &(Solution) {.ini_row=11,.ini_col=8,.end_row=11,.end_col=0}
    };

    //Call reconstruct
    GdkPixbuf* final = reconstruct_from_data(pixbuf, grid, 12, 14, word_list,
            solutions, 13);

    //Save
    gdk_pixbuf_save(final, "save.png", "png", NULL, NULL);
}
