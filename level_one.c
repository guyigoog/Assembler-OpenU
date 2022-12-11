#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "externs.h"
#include "level_one.h"

/*The main function of level one */
void level_one(FILE *file)
{
    char line[LINE_LEN];
    int line_number = 1; /*starts count from line 1*/
    ic = 0;
    dc = 0;

    while (fgets(line, LINE_LEN, file) != NULL) /* Read lines until end of file */
    {
        error_mode = NO_ERROR; /*resets error mode*/
        if (!ignore(line)) /*ignores white spaces and comments*/
        {
            read_line(line); 
        }
        if(is_error()) /*checks if there was an error*/
        {
            was_error = TRUE;
            print_error(line_number); /*prints error*/
        }
        line_number++; /*counts line*/
    }
    offset_label_address(symbols_table, MEMORY_START, FALSE); /*offsets symbols_table address by 100*/
    offset_label_address(symbols_table, ic + MEMORY_START, TRUE); /*offsets symbols_table address by 100+ic*/
}

/*This function reads line from the file and handle each line according to its type*/
void read_line(char *line)
{
    int directive_type = UNKNOWN_TYPE;/*resets directive type*/
    int command_type = UNKNOWN_COMMAND;/*resets command type*/
    bool label = FALSE;
    label_ptr label_node = NULL;
    char current_word[LINE_LEN];
    
    line = skip_whitespace(line);
    if(end_of_line(line)) /*checks if the line is empty*/
        return;
    if(!isalpha(*line) && *line != '.')/*checks if line doesn't start with a letter or a dot*/
    { 
        error_mode = NOT_DOT_OR_LETTER;
        return;
    }

    copy_word(current_word, line);
    if(is_label(current_word, COLON)) /*checks if the word is a label*/
    { 
        label = TRUE;
        label_node = add_label(&symbols_table, current_word, 0, FALSE, FALSE); /*adds word to labels table*/
        if(label_node == NULL)
            return;
        line = next_word(line);
        if(end_of_line(line)) /*checks for empty label*/
        {
            error_mode = LABEL_ISNOT_FOLLOWED_BY_CMD_OR_DIR; 
            return;
        }
        copy_word(current_word, line);
    } 

    if(is_error())  /*checks if there was an error*/
        return;

    if((directive_type = get_directive(current_word)) != NOT_FOUND) /*checks if word is a directive*/
    {
        if(label)
        {
            if(directive_type == EXTERN || directive_type == ENTRY) /*check directive type*/
            {
                delete_label(&symbols_table, label_node->name); /*deletes label from symbols table*/
                label = FALSE;
            }
            else
                label_node -> address = dc; /*sets label address*/
        }
        line = next_word(line);
        handle_directive(directive_type, line); /*handle directive*/
    }

    else if ((command_type = get_command(current_word)) != NOT_FOUND) /*checks if word is a command*/
    {
        if(label)
        {
            label_node -> is_instruction_sentence = TRUE; /*sets label as instruction sentence*/
            label_node -> address = ic; /*sets label address*/
        }
        line = next_word(line);
        handle_command(command_type, line); /*handle command*/
    }

    else
    {
        error_mode = COMMAND_OR_DIRECTIVE_NOT_FOUND; /*error if word is not a command or directive*/
    }

}
/*This function checks if directive is valid (have at least one parameter) 
and calls the handling function according to its type*/
int handle_directive(int type, char *line)
{
    if(line == NULL || end_of_line(line)) /* Directives must have at least one parameter */
    {
        error_mode = DIRECTIVE_MISSING_PARAMETERS;
        return ERROR;
    }
    switch (type)
    {
        case DATA:
            return handle_data_directive(line);
        case STRING:
            return handle_string_directive(line);
        case STRUCT:
            return handle_struct_directive(line);
        case ENTRY:
            if(!end_of_line(next_word(line)))
            {
                error_mode = DIRECTIVE_INCORRECT_NUMBER_PARAMETERS;
                return ERROR;
            }
            break;
        case EXTERN:
            return handle_extern_directive(line);
    }
    return NO_ERROR;
}
/*This function checks if command parameters received and handle each command by his type and demands*/
int handle_command(int type, char *line)
{
    bool is_first = FALSE, is_second = FALSE; /*bool variables to check if parameters received*/
    int first_method, second_method; /*variables to store parameters addressing methods*/
    char first_parameter[20], second_parameter[20]; 

    line = next_word_list(first_parameter, line);
    if(!end_of_line(first_parameter)) /* checks if first parameters isn't empty */
    {
        is_first = TRUE;
        line = next_word_list(second_parameter, line);
        if(!end_of_line(second_parameter)) /* checks if second parameters isn't empty */
        {
            if(second_parameter[0] != ',') /* Two parameters are seperated by a comma */
            {
                error_mode = COMMAND_INVALID_SYNTAX;
                return ERROR;
            }

            else
            {
                line = next_word_list(second_parameter, line); 
                if(end_of_line(second_parameter)) /* checks if second parameters isn't empty */
                {
                    error_mode = COMMAND_INVALID_SYNTAX;
                    return ERROR;
                }
                is_second = TRUE;
            }
        }
    }
    line = skip_whitespace(line);
    if(!end_of_line(line)) /* checks if line isn't empty, meaning that command has too many parameters*/
    {
        error_mode = COMMAND_TOO_MANY_PARAMETERS;
        return ERROR;
    }

    if(is_first)
        first_method = get_method(first_parameter); /* calls a function to get addressing method of the first parameter */
    if(is_second)
        second_method = get_method(second_parameter); /* calls a function to get addressing method of the second parameter */

    if(!is_error())
    {
        if(cmd_isvalid_parameters(type, is_first, is_second)) /* checks if number of parameters is valid for the command */
        {
            if(cmd_isvalid_methods(type, first_method, second_method)) /* checks if addressing methods are valid for the command */
            {
                /* inserts first word of the command to the memory */
                insert_word_to_instruction(build_first_word(type, is_first, is_second, first_method, second_method)); 
                /* increases the ic by the number of additional words */
                ic += count_additional_words(is_first, is_second, first_method, second_method); 
            }
            else
            {
                error_mode = COMMAND_PARAMETERS_METHOD_INVALID;
                return ERROR;
            }
        }
        else
        {
            error_mode = COMMAND_INCORRECT_NUMBER_OF_PARAMTERS;
            return ERROR;
        }
    }

    return NO_ERROR;
}
/*This function handles string type directive, and inserts it to data*/
int handle_string_directive(char *line)
{
    char word[LINE_LEN];

    line = next_word_string(word, line);
    if(!end_of_line(word) && is_string(word)) /*checks if word exists and valid*/
    {
        line = skip_whitespace(line);
        if(end_of_line(line))
        {
            word[strlen(word) - 1] = '\0';
            insert_string_to_data(word + 1); /*inserts word to data*/
        }
        else
        {
            error_mode = STRING_MORE_THAN_ONE_PARAMETER;
            return ERROR;
        }
    }
    else
    {
        error_mode = STRING_PARAMETER_IS_INVALID;
        return ERROR;
    }
    return NO_ERROR;
}
/*This function handles struct type directive*/
int handle_struct_directive(char *line)
{
    char word[LINE_LEN];
    line = next_word_list(word, line);
    if(!end_of_line(word) && is_number(word)) /*checks if the first word is a number*/
    {
        insert_num_to_data(atoi(word)); /* inserts number to data */
        line = next_word_list(word, line); /* get the next word */

        if(!end_of_line(word) && *word == ',') /* checks if there is a comma between the parameters */
        { 
            line = next_word_string(word, line); /* Get the next word */
            if(!end_of_line(word)) 
            { 
                if (is_string(word)) 
                {
                    word[strlen(word) - 1] = '\0';
                    insert_string_to_data(word + 1); /*inserts word to data*/
                } 
                else 
                {
                    error_mode = STRUCT_PARAMETER_NOT_STRING;
                    return ERROR;
                }
            }
            else
            {
                error_mode = STRUCT_MISSING_STRING;
                return ERROR;
            }
        }
        else
        {
            error_mode = STRUCT_MISSING_COMMA;
            return ERROR;
        }
    }
    else
    {
        error_mode = STRUCT_FIRST_PARAMETER_INVALID;
        return ERROR;
    }
    if(!end_of_line(next_word_list(word, line)))
    {
        error_mode = STRUCT_TOO_MANY_PARAMETERS;
        return ERROR;
    }
    return NO_ERROR;
}
/*This function handles data type directive*/
int handle_data_directive(char *line)
{
    char word[20];
    /*bool variable that monitors if there is a number or a comma according to demands*/
    bool number = FALSE, comma = FALSE; 

    while(!end_of_line(line))
    {
        line = next_word_list(word, line);

        if(strlen(word) > 0) /* Checks if word isn't empty */
        {
            if (!number) { /* checks if a number isn't appeared yet */
                if (!is_number(word)) 
                { 
                    error_mode = DATA_MISSING_NUMBER;
                    return ERROR;
                }
                else /*current word is a number*/
                {
                    number = TRUE; /*mark number bool*/
                    comma = FALSE;/*resets comma's bool*/
                    insert_num_to_data(atoi(word)); /* inserts number to data */
                }
            }
            else if (*word != ',') /* checks if a number isn't followed by a comma */
            {
                error_mode = DATA_MISSING_COMMA_AFTER_NUMBER;
                return ERROR;
            }

            else /* too many commas */
            {
                if(comma) 
                {
                    error_mode = DATA_TOO_MANY_COMMAS;
                    return ERROR;
                }
                else 
                {
                    comma = TRUE;/*mark comma bool*/
                    number = FALSE;/*resets number bool*/
                }
            }

        }
    }
    if(comma == TRUE)
    {
        error_mode = DATA_ENDED_WITH_COMMA;
        return ERROR;
    }
    return NO_ERROR;
}

