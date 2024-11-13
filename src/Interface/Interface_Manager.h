#ifndef INTERFACE_MANAGER_H
#define INTERFACE_MANAGER_H

#include "../Core_Manager.h"
typedef enum {
    EXT_PNG = 0,
    EXT_TXT = 1
} EXTENSION;

GdkPixbuf* resize(GdkPixbuf* pixbuf, int max_w, int max_h);
GdkPixbuf* resize_from_container(GdkPixbuf* pixbuf, GtkWidget *container);
GtkWidget* get_controls(char* id);
/*void clear_history_from(STEP step);
void add_history_step(STEP step);*/
void Build_Interface(GtkWidget *window, AppState *state);
/*int file_selector();
int file_save(void* data, EXTENSION type);
void save_step(STEP step);
int confirm_dialog(char* text);
int modify_image();*/

#endif
