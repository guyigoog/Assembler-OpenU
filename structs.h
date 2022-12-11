#ifndef STRUCTS_H

#define STRUCTS_H

#include "assembler.h"

extern unsigned int data[]; /* Data array */
extern unsigned int instructions[]; /* Instructions array */

typedef enum {FALSE, TRUE} bool; /* define a boolean type with enum */

/* define a label linked list */
typedef struct structLabels * label_ptr;
typedef struct structLabels {
	char name[LABEL_LEN]; 
	unsigned int address; 
	bool external; /* bool variable to determine if label is external */
	bool is_instruction_sentence; /* bool variable to determine if label is in an instruction sentence */
	bool entry; /* bool variable to determine if label is entry */
	label_ptr next;
} Labels;

/* define a external labels two way linked list*/
typedef struct ext * ext_ptr;
typedef struct ext {
    char name[LABEL_LEN]; 
    unsigned int address;
    ext_ptr next; 
    ext_ptr prev;
} ext;

ext_ptr add_ext(ext_ptr *hptr, char *name, unsigned int reference);
void free_ext(ext_ptr *hptr);
void print_ext(ext_ptr h);
void offset_label_address(label_ptr label, int num, bool is_data);
int change_entry_in_label(label_ptr h, char *name);
unsigned int get_label_address(label_ptr h, char *name);
bool is_external_label(label_ptr h, char *name);
bool is_label_exist(label_ptr h, char *name);
label_ptr get_label(label_ptr h, char *name);
label_ptr add_label(label_ptr *hptr, char *name, unsigned int address, bool external, ...);
void free_labels(label_ptr *hptr);
int delete_label(label_ptr *hptr, char *name);
void print_labels(label_ptr h);

#endif