/* This function inserts number to data */
void insert_num_to_data(int num)
{
    data[dc++] = (unsigned int) num;
}

/*This function inserts string to data*/
void insert_string_to_data(char *str)
{
    while(!end_of_line(str))
    {
        data[dc++] = (unsigned int) *str; /* inserts char to data array */
        str++;
    }
    data[dc++] = '\0'; /* inserts null terminator to the end of data array */
}

/*This function returns the addressing method of the parameter*/
int get_method(char * parameter)
{
    char *struct_field; /* variable to store struct directive type */

    if(end_of_line(parameter)) 
        return NOT_FOUND;

    if (*parameter == '#') /*checks if first word is #*/
    {
        parameter++;
        if (is_number(parameter)) /*if parameter is a number, returns immediate method*/
            return METHOD_IMMEDIATE;
    }
    else if (is_register(parameter)) /*returns register method*/
        return METHOD_REGISTER;
    else if (is_label(parameter, FALSE)) /*checks if its a label but doesn't have a colon then returns direct method*/
        return METHOD_DIRECT;
    else if (is_label(strtok(parameter, "."), FALSE)) /*checks if its a label by spliting word until dot char*/
    {
        struct_field = strtok(NULL, ""); /* get the rest of the word */
        /* checks if the rest of the word is '1' or '2' then returns struct method*/
        if (strlen(struct_field) == 1 && (*struct_field == '1' || *struct_field == '2')) 
            return METHOD_STRUCT;
    }
    error_mode = COMMAND_METHOD_INVALID;
    return NOT_FOUND;
}
/*This function checks if command is valid,
 by checking if the methods received are correct according to each command demands*/
