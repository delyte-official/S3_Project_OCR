#ifndef GRID_DETECT_H
#define GRID_DETECT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <math.h>
#include <gtk/gtk.h>

typedef struct {
    int x, y;
} Position;

typedef struct {
    Position top_left, bottom_right;
} BoundingBox;

typedef struct {
    Position begin, end;
} Line;

//DEBUG FUNCTIONS
void display_lines(Line *lines, int nb_lines, char dir);

//USEFUL FUNCTIONS
Line* horizontal_search(GdkPixbuf *pixbuf, int *nb_h_lines);
Line* vertical_search(GdkPixbuf *pixbuf, int *nb_v_lines);
void create_outlines(Line **h_lines, int *n_h_lines, 
                     Line **v_lines, int *n_v_lines);
Line** search_for_grid(GdkPixbuf *pixbuf, int *nb_h_lines, int *nb_v_lines);
void save_boundingBox_as_pixbuf(GdkPixbuf *pixbuf, BoundingBox box,
                                char* path_flag, char* filename);
void save_grid_pixbuf(GdkPixbuf *pixbuf, Line **lines);
void save_grid_letters_pixbuf(GdkPixbuf *pixbuf, Line **lines,
                              int n_h_lines, int n_v_lines);
void extraction(GdkPixbuf *pixbuf);



//-----------------------//
//   USELESS (FOR NOW)   //
//-----------------------//

//int calculate_intensity(guchar r, guchar g, guchar b);
//void sobel_edge_detection(GdkPixbuf *pixbuf, GdkPixbuf *new_pixbuf);
//void hough_transform(GdkPixbuf *edge_image, Line *lines, int *max_lines);

#endif