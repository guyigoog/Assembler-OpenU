#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "externs.h"
#include "utils.h"
#include "structs.h"
#include "level_one.h"
#include "level_two.h"

/*The main function of level two */
void level_two(FILE *fp, char *filename)
{
    char line[LINE_LEN];
    int line_num = 1; /*starts count from line 1*/
    ic = 0;

    while(fgets(line, LINE_LEN, fp) != NULL) /* Read lines until end of file */
    {
        error_mode = NO_ERROR; /*resets error mode*/
        if(!ignore(line)) /*ignores white spaces and comments*/
            read_line_phase_two(line);
        if(is_error()) { /*checks if there was an error*/
            was_error = TRUE;
            print_error(line_num); /*prints error*/
        }
        line_num++;
    }
    if(!was_error) /*if there was no error write output files*/
        write_output_files(filename);

    free_labels(&symbols_table); /* Free memory */
    free_ext(&ext_list); /* Free memory */
}

/*This function reads line from the file and handle each line according to its type*/
void read_line_phase_two(char *line)
{
    int dir_type, command_type;
    char current_word[LINE_LEN];
    line = skip_whitespace(line);
    if(end_of_line(line))
        return;
    copy_word(current_word, line);
    if(is_label(current_word, COLON)) /*if word is label, skip it*/
    {
        line = next_word(line);
        copy_word(current_word, line);
    }

    if((dir_type = get_directive(current_word)) != NOT_FOUND) /* checks for entry directive*/
    {
        line = next_word(line);
        if(dir_type == ENTRY)
        {
            copy_word(current_word, line);
            change_entry_in_label(symbols_table, current_word); /* change entry field to TRUE for this label*/
        }
    }

    else if ((command_type = get_command(current_word)) != NOT_FOUND) /*checks if current word is a command, then handle it*/
    {
        line = next_word(line);
        handle_command_level_two(command_type, line);
    }
}

/* This function creates and write the necessary final output files*/
int write_output_files(char *original)
{
    FILE *file;
    file = open_file(original, FILE_OBJECT);

    write_output_ob(file);

    if(entry_exists) /*if entry exist then we write ent output file*/
    { 
        file = open_file(original, FILE_ENTRY);
        write_output_ent(file);
    }

    if(extern_exists) /*if extern exist then we write ext output file*/
    {
        file = open_file(original, FILE_EXTERN);
        write_output_ext(file);
    }

    return NO_ERROR;
}

/*This function writes the .ob file according to these instructions:
  The first line of the file is the size of IC memory and DC memory.
  The rest of the file has two columns the first one contains the address and the second one the word in the memory.*/
void write_output_ob(FILE *fp)
{
    unsigned int address = MEMORY_START;
    int i;
    char *param1 = convert_to_base_32(ic), *param2 = convert_to_base_32(dc); /*converts ic and dc to our base 32*/

    fprintf(fp, "%s\t%s\n", param1, param2); /*writes the first line as we mentioned above*/
    free(param1); /*free the memory*/
    free(param2); /*free the memory*/
    /*loop through instructions memory and converts address and word to our base 32, as mentioned above*/
    for (i = 0; i < ic; address++, i++) 
    {
        param1 = convert_to_base_32(address);
        param2 = convert_to_base_32(instructions[i]);
        fprintf(fp, "%s\t%s\n", param1, param2);
        free(param1);
        free(param2);
    }
    /*loop through data memory and converts address and word to our base 32, as mentioned above*/
    for (i = 0; i < dc; address++, i++) 
    {
        param1 = convert_to_base_32(address);
        param2 = convert_to_base_32(data[i]);
        fprintf(fp, "%s\t%s\n", param1, param2);
        free(param1);
        free(param2);
    }

    fclose(fp);
}

/*This function writes the .ent output file according to these instructions
  The first column contains the name of the label and the second column contains the address*/
void write_output_ent(FILE *fp)
{
    char *base32_address;

    label_ptr label = symbols_table;
    while(label) /*loop through symbols table and print only labels that have an entry*/
    {
        if(label -> entry)
        {
            base32_address = convert_to_base_32(label -> address);
            fprintf(fp, "%s\t%s\n", label -> name, base32_address);
            free(base32_address);
        }
        label = label -> next;
    }
    fclose(fp);
}

/*This function writes the .ext output file according to these instructions
  The first column contains the name of the label 
  and the second column contains the address where we should replace the external label*/
void write_output_ext(FILE *fp)
{
    char *base32_address;
    ext_ptr node = ext_list;

    do
    {
        base32_address = convert_to_base_32(node -> address);
        fprintf(fp, "%s\t%s\n", node -> name, base32_address);
        free(base32_address);
        node = node -> next;
    } while(node != ext_list);
    fclose(fp);
}

/*This function opens file by file name and file type (extension)*/
FILE *open_file(char *filename, int type)
{
    FILE *file;
    filename = create_file_name(filename, type); /*create the filename with given extension*/

    file = fopen(filename, "w");
    free(filename);

    if(file == NULL)
    {
        error_mode = FILE_CANT_BE_OPEN;
        return NULL;
    }
    return file;
}

