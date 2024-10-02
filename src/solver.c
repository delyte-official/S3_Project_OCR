/*

      ##############################################################
      #                                                            #
      #                           solver.c                         #
      #                                                            #
      #         This program is independant from the main.c.       #
      #     It executes from command line and returns in stdout.   #
      #                                                            #
      ##############################################################


This program solves a word search given a word
and the file containing a grid.
*/



////HEADERS Files
#include <stdio.h>
#include <err.h>

////END HEADERS


int main(int argc, char* argv[]) {
    //Verify that we have the correct number of arguments
    if (argc != 3) {
        errx(EXIT_FAILURE, "Incorrect number of arguments.");
    }
}
