#include <gtk/gtk.h>
#include "../Core_Manager.h"
int Identify_Characters() {
    //Save the results
    GtkBuilder *builder = gtk_builder_new_from_file(
            "src/assets/ocr_tile.glade");
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder,
                "grid"));
    gchar *content;
    gsize len;
    if (!g_file_get_contents("examples/grid",&content,&len,NULL))
        return 0;
    gtk_text_buffer_set_text(buffer,content,-1);
    g_free(content);
    buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "wordlist"));
    if (!g_file_get_contents("examples/word_list",&content,&len,NULL))
        return 0;
    gtk_text_buffer_set_text(buffer,content,-1);
    g_free(content);
    GtkWidget *widget = GTK_WIDGET(gtk_builder_get_object(builder,"toplvl"));
    FREESTEPDATA(STEP_OCR);
    SETSTEPDATA(STEP_OCR,widget);
    return 1;
}
