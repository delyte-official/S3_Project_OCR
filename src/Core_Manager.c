/*
      ##############################################################
      #                                                            #
      #                       Core_Manager.c                       #
      #                                                            #
      #         Handles, supervises and links all resources        #
      #      between every systems of the Project Application.     #
      #                                                            #
      ##############################################################
*/


////HEADERS Files
//Integrated C Libraries
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
//Project Headers
#include "Interface/GTK_Window.h"
#include "Interface/Interface_Manager.h"
#include "Interface/Events.h"
#include "Solving/Solver_Manager.h"
#include "Core_Manager.h"
#include "Filter/filter.h" 
//Tools
#include <gtk/gtk.h>
////END HEADERS
////DEFINING
//Constants
#define ID_INIT_SIZE 1
static const char* ID_INIT_PARAMS[ID_INIT_SIZE] = {"--force"};
int global_width, global_height = 0;
////END DEFINING


/* get_app_state():
    Returns a pointer to the static struct containing the entire
    state of the Application. GENERIC DATA
*/
AppState *get_app_state() {
    static AppState state = (AppState) {};
    return &state;
}


/* set_display():
    Change the child of the display section or clear it
*/
void set_display(GtkWidget* widget) {
    AppState *state = get_app_state();
    GtkOverlay *display = GTK_OVERLAY(gtk_builder_get_object(state->builder,
                "display_section_id"));
    GList *children = gtk_container_get_children(GTK_CONTAINER(display));
    if (children) {
        g_object_ref(children->data);
        gtk_container_remove(GTK_CONTAINER(display), children->data);
    }
    if (widget != NULL) {
        gtk_overlay_add_overlay(display, widget);
        gtk_widget_show_all(widget);
    }
}


/* save_internal_step():
    Save the result of a step internally in the AppSate.
*/
void save_internal_step(STEP step, GtkWidget* to_save) {
    AppState *state = get_app_state();
    state->steps_tracker[(int)step] = to_save;
}


/* delete_internal_step():
    Delete an internal step in the AppState if it exists.
*/
void delete_internal_step(STEP step) {
    AppState *state = get_app_state();
    GtkWidget *to_delete = state->steps_tracker[(int)step];
    if (to_delete != NULL) {
        gtk_widget_destroy(to_delete);
        state->steps_tracker[(int)step] = NULL;
    }
}


/* recognized():
  Returns if an element is present or not in the given parameters list.
*/
int recognized(const char** PARAMS, int len, char* to_check) {
    for (int i = 0; i < len; i++) {
        if (to_check == PARAMS[i]) //Found
            return 1;
    }
    return 0; //Not found
}


/* Filter_Params():
  Filters every given parameters into two categories:
  - GTK parameters
  - Application parameters
*/
void Filter_Params(char** all_params, int len,
        char* *init_params, int *init_size,
        char* *gtk_params, int *gtk_size) {
    //Iterating through the given parameters
    for (char** curr = all_params; curr < all_params + len; curr++) {
        size_t length = strlen(*curr);
        if (length < 2) //Skipping unformalized parameters
            continue;
        //Checking if it is a recognized parameter
        if (recognized(ID_INIT_PARAMS, ID_INIT_SIZE, *curr)) { //Init param
            *init_params = *curr;
            init_params++;
            (*init_size)++;
        } else { //Is (garbage || GTK) parameter
            //GTK automatically handles garbage parameters
            *gtk_params = *curr;
            gtk_params++;
            (*gtk_size)++;
        }
    }
}


/* StartUp():
  Initialize all systems necessary for the projects, then run the application.
*/
void StartUp(char** gtk_params, int gtk_len,
        char** init_params, int init_len) {
    //Unused parameters - TO HANDLE LATER ON)
    (void) init_params;
    (void) init_len;

    //Initialization of GTK and AppState
    gtk_init(&gtk_len, &gtk_params);
    AppState *state = get_app_state();

    //Creation of the window
    GtkWidget *window;
    char* title = "OCR Word Search Solver";
    GdkRectangle geometry;
    get_screen_size(&geometry);
    state->width = geometry.width;
    state->height = geometry.height;
    int type = GTK_WINDOW_TOPLEVEL;

    window = create_window(type, title, state->width, state->height);

    //Link signals
    Standard_Signals(window);

    //Running the application
    gtk_widget_show_all(window);
    gtk_main();
}


