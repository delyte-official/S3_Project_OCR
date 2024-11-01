# S3_Project_OCR: #FreeGalane

Application in C done as a project of the S3 EPITA semester. 
Inputs a word search game as an image, solves it,
and returns the solution graphically on a reconstruction.

# How to use the Application

### Compile and Run the Application
Calling "make" (eventually "make all") will compile the entire application.
Calling "./main" afterwards will launch the application.

Example case:
> make

> ./main

### Use the Application
The two most important part of the Application are:

1. The display section (on the left):
    It will display the results of every steps of the solving process.
    You can start by clicking on the Import Button to import an image.
2. The control pannel (on the top left):
    1. "Auto Complete" button:
        Will perform every steps instantly, unless stopped by the user.
    2. "Next" button:
        Performs the next step, if there is one.
    3. "Previous" button:
        Show the previous step without erasing the progress.
    4. "Save Step" button:
        Save the elements of the current step (eg. image, text files..).
    5. "Modify Image" button:
        Only appears after importing an image.
        Allows the user to modify the input image before processing.
3. The history (on the bottom left):
    Shows and records every processed step.
    Allows the user to jump back to a previous step or save it.
