#define _GNU_SOURCE
#include "../Core_Manager.h"
#include "../Extract/Extract.h"
#include "../Filter/Filter.h"
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







char PredictCharacter(struct Neural_Network *net, const char* filename) {
    double* inputMatrix = LoadImageAsMatrix(filename);
    if (!inputMatrix) {
        printf("Failed to load image\n");
        return '\0';
    }
    net = RecupData();
    Neural_Network_OCR(net, inputMatrix, NULL);
    int predicted_pos = RetrievePos(net);
    char predicted_char = RetrieveChar(predicted_pos);

    free(inputMatrix);
    return predicted_char;
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


char read_image(char *filename) 
{    
    /*/Entraîner le réseau de neurones
    //printf("Entraînement du réseau de neurones...\n");
    if(atoi(argv[1]) == 1)
        trainNN();*/

    //Charger le réseau de neurones entraîné
    struct Neural_Network *net = RecupData();
    if (!net) {
        fprintf(stderr, "Erreur lors de la récupération des données du réseau\n");
        return EXIT_FAILURE;
    }
    // Prédire une lettre à partir d'une image 
    printf("Prédiction à partir d'une image...\n"); 
    
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename,NULL);
    GdkPixbuf *resized = convertToBase28(pixbuf);
      
    gdk_pixbuf_save(resized,"src/bin/input_ocr.png","png",NULL,NULL); 
    
    char res =PredictCharacter(net, "src/bin/input_ocr.png"); 
    // Libérer la mémoire allouée pour le réseau 
    free(net);
    return res;
}

void append(GtkTextBuffer *buffer, const char *text) {
    GtkTextIter end_iter;
    gtk_text_buffer_get_end_iter(buffer,&end_iter);
    gtk_text_buffer_insert(buffer,&end_iter,text,-1);
}

int Identify_Characters(Size sizeG, int count_words, Cluster ***wordlist) {
    GtkBuilder *builder = gtk_builder_new_from_file(
            "src/assets/ocr_tile.glade");
    GtkWidget *widget = GTK_WIDGET(gtk_builder_get_object(builder,"toplvl"));
    //GRID
    GtkTextBuffer *buffer_g = GTK_TEXT_BUFFER(gtk_builder_get_object(builder,
                "grid"));
    g_object_set_data(G_OBJECT(widget),"grid_buffer",buffer_g);
    gtk_text_buffer_set_text(buffer_g,"",-1);
    //WORDLIST
    GtkTextBuffer *buffer_w = GTK_TEXT_BUFFER(gtk_builder_get_object(
                builder, "wordlist"));
    g_object_set_data(G_OBJECT(widget),"wordlist_buffer",buffer_w);
    gtk_text_buffer_set_text(buffer_w,"",-1);
    //ITERATE GRID
    printf("BEFORE ITERATION\n");
    char *filename;
    for (int x = 0; x < sizeG.rows; x++) {
        for (int y = 0; y < sizeG.cols; y++) {
            asprintf(&filename,"src/bin/grid_%d_%d.png",x,y);
            char ltr = read_image(filename);
            char *txt;
            asprintf(&txt,"%c",ltr);
            append(buffer_g,txt);
        }
        const char* end = "\n";
        append(buffer_g,end);
    }
    printf("second iteration\n");
    //ITERATE WORDLIST
    for (int i = 0; i < count_words; i++) {
        for (int j = 0; wordlist[i][j]!=NULL;j++) {
            asprintf(&filename,"src/bin/words_%d_ltr_%d.png",i,j);
            char ltr2 = read_image(filename);
            char *txt2;
            asprintf(&txt2,"%c",ltr2);
            append(buffer_w,txt2);
        }
        const char* end2 = "\n";
        append(buffer_w,end2);
    }
    //Save results
    g_object_set_data(G_OBJECT(widget), "builder",builder);
    FREESTEPDATA(STEP_OCR);
    SETSTEPDATA(STEP_OCR,widget);
    return 1;
}