/* This function checks if source and destination operands should be exists by given opcode */
void is_parameters_exist(int type, bool *is_src, bool *is_dest)
{
    switch (type)
    {
        case MOV:
        case CMP:
        case ADD:
        case SUB:
        case LEA:
            *is_src = TRUE;
            *is_dest = TRUE;
            break;

        case NOT:
        case CLR:
        case INC:
        case DEC:
        case JMP:
        case BNE:
        case GET:
        case PRN:
        case JSR:
            *is_src = FALSE;
            *is_dest = TRUE;
            break;

        case RTS:
        case HLT:
            *is_src = FALSE;
            *is_dest = FALSE;
    }
}

/* This function handles commands for the second level */
int handle_command_level_two(int type, char *line)
{
    char first_parameter[LINE_LEN], second_parameter[LINE_LEN]; 
    char *src = first_parameter, *dest = second_parameter;
    bool is_src = FALSE, is_dest = FALSE; /*bool variables to monitor if source and destination exists*/
    int src_method = METHOD_UNKNOWN, dest_method = METHOD_UNKNOWN;

    is_parameters_exist(type, &is_src, &is_dest); /*checks if source and destination exists*/

    if(is_src)/*extract source adressing method bits*/
        src_method = extract_bits(instructions[ic], SRC_METHOD_START_POS, SRC_METHOD_END_POS); 
    if(is_dest)/*extract destination adressing method bits*/
        dest_method = extract_bits(instructions[ic], DEST_METHOD_START_POS, DEST_METHOD_END_POS); 

    if(is_src || is_dest)
    {
        line = next_word_list(first_parameter, line);
        if(is_src && is_dest)
        {
            line = next_word_list(second_parameter, line);
            next_word_list(second_parameter, line);
        }
        else /* if there is only one parameter, it's a destination */
        {
            dest = first_parameter; 
            src = NULL;
        }
    }
    ic++;
    return insert_additional_words(src, dest, is_src, is_dest, src_method, dest_method);
}

/* This function inserts the additional words of the parameters to the instructions memory */
int insert_additional_words(char *src, char *dest, bool is_src, bool is_dest, int src_method, int dest_method) {
    /*if both methods are register methods then they might share the additional word*/
    if(is_src && is_dest && src_method == METHOD_REGISTER && dest_method == METHOD_REGISTER) 
    {
        insert_word_to_instruction(build_register_word(FALSE, src) | build_register_word(TRUE, dest));
    }
    else
    {
        if(is_src)
            insert_additional_word(FALSE, src_method, src); /*inserts additional word to source method*/
        if(is_dest)
            insert_additional_word(TRUE, dest_method, dest); /*inserts additional word to destination method*/
    }
    return is_error();
}

/* This function builds the additional word in a register*/
unsigned int build_register_word(bool is_dest, char *reg)
{
    unsigned int word = (unsigned int) atoi(reg + 1); /* registers */
    if(!is_dest) /*inserts it to the correct bits by source or destination*/
        word <<= BITS_IN_REGISTER;
    word = insert_are(word, ABSOLUTE);
    return word;
}

/* This function inserts a given label to the memory */
void insert_label(char *label)
{
    unsigned int word;
    if(is_label_exist(symbols_table, label)) /*checks if label exists*/
    { 
        word = get_label_address(symbols_table, label); /* get label address from symbols table*/

        if(is_external_label(symbols_table, label)) /*checks if label is extern, then add it to the extern list*/
        {
            add_ext(&ext_list, label, ic + MEMORY_START);
            word = insert_are(word, EXTERNAL);
        }
        else
            word = insert_are(word, RELOCATABLE); /* If it's not an external label, then it's relocatable */

        insert_word_to_instruction(word); /* inserts word to the memory */
    }
    else
    {
        ic++;
        error_mode = COMMAND_LABEL_INVALID;
    }
}

/* This function inserts an additional word to instructions memory according to the addressing method */
void insert_additional_word(bool is_dest, int method, char *parameter)
{
    unsigned int word = EMPTY_WORD;
    char *temp;

    switch (method)
    {
        case METHOD_IMMEDIATE:
            word = (unsigned int) atoi(parameter + 1);
            word = insert_are(word, ABSOLUTE);
            insert_word_to_instruction(word);
            break;

        case METHOD_DIRECT:
            insert_label(parameter);
            break;

        case METHOD_STRUCT:
            temp = strchr(parameter, '.'); /*There's a label before the dot*/
            *temp = '\0';
            insert_label(parameter); /* The first word is the label */
            *temp++ = '.';
            word = (unsigned int) atoi(temp);
            word = insert_are(word, ABSOLUTE);
            insert_word_to_instruction(word); /* The number after the label name is the second word */
        break;

        case METHOD_REGISTER:
            word = build_register_word(is_dest, parameter);
            insert_word_to_instruction(word);
    }
}