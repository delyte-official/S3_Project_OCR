/*
      ##############################################################
      #                                                            #
      #                       grid_detect.c                        #
      #                                                            #
      #   This file extracts the word list, the grid of letters    #
      #          and the letters themselves from a given           #
      #                    preprocessed image.                     #
      #                                                            #
      ##############################################################

List of all functions written in this file (and their type):
[See more description on their purpose and parameters down below]

---TO BE PRECISED SOON---
*/

#include "extraction.h" // create multiple includes in header file



//---------------------//
//   DEBUG FUNCTIONS   //
//---------------------//

/*
    This is a DEBUG function to show the lines of a word search grid.
    Params:
     - *lines : array of lines to be printed
     - nb_lines : nb of lines in the array
     - dir : direction of the lines (H for horizontal, V for vertical)
*/
void display_lines(Line *lines, int nb_lines, char dir)
{
    for (int i = 0; i < nb_lines; i++)
    {
        printf("%c_LINE N°%d = (%d, %d) to (%d, %d)\n", dir, i+1, 
        lines[i].begin.x, lines[i].begin.y, lines[i].end.x, lines[i].end.y);
    }

    printf("\n");
    printf("-----------------------------------------------------\n");
    printf("\n");
}



//----------------------//
//   USEFUL FUNCTIONS   //
//----------------------//

/*
    horizontal_search()
  Returns all the horizontal lines of the grid (if it exists) of a given pixbuf.
  Stores the number of finally created horizontal lines into *nb_h_lines param.
  Params:
    - pixbuf : the pixel buffer of the image from which data will be extracted.
    - nb_h_lines : the number of finally created horizontal lines.
  Return Value:
    - An array of Line representing the horizontal lines of the grid if it 
      exists;
    - NULL if the grid does not exist.
*/
Line* horizontal_search(GdkPixbuf *pixbuf, int *nb_h_lines)
{
    //Get all the useful attributes of the 'pixbuf' parameter
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    //create horizontal line storage
    Line *h_lines = malloc((height/8)*sizeof(Line));
    //trying to save some space by saying that there could be at max 1 grid line
    //each 8 rows of the pixbuf

    int n_h_lines = 0; //count horizontal lines

    //Horizontal search
    for (int y = 0; y < height; y++)
    {
        int x = 0;
        guchar *p;
        Line line = {{0, y}, {0, y}}; //init at y coords

        while (x < width && line.end.x == 0)
        {
            //Look into each pixel
            p = pixels + y * rowstride + x * n_channels;

            //If it is not white/almost white
            if (p[0] < 235 && p[1] < 235 && p[2] < 235)
            {
                //Edge : create a line in the case of a line
                line.begin.x = line.end.x = x;
                
                //Increment x by 1 so that we can access next elem
                x++;

                //(right below we are doing an additionnal calculation but it's
                //mandatory since else we won't have the info about the second
                //pixel after the beginning)
                p = pixels + y * rowstride + x * n_channels;

                //while it is not white/almost white
                while (x < width && (p[0] < 235 && p[1] < 235 && p[2] < 235))
                {
                    p = pixels + y * rowstride + x * n_channels;
                    line.end.x = x; //increment the end of the line
                    x++;
                }

                //Line is finished : verify if the line is making at least 50%
                //of the pixbuf's width (arbitrary)
                if (line.end.x - line.begin.x >= width/2)
                {
                    //Then we can add the line to the array  containing 
                    //the final lines
                    if (n_h_lines < height/8)
                    {
                        h_lines[n_h_lines] = line;
                        n_h_lines++;
                    }
                    else
                    {
                        printf("Horizontal line array full !\n");
                    }
                }
                else //if line not long enough, reset x coords
                {
                    line.begin.x = 0;
                    line.end.x = 0;
                }
            }

            x++;
        }
    }

    if (n_h_lines <= 5)
    {
        //Not enough lines, the grid is hence not existing
        free(h_lines);
        return NULL;
    }

    //reallocating h_lines (it might be smaller than expected)
    if (n_h_lines != height/8) //if h_lines not full
    {
        h_lines = realloc(h_lines, n_h_lines*sizeof(Line));
    }

    *nb_h_lines = n_h_lines;
    return h_lines;
}



