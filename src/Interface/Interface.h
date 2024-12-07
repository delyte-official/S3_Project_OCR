#ifndef INTERFACE_H
#define INTERFACE_H

typedef enum {
    EXT_PNG = 0,
    EXT_TXT = 1
} EXTENSION;

void Build_Interface();
void ShowPage(const char* page);
void AddPage(const char* page, GtkWidget *widget);
GdkPixbuf *resize_from_container(GdkPixbuf* pixbuf, GtkWidget* container);
int Load_Image();
int confirm_dialog(char* text);
void error_dialog(char* text);
void save_step(STEP step);

#endif
