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
    2. **Next** (the Left Arrow button): Performs next step.
    3. **Previous** (the Right Arrow button): Shows previous step without resetting any progress.
    4. **Save Step**: Save current step ouput (eg. image, text files..).
    5. **Modify/Edit** (image, text, data): Allows modifications of the current step before the next processing.
3. History (left):
    Lists every processed step.

### How to use
1. Import an image:
    - Click the import button or **Next**.
    - Click **Auto Complete** (note: input image will not be modifiable).
2. Advance steps:
    - Click **Next** (Left Arrow button).
    - Click **Auto Complete** for all steps.
3. Save output (eg. final result):
    - Click **Save Step**.
4. Review previous steps:
    1. Click **Previous** (Right Arrow button).
    2. Modify from a previous step to re-run the process on new input.


# Visuals

## Input/Output comparisons

### Image LEVEL 1
* Basic grid
* Normal orientation
* No other elements
* Clear colours
* Word list on the right of the grid
![Input/Output comparison of image LEVEL 1](assets/input_output_comparison_level1.png)

### Image LEVEL 3
* Complex grid
* Word list on multiple columns and lines
* Other big FULL elements (drawings, logos, etc.)
* Other small elements (letters, sentences, instructions, sources, etc.)
![Input/Output comparison of image LEVEL 3](assets/input_output_comparison_level3.png)

### Image LEVEL 4
* Low contrast between grid and background
* Non-uniform spacing with letters in cells, grid and between other letters
* Other big EMPTY elements (drawed with outlines only)
![Input/Output comparison of image LEVEL 4](assets/input_output_comparison_level4.png)

## Full process demonstration

<img src="assets/s3_freegalane_ocr_full_demonstration.gif" width="100%">