/*
    vertical_search()
  Returns all the vertical lines of the grid (if it exists) of a given pixbuf.
  Stores the number of finally created vertical lines into *nb_v_lines param.
  Params:
    - pixbuf : the pixel buffer of the image from which data will be extracted.
    - nb_v_lines : number of finally created vertical lines.
  Return Value:
    - An array of Line representing the vertical lines of the grid if it exists;
    - NULL if the grid does not exist.
*/
Line* vertical_search(GdkPixbuf *pixbuf, int *nb_v_lines)
{
    //Get all the useful attributes of the 'pixbuf' parameter
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    //create vertical line storage
    Line *v_lines = malloc((width/8)*sizeof(Line)); 
    //trying to save some space by saying that there could be at max 1 grid line
    //each 8 columns of the pixbuf

    int n_v_lines = 0; //count vertical lines

    //Vertical search
    for (int x = 0; x < width; x++)
    {
        int y = 0;
        guchar *p;
        Line line = {{x, 0}, {x, 0}}; //init at x coords

        //While traversing the column and we did not foung a long-enough line
        while (y < height && line.end.y == 0)
        {
            //Look into each pixel
            p = pixels + y * rowstride + x * n_channels;

            //If it is not white/almost white
            if (p[0] < 235 && p[1] < 235 && p[2] < 235)
            {
                //Edge : create a line in the case of a line
                line.begin.y = line.end.y = y;
                
                //Increment x by 1 so that we can access next elem
                y++;

                //(right below we are doing an additionnal calculation but it's
                //mandatory since else we won't have the info about the second
                //pixel after the beginning)
                p = pixels + y * rowstride + x * n_channels;

                //while it is not white/almost white
                while (x < width && (p[0] < 235 && p[1] < 235 && p[2] < 235))
                {
                    p = pixels + y * rowstride + x * n_channels;
                    line.end.y = y; //increment the end of the line
                    y++;
                }

                //Line is finished : verify if the line is making at least 50%
                //of the pixbuf's height (arbitrary)
                if (line.end.y - line.begin.y >= height/2)
                {
                    //Then we can add the line to the array containing 
                    //the final lines
                    if (n_v_lines < width/8)
                    {
                        v_lines[n_v_lines] = line;
                        n_v_lines++;
                    }
                    else
                    {
                        printf("Vertical line array full !\n");
                    }
                }
                else //if line not long enough, reset y coords
                {
                    line.begin.y = 0;
                    line.end.y = 0;
                }
            }

            y++;
        }
    }

    if (n_v_lines <= 5)
    {
        //Not enough lines, the grid is hence not existing
        free(v_lines);
        return NULL;
    }

    //reallocating v_lines (it might be smaller than expected)
    if (n_v_lines != width/8) //if v_lines not full
    {
        v_lines = realloc(v_lines, n_v_lines*sizeof(Line));
    }

    *nb_v_lines = n_v_lines;
    return v_lines;
}


