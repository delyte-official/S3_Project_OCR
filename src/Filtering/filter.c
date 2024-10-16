// filter.c - Image processing using GTK/GDK with Sauvola's thresholding
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Function to calculate the local mean and standard deviation for Sauvola's method
void calculate_local_mean_and_std(guchar *pixels, int width, int height, int rowstride, int n_channels, int window_size, guchar *mean, guchar *std) {
    int half_window = window_size / 2;

    // Iterate through each pixel in the image
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int sum = 0;
            int sum_sq = 0;
            int count = 0;

            // Compute the sum and squared sum of pixel values in the local window
            for (int wy = -half_window; wy <= half_window; wy++) {
                for (int wx = -half_window; wx <= half_window; wx++) {
                    int nx = x + wx;
                    int ny = y + wy;

                    // Ensure the window doesn't go out of bounds
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        guchar *p = pixels + ny * rowstride + nx * n_channels;
                        guchar r = p[0];
                        guchar g = p[1];
                        guchar b = p[2];

                        // Calculate the grayscale value for the pixel
                        guchar gray = (guchar)(0.299 * r + 0.587 * g + 0.114 * b);

                        sum += gray;
                        sum_sq += gray * gray;
                        count++;
                    }
                }
            }

            // Calculate the mean and standard deviation
            guchar local_mean = sum / count;
            guchar local_std = (guchar)sqrt((sum_sq - (sum * sum) / count) / count);

            // Store the calculated values
            mean[y * width + x] = local_mean;
            std[y * width + x] = local_std;
        }
    }
}

// Function to apply Sauvola's thresholding
void sauvola_thresholding(guchar *pixels, int width, int height, int rowstride, int n_channels, int window_size, double k) {
    // Allocate memory for local mean and standard deviation
    guchar *mean = malloc(width * height * sizeof(guchar));
    guchar *std = malloc(width * height * sizeof(guchar));
    if (!mean || !std) {
        fprintf(stderr, "Memory allocation failed.\n");
        return;
    }

    // Calculate local mean and standard deviation
    calculate_local_mean_and_std(pixels, width, height, rowstride, n_channels, window_size, mean, std);

    // Dynamic range for standard deviation (typically 128 for 8-bit images)
    double R = 128.0;

    // Apply Sauvola's thresholding
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Get the local mean and standard deviation for the pixel
            guchar local_mean = mean[y * width + x];
            guchar local_std = std[y * width + x];

            // Calculate the threshold using Sauvola's formula
            double threshold = local_mean * (1 + k * (local_std / R - 1));

            // Get the original pixel's grayscale value
            guchar *p = pixels + y * rowstride + x * n_channels;
            guchar r = p[0];
            guchar g = p[1];
            guchar b = p[2];
            guchar gray = (guchar)(0.299 * r + 0.587 * g + 0.114 * b);

            // Apply the threshold to convert to black or white
            guchar bw = gray < threshold ? 0 : 255;

            // Set the new RGB values to either black or white
            p[0] = bw;
            p[1] = bw;
            p[2] = bw;
        }
    }

    // Free allocated memory
    free(mean);
    free(std);
}

GdkPixbuf *get_pixbuf(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Check if the user provided the correct number of arguments
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <image_path> <window_size> <k_value>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Parse the command-line arguments
    const char *image_path = argv[1];
    int window_size = atoi(argv[2]);
    double k = atof(argv[3]);

    // Validate the input parameters
    if (window_size <= 0) {
        fprintf(stderr, "Window size must be a positive integer.\n");
        return EXIT_FAILURE;
    }
    if (k < 0.0 || k > 1.0) {
        fprintf(stderr, "k value must be between 0.0 and 1.0.\n");
        return EXIT_FAILURE;
    }

    // Load the image using GdkPixbuf from the specified file path
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_path, NULL);
    if (!pixbuf) {
        fprintf(stderr, "Error loading image: %s\n", image_path);
        return EXIT_FAILURE;
    }

    // Get image dimensions and pixel data
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    // Apply Sauvola's thresholding with the specified parameters
    sauvola_thresholding(pixels, width, height, rowstride, n_channels, window_size, k);

    return pixbuf;
}

int main(int argc, char *argv[]) {
    // Get the processed pixbuf using Sauvola's thresholding
    GdkPixbuf *pixbuf = get_pixbuf(argc, argv);

    if (pixbuf == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    // Save the modified image to a file named "output.png"
    gdk_pixbuf_save(pixbuf, "output.png", "png", NULL, "quality", "100", NULL);

    // Free the GdkPixbuf object
    g_object_unref(pixbuf);

    printf("Processing complete. Output saved to output.png\n");

    return EXIT_SUCCESS;
}
