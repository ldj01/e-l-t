#-----------------------------------------------------------------------------
# Makefile
# for raw binary tools
#-----------------------------------------------------------------------------
.PHONY: all install clean

# Inherit from upper-level make.config
TOP = ..
include $(TOP)/make.config

#-----------------------------------------------------------------------------
# Set up compile options
CC    = gcc
RM    = rm
EXTRA = -Wall $(EXTRA_OPTIONS)

# Define the include files
INC =

# Define the source code and object files
SRC1 = test_read_level1_qa.c
OBJ1 = $(SRC1:.c=.o)
SRC2 = generate_class_based_qa.c
OBJ2 = $(SRC2:.c=.o)

# Define include paths
INCDIR  = -I. -I../include -I$(ESPAINC) -I$(XML2INC) 
NCFLAGS = $(EXTRA) $(INCDIR)

# Define the object libraries and paths
MATHLIB = -lm

LIB1   = -L../lib -l_espa_level1_qa \
    -L$(ESPALIB) -l_espa_raw_binary -l_espa_common \
    -L$(XML2LIB) -lxml2 \
    -L$(LZMALIB) -llzma \
    -L$(ZLIBLIB) -lz \
    $(MATHLIB)

LIB2   = -L../lib -l_espa_class_based_qa -l_espa_level1_qa \
    -L$(ESPALIB) -l_espa_raw_binary -l_espa_common \
    -L$(XML2LIB) -lxml2 \
    -L$(LZMALIB) -llzma \
    -L$(ZLIBLIB) -lz \
    $(MATHLIB)

# Define C executables
EXE1 = test_read_level1_qa
EXE2 = generate_class_based_qa
ALL_EXES = $(EXE1) $(EXE2)

#-----------------------------------------------------------------------------
all: $(ALL_EXES)

$(EXE1): $(OBJ1) $(INC)
	$(CC) $(NCFLAGS) -o $(EXE1) $(OBJ1) $(LIB1)

$(EXE2): $(OBJ2) $(INC)
	$(CC) $(NCFLAGS) -o $(EXE2) $(OBJ2) $(LIB2)

#-----------------------------------------------------------------------------
install: $(ALL_EXES)
	install -d $(bin_link_path)
	install -d $(level2_qa_bin_install_path)
	install -m 755 $(ALL_EXES) $(level2_qa_bin_install_path)
	@for executable in $(ALL_EXES); do \
        echo "ln -sf $(level2_qa_link_bin_path)/$$executable $(bin_link_path)/$$executable"; \
        ln -sf $(level2_qa_link_bin_path)/$$executable $(bin_link_path)/$$executable; \
        done

#-----------------------------------------------------------------------------
clean:
	$(RM) -f *.o $(ALL_EXES)

#-----------------------------------------------------------------------------
$(OBJ1): $(INC)

.c.o:
	$(CC) $(NCFLAGS) -c $<