/*
    create_outlines()
  Creates grid outlines if needed on the top, down, left and/or right side of
  the grid. Adds the needed outlines into the corresponding arrays of lines.
  Params:
   - h_lines : the horizontal line array
   - n_h_lines : number of horizontal lines
   - v_lines : the vertical line array
   - n_v_lines : number of vertical lines
*/
void create_outlines(Line **h_lines, int *n_h_lines, 
                     Line **v_lines, int *n_v_lines)
{
    //top
    if (abs((*v_lines)[0].begin.y - (*h_lines)[0].begin.y) > 5)
    {
        Line top_line = {{(*h_lines)[0].begin.x, (*v_lines)[0].begin.y},
                         {(*h_lines)[0].end.x, (*v_lines)[0].begin.y}};
        
        //putting 1st h_line to 1st pos
        Line new_h_l[*n_h_lines+1];
        new_h_l[0] = top_line;
        for (int row = 0; row < *n_h_lines; row++)
        {
            new_h_l[row+1] = (*h_lines)[row];
        }

        (*n_h_lines)++;
        *h_lines = realloc(*h_lines, *n_h_lines*sizeof(Line));
        memcpy(*h_lines, new_h_l, *n_h_lines*sizeof(Line));
    }

    //left
    if (abs((*v_lines)[0].begin.x - (*h_lines)[0].begin.x) > 5)
    {
        Line left_line = {{(*h_lines)[0].begin.x, (*v_lines)[0].begin.y},
                          {(*h_lines)[0].begin.x, (*v_lines)[0].end.y}};
        
        //putting 1st v_line to 1st pos
        Line new_v_l[*n_v_lines+1];
        new_v_l[0] = left_line;
        for (int col = 0; col < *n_v_lines; col++)
        {
            new_v_l[col+1] = (*v_lines)[col];
        }

        (*n_v_lines)++;
        *v_lines = realloc(*v_lines, *n_v_lines*sizeof(Line));
        memcpy(*v_lines, new_v_l, *n_v_lines*sizeof(Line));
    }

    //down
    if (abs((*h_lines)[*n_h_lines-1].end.y-(*v_lines)[*n_v_lines-1].end.y) > 5)
    {
        Line down_line = {{(*h_lines)[*n_h_lines-1].begin.x, 
                           (*v_lines)[*n_v_lines-1].end.y},
                          {(*h_lines)[*n_h_lines-1].end.x, 
                           (*v_lines)[*n_v_lines-1].end.y}};
        
        Line new_h_l[*n_h_lines+1];
        for (int row = 0; row < *n_h_lines; row++)
        {
            new_h_l[row] = (*h_lines)[row];
        }
        new_h_l[*n_h_lines] = down_line;

        (*n_h_lines)++;
        *h_lines = realloc(*h_lines, *n_h_lines*sizeof(Line));
        memcpy(*h_lines, new_h_l, *n_h_lines*sizeof(Line));
    }

    //right
    if (abs((*v_lines)[*n_v_lines-1].end.x-(*h_lines)[*n_h_lines-1].end.x) > 5)
    {
        Line right_line = {{(*h_lines)[*n_h_lines-1].end.x, 
                            (*v_lines)[*n_v_lines-1].begin.y},
                           {(*h_lines)[*n_h_lines-1].end.x, 
                            (*v_lines)[*n_v_lines-1].end.y}};
        
        Line new_v_l[*n_v_lines+1];
        for (int col = 0; col < *n_v_lines; col++)
        {
            new_v_l[col] = (*v_lines)[col];
        }
        new_v_l[*n_v_lines] = right_line;

        (*n_v_lines)++;
        *v_lines = realloc(*v_lines, *n_v_lines*sizeof(Line));
        memcpy(*v_lines, new_v_l, *n_v_lines*sizeof(Line));
    }
}



/*
    search_for_grid()
  Detects and returns each line of the letter grid (if existing) as a 'Line' 
  structure (contains start coords (x1, y1) and end coords (x2, y2)).
  Params:
   - pixbuf: the pixel buffer of the image from which data will be extracted.
  Return Value: 
   - An array of Line elements which represent all the lines of the grid (if
     it exists). 
      - [0]: the horizontal lines.
      - [1]: the vertical lines.
   - NULL if the grid does not exist.
*/
Line** search_for_grid(GdkPixbuf *pixbuf, int *nb_h_lines, int *nb_v_lines)
{
    //Creating an array for all lines
    Line **lines = malloc(2*sizeof(Line*));

    //array for horizontal lines
    int n_h_lines = 0;
    Line *h_lines = horizontal_search(pixbuf, &n_h_lines);

    //if no horizontal lines, we stop here : no grid.
    if (h_lines == NULL) 
    {
        return NULL;
    }
    //Else, the grid exists. But we still have to know the position of each
    //vertical grid line.

    //array for vertical lines
    int n_v_lines = 0;
    Line *v_lines = vertical_search(pixbuf, &n_v_lines);

    //create outlines if necessary
    create_outlines(&h_lines, &n_h_lines, &v_lines, &n_v_lines);

    //fill the 'lines' array
    lines[0] = h_lines;
    lines[1] = v_lines;

    *nb_h_lines = n_h_lines;
    *nb_v_lines = n_v_lines;

    return lines;
}



