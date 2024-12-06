#ifndef INTERFACE_H
#define INTERFACE_H

typedef enum {
    EXT_PNG = 0,
    EXT_TXT = 1
} EXTENSION;

void Build_Interface();
void ShowWidget(GtkWidget *widget);
GdkPixbuf *resize_from_container(GdkPixbuf* pixbuf, GtkWidget* container);
int Load_Image();
int confirm_dialog(char* text);
void save_step(STEP step);

#endif
