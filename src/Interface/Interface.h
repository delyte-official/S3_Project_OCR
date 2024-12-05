#ifndef INTERFACE_H
#define INTERFACE_H

void Build_Interface();
void ShowWidget(GtkWidget *widget);
GdkPixbuf *resize_from_container(GdkPixbuf* pixbuf, GtkWidget* container);
int Load_Image();
int confirm_dialog(char* text);

#endif