/*
    cut_from_pixbuf()
  Creates and returns a new subpixbuf from the original pixbuf and a BoundingBox
  representing the positions of the element to cut.
  Params:
   - original : the original pixbuf.
   - bounds : the bounds of the element to cut.
  Return Value: a new subpixbuf of the element we had to cut.
*/
GdkPixbuf* cut_from_pixbuf(GdkPixbuf *original, BoundingBox bounds)
{
    int sub_w = bounds.bottom_right.x - bounds.top_left.x;
    int sub_h = bounds.bottom_right.y - bounds.top_left.y;
    GdkPixbuf *subpix = gdk_pixbuf_new_subpixbuf(original, //Source
            bounds.top_left.x, bounds.top_left.y,//Top_left pos
            sub_w, sub_h); //Top_right pos relative to top_left pos

    //Create copy bc "subpix" shares data with "original"
    GdkPixbuf *res = gdk_pixbuf_copy(subpix);
    return res;
}



/*
    save_boundingBox_as_pixbuf()
  Saves a BoundingBox as a pixbuf from the original pixbuf into a determined
  folder with a determined name. File format is "png".
  Params:
   - pixbuf : the original pixbuf.
   - box : the BoundingBox to save.
   - path_flag : This flag determines the destination folder of the BoundingBox.
      - "GRID" => path = "./src/Extraction/extracted_images/grid/"
      - "WORDLIST" => path = "./src/Extraction/extracted_images/wordlist/"
   - filename : the name of the saved file (without the extension).
*/
void save_boundingBox_as_pixbuf(GdkPixbuf *pixbuf, BoundingBox box,
                                char* path_flag, char* filename)
{
    if (strcmp(path_flag, "GRID") == 0) //"GRID"
    {
        char path[44 + strlen(filename)];  //44 = len(path) + len(.png) + \0
        strcpy(path, "./src/Extraction/extracted_images/grid/");
        strcat(path, filename);
        strcat(path, ".png");

        GdkPixbuf* box_pixbuf = cut_from_pixbuf(pixbuf, box);
        gdk_pixbuf_save(box_pixbuf, path, "png", NULL, NULL);
    }
    else //"WORDLIST"
    {
        char path[48 + strlen(filename)];  //48 = len(path) + len(.png) + \0
        strcpy(path, "./src/Extraction/extracted_images/wordlist/");
        strcat(path, filename);
        strcat(path, ".png");

        GdkPixbuf* box_pixbuf = cut_from_pixbuf(pixbuf, box);
        gdk_pixbuf_save(box_pixbuf, path, "png", NULL, NULL);
    }
}



/*
    save_grid_pixbuf()
  Saves the BoundingBox of the grid as a pixbuf from the original pixbuf into
  the path "./src/Extraction/extracted_images/grid/grid.png".
  Params:
   - pixbuf: the original pixbuf.
   - lines: the lines of the word search grid
      - [0]: the horizontal lines.
      - [1]: the vertical lines.
*/
void save_grid_pixbuf(GdkPixbuf *pixbuf, Line **lines)
{
    //1 - Take the grid boundaries
    //Take the first horizontal line's 'x' beginning value
    int x_begin = lines[0][0].begin.x;

    //Take the first vertical line's 'y' beginning value
    int y_begin = lines[1][0].begin.y;

    //Take the first horizontal line's 'x' ending value
    int x_end = lines[0][0].end.x;

    //Take the first vertical line's 'y' ending value
    int y_end = lines[1][0].end.y;

    //Now you have the grid boundaries !
    BoundingBox grid = {{x_begin, y_begin}, {x_end, y_end}};
    
    //2 - Save the grid pixbuf
    save_boundingBox_as_pixbuf(pixbuf, grid, "GRID", "grid");
}



