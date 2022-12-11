#include <stdlib.h>
#include "structs.h"

void level_one(FILE *file);
void read_line(char *line);
int handle_directive(int type, char *line);
int handle_command(int type, char *line);
int handle_string_directive(char *line);
int handle_struct_directive(char *line);
int handle_data_directive(char *line);
void insert_num_to_data(int num);
void insert_string_to_data(char *str);
int get_method(char * operand);
bool cmd_isvalid_methods(int type, int first_method, int second_method);
bool cmd_isvalid_parameters(int type, bool first_operand, bool second_operand);
int count_additional_words(int is_first, int is_second, int first_method, int second_method);
unsigned int build_first_word(int type, int is_first, int is_second, int first_method, int second_method);
int handle_extern_directive(char *line);
bool is_label(char *word, int colon);
