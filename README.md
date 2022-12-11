# Assembler-OpenU : Grade 100
The final project of the C course at the Open University of Israel.
This project is a two phase assembler with a pre-assmbler stage in a imaginary assembly language and computer.
# How To Run
This project require ubunto (may work on different linux versions too).

This project use makefile in order to compile, run:

>  make

Make your assembly files with an .as extension, open terminal and pass file names as parameters (file name only), example:

file.as:

>   assembler file

The assembler will generate the output files with the same filename and the following extensions:

.am - After macro proccess

.ob - Object file

.ent - Entries file

.ext - Externals file

You can find examples of inputs and outputs in the 'tests' folder.

# More info
For more information about the project and the instructions, read maman14 pdf file.
