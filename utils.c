#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include "level_one.h"
#include "level_two.h"
#include "assembler.h"
#include "externs.h"
#include "utils.h"

/* This function extracts bits, by the given position of the bit-sequence */
unsigned int extract_bits(unsigned int word, int start, int end)
{
    unsigned int result;
    int length = end - start + 1;
    unsigned int mask = (int) pow(2, length) - 1; /* creates a mask with the length above*/

    mask <<= start; /* moves the mask to the extracting position */
    result = word & mask; /* extracts the bits */
    result >>= start; /* moves the result to the right position */
    return result;
}

/*This function converts a word to our base 32 (2 digits string)*/
char *convert_to_base_32(unsigned int num)
{
    char *base32_seq = (char *) malloc(BASE32_SEQUENCE_LENGTH); /*allocates memory to the base32 sequence*/
    base32_seq[0] = base32[extract_bits(num, 5, 9)]; /*Converts to base 32 by taking the 5 right binary digits*/
    base32_seq[1] = base32[extract_bits(num, 0, 4)];/*Converts to base 32 by taking the 5 left binary digits*/
    base32_seq[2] = '\0'; /*adds null terminator*/
    return base32_seq;
}


/*This function checks if given string is a number (can contain + or - signs)*/
bool is_number(char *seq)
{
    if(end_of_line(seq)) 
        return FALSE;
    if(*seq == '+' || *seq == '-')
    {
        seq++;
        if(!isdigit(*seq++))
            return FALSE; /* sign must be followed by a number */
    }
    while(!end_of_line(seq))
    {
        if(!isdigit(*seq++))
            return FALSE;
    }
    return TRUE;
}


/*This function checks if a given string is valid*/
bool is_string(char *str)
{
    if(str == NULL)
        return FALSE;

    if (*str == '"') /*string must start with " */
        str++;
    else
        return FALSE;

    while (*str && *str != '"')
    {
        str++;
    }

    if (*str != '"') /* string must end with " */
        return FALSE;

    str++;
    if (*str != '\0') /*There cannot be any characters after the second " */
        return FALSE;

    return TRUE;
}

/*This function inserts ARE bits to the given info bit-sequence*/
unsigned int insert_are(unsigned int info, int are)
{
    return (info << BITS_IN_ARE) | are; /*inserts ARE bits to the info bit-sequence*/
}

/*This function creates a file name with the given extenstion*/
char *create_file_name(char *original, int type)
{
    /*Allocate memory for the modified filename*/
    char *modified = (char *) malloc(strlen(original) + MAX_EXTENSION_LENGTH); 
    if(modified == NULL)
    {
        fprintf(stderr, "Error: couldn't allocate memory for filename\n");
        exit(ERROR);
    }

    strcpy(modified, original); /* copy original filename */

    /* concatenating the given file extension */

    switch (type)
    {
        case FILE_INPUT:
            strcat(modified, ".as");
            break;
            
       	case FILE_AM:
            strcat(modified, ".am");
            break;

        case FILE_OBJECT:
            strcat(modified, ".ob");
            break;

        case FILE_ENTRY:
            strcat(modified, ".ent");
            break;

        case FILE_EXTERN:
            strcat(modified, ".ext");

    }
    return modified;
}


/*This function inserts word to instruction memory*/
void insert_word_to_instruction(unsigned int word)
{
    instructions[ic++] = word;
}

/*This function returns if there was an error by checking the global variable "error_mode*/
int is_error()
{
    return error_mode != NO_ERROR;
}

/*This function prints the relevant error according to the "error_mode" variable,
 with the line number where the error was found*/
