/*
      ##############################################################
      #                                                            #
      #                       Core_Manager.c                       #
      #                                                            #
      #         Handles, supervises and links all resources        #
      #      between every systems of the Project Application.     #
      #                                                            #
      ##############################################################

List of all functions written in this file (and their type):
[See more description on their purpose and parameters down below]

STEP* get_step();
GtkWidget* get_display(GtkWidget**);
void set_display(GtkWidget*);
GtkWidget* step_widget(int step, GtkWidget* widget);
int recognized(char**,size_t,char*);
void Filter_Params(char**,size_t,char***,size_t*,char***,size_t*);
void StartUp(char**,size_t,char**,size_t);
void NextStep(GtkWidget,int);
void PreviousStep(GtkWidget,int);
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
#include "Debug.h"
#include "Core_Manager.h"
//Tools
#include <gtk/gtk.h>
////END HEADERS
////DEFINING
//Constants
#define ID_INIT_SIZE 1
static const char* ID_INIT_PARAMS[ID_INIT_SIZE] = {"--force"};
////END DEFINING


/* get_step():
    Returns a pointer to the static variable representing the current
    step that the program is at.
*/
STEP* get_step() {
    static STEP curr_step = 0;
    return &curr_step;
}


/* get_display():
    Returns the the display section and its child thru the pointer
    Initialize on first call.
*/
GtkWidget* get_display(GtkWidget** widget) {
    static GtkWidget* display;
    if (display==NULL)
        display = *widget;
    if (widget != NULL) {
        GList *children = gtk_container_get_children(GTK_CONTAINER(display));
        if (children)
            *widget = GTK_WIDGET(children->data);
    }
    return display;
}


/* set_display():
    Change the child of the display section or clear it
*/
void set_display(GtkWidget* widget) {
    GtkWidget* display = get_display(NULL);
    GList *children = gtk_container_get_children(GTK_CONTAINER(display));
    if (children) {
        g_object_ref(children->data);
        gtk_container_remove(GTK_CONTAINER(display), children->data);
    }
    if (widget != NULL) {
       gtk_box_pack_start(GTK_BOX(display), widget, TRUE, TRUE, 0);
       gtk_widget_show_all(widget);
    }
}


/* step_widget():
    Set the value of the widget associated with step
    Returns the widget associated with the given step
*/
GtkWidget* step_widget(int step, GtkWidget* set) {
    static GtkWidget* step_widgets[6] = {NULL};
    if (step < 0) {
        int new_step = step * (-1);
        if (step_widgets[new_step] != NULL) {
            gtk_widget_destroy(step_widgets[new_step]);
            step_widgets[new_step] = NULL;
        }
    } else if (set != NULL)
        step_widgets[step] = set;
    return step_widgets[step];
}


/*  recognized():
  Returns if an element is present or not in the given parameters list.

Requisites assumed:
  The given parameters list is correct.
  The parameter to check is in correct format.
*/
int recognized(const char** PARAMS, int len, char* to_check) {
    for (int i = 0; i < len; i++) {
        if (to_check == PARAMS[i]) //Found
            return 1;
    }
    return 0; //Not found
}


/*  Filter_Params():
  Filters every given parameters into two categories:
  - GTK parameters
  - Application parameters
  Returns them in given arrays, and their sizes.
  Every unrecognized parameters will be thrown away.

Requisites assumed:
  The pointed arrays have enough space to hold every parameters by themselves.
  The pointed arrays and size_t are already initialized and empty.
*/
void Filter_Params( //Parameters
        char** all_params, //The given command-line parameters
        int len, //The size of all_parameters
        char* *init_params, //Array to write initialization parameters
        int *init_size, //To store size of init_params
        char* *gtk_params, //Array to write Application parameters
        int *gtk_size) { //To store size of run_params
    //Iterating through the given parameters
    for (char** curr = all_params; curr < all_params + len; curr++) {
        size_t length = strlen(*curr);
        if (length < 2) //Skipping unformalized parameters
            continue;

        //Checking if it is a recognized parameter
        if (recognized(ID_INIT_PARAMS, ID_INIT_SIZE, *curr)) {
            //Putting the parameter into its corresponding category
            *init_params = *curr;
            init_params++;
            (*init_size)++;
        } else { //Is a garbage parameter or a GTK Parameter
                 //We let GTK treat them however it wants
            *gtk_params = *curr;
            gtk_params++;
            (*gtk_size)++;
        }
    }
}


