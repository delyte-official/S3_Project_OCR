# S3_Project_OCR: #FreeGalane

Application in C done as a project of the S3 EPITA semester. 
Inputs a word search game as an image, solves it,
and returns the solution graphically on a reconstruction.

# Application Guide

### Launch the Application
1. Run "make" (or "make all") to compile the application.
2. Launch with "./main".

### Key Sections
1. Display Section (left side):
    Shows the results of each solving step.
2. Control Pannel (top left):
    1. **Auto Complete**: Completes all steps instantly.
    2. **Next**: Performs next step.
    3. **Previous**: Shows previous step without resetting any progress.
    4. **Save Step**: Save current step ouput (eg. image, text files..).
    5. **Modify Image**: Allows image modifications before processing.
3. History (bottom left):
    Lists every processed step.
    You can jump back to or save any step.

### How to use
1. Import an image:
    - Click the import button or **Next**.
    - Click **Auto Complete** (note: input image will not be modifiable).
2. Advance steps:
    - Click **Next**.
    - Click **Auto Complete** for all steps.
3. Save output (eg. final result):
    - Click **Save Step**.
    - Click **Save Output** in the history tile.
4. Review previous steps:
    1. Click **Previous**.
    2. Modify from a previous step to re-run the process on new input.
