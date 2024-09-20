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

recognized(char**,size_t,char*);
Filter_Params(char**,size_t,char***,size_t*,char***,size_t*);
Initialize(char**,int);
Run_Application(char**,int);
*/











////HEADERS Files
//Integrated C Libraries


//Project Headers


////END HEADERS

////DEFINING
//Constants
const size_t INIT_SIZE = 0;
const char* INIT_PARAMS[INIT_SIZE] = {};
const size_t RUN_SIZE = 1;
const char* RUN_PARAMS[RUN_SIZE] = {"--force"};

////END DEFINING





/*  recognized():
  Returns if an element is present or not in the given parameters list.

Requisites assumed:
  The given parameters list is correct.
  The parameter to check is in correct format.
*/
int recognized(char** PARAMS, size_t len, char* to_check) {
  for (size_t i = 0; i < len; i++) {
    if (to_check == PARMAS[i]) //Found
      return 1;
  }

  //Not found
  return 0;
}



/*  Filter_Params():
  Filter every given parameters into two categories:
  - Initialization parameters
  - Application parameters
  Returns them in given arrays, and their sizes.
  Every unrecognized parameters will be thrown away.

Requisites assumed:
  The pointed arrays have enough space to hold every parameters by themselves.
  The pointed arrays and size_t are already initialized and empty.
*/
void Filter_Params( //Parameters
		char** all_params, //The given command-line parameters
		size_t len, //The size of all_parameters
		char** *init_params, //Array to write initialization parameters
		size_t *init_size, //To store size of init_params
		char** *run_params, //Array to write Application parameters
		size_t *run_size) { //To store size of run_params

  //Iterating through the given parameters
  for (char** curr = &all_params; curr - &all_params < len; curr++) {
    size_t length = strlen(*curr_param);

    //Start checking with recognized parameters
    if (length < 2) //Skipping unformalized parameters
      continue;

    //Checking if it is a recognized parameter
    if (recognized(INIT_PARAMS, INIT_SIZE, curr_param)) { //Is an initialization parameter
      //Putting the parameter into its corresponding category
      *init_params = curr_param;
      init_params++;
      init_size++;
    }
    else if (recognized(RUN_PARAMS, RUN_SIZE, curr_param)) { //Is an Application parameter
      //Putting the parameter into its corresponding category
      *run_params = curr_param;
      run_params++;
      run_size++;
    }
  }
}





/*  Initialize():
  Initialize all systems necessary for the projects:
  - C Libraries
  - Memory Tracking
  - SDL Systems
  - Others

Requisites assumed:
  It must be the first and last time that this function is called.
  Any error in this function must terminate the entire processus.
  The given parameters are valid, and their number correct.
*/
void Initialize( //Parameters:
      char** params, //Given parameters for initialization
      size_t len) { //Size of params

  //Initialize the Memory Handler
  Memory_Init();



  //Defining necessary variables for SDL_Init
  Uint32 init_flags = SDL_INIT_TIMER|SDL_INIT_VIDEO;
  //Uint32 init_addons = 0; Not accepting additional flags for SDL_INIT

  //Initialize SDL and its sub-systems
  SDL_Init(init_flags);


  //Defining necessary variables for Window_Init
  char* title = "OCR Application";
  int x = SDL_WINDOWPOS_CENTERED;
  int y = SDL_WINDOWPOS_CENTERED;
  int width = 100;
  int height = 100;
  int window_flags = 0;
  int addons_window = 0;

  //Initialize SDL Main Project Window
  State.window = create_window(title,x,y,width,height,
        window_flags|addons_window);

  //Defining necessary variables for Renderer_Init
  int renderer_flags = 0;
  int addons_renderer = 0;

  //Initialize SDL Main Project Window's Renderer
  State.renderer = create_renderer(State.window,
        renderer_flags|addons_renderer);
}
