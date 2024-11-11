#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void get_info_from_pixbuf(GdkPixbuf *input, int *width, int *height,
        int *rowstride, int *channels, guchar **pixels) {
    *width = gdk_pixbuf_get_width(input);
    *height = gdk_pixbuf_get_height(input);
    *rowstride = gdk_pixbuf_get_rowstride(input);
    *channels = gdk_pixbuf_get_n_channels(input);
    *pixels = gdk_pixbuf_get_pixels(input);
}

void *pixbuf_to_grayscale(GdkPixbuf *input) {
    //Get input information
    int width, height, rowstride, N;
    guchar *pixels;
    get_info_from_pixbuf(input, &width, &height, &rowstride, &N, &pixels);

    //Iterating over the pixels of pixbuf
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            guchar *p = pixels + y * rowstride + x*N;
            //The RGB values
            guchar r = p[0];guchar g = p[1];guchar b = p[2];
            //Gray value
            guchar gray = (guchar)(0.3 * r + 0.59 * g + 0.11 * b);
            //Apply
            p[0] = gray;p[1] = gray;p[2] = gray;
        }
    }
}


void enhance_contrast(GdkPixbuf *input) {
    //Get input information
    int width, height, rowstride, N;
    guchar *pixels;
    get_info_from_pixbuf(input, &width, &height, &rowstride, &N, &pixels);

    unsigned char min = 255;
    unsigned char max = 0;
    //First iteration: find min-max RANGE
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            guchar *p = pixels + y*rowstride + x*N;
            //Assuming the image is already in grayscale (p[0]=p[1]=p[2])
            if (p[0] < min) min = p[0];
            if (p[0] > max) max = p[0];
        }
    }

    //Second iteration: apply the contrast range
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            guchar *p = pixels + y*rowstride + x*N;
            int contrasted = (p[0]-min)*255/(max-min);
            //Applying
            p[0]=contrasted;
            p[1]=contrasted;
            p[2]=contrasted;
        }
    }
}


void pixbuf_denoise(GdkPixbuf *input) {
    //Get input information
    int width, height, rowstride, N;
    guchar *pixels;
    get_info_from_pixbuf(input, &width, &height, &rowstride, &N, &pixels);

    //Iterating
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            
        }
    }
}


float calcMean(guchar *pixels, int width, int rowstride, int x, int y, int N) {
    const int WINDOW_SIZE = 15;
    int half_wind = WINDOW_SIZE/2;
    float sum = 0;
    int count = 0;
    //Iterating
    for (int dy = -half_wind; dy <= half_wind; dy++) {
        for (int dx = -half_wind; dx <= half_wind; dx++) {
            int nx = x + dx;
            int ny = y + dy;
            if (nx < 0 || nx >= width || ny < 0 || ny >= width)
                continue;
            guchar *p = pixels + ny * rowstride + nx * N;
            sum += p[0];
            count++;
        }
    }
    return sum/count;
}


float calcStdDev(unsigned char *pixels, int width, int rowstride,
        int x, int y, int N, float mean) {
    const int WINDOW_SIZE = 15;
    int half_wind = WINDOW_SIZE / 2;
    float sum = 0;
    int count = 0;

    //Iteration
    for (int dy = -half_wind; dy <= half_wind; dy++) {
        for (int dx = -half_wind; dx <= half_wind; dx++) {
            int nx = x + dx;
            int ny = y + dy;
            if (nx < 0 || nx >= width || ny < 0 || ny >= width)
                continue;
            guchar *p = pixels+ ny * rowstride + nx * N;
            sum += (p[0]- mean) * (p[0]-mean);
            count++;
        }
    }
    return sqrt(sum/count);
}


void sauvola_threshold(GdkPixbuf *input) {
    const float K = 0.5f;
    const unsigned char R = 128;
    //Get input information
    int width, height, rowstride, N;
    guchar *pixels;
    get_info_from_pixbuf(input, &width, &height, &rowstride, &N, &pixels);
    GdkPixbuf *copy = gdk_pixbuf_copy(input);
    guchar *pixels_t = gdk_pixbuf_get_pixels(copy);

    //Iterating
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            guchar *p_t = pixels_t + y*rowstride + x*N;
            guchar *p = pixels + y*rowstride +x*N;
            float mean = calcMean(pixels, width, rowstride, x, y, N);
            float std = calcStdDev(pixels,width,rowstride, x, y, N, mean);
            float threshold = mean * (1+K*(std/R-1));

            if (p_t[0] > threshold) {
                p[0]=p[1]=p[2]=255;
            } else {
                p[0]=p[1]=p[2]=0;
            }
        }
    }
    g_object_unref(copy);
}

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


void Filter_Run(GdkPixbuf *input, int steps) {
    pixbuf_to_grayscale(input);
    if (steps > 1 && steps < 3)
        enhance_contrast(input);
    if (steps > 2)
        binary_threshold(input,170);
}
