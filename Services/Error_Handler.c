/*


        ###########################################################
        #                                                         #
        #                     Error Handler                       #
        #                                                         #
        #             Links and handle given error                #
        #            from any SDL involved services.              #
        #                                                         #
        ###########################################################

This file only serves two purposes:

- Linker:
Error_Handler allows every Services C code source files to call case-specific
errors related to SDL systems.

- Handler:
Error_Handler redirects the received custom errors and send the appropriate
one to the standard error output and standard output. It computes the
important details to enhance the developpers' debugging sessions.

*/


////HEADERS Files
//C Libraries