bool cmd_isvalid_methods(int type, int first_method, int second_method)
{
    switch (type)
    {
        /*Source: 0,1,2,3*/
        /*Destination: 1,2,3*/
        case MOV:
        case ADD:
        case SUB:
            return (first_method == METHOD_IMMEDIATE || first_method == METHOD_DIRECT || first_method == METHOD_STRUCT 
            || first_method == METHOD_REGISTER) && (second_method == METHOD_DIRECT || second_method == METHOD_STRUCT || 
            second_method == METHOD_REGISTER);
        /*Source: None*/
        /*Destination: 1,2,3*/
        case NOT:
        case CLR:
        case INC:
        case DEC:
        case JMP:
        case BNE:
        case GET:
        case JSR:
            return first_method == METHOD_DIRECT ||
                   first_method == METHOD_STRUCT ||
                   first_method == METHOD_REGISTER;
        /*Source: 1,2*/
        /*Destination: 1,2,3*/
        case LEA:
            return (first_method == METHOD_DIRECT || first_method == METHOD_STRUCT) && 
            (second_method == METHOD_DIRECT || second_method == METHOD_STRUCT || second_method == METHOD_REGISTER);
        /*Source: None*/
        /*Destination: 0,1,2,3*/
        case PRN:
        /*Source: 0,1,2,3*/
        /*Destination: 0,1,2,3*/
        case CMP:
        /*Source: None*/
        /*Destination: None*/
        case RTS:
        case HLT:
            return TRUE;
    }
    return FALSE;
}
/*This function checks if command is valid,
 by checking if the number of parameters received are correct to each command demands*/
