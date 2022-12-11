#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "assembler.h"
#include "level_one.h"
#include "structs.h"
#include "externs.h"

#define MACRO_NAME 30 /* Maximum macro name length */

/*define macros struct*/
typedef struct macros_struct * macros_ptr;
typedef struct macros_struct
{
    char name[MACRO_NAME];
    /*we assume that macro doesn't have more than 10 lines, that each line is maximum 80 char*/
    char value[LINE_LEN * 10]; 
    macros_ptr next;
} macros;

void pre_assembler(FILE *file1, char *file_name);
void read_line_macro(char *line, int line_count);
void add_macro(macros_ptr * macros_list, char * macro_name);
macros_ptr get_macro(macros_ptr macros_list, char * macro_name);
void add_line(char *line, char *word);
void free_list(macros_ptr *macros_list);

