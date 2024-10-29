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

//SOBEL_THRESHOLD: Threshold to keep only the strongest edges
#define SOBEL_THRESHOLD = 125;

/*
    extraction()
  Detects and returns the position of 
  the grid and all the letters in it.
  Params:
   - image: the image from which data will be extracted
  Return value: Returns an array of size_t tuples.
   - [0]: grid position tuple (x, y)
   - [1]: array containing letters' position 
     [(x1, y1), (x2, y2), ...]
*/
size_t[] extraction(GtkWidget *image)
{
    //1 - pre-param (get pixbuf, width, height and raw pixels of image)
    
    gtk_init();
    GdkPixbuf *pixbuf = g_object_get_data(G_OBJECT(image), "pixbuf");
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    //2 - Detect rotation angle of the image using Edge detection 
    //and Hough's Transform

    //Detect edges with Sobel Edge detection
    sobel_edge_detection(pixbuf);
    
    //3 - Rotate image

    //4 - Detect grid
        //if there is a grid: segment it into cells and get pos
        //else: detect lines where there isn't (find aligned 
        //letters) then segment into cells, etc.

    //5 - Detect word list outside grid zone

    //post processing & return
}



/*
    sobel_edge_detection()
  Detect the edges (the major shapes) of the pixbuf
  and returns the new pixbuf.
  Params:
   - pixbuf: the pixbuf from the initial image
  Return Value: Returns a pixbuf with only the major shapes 
  of the image.
*/
GdkPixbuf* sobel_edge_detection(GdkPixbuf *pixbuf)
{
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

    //2 - Iterate over each pixel and calculate gradients in all
    //directions with the neighboring pixels
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
                    int pixel = get_pixel(image, x + j, y + i);
                    gx += pixel * sobel_x[i + 1][j + 1];
                    gy += pixel * sobel_y[i + 1][j + 1];
                }
            }

            // Calculate gradient magnitude
            int gradient_magnitude = sqrt(gx * gx + gy * gy);

            // Threshold the magnitude to detect significant edges
            if (gradient_magnitude > SOBEL_THRESHOLD)
            {
                set_pixel(image, x, y, 255);  //edge
            } 
            else
            {
                set_pixel(image, x, y, 0);  //not an edge
            }
        }
    }
}


int main()
{
    
}