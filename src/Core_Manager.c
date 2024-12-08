/*
 * i
      ##############################################################
      #                                                            #
      #                       Core_Manager.c                       #
      #                                                            #
      #         Handles, supervises and links all resources        #
      #      between every systems of the Project Application.     #
      #                                                            #
      ##############################################################
*/


////HEADERS
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <gtk/gtk.h>
//Project Headers
#include "Core_Manager.h"
#include "Interface/GTK_Window.h"
#include "Interface/Events.h"
#include "Interface/Interface.h"
#include "Filter/Filter.h"
#include "Extract/Extraction_Manager.h"
#include "OCR/OCR_Manager.h"
#include "Solving/Solver_Manager.h"
#include "Reconstruct/Reconstruct.h"
////DEFINING
#define ID_INIT_SIZE 1
static const char* ID_INIT_PARAMS[ID_INIT_SIZE] =  {"--force"};


AppState *get_appState() {
    static AppState state = (AppState) {0};
    return &state;
}

/* set_step_data():
    Set and stores the data of a specific step using a widget.
*/
void set_step_data(STEP step, GtkWidget *data) {
    APPSTATE->steps_tracker[step]=data;
    gtk_stack_add_named(GTK_STACK(DISPLAY),data,STEPtoSTR(step));
    gtk_widget_show(data);
}


void free_all_steps(STEP from, STEP to) {
    for (int i = from; i <= (int)to; i++)
        free_step_data(i);
}


/* free_step_data():
    Destroys the widget and free all data associated with the step, if any.
*/
void free_step_data(STEP step) {
    GtkWidget *data = GETSTEPDATA(step);
    if (data)
        gtk_widget_destroy(data);
    APPSTATE->steps_tracker[step]=NULL;
}


int check_param(const char** PARAMS, int len, char* to_check) {
    for (int i = 0; i < len; i++) {
        if (to_check == PARAMS[i])
            return 1; //Found
    }
    return 0; //Not found
}


void Filter_Params(char** all_params, int len, char* *init_params,
        int *init_size, char* *gtk_params, int *gtk_size) {
    for (char** curr = all_params; curr < all_params + len; curr++) {
        size_t length = strlen(*curr);
        if (length < 2) //Wrong format
            continue;
        if (check_param(ID_INIT_PARAMS, ID_INIT_SIZE, *curr)) { //Init param
            *init_params = *curr;
            init_params++;
            (*init_size)++;
        } else { //Sent to GTK
            *gtk_params = *curr;
            gtk_params++;
            (*gtk_size)++;
        }
    }
}


void StartUp(char** gtk_params, int gtk_len,
        char** init_params, int init_len) {
    (void) init_params; //No possible init parameters.
    (void) init_len;

    gtk_init(&gtk_len, &gtk_params);
    AppState *state = APPSTATE;

    //Creation of the window
    char* title = "#Free Galane - Word Search Solver";
    GdkRectangle geometry;
    get_screen_size(&geometry);
    state->width = geometry.width;
    state->height = geometry.height;
    int type = GTK_WINDOW_TOPLEVEL;
    WINDOW = create_window(type, title, state->width, state->height);
    //Redirecting logs
    g_log_set_default_handler(_log_handler,NULL);
    //Run the application
    Standard_Signals();
    gtk_widget_show_all(WINDOW);
    gtk_main();
}


