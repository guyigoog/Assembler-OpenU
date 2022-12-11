#include "structs.h"
#include "assembler.h"

/*extern variables to be later defined in the program*/
extern int ic, dc;
extern int error_mode;
extern bool was_error;
extern label_ptr symbols_table;
extern ext_ptr ext_list;
extern const char base32[32];
extern const char *commands[];
extern const char *directives[];
extern bool entry_exists, extern_exists;