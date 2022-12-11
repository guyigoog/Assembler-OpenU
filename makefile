assembler: assembler.o level_one.o structs.o level_two.o utils.o pre_assembler.o
	gcc -g -ansi -Wall -pedantic assembler.o level_one.o structs.o level_two.o utils.o pre_assembler.o -lm -o assembler

assembler.o: assembler.c externs.h assembler.h structs.h utils.h pre_assembler.h
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o
	
pre_assembler.o: pre_assembler.c pre_assembler.h level_one.h assembler.h structs.h utils.h
	gcc -c -ansi -Wall -pedantic pre_assembler.c -o pre_assembler.o

level_one.o: level_one.c level_one.h assembler.h externs.h structs.h utils.h
	gcc -c -ansi -Wall -pedantic level_one.c -o level_one.o

structs.o: structs.c structs.h assembler.h externs.h utils.h
	gcc -c -ansi -Wall -pedantic structs.c -o structs.o

utils.o: utils.c assembler.h externs.h structs.h utils.h
	gcc -c -ansi -Wall -pedantic utils.c -o utils.o

level_two.o: level_two.c level_two.h assembler.h externs.h structs.h utils.h
	gcc -c -ansi -Wall -pedantic level_two.c -o level_two.o
