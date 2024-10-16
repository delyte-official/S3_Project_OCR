#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Function to convert the image to grayscale
void to_grayscale(guchar *pixels, int width, int height, int rowstride, int n_channels) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            guchar *p = pixels + y * rowstride + x * n_channels;
            guchar r = p[0];
            guchar g = p[1];
            guchar b = p[2];

            // Calculate grayscale value
            guchar gray = (guchar)(0.299 * r + 0.587 * g + 0.114 * b);

            // Set the RGB values to the grayscale value
            p[0] = gray;
            p[1] = gray;
            p[2] = gray;
        }
    }
}

// Function to apply a basic thresholding technique
void simple_threshold(guchar *pixels, int width, int height, int rowstride, int n_channels, int threshold) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            guchar *p = pixels + y * rowstride + x * n_channels;

            // Apply the threshold to convert to black or white
            guchar bw = p[0] < threshold ? 0 : 255;

            // Set the pixel to black or white
            p[0] = bw;
            p[1] = bw;
            p[2] = bw;
        }
    }
}

// Function to apply a morphological operation to clean up noise
void morphological_close(guchar *pixels, int width, int height, int rowstride, int n_channels, int kernel_size) {
    int half_kernel = kernel_size / 2;
    guchar *copy = malloc(width * height * n_channels);
    memcpy(copy, pixels, width * height * n_channels);

    // Perform dilation followed by erosion (closing)
    for (int y = half_kernel; y < height - half_kernel; y++) {
        for (int x = half_kernel; x < width - half_kernel; x++) {
            guchar max_val = 0;
            guchar min_val = 255;

            // Dilation (max value in neighborhood)
            for (int ky = -half_kernel; ky <= half_kernel; ky++) {
                for (int kx = -half_kernel; kx <= half_kernel; kx++) {
                    guchar *p = copy + (y + ky) * rowstride + (x + kx) * n_channels;
                    if (p[0] > max_val) {
                        max_val = p[0];
                    }
                }
            }

            // Set the pixel to the max value (dilation result)
            guchar *p_dilated = pixels + y * rowstride + x * n_channels;
            p_dilated[0] = max_val;
            p_dilated[1] = max_val;
            p_dilated[2] = max_val;
        }
    }

    // Copy the result of dilation to a new buffer for erosion
    memcpy(copy, pixels, width * height * n_channels);

    // Perform erosion (min value in neighborhood)
    for (int y = half_kernel; y < height - half_kernel; y++) {
        for (int x = half_kernel; x < width - half_kernel; x++) {
            guchar min_val = 255;

            // Erosion (min value in neighborhood)
            for (int ky = -half_kernel; ky <= half_kernel; ky++) {
                for (int kx = -half_kernel; kx <= half_kernel; kx++) {
                    guchar *p = copy + (y + ky) * rowstride + (x + kx) * n_channels;
                    if (p[0] < min_val) {
                        min_val = p[0];
                    }
                }
            }

            // Set the pixel to the min value (erosion result)
            guchar *p_eroded = pixels + y * rowstride + x * n_channels;
            p_eroded[0] = min_val;
            p_eroded[1] = min_val;
            p_eroded[2] = min_val;
        }
    }

    free(copy);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <image_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *image_path = argv[1];

    // Initialize GTK
    gtk_init(&argc, &argv);

    // Load the image using GdkPixbuf
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

    // Convert image to grayscale
    to_grayscale(pixels, width, height, rowstride, n_channels);

    // Apply simple thresholding
    int threshold_value = 128;  // Adjust as needed
    simple_threshold(pixels, width, height, rowstride, n_channels, threshold_value);

    // Apply morphological closing to clean up small noise
    int kernel_size = 3;  // Adjust as needed for better results
    morphological_close(pixels, width, height, rowstride, n_channels, kernel_size);

    // Save the processed image
    gdk_pixbuf_save(pixbuf, "processed_image.png", "png", NULL, "quality", "100", NULL);

    // Free the GdkPixbuf object
    g_object_unref(pixbuf);

    printf("Image preprocessing complete. Output saved to 'processed_image.png'.\n");
    return EXIT_SUCCESS;
}
