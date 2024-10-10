#ifndef INTERFACE_MANAGER_H
#define INTERFACE_MANAGER_H

GdkPixbuf* resize(GdkPixbuf* pixbuf, int origin_w, int origin_h,
        int max_w, int max_h);
GdkPixbuf* resize_from_container(GdkPixbuf* pixbuf, int origin_w,
        int origin_h, GtkWidget *container);
void Build_Interface(GtkWidget *window, int width, int height, char* title);

#endif
