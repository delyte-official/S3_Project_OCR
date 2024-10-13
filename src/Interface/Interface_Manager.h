#ifndef INTERFACE_MANAGER_H
#define INTERFACE_MANAGER_H

typedef enum {
    EXT_PNG = 0,
    EXT_TXT = 1
} EXTENSION;

GdkPixbuf* resize(GdkPixbuf* pixbuf, int origin_w, int origin_h,
        int max_w, int max_h);
GdkPixbuf* resize_from_container(GdkPixbuf* pixbuf, int origin_w,
        int origin_h, GtkWidget *container);
void get_controls(int place, GtkWidget** widget);
void Build_Interface(GtkWidget *window, int width, int height, char* title);
int file_selector();
int file_save(void* data, EXTENSION type);

#endif
