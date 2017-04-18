#-----------------------------------------------------------------------------
# Makefile
#
# for ESPA common libraries
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
INC = l2qa_common.h

# Define the source code and object files
#SRC =
#OBJ = $(SRC:.c=.o)

# Define include paths
INCDIR  = -I.
NCFLAGS = $(EXTRA) $(INCDIR)

# Define the object libraries and paths
# Not used in this library only directory
#EXLIB   =
#MATHLIB =
#LOADLIB = $(EXLIB) $(MATHLIB)

# Define the C library/archive
ARCHIVE = l2qa_common

#-----------------------------------------------------------------------------
all: $(ARCHIVE)

$(ARCHIVE): $(OBJ) $(INC)
#	$(AR) $(ARCHIVE) $(OBJ)
#	install -d ../lib
	install -d ../include
#	install -m 644 $(ARCHIVE) ../lib
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
#	install -d $(lib_link_path)
#	install -d $(level2_qa_lib_install_path)
#	install -m 644 $(ARCHIVE) $(level2_qa_lib_install_path)
#	ln -sf $(level2_qa_link_lib_path)/$(ARCHIVE) $(lib_link_path)/$(ARCHIVE)

#-----------------------------------------------------------------------------
#install: install-lib install-headers
install: install-headers

#-----------------------------------------------------------------------------
clean:
	$(RM) -f *.o $(ARCHIVE)

#-----------------------------------------------------------------------------
$(OBJ): $(INC)

.c.o:
	$(CC) $(NCFLAGS) -c $<

