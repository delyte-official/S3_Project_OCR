#include <gtk/gtk.h>
#include "../Core_Manager.h"
int Identify_Characters() {
    //Widget creation
    GtkBuilder *builder = gtk_builder_new_from_file(
            "src/assets/ocr_tile.glade");
    GtkWidget *widget = GTK_WIDGET(gtk_builder_get_object(builder,"toplvl"));
    //GRID
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder,
                "grid"));
    gchar *content;
    gsize len;
    if (!g_file_get_contents("examples/grid",&content,&len,NULL))
        return 0;
    gtk_text_buffer_set_text(buffer,content,-1);
    //Write file
    if (!g_file_set_contents("src/bin/grid",content,-1,NULL)) {
        g_log("GLib",G_LOG_LEVEL_ERROR,"Impossible to write to "
                "'src/bin/grid'.");
        return 0;
    }
    g_object_set_data(G_OBJECT(widget),"grid_buffer",buffer);
    g_free(content);
    //WORDLIST
    buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "wordlist"));
    if (!g_file_get_contents("examples/word_list",&content,&len,NULL))
        return 0;
    gtk_text_buffer_set_text(buffer,content,-1);
    //Write file
    if (!g_file_set_contents("src/bin/wordlist",content,-1,NULL)) {
        g_log("GLib",G_LOG_LEVEL_ERROR,"Impossible to write to "
                "'src/bin/wordlist'.");
        return 0;
    }
    g_object_set_data(G_OBJECT(widget),"wordlist_buffer",buffer);
    g_free(content);
    //Save results
    g_object_set_data(G_OBJECT(widget), "builder",builder);
    FREESTEPDATA(STEP_OCR);
    SETSTEPDATA(STEP_OCR,widget);
    return 1;
}