/*
    save_grid_letters_pixbuf()
  Saves the BoundingBox of each letter as a pixbuf from the original pixbuf into
  a determined folder as 'letter_X_Y'. File will be saved as a .png.
  Params:
   - pixbuf: the original pixbuf.
   - lines : the lines of the word search grid
      - [0]: the horizontal lines.
      - [1]: the vertical lines.
   - n_h_lines : number of horizontal lines
   - n_v_lines : number of vertical lines
*/
void save_grid_letters_pixbuf(GdkPixbuf *pixbuf, Line **lines,
                              int n_h_lines, int n_v_lines)
{
    for (int row = 0; row < n_h_lines-1; row++)
    {
        for (int col = 0; col < n_v_lines-1; col++)
        {
            //finding position
            BoundingBox letter = {{lines[1][col].begin.x, 
                                   lines[0][row].begin.y},
                                  {lines[1][col+1].end.x,
                                   lines[0][row+1].end.y}};
            
            /*
            printf("box = (%d, %d) to (%d, %d)\n", letter.top_left.x, 
                                                   letter.top_left.y,
                                                   letter.bottom_right.x,
                                                   letter.bottom_right.y);
            */

            int size_x;
            int size_y;

            if (col/10 > 0)
                size_x = 3;
            else
                size_x = 2;

            if (row/10 > 0)
                size_y = 3;
            else
                size_y = 2;

            //saving position as an image
            char posx[size_x];
            snprintf(posx, sizeof(posx), "%d", col);
            char posy[size_y];
            snprintf(posy, sizeof(posy), "%d", row);
            
            char filename[9+(size_x-1)+(size_y-1)]; //9 = len(letter_)+len(_)+\0
            strcpy(filename, "letter_");
            strcat(filename, posx);
            strcat(filename, "_");
            strcat(filename, posy);

            save_boundingBox_as_pixbuf(pixbuf, letter, "GRID", filename);
        }
    }
}



/*
    extraction()
  Detects and saves the position of the grid, the word list and 
  all the letters in both the grid and the word list.
  Params:
   - pixbuf: the pixel buffer of image from which data will be extracted
*/
void extraction(GdkPixbuf *pixbuf)
{
    //Steps to locate the grid, the word list, the letters in the grid,
    //the words in the list and the letters in the words

    //1 - Search for the grid
    int n_h_lines = 0;
    int n_v_lines = 0;
    Line **lines = search_for_grid(pixbuf, &n_h_lines, &n_v_lines);

    if (lines == NULL)  //2 - If it does not exists :
    {
        //to be defined
        printf("GRID DOES NOT EXIST !\n");
    }
    else
    {
        //Now we have all the rows and columns' boundaries of the grid
        //in an array.

        //3 - Locate, cut and save grid_pos' BoundingBox
        save_grid_pixbuf(pixbuf, lines);

        //4 - Locate, cut and save each grid letter's BoundingBox
        save_grid_letters_pixbuf(pixbuf, lines, n_h_lines, n_v_lines);

        //Now the word list should be in the image and outside the grid
        //5 - Locate the top left and bottom right of the word list
        //6 - Locate Locate the top left and bottom right of each letter of
        //    the word list
    }

    //Do not forget to free the 'lines' array and the 'lines[0]' and 'lines[1]'
    //array
    free(lines[0]);
    free(lines[1]);
    free(lines);
}



int main()
{
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(
                        "./examples/level_1_image_1.png", NULL);
    extraction(pixbuf);
    return 0;
}




                                                                            //80

