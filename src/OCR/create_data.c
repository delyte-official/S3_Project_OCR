#include <gtk/gtk.h>
#include <stdio.h>

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


double * getMatrix(const char* filename)
{
    
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
    return matrix;
}


// Fonction pour écrire une matrice 28x28 dans un fichier texte
void WriteBinaryMatrixToFile(double* matrix, const char* filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier\n");
        return;
    }

    for (int i = 0; i < 28; i++) {
        for (int j = 0; j < 28; j++) {
            // Écrire directement les valeurs 0 ou 1
            int value = (int)matrix[i * 28 + j];
            fprintf(file, "%d", value);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}




int main() 
{
    
  char majs_path[17+9] = "../dataset3/majs/0/04.png\0";
  char mins_path[17+9] = "../dataset3/mins/0/03.png\0";
  char majs_save[17+9] = "../dataset3/majs/0/0S.png\0";
  char mins_save[17+9] = "../dataset3/mins/0/0S.png\0";
  char majs_txt[17+9] = "../dataset3/majs/0/04.txt\0";
  char mins_txt[17+9] = "../dataset3/mins/0/03.txt\0";
  char maj = 'A';
  char min = 'a';


  for(int i = 0; i < 52; i++)
  {
    if(i < 26 * 1)
    {
        majs_path[5+3+9] = maj;
        majs_path[7+3+9] = maj;
	    majs_save[5+3+9] = maj;
	    majs_save[7+3+9] = maj;
	    majs_txt[5+3+9] = maj;
	    majs_txt[7+3+9] = maj;

        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(majs_path,NULL);
        GdkPixbuf *resized = convertToBase28(pixbuf);
      
        gdk_pixbuf_save(resized,majs_save,"png",NULL,NULL); 
        WriteBinaryMatrixToFile(getMatrix(majs_save), majs_txt);
        
        maj++;
    }

    else if(i >= 26*1)
    {
        mins_path[5+3+9] = min;
        mins_path[7+3+9] = min;
        mins_save[5+3+9] = min;
        mins_save[7+3+9] = min;
        mins_txt[5+3+9] = min;
        mins_txt[7+3+9] = min;
      
      
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(mins_path,NULL);
        GdkPixbuf *resized = convertToBase28(pixbuf);
      
        gdk_pixbuf_save(resized,mins_save,"png",NULL,NULL); 
        WriteBinaryMatrixToFile(getMatrix(mins_save), mins_txt);
      
        min++;

    }
  }
   
  return 1;
}