/*  StartUp():
  Initialize all systems necessary for the projects:
  - C Libraries
  - Signals Tracking
  - SDL/GTK Systems
  - Others
  Then run the application.

Requisites assumed:
  It must be the first and last time that this function is called.
  Any error in this function must terminate the entire processus.
  The given parameters are valid, and their number correct.
*/
void StartUp( //Parameters:
        char** gtk_params, //Given parameters for gtk_initialization
        int gtk_len, //Size of gtk_params
        char** init_params, //Given parameters for developper testing
        int init_len) { //Size of init_params
    //Unused parameters - TO REMOVE)
    (void) init_params;
    (void) init_len;
    //Initialize GTK3 and its sub-systems
    gtk_init(&gtk_len, &gtk_params);

    //Defining necessary variables for Window_Init
    GtkWidget *window;
    char* title = "OCR Word Search Solver";
    GdkRectangle geometry;
    get_screen_size(&geometry);
    int width = geometry.width;
    int height = geometry.height;
    int type = GTK_WINDOW_TOPLEVEL;

    //Initialize GTK Main Project Window
    window = create_window(type,title,width,height);

    //Building the interface and setup the associated signals and events
    Build_Interface(window, width, height, title);
    
    //Link all standard signals and events of the window
    Standard_Signals(window);

    //Running the application
    gtk_main();
}


/* ShowNext():
    Show the next operation that has already been done.
*/
void ShowNext() {
    STEP* curr_step = get_step();
    switch (*curr_step) {
        case STEP_LOAD:
            //Grab the image to display
            GtkWidget *image1 = step_widget(1, NULL);
            set_display(image1);
            
            GtkWidget* control03_btn;
            get_controls(0, &control03_btn);
            gtk_widget_set_sensitive(control03_btn, TRUE);
            get_controls(3, &control03_btn);
            gtk_widget_set_sensitive(control03_btn, TRUE);
            break;
        case STEP_FILTER:
            //Grab the image to display
            GtkWidget *image2 = step_widget(2, NULL);
            set_display(image2);
            break;
        case STEP_EXTRACT:
            break;
        case STEP_SOLVE:
            break;
        case STEP_RECONSTRUCT:
            GtkWidget* control12_btn;
            get_controls(1, &control12_btn);
            gtk_widget_set_sensitive(control12_btn, FALSE);
            get_controls(2, &control12_btn);
            gtk_widget_set_sensitive(control12_btn, FALSE);
            break;
        default:
            errx(EXIT_FAILURE, "STEP is in incorrect format.");
    }
}


/* NextStep():
    Performs the next operation of the OCR Word Search program.
    Returns true if operation succeeded.
*/
int NextStep(GtkWidget*, gpointer) {
    STEP* curr_step = get_step();
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
            ShowNext();
            break;
        case STEP_FILTER:
            GtkWidget *image = step_widget(1, NULL);
            GdkPixbuf *pixbuf = g_object_get_data(G_OBJECT(image), "pixbuf");
            //FOR FUTURE: MODIFY PIXBUF WITH FILTERING
            GdkPixbuf *new_pixbuf = gdk_pixbuf_copy(pixbuf);
            GtkWidget *new_image = gtk_image_new_from_pixbuf(new_pixbuf);
            g_object_ref(new_pixbuf);
            g_object_set_data(G_OBJECT(new_image), "pixbuf", new_pixbuf);
            step_widget(2, new_image);
            //TO PLACE IN ZYPAW FUNCTION (whats between comments)
            ShowNext();
            break;
        case STEP_EXTRACT:
            break;
        case STEP_SOLVE:
            Solver_Run("src/bin/grid", "src/bin/word_list", 8);
            break;
        case STEP_RECONSTRUCT:
            ShowNext();
            break;
        default:
            errx(EXIT_FAILURE, "STEP is in incorrect format.");
    }
    //Advancing the current step
    (*curr_step)++;
    return !EXIT_SUCCESS;
}


/* ShowPrevious():
    Returns to the last step. DOES NOT CANCEL IT:
    - History is kept
    - All operations are kept
    - Only display is changed to visualize a previous step
*/
void ShowPrevious(GtkWidget*, gpointer) {
    STEP* curr_step = get_step();
    //Performing operation according to curr_step
    switch (*curr_step) {
        case STEP_END:
            //Activate control buttons
            GtkWidget* control12_btn;
            get_controls(1, &control12_btn);
            gtk_widget_set_sensitive(control12_btn, TRUE);
            get_controls(2, &control12_btn);
            gtk_widget_set_sensitive(control12_btn, TRUE);
            break;
        case STEP_RECONSTRUCT:
            break;
        case STEP_SOLVE:
            break;
        case STEP_EXTRACT:
            break;
        case STEP_FILTER:
            //Reshow select file button
            GtkWidget* btn = step_widget(0, NULL);
            set_display(btn);
            //Deactivate button
            GtkWidget* control03_btn;
            get_controls(0, &control03_btn);
            gtk_widget_set_sensitive(control03_btn, FALSE);
            get_controls(3, &control03_btn);
            gtk_widget_set_sensitive(control03_btn, FALSE);
            break;
        default:
            errx(EXIT_FAILURE, "STEP is in incorrect form.");
    }

    //Regressing the current step
    (*curr_step)--;
}
