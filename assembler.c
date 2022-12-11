/*This project created by Neta Hayat and Guy Perry for System Programming Laboratory 20465 Course
 at The Open University of Israel, Enjoy :)
This is an assembler program in C language, this program translate assembly to machine language accroding to the course instructions.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "level_one.h"
#include "level_two.h"
#include "pre_assembler.h"
#include "externs.h"
#include "utils.h"
#include "assembler.h"

/*Defining global variables (extern varibels) because they are initialized in this file */
unsigned int data[MACHINE_RAM];
unsigned int instructions[MACHINE_RAM];
int ic;
int dc;
int error_mode;
label_ptr symbols_table;
ext_ptr ext_list;
bool entry_exists, extern_exists, was_error;
/*Define our special base*/
const char base32[32] = {
        '!', '@', '#', '$', '%', '^', '&', '*', '<', '>', 'a', 'b', 'c',
        'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
        'q', 'r', 's', 't', 'u', 'v'};
/*Define an array of commands*/
const char *commands[] = {
        "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne",
        "get", "prn", "jsr", "rts", "hlt"
};
/*Define an array of directives*/
const char *directives[] = {
        ".data", ".string", ".struct", ".entry", ".extern"
};

/*This function resets global variables*/
void reset_globals()
{
    symbols_table = NULL;
    ext_list = NULL;

    entry_exists = FALSE;
    extern_exists = FALSE;
    was_error = FALSE;
}
/*This is the main function of the program
**This function runs and manages all the levels of the program
*/
int main(int argc, char *argv[]){
    int i;
    char *input_filename,*aftermacro_filename;
    FILE *file1, *file2;

    for(i = 1; i < argc; i++) /*Loop all given files*/
    {	
        input_filename = create_file_name(argv[i], FILE_INPUT); /*Adds .as format to the given file name*/ 
        file1 = fopen(input_filename, "r");
        if(file1 != NULL) { /* Checks if file exist */
            printf("Assembeling %s\n\n", input_filename);
            reset_globals();
            pre_assembler(file1, argv[i]); /*Starts pre assembling proccess (Handle macros)*/
            aftermacro_filename = create_file_name(argv[i], FILE_AM); /*Adds .am format to the given file name*/
        	file2 = fopen(aftermacro_filename, "r");
            rewind(file2);
            level_one(file2); /*Starts level one of the assembeling proccess*/
            if (!was_error) { /* Checks if there was an error before continue to the next level */
                rewind(file2);
                level_two(file2, argv[i]); /*Starts level two of the assembeling proccess*/
            }

            printf("\n\nFinished assembeling %s\n\n", input_filename);
        }
        else
            print_error(FILE_CANT_BE_OPEN);
        free(input_filename);/*Free used memory*/
        free(aftermacro_filename);
    }
	return 0;
}
