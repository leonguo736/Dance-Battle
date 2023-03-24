CC			= gcc
CCFLAGS		= -g -Wall -fPIC
SOFLAGS		= -shared -o

LIBNAME		= intelfpgaup
TARGET		= lib$(LIBNAME).so

SRCDIR		= ./src
OBJDIR		= ./obj
INCDIR		= ./include

# global library and include directories.
GLIBDIR		= /usr/lib
GINCDIR		= /usr/include

RM			= rm -f
RMR			= rm -rf
CP			= cp
LDCONFIG	= ldconfig

SRCS		= $(wildcard $(SRCDIR)/*.c)
OBJS		:= $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

## Build the (static) library file from the object files.
$(TARGET): $(OBJS)
	@$(CC) $(SOFLAGS) $(TARGET) $(OBJS)
	@echo "Dynamic library file: "$(TARGET)" created."

# Build the object files from the source files.
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	@$(CC) $(CCFLAGS) -c $< -o $@
	@echo "Compiled "$<""

# clean: Remove the object files, but does not remove the library file.
.PHONY: clean
clean:
	@$(RMR) $(OBJDIR)
	@echo "Clean done."

# remove: Runs a clean as well as removing the library file (by removing the bin folder).
.PHONY: remove
remove: clean
	@$(RMR) $(TARGET)
	@echo "Library file removed."

# install: Installs the library file to the /user/lib directory and include files to /user/include
.PHONY: install
install:
	@$(CP) $(TARGET) $(GLIBDIR)/
	@mkdir -p $(GINCDIR)/$(LIBNAME)
	@$(CP) -f $(INCDIR)/* $(GINCDIR)/$(LIBNAME)/
	@$(LDCONFIG)
	@echo "Installed $(TARGET) to $(GLIBDIR) and copied include files to $(GINCDIR)/$(LIBNAME)"

# uninstall: Uninstalls the library file from /user/lib directory and removes includes directiry from /user/include.
.PHONY: uninstall
uninstall:
	@$(RM) $(GLIBDIR)/$(TARGET)
	@$(RMR) $(GINCDIR)/$(LIBNAME)
	@$(LDCONFIG)
	@echo "Uninstalled $(TARGET) from $(GLIBDIR) and removed include files from $(GINCDIR)/$(LIBNAME)"