//---------------------------------//
//   USELESS FUNCTIONS (FOR NOW)   //
//---------------------------------//



/*
    This is a DEBUG function to show the positions of all the letters in the
    word search grid / word list.
    Params:
     - **letter_struct : a 2D-array of letters representing the grid or list.
*/
/*
void display_letter_pos(int width, int height, 
                        BoundingBox letter_struct[height][width])
{
    for (int x = 0; x < height-1; x++)
    {
        for (int y = 0; y < width-1; y++)
        {
            printf("BOUND_BOX at POS(%d, %d) => (%d, %d) to (%d, %d)\n", x, y, 
                   letter_struct[x][y].top_left.x, 
                   letter_struct[x][y].top_left.y,
                   letter_struct[x][y].bottom_right.x, 
                   letter_struct[x][y].bottom_right.y);
        }

        printf("\n");
    }
}
*/

/*
//THIS FUNCTION IS NOT WORKING
void display_lines(GdkPixbuf *src, GdkPixbuf *dst, Line *lines, int max_lines)
{
    int n_channels = gdk_pixbuf_get_n_channels(src);
    int rowstride = gdk_pixbuf_get_rowstride(src);
    guchar *pixels = gdk_pixbuf_get_pixels(dst);

    for (int i = 0; i < max_lines; i++)
    {
        guchar *pixel1 = pixels + lines[i].y1 * rowstride
                       + lines[i].x1 * n_channels;
        
        pixel1[0] = 255;
        pixel1[1] = 0;
        pixel1[2] = 0;
        if (n_channels == 4)
        {
            pixel1[3] = 255;
        }

        guchar *pixel2 = pixels + lines[i].y2 * rowstride
                       + lines[i].x2 * n_channels;
        
        pixel2[0] = 0;
        pixel2[1] = 0;
        pixel2[2] = 255;
        if (n_channels == 4)
        {
            pixel2[3] = 255;
        }
        
        i++;
    }
}
*/

/*
// Aux. function of sobel_edge_detection() and hough_transform():
// Calculates & returns grayscale intensity from RGB values of pixel
int calculate_intensity(guchar r, guchar g, guchar b)
{
    return (int)(0.3 * r + 0.59 * g + 0.11 * b); // Common grayscale weights
}
*/

/*
    sobel_edge_detection()
  Detect the edges (the major shapes) of the pixbuf
  and modifies the new pixbuf in place.
  Params:
   - pixbuf: the pixbuf from the initial image (before the call).
   - pixbuf: a pixbuf in which only the strongest edges remains 
     (after the call).
  No Return Value.

void sobel_edge_detection(GdkPixbuf *src, GdkPixbuf *dst)
{
    //0 - Get the width, height, rowstride and pixels of the pixbuf.
    int width = gdk_pixbuf_get_width(src);
    int height = gdk_pixbuf_get_height(src);
    int rowstride = gdk_pixbuf_get_rowstride(src);
    int n_channels = gdk_pixbuf_get_n_channels(src);
    guchar *pixels = gdk_pixbuf_get_pixels(src);
    guchar *new_pixels = gdk_pixbuf_get_pixels(dst);

    //1 - Sobel Kernels
    int sobel_x[3][3] = 
    {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    int sobel_y[3][3] = 
    {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}
    };
                                                                             
    //2 - Iterate over each pixel (excluding borders)
    //and calculate gradients in all directions with the 
    //neighboring pixels
    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            int gx = 0, gy = 0;

            // Apply Sobel x and y kernels
            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    int p_x = x + j;
                    int p_y = y + i;

                    guchar *pix = pixels + p_y * rowstride + p_x * n_channels;

                    //Calculate intensity of pixel depending on its
                    //RGB Values
                    int intensity = calculate_intensity(pix[0], pix[1], pix[2]);

                    gx += intensity * sobel_x[i + 1][j + 1];
                    gy += intensity * sobel_y[i + 1][j + 1];
                }
            }

            // Calculate gradient magnitude
            int gradient_magnitude = sqrt(gx * gx + gy * gy);

            // Determine the new pixel location in the output buffer
            guchar *new_pixel = new_pixels + y * rowstride + x * n_channels;

            // Threshold the magnitude to detect significant edges
            if (gradient_magnitude > SOBEL_THRESHOLD)
            {
                new_pixel[0] = 255;  //edge
                new_pixel[1] = 255;
                new_pixel[2] = 255;

                if (n_channels == 4)  //contains alpha
                {
                    new_pixel[3] = 255;
                }
            } 
            else
            {
                new_pixel[0] = 0;  //not an edge
                new_pixel[1] = 0;
                new_pixel[2] = 0;

                if (n_channels == 4)  //contains alpha
                {
                    new_pixel[3] = 255;
                }
            }
        }
    }
}
*/