/* ShowNext():
    Show the next operation that has already been done.
*/
/*void ShowNext() {
    STEP curr_step = get_app_state()->step;
    switch (curr_step) {
        case STEP_LOAD:
            //Grab the image to display
            GtkWidget *image1 = step_widget(1, NULL);
            set_display(image1);
            
            GtkWidget* control1 = get_controls("previous_ctrl_id");
            gtk_widget_set_sensitive(control1, TRUE);
            control1 = get_controls("next_ctrl_id");
            gtk_widget_set_sensitive(control1, TRUE);
            control1 = get_controls("modify_ctrl_id");
            gtk_widget_show(control1);
            break;
        case STEP_FILTER:
            //Grab the image to display
            GtkWidget *image2 = step_widget(2, NULL);
            set_display(image2);

            GtkWidget* control2 = get_controls("modify_ctrl_id");
            gtk_widget_hide(control2);
            break;
        case STEP_EXTRACT:
            //Grab the container to display
            GtkWidget *container3 = step_widget(3, NULL);
            set_display(container3);
            break;
        case STEP_SOLVE:
            //Grab the widget to display
            GtkWidget *widget = step_widget(STEP_SOLVE+1, NULL);
            set_display(widget);
            break;
        case STEP_RECONSTRUCT:
            //Grab the image to display
            GtkWidget *reconstruct = step_widget(STEP_RECONSTRUCT+1, NULL);
            set_display(reconstruct);

            GtkWidget* control5 = get_controls("automatic_ctrl_id");
            gtk_widget_set_sensitive(control5, FALSE);
            control5 = get_controls("next_ctrl_id");
            gtk_widget_set_sensitive(control5, FALSE);
            break;
        default:
            errx(EXIT_FAILURE, "STEP is in incorrect format.");
    }
}*/


/* NextStep():
    Performs the next operation of the OCR Word Search program.
    Returns true if operation succeeded.
*/
/*int NextStep(GtkWidget*, gpointer) {
    STEP* curr_step = &get_app_state()->step;
    if (step_widget(*curr_step + 1, NULL) != NULL) {
        ShowNext();
        (*curr_step)++; //Readjusting the current step
        return !EXIT_SUCCESS;
    }

    //Performing operation according to curr_step
    switch (*curr_step) {
        case STEP_LOAD:
            if (!file_selector(NULL, NULL))
                return !EXIT_FAILURE;
            //Add to history
            add_history_step(STEP_LOAD);
            ShowNext();
            break;
        case STEP_FILTER:
            GtkWidget *image = step_widget(1, NULL);
            GdkPixbuf *pixbuf = g_object_get_data(G_OBJECT(image), "pixbuf");
            //Defining the constant variables for the filtering
            int threshold = 180;
            GdkPixbuf *new_pixbuf = grayscale_pixbuf(pixbuf, threshold);
            //Resizing for display
            GdkPixbuf *resized = resize_from_container(new_pixbuf, DISPLAY);
            GtkWidget *new_image = gtk_image_new_from_pixbuf(resized);
            g_object_set_data(G_OBJECT(new_image), "pixbuf", new_pixbuf);
            g_object_ref(new_pixbuf);
            //Display
            step_widget(2, new_image);
            add_history_step(STEP_FILTER);
            ShowNext();
            break;
        case STEP_EXTRACT:
            //TO REPLACE WITH NOE's FUNCTION:
            GtkWidget *widget = gtk_scrolled_window_new(NULL, NULL);
            GtkWidget *text_view1 = gtk_text_view_new();
            GtkWidget *text_view2 = gtk_text_view_new();
            GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
            gtk_container_add(GTK_CONTAINER(widget), box);
            gtk_box_pack_start(GTK_BOX(box), text_view1, TRUE, TRUE, 0);
            gtk_box_pack_start(GTK_BOX(box), text_view2, TRUE, TRUE, 0);
            GtkTextBuffer *buffer = gtk_text_view_get_buffer(
                    GTK_TEXT_VIEW(text_view1));
            FILE* file = fopen("src/bin/grid", "r");
            //Assume no erros
            char line[1024];
            while (fgets(line, sizeof(line), file)) {
                gtk_text_buffer_insert_at_cursor(buffer, line, -1);
            }
            fclose(file);
            GtkTextBuffer *buffer2 = gtk_text_view_get_buffer(
                    GTK_TEXT_VIEW(text_view2));
            FILE *file2 = fopen("src/bin/word_list", "r");
            while (fgets(line, sizeof(line), file2))
                gtk_text_buffer_insert_at_cursor(buffer2, line, -1);
            fclose(file2);
            g_object_set_data(G_OBJECT(widget), "path:grid", "src/bin/grid");
            g_object_set_data(G_OBJECT(widget), "path:wordlist",
                    "src/bin/word_list");
            g_object_set_data(G_OBJECT(widget), "buffer", buffer);
            g_object_set_data(G_OBJECT(widget), "buffer:words", buffer2);
            g_object_set_data(G_OBJECT(widget), "size:wordlist",
                    GINT_TO_POINTER(8));
            step_widget(STEP_SOLVE, widget);
            //END OF REPLACING
            add_history_step(STEP_EXTRACT);
            ShowNext();
            break;
        case STEP_SOLVE:
            GtkWidget *extracted = step_widget(STEP_EXTRACT+1, NULL);
            char* grid = g_object_get_data(G_OBJECT(extracted),
                    "path:grid");
            char* wordlist = g_object_get_data(G_OBJECT(extracted),
                    "path:wordlist");
            int size = GPOINTER_TO_INT(g_object_get_data(
                        G_OBJECT(extracted), "size:wordlist"));
            Solver_Run(grid, wordlist, size);
            add_history_step(STEP_SOLVE);
            ShowNext();
            break;
        case STEP_RECONSTRUCT:
            //CHANGE THIS WITH RECONSTRUCTION
            GtkWidget *imageR = step_widget(1, NULL);
            GdkPixbuf *pixbufR = g_object_get_data(G_OBJECT(imageR), "pixbuf");
            GdkPixbuf *new_pixbufR = gdk_pixbuf_copy(pixbufR);
            //Resized
            GdkPixbuf *resizedR = resize_from_container(new_pixbufR, DISPLAY);
            GtkWidget *new_imageR = gtk_image_new_from_pixbuf(resizedR);
            g_object_set_data(G_OBJECT(new_imageR), "pixbuf", new_pixbufR);
            g_object_ref(new_pixbufR);
            step_widget(STEP_RECONSTRUCT+1, new_imageR);
            //TO PLACE IN ZYPAW FUNCTION (whats between comments)
            add_history_step(STEP_RECONSTRUCT);
            ShowNext();
            break;
        default:
            errx(EXIT_FAILURE, "STEP is in incorrect format.");
    }
    //Advancing the current step
    (*curr_step)++;
    return !EXIT_SUCCESS;
}*/


