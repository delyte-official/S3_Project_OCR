#ifndef INTERFACE_MANAGER_H
#define INTERFACE_MANAGER_H

#include "../Core_Manager.h"
typedef enum {
    EXT_PNG = 0,
    EXT_TXT = 1
} EXTENSION;

GdkPixbuf* resize(GdkPixbuf* pixbuf, int max_w, int max_h);
GdkPixbuf* resize_from_container(GdkPixbuf* pixbuf, GtkWidget *container);
void get_controls(int place, GtkWidget** widget);
void clear_history();
void add_history_step(STEP step);
void Build_Interface(GtkWidget *window, int width, int height);
int file_selector();
int file_save(void* data, EXTENSION type);
void save_step(STEP step);
int confirm_dialog(char* text);

#endif
