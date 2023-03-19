# The compiler.
CC		= gcc

# Compiler flags.
CCFLAGS	= -Wall -o

# The target executable name.
TARGET	= test_audio

# The source to compile.
SRC		= test_audio.c

# The included libraries.
LIBS	= -lintelfpgaup -lm

# Compile the source file: gcc -Wall -o test test.c -lintelfpgaup -lm
$(TARGET): $(SRC)
	@$(CC) $(CCFLAGS) $(TARGET) $(SRC) $(LIBS)


# Remove the target (the executable).
.PHONY: clean
clean:
	@rm -f $(TARGET)


