#include "pre_assembler.h"

macros_ptr macro_node;
macros_ptr macro_ptr;
FILE *macro_file;
bool is_macro;

/*The main function of the pre assembler proccess */
void pre_assembler(FILE *file1, char *file_name)
{
	char line[LINE_LEN];
	int line_number;
	char *am_filename;
	is_macro = FALSE;
	line_number = 1; /*starts count from line 1*/
	am_filename = create_file_name(file_name, FILE_AM); /*creates filename with .am extenstion*/
	macro_file = fopen(am_filename, "w");
	macro_node = NULL;
	error_mode = NO_ERROR;

	while(fgets(line, LINE_LEN, file1) != NULL) /* Read lines until end of file */
	{
		read_line_macro(line, line_number);
		if(is_error()) /*checks if there was an error*/
       {
            was_error = TRUE;
            print_error(line_number); /*prints error*/
            error_mode = NO_ERROR;
        }
		line_number++; /*count next line*/
	}
	free_list(&macro_node); /*free the memory*/
	fclose(macro_file); /*close the file*/
}

/*This function reads line from the file and handle the macros if any*/
void read_line_macro(char *line, int line_count){
	char word[LINE_LEN];
	char * all_lines = line;
	line = skip_whitespace(line);
	copy_word(word, line);

	if(is_macro) /*checks if we already entered macro state*/
	{
		if(!strcmp(word, "endmacro")) /*checks if macro is ended*/
		{
			is_macro = FALSE;
			return;
		}
		add_line(all_lines, word); /*add word to the line*/
	}
	else /*we aren't on macro state*/
	{
		if(!strcmp(word, "macro")) /*if word is macro*/
		{
			is_macro = TRUE;
			line = next_word(line);
			copy_word(word, line);
			if(is_macro_valid(word)) /*checks if macro is valid*/
			{
				add_macro(&macro_node, word); /*if it is, then add it to the macro list*/
				return;
			}
			else
			{
				error_mode = MACRO_NAME_INVALID;
				return;
			}
		}
		add_line(all_lines, word); /*add word to the line*/
	}
}

/*This function adds given macro to the macro linked list*/
void add_macro(macros_ptr * macros_list, char * macro_name)
{
	macros_ptr p1, p2;
	p1 = (macros_ptr)malloc(sizeof(macros)); /*allocate memory*/
	if(!p1)
	{
		fprintf(stderr, "Error: couldn't allocate memory for macro\n");
		exit(ERROR);
	}
	strcpy(p1->name, macro_name); /*copy macro name*/
	if(*macros_list == NULL) /*macro linked list is empty*/
	{
		*macros_list = p1; /*then the given macro is the head of the linked list*/
	}
	else /*else we add the given macro to the linked list*/
	{
		p2 = *macros_list;
		*macros_list = p1;
		p1->next = p2;
	}
}

/*This function adds each line in the file to the .am file*/
void add_line(char *line, char *word)
{
	if(is_macro) /*checks if we in macro state then contcatenates the macro value to the line*/
	{ 
		strcat(macro_node->value, line);
	}
	else
	{
		/*checks if the line is macro, then it adds only the value of it to the file*/
		if((macro_ptr = get_macro(macro_node, word)) != NULL) 
		{
			fputs(macro_ptr->value, macro_file);
		}
		else /*if line isn't a macro then we simply add it as it is*/
		{
			fputs(line, macro_file);
		}
	}
	
}

/*This function returns the pointer to the given macro in the macro linked list*/
macros_ptr get_macro(macros_ptr macros_list, char * macro_name)
{
	macros_ptr p;
	p = macros_list;
	while(p != NULL)
	{
		if(!strcmp(p->name, macro_name))
		{
			return p;
		}
		p = p->next;
	}
	return NULL;
}

/*This function free the macro linked list memory, by free each node*/
void free_list(macros_ptr *macros_list)
{
	macros_ptr p;
	while(*macros_list != NULL)
	{
		p = *macros_list;
		*macros_list = (*macros_list)->next;
		free(p);
	}
}