void print_error(int line_num)
{
    fprintf(stderr, "Error found in line %d: ", line_num);

    switch (error_mode)
    {
        case NOT_DOT_OR_LETTER:
            fprintf(stderr, "First character isn't a dot or a letter.\n");
            break;

        case MACRO_NAME_INVALID:
            fprintf(stderr, "Macro name can't be command or directive name.\n");
            break;

        case LABEL_NAME_ALREADY_EXISTS:
            fprintf(stderr, "Label name is already exists.\n");
            break;

        case LABEL_IS_TOO_LONG:
            fprintf(stderr, "Label name is too long (Maximum chars allowed: %d).\n", LABEL_LEN);
            break;

        case LABEL_NAME_ISNT_ALPHANUMERIC:
            fprintf(stderr, "Label name must contain only alphanumeric characters.\n");
            break;

        case LABEL_NAME_SAME_AS_COMMAND:
            fprintf(stderr, "Label name has to be different than a command name.\n");
            break;

        case LABEL_NAME_SAME_AS_REGISTER:
            fprintf(stderr, "Label name has to be different than a register name.\n");
            break;

        case LABEL_ISNOT_FOLLOWED_BY_CMD_OR_DIR:
            fprintf(stderr, "Label isn't followed by a command or a directive.\n");
            break;

        case DIRECTIVE_MISSING_PARAMETERS:
            fprintf(stderr, "Directive missing parameters.\n");
            break;

        case DIRECTIVE_INCORRECT_NUMBER_PARAMETERS:
            fprintf(stderr, "Directive has incorrect number of parameters.\n");
            break;

        case DATA_TOO_MANY_COMMAS:
            fprintf(stderr, ".data directive has too many commas in a row.\n");
            break;

        case DATA_MISSING_NUMBER:
            fprintf(stderr, ".data directive missing a number parameter.\n");
            break;

        case DATA_MISSING_COMMA_AFTER_NUMBER:
            fprintf(stderr, ".data directive missing a comma after a number parameter.\n");
            break;

        case DATA_ENDED_WITH_COMMA:
            fprintf(stderr, ".data directive can't end with a comma.\n");
            break;

        case STRING_MORE_THAN_ONE_PARAMETER:
            fprintf(stderr, ".string directive has more than one parameter.\n");
            break;

        case STRING_PARAMETER_IS_INVALID:
            fprintf(stderr, ".string directive parameter is invalid.\n");
            break;

        case STRUCT_FIRST_PARAMETER_INVALID:
            fprintf(stderr, ".struct directive first parameter isn't a number.\n");
            break;

        case STRUCT_MISSING_STRING:
            fprintf(stderr, ".struct directive missing a string parameter.\n");
            break;

        case STRUCT_PARAMETER_NOT_STRING:
            fprintf(stderr, ".struct directive second parameter isn't a string.\n");
            break;

        case STRUCT_TOO_MANY_PARAMETERS:
            fprintf(stderr, ".struct directive has too many parameters.\n");
            break;

        case STRUCT_MISSING_COMMA:
            fprintf(stderr, ".struct directive missing a comma between two parameters.\n");
            break;

        case EXTERN_NOT_FOLLOWED_BY_LABEL:
            fprintf(stderr, ".extern directive isn't followed by a label.\n");
            break;

        case EXTERN_LABEL_INVALID:
            fprintf(stderr, ".extern directive label is invalid.\n");
            break;

        case EXTERN_TOO_MANY_PARAMETERS:
            fprintf(stderr, ".extern directive has too many parameters.\n");
            break;

        case COMMAND_OR_DIRECTIVE_NOT_FOUND:
            fprintf(stderr, "Command or directive not found.\n");
            break;

        case COMMAND_INVALID_SYNTAX:
            fprintf(stderr, "Command syntax is invalid.\n");
            break;

        case COMMAND_TOO_MANY_PARAMETERS:
            fprintf(stderr, "Command has too many parameters.\n");
            break;

        case COMMAND_METHOD_INVALID:
            fprintf(stderr, "Command paramter addressing method is invalid.\n");
            break;

        case COMMAND_INCORRECT_NUMBER_OF_PARAMTERS:
            fprintf(stderr, "Command has incorrect number of parameters.\n");
            break;

        case COMMAND_PARAMETERS_METHOD_INVALID:
            fprintf(stderr, "Command parameters addressing methods is invalid.\n");
            break;

        case ENTRY_IS_NOT_FOLLOWED_BY_LABEL:
            fprintf(stderr, ".entry directive isn't followed by an existing label.\n");
            break;

        case ENTRY_CANNOT_APPLY_TO_EXTERN:
            fprintf(stderr, ".entry directive can't apply to an external label.\n");
            break;

        case COMMAND_LABEL_INVALID:
            fprintf(stderr, "Command label does not exist.\n");
            break;

        case FILE_CANT_BE_OPEN:
            fprintf(stderr, "The file you requested can't be opened.\n");
    }
}