/* ShowPrevious():
    Returns to the last step. DOES NOT CANCEL IT:
    - History is kept
    - All operations are kept
    - Only display is changed to visualize a previous step
*/
/*void ShowPrevious(GtkWidget*, gpointer) {
    STEP *curr_step = &(get_app_state()->step);
    //Performing operation according to curr_step
    switch (*curr_step) {
        case STEP_END:
            //Reshow solution
            GtkWidget *buffer = step_widget(STEP_SOLVE+1, NULL);
            set_display(buffer);

            GtkWidget* control1 =get_controls("automatic_ctrl_id");
            gtk_widget_set_sensitive(control1, TRUE);
            control1 = get_controls("next_ctrl_id");
            gtk_widget_set_sensitive(control1, TRUE);
            break;
        case STEP_RECONSTRUCT:
            //Reshow extraction
            GtkWidget *container2 = step_widget(STEP_EXTRACT+1, NULL);
            set_display(container2);
            break;
        case STEP_SOLVE:
            //Reshow filtered image
            GtkWidget* image3 = step_widget(STEP_FILTER+1, NULL);
            set_display(image3);
            break;
        case STEP_EXTRACT:
            //Reshow loaded image not filtered
            GtkWidget* image4 = step_widget(STEP_LOAD+1, NULL);
            set_display(image4);

            GtkWidget *control4 = get_controls("modify_ctrl_id");
            gtk_widget_show(control4);
            break;
        case STEP_FILTER:
            //Reshow select file button
            GtkWidget* btn = step_widget(0, NULL);
            set_display(btn);

            GtkWidget* control5 = get_controls("previous_ctrl_id");
            gtk_widget_set_sensitive(control5, FALSE);
            control5 = get_controls("save_ctrl_id");
            gtk_widget_set_sensitive(control5, FALSE);
            control5 = get_controls("modify_ctrl_id");
            gtk_widget_hide(control5);
            break;
        default:
            errx(EXIT_FAILURE, "STEP is in incorrect form.");
    }

    //Regressing the current step
    (*curr_step)--;
}*/
