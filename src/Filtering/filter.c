#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void grayscale_image(const char *input_filename, const char *output_filename, int threshold, int method) {
    // Load the image from a file
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(input_filename, NULL);
    if (!pixbuf) {
        fprintf(stderr, "Failed to load image\n");
        return;
    }

    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int channels = gdk_pixbuf_get_n_channels(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    // Iterate through pixels and convert them to grayscale
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            guchar *pixel = pixels + y * rowstride + x * channels;
            guchar r = pixel[0];
            guchar g = pixel[1];
            guchar b = pixel[2];
            guchar gray = (guchar)(0.3 * r + 0.59 * g + 0.11 * b);

            // Apply threshold
            if (gray > threshold) {
                gray = 255;
            } else {
                gray = 0;
            }

            pixel[0] = gray;
            pixel[1] = gray;
            pixel[2] = gray;
        }
    }

    // Save the grayscale image
    gdk_pixbuf_save(pixbuf, output_filename, "png", NULL, NULL);
    g_object_unref(pixbuf);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <input_image> <output_image> <threshold> <method>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_filename = argv[1];
    const char *output_filename = argv[2];
    int threshold = atoi(argv[3]);
    int method = atoi(argv[4]);

    gtk_init(&argc, &argv);

    grayscale_image(input_filename, output_filename, threshold, method);

    printf("Image preprocessing complete. Output saved to '%s'.\n", output_filename);
    return EXIT_SUCCESS;
}
