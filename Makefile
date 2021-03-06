#-----------------------------------------------------------------------------
# Makefile
#
# Simple makefile for building and installing raw_binary.
#-----------------------------------------------------------------------------
.PHONY: all install-headers install-lib install clean

LIBDIRS = \
           common      \
           level1_lib  \
           level2_lib  \
           pixel_qa
EXEDIRS = tools

#-----------------------------------------------------------------------------
all: executables

#-----------------------------------------------------------------------------
libraries:
	@for dir in $(LIBDIRS); do \
        echo "make all in $$dir..."; \
        $(MAKE) -C $$dir || exit 1; done

#-----------------------------------------------------------------------------
executables: libraries
	@for dir in $(EXEDIRS); do \
        echo "make all in $$dir..."; \
        $(MAKE) -C $$dir || exit 1; done

#-----------------------------------------------------------------------------
install-headers:
# if the ESPA_LEVEL2QA_INC environment variable points to the 'include'
# directory, then there is no need to install anything
ifneq ($(ESPA_LEVEL2QA_INC), $(CURDIR)/include)
	@for dir in $(LIBDIRS); do \
        echo "installing all in $$dir..."; \
        $(MAKE) -C $$dir install-headers || exit 1; done
else
	echo "$(ESPA_LEVEL2QA_INC) is the same as the include directory. Include files already installed."
endif

#-----------------------------------------------------------------------------
install-lib: libraries
# if the ESPA_LEVEL2QA_LIB environment variable points to the 'lib' directory,
# then there is no need to install anything
ifneq ($(ESPA_LEVEL2QA_LIB), $(CURDIR)/lib)
	@for dir in $(LIBDIRS); do \
        echo "installing all in $$dir..."; \
        $(MAKE) -C $$dir install-lib || exit 1; done
else
	echo "$(ESPA_LEVEL2QA_LIB) is the same as the lib directory. Libraries already installed."
endif

#-----------------------------------------------------------------------------
install-executables: executables
	@for dir in $(EXEDIRS); do \
        echo "installing all in $$dir..."; \
        $(MAKE) -C $$dir install || exit 1; done

#-----------------------------------------------------------------------------
install: install-lib install-headers install-executables

#-----------------------------------------------------------------------------
clean:
# all directories need to be cleaned
	@for dir in $(LIBDIRS) $(EXEDIRS); do \
        echo "make clean in $$dir..."; \
        $(MAKE) -C $$dir clean || exit 1; done
	rm -r include lib
