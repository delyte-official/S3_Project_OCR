#include "filter.h"

int main(int argc, char* argv[]) {
    if (argc != 2)
        return EXIT_FAILURE;
    GdkPixbuf *load = gdk_pixbuf_new_from_file(argv[1], NULL);
    GdkPixbuf *load1 = gdk_pixbuf_new_from_file(argv[1],NULL);
    Filter_Run(load1,1);
    GdkPixbuf *load2 = gdk_pixbuf_new_from_file(argv[1],NULL);
    Filter_Run(load2,2);
    GdkPixbuf *load3 = gdk_pixbuf_new_from_file(argv[1],NULL);
    Filter_Run(load3,3);
    GdkPixbuf *load4 = gdk_pixbuf_new_from_file(argv[1],NULL);
    Filter_Run(load4,4);
    gdk_pixbuf_save(load,"s_ORIGINAL.png", "png", NULL, NULL);
    gdk_pixbuf_save(load1,"s_GRAYSCALE.png", "png", NULL, NULL);
    gdk_pixbuf_save(load2,"s_GRAY_ENHANCE.png", "png", NULL, NULL);
    gdk_pixbuf_save(load3,"s_ALL_IN_ONE.png", "png", NULL, NULL);
    gdk_pixbuf_save(load4,"s_GRAY_BINARY.png", "png", NULL, NULL);
}