/* NextStep():
    Performs the next step. Do so by calling each function. Interpretation:
    1: No errors.
    0: Code stop => normal.
    -1: Error.
*/
int NextStep(GtkWidget*, gpointer) {
    AppState *state = APPSTATE;
    GtkWidget *data;
    if (state->steps_tracker[state->step]!=NULL) {
        ShowNext();
        state->step++;
        return 1;
    }
    switch (state->step) {
        case STEP_LOAD:
            if (!Load_Image())
                return 0;
            break;
        case STEP_FILTER:
            if (state->settings.unsaved_changes) {
                error_dialog("You have unsaved changes.\n"
                        " Save or cancel to continue.");
                return 0;
            }
            data = GETSTEPDATA(STEP_LOAD);
            if (!Filter_Image(g_object_get_data(G_OBJECT(data),"pixbuf")))
                return 0;
            break;
        case STEP_EXTRACT:
            data = GETSTEPDATA(STEP_FILTER);
            int err =Extract_Data(g_object_get_data(G_OBJECT(data),"pixbuf"),
                    "src/bin/");
            if (err == -1) {
                error_dialog("Data could not be found automatically.\n"
                        " Please provide a clearer image.");
                return 0;
            }
            if (!err)
                return 0;
            break;
        case STEP_OCR:
            GObject *obj2 = G_OBJECT(GETSTEPDATA(STEP_EXTRACT));
            Size sizeG =*(Size*)g_object_get_data(obj2,"grid_size");
            int wcount =*(int*)g_object_get_data(obj2,"word_count");
            Cluster ***wordlist2=g_object_get_data(obj2,"wordlist");
            printf("BEFORE\n");
            if (!Identify_Characters(sizeG, wcount, wordlist2))
                return 0;
            printf("AFTER\n");
            break;
        case STEP_SOLVE:
            if (state->settings.unsaved_changes) {
                error_dialog("You have unsaved changes.\n"
                        " Save or cancel to continue.");
                return 0;
            }
            data = GETSTEPDATA(STEP_EXTRACT);
            int count = *(int*)g_object_get_data(G_OBJECT(data),"word_count");
            if (!Solve("src/bin/grid","src/bin/wordlist",count))
                return 0;
            break;
        case STEP_RECONSTRUCT:
            GObject *obj = G_OBJECT(GETSTEPDATA(STEP_LOAD));
            GdkPixbuf *pixbuf = g_object_get_data(obj,"pixbuf");
            obj = G_OBJECT(GETSTEPDATA(STEP_EXTRACT));
            int wcount2 =*(int*)g_object_get_data(obj,"word_count");
            Cluster ***grid=g_object_get_data(obj,"grid");
            Cluster ***wordlist=g_object_get_data(obj,"wordlist");
            obj = G_OBJECT(GETSTEPDATA(STEP_SOLVE));
            Solution* *solutions = g_object_get_data(obj,"solutions");
            if (!Reconstruct(pixbuf,grid,wordlist,solutions,wcount2))
                return 0;
            break;
        default:
            errx(EXIT_FAILURE, "Step format error.");
    }
    g_log("App",G_LOG_LEVEL_MESSAGE,"Step advanced to %s.",
            STEPtoSTR(state->step));
    ShowNext();
    state->step++;
    return 1;
}


void ShowNext() {
    AppState *state=APPSTATE;
    GtkAdjustment *adj = GTK_ADJUSTMENT(gtk_builder_get_object(
                APPSTATE->builder,"step_show_adj"));
    GtkWidget *label;
    switch (state->step) {
        case STEP_LOAD:
            gtk_widget_set_sensitive(GETWIDGET("previous_btn"), TRUE);
            ShowPage(STEPtoSTR(state->step));
            gtk_stack_set_visible_child_name(GTK_STACK(
                        GETWIDGET("input_section")), "TO_FILTER");
            gtk_adjustment_set_value(adj, 100);
            label=GETWIDGET("label_filter");
            label_set_font_color(label,255,255,255);
            label=GETWIDGET("label_load");
            label_set_font_color(label,180,180,180);
            break;
        case STEP_FILTER:
            ShowPage(STEPtoSTR(state->step));
            gtk_stack_set_visible_child_name(GTK_STACK(
                        GETWIDGET("input_section")),"TO_EXTRACT");
            gtk_adjustment_set_value(adj,200);
            label=GETWIDGET("label_extract");
            label_set_font_color(label,255,255,255);
            label=GETWIDGET("label_filter");
            label_set_font_color(label,180,180,180);
            break;
        case STEP_EXTRACT:
            ShowPage(STEPtoSTR(state->step));
            gtk_stack_set_visible_child_name(GTK_STACK(
                        GETWIDGET("input_section")),"TO_OCR");
            gtk_adjustment_set_value(adj,300);
            label=GETWIDGET("label_ocr");
            label_set_font_color(label,255,255,255);
            label=GETWIDGET("label_extract");
            label_set_font_color(label,180,180,180);
            break;
        case STEP_OCR:
            ShowPage(STEPtoSTR(state->step));
            gtk_stack_set_visible_child_name(GTK_STACK(
                        GETWIDGET("input_section")),"TO_SOLVE");
            gtk_adjustment_set_value(adj,400);
            label=GETWIDGET("label_solve");
            label_set_font_color(label,255,255,255);
            label=GETWIDGET("label_ocr");
            label_set_font_color(label,180,180,180);
            break;
        case STEP_SOLVE:
            ShowPage(STEPtoSTR(state->step));
            gtk_stack_set_visible_child_name(GTK_STACK(
                        GETWIDGET("input_section")),"TO_RECONSTRUCT");
            gtk_adjustment_set_value(adj,500);
            label=GETWIDGET("label_reconstruct");
            label_set_font_color(label,255,255,255);
            label=GETWIDGET("label_solve");
            label_set_font_color(label,180,180,180);
            break;
        case STEP_RECONSTRUCT:
            gtk_widget_set_sensitive(GETWIDGET("next_btn"), FALSE);
            gtk_widget_set_sensitive(GETWIDGET("auto_complete"), FALSE);
            ShowPage(STEPtoSTR(state->step));
            gtk_stack_set_visible_child_name(GTK_STACK(
                        GETWIDGET("input_section")),"END");
            gtk_adjustment_set_value(adj,600);
            label=GETWIDGET("label_save");
            label_set_font_color(label,255,255,255);
            label=GETWIDGET("label_reconstruct");
            label_set_font_color(label,180,180,180);
            break;
        default:
            errx(EXIT_FAILURE, "STEP is in incorrect format.");
    }
}


