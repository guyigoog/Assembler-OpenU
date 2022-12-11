#include <stdlib.h>

void level_two(FILE *fp, char *filename);
void read_line_phase_two(char *line);
int write_output_files(char *original);
void write_output_ob(FILE *fp);
void write_output_ent(FILE *fp);
void write_output_ext(FILE *fp);
FILE *open_file(char *filename, int type);
void is_parameters_exist(int type, bool *is_src, bool *is_dest);
int handle_command_level_two(int type, char *line);
int insert_additional_words(char *src, char *dest, bool is_src, bool is_dest, int src_method, int dest_method);
unsigned int build_register_word(bool is_dest, char *reg);
void insert_label(char *label);
void insert_additional_word(bool is_dest, int method, char *operand);
