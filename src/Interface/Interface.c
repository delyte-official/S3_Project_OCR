////HEADERS
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <gtk/gtk.h>
#include <pango/pango.h>
//Project Headers
#include "Events.h"
#include "../Core_Manager.h"
#include "Interface.h"


GtkWidget* image_new_from_file(
        char* filename) {
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
    GtkWidget* image = gtk_image_new_from_pixbuf(pixbuf);
    g_object_unref(pixbuf);
    return image;
}


/* gtk_builder_new_custom():
    Load XML file and modify its size into percentage of given dimensions.
*/
GtkBuilder *gtk_builder_new_custom(char* filename, int width, int height) {
    FILE *file = fopen(filename,"r");
    if (!file)
        errx(EXIT_FAILURE,"fopen()"); //Can't load application

    //Read content of XML file
    fseek(file,0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file,0,SEEK_SET);
    char* xml_content = malloc(file_size+1); //include \0
    if (!xml_content)
        errx(EXIT_FAILURE,"malloc()");
    fread(xml_content, 1, file_size, file);
    xml_content[file_size] = '\0';
    fclose(file);

    //Modifying the content
    size_t new_size = file_size+1;
    char* xml_modified = malloc(new_size);
    if (!xml_modified)
        errx(EXIT_FAILURE,"malloc()");
    char* toread = xml_content;
    char* towrite = xml_modified;
    //Modify every properties "width-request" and "height-request":
    while (*toread) {
        char* width_prop = strstr(toread, "width-request");
        char *height_prop = strstr(toread, "height-request");
        char *prop = NULL;
        int isWIDTH = 0;
        if (width_prop && (!height_prop || width_prop<height_prop)) {
            prop = width_prop;
            isWIDTH=1;
        } else if (height_prop)
            prop = height_prop;
        if (!prop) { //no more properties to modify
            strcpy(towrite, toread);
            break;
        }
        //Changing size of property
        char *value = strchr(prop,'>')+1;
        size_t prefix_l = value-toread;
        strncpy(towrite, toread, prefix_l);
        towrite+=prefix_l;
        char *end_value = strchr(value,'<');
        *end_value = '\0';
        int read_value=atoi(value);
        *end_value = '<';
        //Calculating and writing the new value
        int new_value = read_value;
        if (read_value>=0&&read_value<=100)
            new_value= (int)((read_value/100.0f)*(isWIDTH?width:height));
        else if (read_value>100)
            new_value=read_value-100;
        int expand = snprintf(NULL,0,"%d",new_value) - (end_value-value);
        new_size+=expand;
        size_t curr_len = towrite-xml_modified;
        char* tmp = realloc(xml_modified,new_size);
        if (!tmp)
            errx(EXIT_FAILURE,"realloc()");
        xml_modified = tmp;
        towrite = xml_modified + curr_len;
        towrite += sprintf(towrite, "%d", new_value);
        toread=end_value; //Prevent loops
    }
    xml_modified[new_size-1]='\0';
    //Load interface
    GtkBuilder *builder = gtk_builder_new_from_string(xml_modified,-1);
    free(xml_content);
    free(xml_modified);
    return builder;
}


void Build_Interface() {
    AppState *state = APPSTATE;
    //Construct interface from XML file
    state->builder = gtk_builder_new_custom("main.glade",
            state->alloc_width, state->alloc_height);
    //Connect to window
    GtkWidget *toplvl = GETWIDGET("toplvl_id");
    gtk_container_add(GTK_CONTAINER(WINDOW), toplvl);
    gtk_builder_connect_signals(state->builder, NULL);
    //Manual data
    state->display = GETWIDGET("display_section");
}


void ShowPage(const char* page) {
    gtk_stack_set_visible_child_name(GTK_STACK(DISPLAY), page);
}


void AddPage(const char* page, GtkWidget *widget) {
    gtk_stack_add_named(GTK_STACK(DISPLAY), widget, page);
}


GdkPixbuf* resize_pixbuf(GdkPixbuf* pixbuf, int max_w, int max_h) {
    int origin_w = gdk_pixbuf_get_width(pixbuf);
    int origin_h = gdk_pixbuf_get_height(pixbuf);
    float w_ratio = (float)max_w/origin_w;
    float h_ratio = (float)max_h/origin_h;
    float scale = w_ratio < h_ratio ? w_ratio : h_ratio;
    int width = (int)(origin_w * scale);
    int height = (int)(origin_h *scale);
    GdkPixbuf *scaled = gdk_pixbuf_scale_simple(pixbuf, width, height,
            GDK_INTERP_HYPER);
    return scaled;
}


GdkPixbuf* resize_from_container(GdkPixbuf* pixbuf, GtkWidget* container) {
    GtkAllocation alloc;
    gtk_widget_get_allocation(container, &alloc);
    return resize_pixbuf(pixbuf, alloc.width, alloc.height);
}


