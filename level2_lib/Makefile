#-----------------------------------------------------------------------------
# Makefile for LaSRC code
#-----------------------------------------------------------------------------
.PHONY: all install-headers install-lib install clean

# Inherit from upper-level make.config
TOP = ..
include $(TOP)/make.config

#-----------------------------------------------------------------------------
# Set up compile options
CC    = gcc
RM    = rm
AR    = ar rcsv
EXTRA = -Wall $(EXTRA_OPTIONS)

# Define the include files
INC = read_level2_qa.h write_level2_qa.h generate_level2_qa.h

# Define the source code and object files
SRC = \
      read_level2_qa.c \
      write_level2_qa.c \
      generate_level2_qa.c
OBJ = $(SRC:.c=.o)

# Define include paths
INCDIR = -I. -I../include -I$(ESPAINC) -I$(XML2INC)
NCFLAGS = $(EXTRA) $(INCDIR)

# Define the object libraries and paths
#EXLIB = -L$(ESPALIB) -l_espa_raw_binary -l_espa_common \
#        -L$(XML2LIB) -lxml2
#MATHLIB = -lm
#LOADLIB = $(EXLIB) $(MATHLIB)

# Define C library/archive
ARCHIVE = lib_espa_level2_qa.a

#-----------------------------------------------------------------------------
all: $(ARCHIVE)

$(ARCHIVE): $(OBJ) $(INC)
	$(AR) $(ARCHIVE) $(OBJ)
	install -d ../lib
	install -d ../include
	install -m 644 $(ARCHIVE) ../lib
	install -m 644 $(INC) ../include

#-----------------------------------------------------------------------------
install-headers:
	install -d $(inc_link_path)
	install -d $(level2_qa_inc_install_path)
	@for inc in $(INC); do \
        echo "install -m 644 $$inc $(level2_qa_inc_install_path)/$$inc"; \
        install -m 644 $$inc $(level2_qa_inc_install_path)/$$inc; \
        echo "ln -sf $(level2_qa_link_inc_path)/$$inc $(inc_link_path)/$$inc"; \
        ln -sf $(level2_qa_link_inc_path)/$$inc $(inc_link_path)/$$inc; \
        done

#-----------------------------------------------------------------------------
install-lib: all
	install -d $(lib_link_path)
	install -d $(level2_qa_lib_install_path)
	install -m 644 $(ARCHIVE) $(level2_qa_lib_install_path)
	ln -sf $(level2_qa_link_lib_path)/$(ARCHIVE) $(lib_link_path)/$(ARCHIVE)

#-----------------------------------------------------------------------------
install: install-lib install-headers

#-----------------------------------------------------------------------------
clean:
	$(RM) -f *.o $(ARCHIVE)

#-----------------------------------------------------------------------------
$(OBJ): $(INC)

.c.o:
	$(CC) $(NCFLAGS) -c $<