/*This function copies the next word of the line to the destination array 
and then returns a pointer to the first char of the next word*/
char *next_word_list(char *dest, char *line)
{
    char *temp = dest;

    if(end_of_line(line))
    {
        dest[0] = '\0';
        return NULL;
    }

    if(isspace(*line))
        line = skip_whitespace(line);

    if(*line == ',')
    {
        strcpy(dest, ",");
        return ++line;
    }
    /*copying the word until we reach comma, whitespace or end of line*/
    while(!end_of_line(line) && *line != ',' && !isspace(*line)) 
    {
        *temp = *line;
        temp++;
        line++;
    }
    *temp = '\0';

    return line;
}

/*This function copies the next string to the destination array 
and then returns a pointer to the first char of the next string*/
char *next_word_string(char *dest, char *line)
{
    char temp[LINE_LEN];
    line = next_word_list(dest, line);
    if(*dest != '"')
        return line;
    while(!end_of_line(line) && dest[strlen(dest) - 1] != '"')
    {
        line = next_word_list(temp, line);
        if(line) strcat(dest, temp);
    }
    return line;
}

/*This function checks if we reached to end of line*/
int end_of_line(char *line)
{
    return line == NULL || *line == '\0' || *line == '\n';
}

/* This function returns a pointer to the first character of the next word in line */
char *next_word(char *seq)
{
    if(seq == NULL)
    	return NULL;
    while(!isspace(*seq) && !end_of_line(seq))
    	seq++;
    seq = skip_whitespace(seq);
    if(end_of_line(seq))
    	return NULL;
    return seq;
}

/* This function copies the next word to a destination string */
void copy_word(char *dest, char *line)
{
    int i = 0;
    if(dest == NULL || line == NULL)
        return;
    /* copying word until it reaches space or end of line */
    while(i < LINE_LEN && !isspace(line[i]) && line[i] != '\0') 
    {
        dest[i] = line[i];
        i++;
    }
    dest[i] = '\0';
}

/* This function gets a string index from string array  */
int get_index(char *word, const char *arr[], int n)
{
    int i;
    for(i = 0; i < n; i++)
        if (strcmp(word, arr[i]) == 0)
            return i;
    return NOT_FOUND;
}


/*This function checks if given word is a register name*/
bool is_register(char *word)
{
    return strlen(word) == REGISTER_LENGTH && word[0] == 'r' && word[1] >= '0' && word[1] <= '7';
}

/*This function checks if given word is a directive name*/
int get_directive(char *word)
{
    if(word == NULL || *word != '.') 
        return NOT_FOUND;
    return get_index(word, directives, NUM_DIRECTIVES);
}

/*This function checks if given word is a command name*/
int get_command(char *word)
{
    int word_len = strlen(word);
    if(word_len > MAX_COMMAND_LENGTH || word_len < MIN_COMMAND_LENGTH)
        return NOT_FOUND;
    return get_index(word, commands, NUM_COMMANDS);
}

/*This function checks if given word is a valid macro name*/
bool is_macro_valid(char *word)
{
    int i,j;
    for(i = 0; i < NUM_COMMANDS; i++) /*checks if word isn't a command*/
    {
        if(strcmp(word, commands[i]) == 0)
            return FALSE;
    }
        for(j = 0; j < NUM_DIRECTIVES; j++) /*checks if word isn't a directive*/
    {
        if(strcmp(word, directives[j]) == 0)
            return FALSE;
    }
    return TRUE;
}

/* This function skips whitespaces and return a pointer to the first character that isn't whitespace*/
char *skip_whitespace(char *ch)
{
    if(ch == NULL) return NULL;
    while (isspace(*ch))
        ch++;
    return ch;
}

/*This function ignores whitespaces, blank lines and comments*/
int ignore(char *line)
{
    line = skip_whitespace(line);
    return *line == ';' || *line == '\0' || *line == '\n';
}
