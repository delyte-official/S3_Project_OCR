
#include <gtk/gtk.h>
#include <stdlib.h>
#include <math.h>
#include "TRAIN.h"
#include "data.h"

double* LoadImageAsMatrix(const char* filename) {
    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, &error);
    if (!pixbuf) {
        g_error_free(error);
        return NULL;
    }
    
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    g_assert(width == 28 && height == 28);

    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    guchar* pixels = gdk_pixbuf_get_pixels(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);

    double* matrix = malloc(width * height * sizeof(double));
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            guchar* p = pixels + y * rowstride + x * n_channels;
            guchar r = p[0];
            guchar g = p[1];
            guchar b = p[2];
            matrix[y * width + x] = r==0?1:0;
        }
    }

    g_object_unref(pixbuf);
    return matrix;
}





// Debug Functions


void PrintMatrix(double* matrix) { 
    for (int i = 0; i < 28; i++) 
    { 
        for (int j = 0; j < 28; j++) 
        { 
            printf("%i", (int)matrix[i * 28 + j]); 
        } 
        printf("\n"); 
    } 
}







void PredictCharacter(struct Neural_Network *net, const char* filename) {
    double* inputMatrix = LoadImageAsMatrix(filename);
    if (!inputMatrix) {
        printf("Failed to load image\n");
        return;
    }
    
    PrintMatrix(inputMatrix);

    // Initialiser le réseau
    net = RecupData();

    // Passer l'image à travers le réseau
    Neural_Network_OCR(net, inputMatrix, NULL);

    // Récupérer la prédiction
    int predicted_pos = RetrievePos(net);
    char predicted_char = RetrieveChar(predicted_pos);

    printf("Predicted Character: %c\n", predicted_char);

    free(inputMatrix);
}


void get_info_from_pixbuf(GdkPixbuf *input, int *width, int *height,
        int *rowstride, int *channels, guchar **pixels) {
    *width = gdk_pixbuf_get_width(input);
    *height = gdk_pixbuf_get_height(input);
    *rowstride = gdk_pixbuf_get_rowstride(input);
    *channels = gdk_pixbuf_get_n_channels(input);
    *pixels = gdk_pixbuf_get_pixels(input);
}

void pixbuf_to_grayscale(GdkPixbuf *input) {
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

GdkPixbuf *convertToBase28(GdkPixbuf *input) {
    int width = gdk_pixbuf_get_width(input);
    int height = gdk_pixbuf_get_height(input);
    double ratio_w = 28 / (double)width;
    double ratio_h = 28 / (double)height;
    double ratio =ratio_w<=ratio_h ? ratio_w:ratio_h;
    int new_w = width * ratio;
    int new_h = height * ratio;
    GdkPixbuf *resized = gdk_pixbuf_scale_simple(input,new_w,new_h,
            GDK_INTERP_HYPER);
    gdk_pixbuf_save(resized,"intermediate.png","png",NULL,NULL);
    GdkPixbuf *rewrite = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 28, 28);
    gdk_pixbuf_fill(rewrite,0xFFFFFFFF);
    int offset_x = (28-new_w)/2;
    int offset_y = (28-new_h)/2;
    gdk_pixbuf_composite(resized, rewrite, offset_x, offset_y, new_w, new_h,
            offset_x, offset_y, 1.0, 1.0, GDK_INTERP_HYPER,255);
    pixbuf_to_grayscale(rewrite);
    binary_threshold(rewrite,150);
    return rewrite;
}


int main(int argc, char *argv[]) 
{    

    gtk_init(&argc, &argv);

    // Entraîner le réseau de neurones
  //  printf("Entraînement du réseau de neurones...\n");
    if(atoi(argv[1]) == 1)
        trainNN();

    // Charger le réseau de neurones entraîné
    struct Neural_Network *net = RecupData();
    if (!net) {
        fprintf(stderr, "Erreur lors de la récupération des données du réseau\n");
        return EXIT_FAILURE;
    }
    // Prédire une lettre à partir d'une image 
    printf("Prédiction à partir d'une image...\n"); 
    
    
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("X4.png",NULL);
    GdkPixbuf *resized = convertToBase28(pixbuf);
      
    gdk_pixbuf_save(resized,"X4s.png","png",NULL,NULL); 
    
    PredictCharacter(net, "X4s.png"); 
    // Libérer la mémoire allouée pour le réseau 
    free(net); 
    return 0;
  
}


