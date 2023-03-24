README file for the IntelFPGA UP C library on the DE1-SoC board.

include/ :	The C header file for the library. These are provided here only for reference. 
They are installed in the directory /user/include/intelfpgaup so that they can be seen globally 
when compiling C programs.

src/ :	The C source file for the wrappers. These are provided here only for reference. User 
programs can call the wrappers by including <intelfpgaup/xxx.h> in C code, where xxx is the 
name of a driver (e.g. KEY, or video), as well as appending -l intelfpgaup at the end of the 
gcc compiler command

libintelfpgaup.so	:	The dynamic library file for the wrappers. This file is installed in
/usr/lib so that it is global and can be linked by simply adding the flag -lintelfpgaup to the 
end of the GCC compiler command.
