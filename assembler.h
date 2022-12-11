#ifndef ASSEMBLER_H

#define ASSEMBLER_H

/*Consts*/

#define MEMORY_START 100
#define EMPTY_WORD 0
#define EXTERNAL_DEFAULT_ADDRESS 0
#define NOT_FOUND -1 /*Not found state*/
#define NO_ERROR 0 /*No error state*/
#define ERROR 1 /*Error state*/
#define LINE_LEN 80 /* Maximum line length */
#define MINIMUM_LABEL_LENGTH_WITH_COLON 2
#define MINIMUM_LABEL_LENGTH_WITHOUT_COLON 1
#define LABEL_LEN 30 /* Maximum label length */
#define MAX_COMMAND_LENGTH 4 /* Maximum command length */
#define MIN_COMMAND_LENGTH 3 /* Minimum command length */
#define REGISTER_LENGTH 2 /* Register name only contain 2 chars */
#define MIN_REGISTER 0 /* First register is r0 */
#define MAX_REGISTER 7 /* Last register is r7 */
#define MAX_EXTENSION_LENGTH 5
#define BASE32_SEQUENCE_LENGTH 3 /*2 digits and '\0' ending */
#define NUM_DIRECTIVES 5 /* The number of directives*/
#define NUM_COMMANDS 16 /* The number of commands */
#define FIRST_STRUCT_FIELD 1 /* Index of first struct field */
#define SECOND_STRUCT_FIELD 2 /* Index of second struct field */

/*Bits consts*/
#define BITS_IN_WORD 10 
#define BITS_IN_OPCODE 4 
#define BITS_IN_METHOD 2 
#define BITS_IN_ARE 2 
#define BITS_IN_REGISTER 4
#define BITS_IN_ADDRESS 8

/* The position of source and destination methods in commands*/
#define SRC_METHOD_START_POS 4
#define SRC_METHOD_END_POS 5
#define DEST_METHOD_START_POS 2
#define DEST_METHOD_END_POS 3

#define MACHINE_RAM 2000

void reset_globals();

/* Directives types enum */
enum directives {DATA, STRING, STRUCT, ENTRY, EXTERN, UNKNOWN_TYPE}; 

/* Commands types enum ordered by their opcode*/
enum commands {MOV, CMP, ADD, SUB, NOT, CLR, LEA, INC, DEC, JMP, BNE, GET, PRN, JSR, RTS, HLT, UNKNOWN_COMMAND};

/* Errors enum */
enum errors {
    NOT_DOT_OR_LETTER = 1, MACRO_NAME_INVALID ,LABEL_NAME_ALREADY_EXISTS, LABEL_IS_TOO_LONG, LABEL_NAME_ISNT_ALPHANUMERIC,
    LABEL_NAME_SAME_AS_COMMAND, LABEL_ISNOT_FOLLOWED_BY_CMD_OR_DIR, LABEL_NAME_SAME_AS_REGISTER,
    DIRECTIVE_MISSING_PARAMETERS, DIRECTIVE_INCORRECT_NUMBER_PARAMETERS, DATA_TOO_MANY_COMMAS, DATA_MISSING_NUMBER,
    DATA_MISSING_COMMA_AFTER_NUMBER, DATA_ENDED_WITH_COMMA,
    STRING_MORE_THAN_ONE_PARAMETER, STRING_PARAMETER_IS_INVALID, STRUCT_MISSING_STRING,
    STRUCT_PARAMETER_NOT_STRING, STRUCT_MISSING_COMMA, STRUCT_FIRST_PARAMETER_INVALID, STRUCT_TOO_MANY_PARAMETERS,
    EXTERN_NOT_FOLLOWED_BY_LABEL, EXTERN_LABEL_INVALID, EXTERN_TOO_MANY_PARAMETERS,
    COMMAND_OR_DIRECTIVE_NOT_FOUND, COMMAND_INVALID_SYNTAX, COMMAND_TOO_MANY_PARAMETERS,
    COMMAND_METHOD_INVALID, COMMAND_INCORRECT_NUMBER_OF_PARAMTERS, COMMAND_PARAMETERS_METHOD_INVALID,
    ENTRY_IS_NOT_FOLLOWED_BY_LABEL, ENTRY_CANNOT_APPLY_TO_EXTERN, COMMAND_LABEL_INVALID,
    FILE_CANT_BE_OPEN
};

/* if label has colon or not enum */
enum {NO_COLON, COLON};

/* Addressing methods enum ordered by their code */
enum methods {METHOD_IMMEDIATE, METHOD_DIRECT, METHOD_STRUCT, METHOD_REGISTER, METHOD_UNKNOWN};

/* ARE modes enum ordered by value */
enum ARE {ABSOLUTE, EXTERNAL, RELOCATABLE};

/* Types of files enum */
enum filetypes {FILE_INPUT, FILE_AM, FILE_OBJECT, FILE_ENTRY, FILE_EXTERN};

#endif