/*
//THIS FUNCTION IS NOT WORKING
void hough_transform(GdkPixbuf *pixbuf, Line *lines, int *max_lines)
{
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    printf("width = %d\n", width);
    printf("height = %d\n", height);
    printf("\n");

    // Define the range of theta and the max length of line
    int max_line = (int) sqrt(width * width + height * height); //diagonal line
    int rho_range = 2 * max_rho;
    int theta_range = 180;

    // Allocate the accumulator
    int *accumulator = calloc(rho_range * theta_range, sizeof(int));

    // Populate the accumulator
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            guchar *pixel = pixels + y * rowstride + x * n_channels;

            int intensity = calculate_intensity(pixel[0], pixel[1], pixel[2]);
            if (intensity > 0) // if pixel is black (then we're in an edge)
            {
                for (int theta = 0; theta < theta_range; theta++)
                {
                    double theta_rad = theta * DEG_TO_RAD;
                    int rho = (int)(x*cos(theta_rad)+y*sin(theta_rad))+max_rho;
                    if (rho >= 0 && rho < rho_range)
                    {
                        accumulator[rho_range * theta + rho]++;
                    }
                }
            }
        }
    }

    //Extract lines by finding high number of votes 
    //in the accumulator
    int line_count = 0;
    for (int theta = 0; theta < theta_range; theta++)
    {
        for (int rho = 0; rho < rho_range; rho++)
        {
            int votes = accumulator[rho_range * theta + rho];
            if (votes > HOUGH_THRESHOLD)
            {
                if (line_count < *max_lines)
                {
                    double theta_rad = theta * M_PI / 180.0; //Convert to rad
                    int adjusted_rho = rho - max_rho; //To original rho value

                    // Calculate boundaries of detected line
                    int x1 = (int)(adjusted_rho * cos(theta_rad)
                           - sin(theta_rad) * height);
                    int y1 = (int)(adjusted_rho * sin(theta_rad)
                           + cos(theta_rad) * height);
                    int x2 = (int)(adjusted_rho * cos(theta_rad)
                           + sin(theta_rad) * width);
                    int y2 = (int)(adjusted_rho * sin(theta_rad)
                           - cos(theta_rad) * width);

                    // Store the line endpoints
                    lines[line_count].x1 = x1;
                    lines[line_count].y1 = y1;
                    lines[line_count].x2 = x2;
                    lines[line_count].y2 = y2;

                    line_count++;
                }
            }
        }
    }

    if (line_count < *max_lines)
    {
        lines = realloc(lines, line_count*sizeof(Line));
        *max_lines = line_count;
        printf("max_lines = %d\n", *max_lines);
    }

    // Cleanup
    free(accumulator);
}
*/

/*
int main()   //FORMER MAIN
{
    
    //SOBEL IS DONE !!!
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("./examples/filtered1.png",
                                                 NULL);
    GdkPixbuf *h_t_pixbuf = extraction(pixbuf);
    //gdk_pixbuf_save(h_t_pixbuf, "./examples/hough1.png", "png", NULL, NULL);

    return 0;
    
}
*/