bool cmd_isvalid_parameters(int type, bool first_parameter, bool second_parameter)
{
    switch (type)
    {
        /* These commands have 2 parameters */
        case MOV:
        case CMP:
        case ADD:
        case SUB:
        case LEA:
            return first_parameter && second_parameter;

        /* These commands have only one parameter */
        case NOT:
        case CLR:
        case INC:
        case DEC:
        case JMP:
        case BNE:
        case GET:
        case PRN:
        case JSR:
            return first_parameter && !second_parameter;

        /* These commands doesn't have any parameters */
        case RTS:
        case HLT:
            return !first_parameter && !second_parameter;
    }
    return FALSE;
}

/*This function counts additional words*/
int count_additional_words(int is_first, int is_second, int first_method, int second_method)
{
    int count = 0;
    if(is_first)
        count += ((first_method == METHOD_STRUCT) ? 2 : 1);
    if(is_second)
        count += ((second_method == METHOD_STRUCT) ? 2 : 1);
    /*checks if both methods are register methods, then they share the additional words*/
    if(is_first && is_second && first_method == METHOD_REGISTER && second_method == METHOD_REGISTER) 
        count--;
    return count;
}

/*This function build the first word of a command*/
unsigned int build_first_word(int type, int is_first, int is_second, int first_method, int second_method)
{
    unsigned int word = 0;
    word = type; /*inserts opcode*/
    word <<= BITS_IN_METHOD; /* saving space for the first addressing method */
    if(is_first && is_second)/* inserts first parameter */
        word |= first_method;

    word <<= BITS_IN_METHOD; /* saving space for the second addressing method */
    if(is_first && is_second)/* inserts second parameter */
        word |= second_method;

    else if(is_first) /*if there was only one parameter, inserts it*/
        word |= first_method;

    word = insert_are(word, ABSOLUTE); /* inserts ARE mode to word */
    return word;
}

/*This function handles extern type directives*/    
int handle_extern_directive(char *line)
{
    char word[LABEL_LEN];
    copy_word(word, line);
    if(end_of_line(word)) /* word is empty, no label found */
    {
        error_mode = EXTERN_NOT_FOLLOWED_BY_LABEL;
        return ERROR;
    }
    if(!is_label(word, FALSE))
    {
        error_mode = EXTERN_LABEL_INVALID;
        return ERROR;  
    }  

    line = next_word(line);
    if(!end_of_line(line))
    {
        error_mode = EXTERN_TOO_MANY_PARAMETERS;
        return ERROR;
    }
    if(add_label(&symbols_table, word, EXTERNAL_DEFAULT_ADDRESS, TRUE) == NULL) /*add label to the symbols table*/
        return ERROR;
    return is_error();
}

/*This function checks if given word is a label*/
bool is_label(char *word, int colon)
{
    bool has_digits = FALSE;
    int i;
    int word_len = strlen(word);
    /*checks if label name isn't too short*/
    if(word == NULL || word_len < (colon ? MINIMUM_LABEL_LENGTH_WITH_COLON: MINIMUM_LABEL_LENGTH_WITHOUT_COLON)) 
        return FALSE;

    if(colon && word[word_len - 1] != ':') /*checks if there is a colon at the end of the word, if necessary*/
        return FALSE;

    if (word_len > LABEL_LEN) /*checks if label name is too long*/
    {
        if(colon)
            error_mode = LABEL_IS_TOO_LONG;
        return FALSE;
    }
    if(!isalpha(*word)) /*checks if first char is non-alphanumeric*/
    {
        if(colon) 
            error_mode = LABEL_NAME_ISNT_ALPHANUMERIC;
        return FALSE;
    }

    if (colon) 
    {
        word[word_len - 1] = '\0';
        word_len--;
    }
	
    /*checks if label name contain non-alphanumeric chars*/
    for(i = 1; i < word_len; i++)
    {
        if(isdigit(word[i]))
            has_digits = TRUE;
        else if(!isalpha(word[i])) 
        {
            if(colon)
                error_mode = LABEL_NAME_ISNT_ALPHANUMERIC;
            return FALSE;
        }
    }
    if(!has_digits) /*if label name doesn't have digits then we check if its a command name*/
    {
        if (get_command(word) != NOT_FOUND) {
            if(colon)
                error_mode = LABEL_NAME_SAME_AS_COMMAND; 
            return FALSE;
        }
    }

    if(is_register(word)) /* checks if label name isn't register name */
    {
        if(colon)
            error_mode = LABEL_NAME_SAME_AS_REGISTER;
        return FALSE;
    }
    return TRUE;
}
