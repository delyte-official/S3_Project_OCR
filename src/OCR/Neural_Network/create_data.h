#include <gtk/gtk.h>
#include <stdio.h>

void get_info_from_pixbuf(GdkPixbuf *input, int *width, int *height,
        int *rowstride, int *channels, guchar **pixels);

void pixbuf_to_grayscale(GdkPixbuf *input); 

void binary_threshold(GdkPixbuf *input, int threshold); 


GdkPixbuf *convertToBase28(GdkPixbuf *input); 

double * getMatrix(const char* filename);

// Fonction pour écrire une matrice 28x28 dans un fichier texte
void WriteBinaryMatrixToFile(double* matrix, const char* filename); 