void ShowPrevious(GtkWidget*, gpointer) {
    AppState *state = APPSTATE;
    state->step--;
    GtkAdjustment *adj = GTK_ADJUSTMENT(gtk_builder_get_object(
            APPSTATE->builder,"step_show_adj"));
    GtkWidget *label;
    switch (state->step) {
        case STEP_RECONSTRUCT:
            gtk_widget_set_sensitive(GETWIDGET("next_btn"), TRUE);
            gtk_widget_set_sensitive(GETWIDGET("auto_complete"), TRUE);
            ShowPage(STEPtoSTR(state->step-1));
            gtk_stack_set_visible_child_name(GTK_STACK(
                        GETWIDGET("input_section")),"TO_RECONSTRUCT");
            gtk_adjustment_set_value(adj,500);
            label=GETWIDGET("label_reconstruct");
            label_set_font_color(label,255,255,255);
            label=GETWIDGET("label_save");
            label_set_font_color(label,180,180,180);
            break;
        case STEP_SOLVE:
            ShowPage(STEPtoSTR(state->step-1));
            gtk_stack_set_visible_child_name(GTK_STACK(
                        GETWIDGET("input_section")),"TO_SOLVE");
            gtk_adjustment_set_value(adj,400);
            label=GETWIDGET("label_solve");
            label_set_font_color(label,255,255,255);
            label=GETWIDGET("label_reconstruct");
            label_set_font_color(label,180,180,180);
            break;
        case STEP_OCR:
            ShowPage(STEPtoSTR(state->step-1));
            gtk_stack_set_visible_child_name(GTK_STACK(
                        GETWIDGET("input_section")),"TO_OCR");
            gtk_adjustment_set_value(adj,300);
            label=GETWIDGET("label_ocr");
            label_set_font_color(label,255,255,255);
            label=GETWIDGET("label_solve");
            label_set_font_color(label,180,180,180);
            break;
        case STEP_EXTRACT:
            ShowPage(STEPtoSTR(state->step-1));
            gtk_stack_set_visible_child_name(GTK_STACK(
                        GETWIDGET("input_section")),"TO_EXTRACT");
            gtk_adjustment_set_value(adj,200);
            label=GETWIDGET("label_extract");
            label_set_font_color(label,255,255,255);
            label=GETWIDGET("label_ocr");
            label_set_font_color(label,180,180,180);
            break;
        case STEP_FILTER:
            ShowPage(STEPtoSTR(state->step-1));
            gtk_stack_set_visible_child_name(GTK_STACK(
                        GETWIDGET("input_section")),"TO_FILTER");
            gtk_adjustment_set_value(adj,100);
            label=GETWIDGET("label_filter");
            label_set_font_color(label,255,255,255);
            label=GETWIDGET("label_extract");
            label_set_font_color(label,180,180,180);
            break;
        case STEP_LOAD:
            gtk_widget_set_sensitive(GETWIDGET("previous_btn"), FALSE);
            ShowPage("IMPORT");
            gtk_stack_set_visible_child_name(GTK_STACK(
                        GETWIDGET("input_section")), "TO_LOAD");
            gtk_adjustment_set_value(adj,0);
            label=GETWIDGET("label_load");
            label_set_font_color(label,255,255,255);
            label=GETWIDGET("label_filter");
            label_set_font_color(label,180,180,180);
            break;
        default:
            errx(EXIT_FAILURE, "STEP is in incorrect form.");
    }
}


/* STEPtoSTR():
    Converts the step value into its string representation.
*/
const char* STEPtoSTR(STEP step) {
    if (step == STEP_LOAD)
        return "STEP_LOAD";
    else if (step == STEP_FILTER)
        return "STEP_FILTER";
    else if (step == STEP_EXTRACT)
        return "STEP_EXTRACT";
    else if (step == STEP_OCR)
        return "STEP_OCR";
    else if (step == STEP_SOLVE)
        return "STEP_SOLVE";
    else if (step == STEP_RECONSTRUCT)
        return "STEP_RECONSTRUCT";
    return "None";
}


/* print():
    Custom print function compatible with the application.
*/
void print(const char *format, ...) {
    va_list args;
    va_start(args,format);
    char *result;
    if (vasprintf(&result,format,args)==-1)
        return;
    char *toprint;
    if (asprintf(&toprint, "| %s",result) ==-1)
        return;
    GtkTextIter end;
    GtkTextView *textview = GTK_TEXT_VIEW(GETWIDGET("logs"));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(textview);
    gtk_text_buffer_get_end_iter(buffer,&end);
    gtk_text_buffer_insert(buffer,&end,toprint,-1);
    va_end(args);
    free(result);
    free(toprint);
}