int Load_Image() {
    GtkBuilder *builder = gtk_builder_new_from_file(
            "src/assets/image_selector.glade");
    GtkWidget *dialog = GTK_WIDGET(gtk_builder_get_object(builder, "dialog"));
    gtk_window_set_transient_for(GTK_WINDOW(dialog),GTK_WINDOW(WINDOW));
    int response = gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT;
    if (response) {
        char* filename = gtk_file_chooser_get_filename(
                GTK_FILE_CHOOSER(dialog));
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
        if (pixbuf != NULL) {
            GdkPixbuf *resized = resize_from_container(pixbuf, DISPLAY);
            GtkWidget *image = gtk_image_new_from_pixbuf(resized);
            g_object_set_data(G_OBJECT(image), "pixbuf", pixbuf);
            g_object_ref(pixbuf);
            FREESTEPDATA(STEP_LOAD);
            SETSTEPDATA(STEP_LOAD,image);
        }
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
    g_object_unref(builder);
    return response;
}


/* confirm_dialog():
    Pops up a confirmation dialog on the window, with custom message.
*/
int confirm_dialog(char* text) {
    GtkBuilder *builder = gtk_builder_new_from_file(
            "src/assets/confirm_dialog.glade");
    GtkWidget *dialog = GTK_WIDGET(gtk_builder_get_object(builder,"dialog"));
    gtk_window_set_transient_for(GTK_WINDOW(dialog),GTK_WINDOW(WINDOW));
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                    "confirm_text")), text);
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return response == GTK_RESPONSE_ACCEPT;
}


int file_save(void* data, EXTENSION type) {
    //Creating the dialog window
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Save File",
            GTK_WINDOW(WINDOW), GTK_FILE_CHOOSER_ACTION_SAVE,
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Save", GTK_RESPONSE_ACCEPT,
            NULL);
    GtkFileChooser* chooser = GTK_FILE_CHOOSER(dialog);
    //Filter
    GtkFileFilter *filter = gtk_file_filter_new();
    if (type == EXT_PNG) {
        gtk_file_filter_set_name(filter, "PNG files");
        gtk_file_filter_add_mime_type(filter, "image/png");
        gtk_file_chooser_set_current_name(chooser, "untilted.png");
    } else if (type == EXT_TXT) {
        gtk_file_filter_set_name(filter, "Text files");
        gtk_file_filter_add_pattern(filter, "*.txt");
        gtk_file_chooser_set_current_name(chooser, "untitlted.txt");
    }
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    //Run the dialog
    int response = gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT;
    if (response) {
        //Get the selected image
        char* filename = gtk_file_chooser_get_filename(chooser);
        if (type == EXT_PNG) {
            //Checking extension
            if (!g_str_has_suffix(filename, ".png") && !strchr(filename,'.')) {
                char *newfile = g_strconcat(filename, ".png", NULL);
                g_free(filename);
                filename = newfile;
            }
            //Data is a pixbuf
            GdkPixbuf *pixbuf = *(GdkPixbuf**)data;
            //Save to file
            if (!gdk_pixbuf_save(pixbuf, filename, "png", NULL, NULL))
                response = 0;
        } else if (type == EXT_TXT) {
            //Data is a buffer
            GtkTextBuffer *buffer = *(GtkTextBuffer**)data;
            //Grab content
            GtkTextIter start, end;
            gtk_text_buffer_get_start_iter(buffer, &start);
            gtk_text_buffer_get_end_iter(buffer, &end);
            char* text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
            //Save to File
            FILE *file =fopen(filename, "w");
            if (file == NULL) {
                response = 0;
            } else {
                fprintf(file, "%s",text);
                fclose(file);
            }
        }
        g_free(filename);
    }
    //Closing dialog wether cancel or accepted
    gtk_widget_destroy(dialog);
    return response;
}


void save_step(STEP step) {
    if (step == STEP_LOAD || step == STEP_FILTER || step == STEP_EXTRACT ||
            step == STEP_RECONSTRUCT) {
        GdkPixbuf *pixbuf = g_object_get_data(G_OBJECT(GETSTEPDATA(step)),
                "pixbuf");
        file_save(&pixbuf, EXT_PNG);
    } else if (step == STEP_OCR) {
        GtkWidget *extract = GETSTEPDATA(step);;
        //Grab content of text view or smth
        GtkTextBuffer *b_grid = g_object_get_data(G_OBJECT(extract),
                "grid_buffer");
        file_save(&b_grid, EXT_TXT);
        GtkTextBuffer *b_wordlist = g_object_get_data(G_OBJECT(extract),
                "wordlist_buffer");
        file_save(&b_wordlist, EXT_TXT);
    } else if (step == STEP_SOLVE) {
        GtkTextBuffer *buffer = g_object_get_data(G_OBJECT(GETSTEPDATA(
                        STEP_SOLVE)),"buffer");
        file_save(&buffer, EXT_TXT);
    }
}
