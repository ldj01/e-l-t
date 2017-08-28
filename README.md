# ESPA-L2QA-TOOLS Version 1.6.0 Release Notes
Release Date: October 2017

ESPA modules for reading and interrogating the Level-1 QA band information, and
for supporting a Level-2 QA band.  The Level-2 QA band will include unique attributes from the Level-2 processing.

This project is provided by the US Geological Survey (USGS) Earth Resources Observation and Science (EROS) Land Satellite Data Systems (LSDS) Science Research and Development (LSRD) Project. For questions regarding this source code, please contact the Landsat Contact Us page and specify USGS CDR/ECV in the "Regarding" section. https://landsat.usgs.gov/contactus.php

### Downloads
espa-l2qa-tools source code

    git clone https://github.com/USGS-EROS/espa-l2qa-tools.git

See git tag [version_1.6.0]

### Dependencies
  * XML2 libraries -- ftp://xmlsoft.org/libxml2/

### Installation
  * Install dependent libraries - XML2

  * Set up environment variables.  Can create an environment shell file or add the following to your bash shell.  For C shell, use 'setenv VAR "directory"'.
  ```
    export XML2INC="path_to_XML2_include_files"
    export XML2LIB="path_to_XML2_libraries"
    export ESPAINC="path_to_format_converter_raw_binary_include_directory"
    export ESPALIB="path_to_format_converter_raw_binary_lib_directory"
    export ESPA_LEVEL2QA_INC="path_to_level2_qa_include_directory"
    export ESPA_LEVEL2QA_LIB="path_to_level2_qa_lib_directory"
  ```
  Define $PREFIX to point to the directory in which you want the executables, static data, etc. to be installed.
  ```
    export PREFIX="path_to_directory_for_format_converter_build_data"
   ```

* Install ESPA product formatter libraries and tools by downloading the source from Downloads above.  Go to the src/raw\_binary directory and build the source code there. ESPAINC and ESPALIB above refer to the include and lib directories created by building this source code using make followed by make install. The ESPA raw binary conversion tools will be located in the $PREFIX/bin directory.

### Linking these libraries for other applications
The following is an example of how to link these libraries into your
source code. Depending on your needs, some of these libraries may not
be needed for your application or other espa product formatter libraries may need to be added.
```
 -L$(ESPA_LEVEL2QA_LIB) -l_espa_class_based_qa -l_espa_level1_qa \
                        -l_espa_level2_qa \
 -L$(ESPALIB) -l_espa_raw_binary -l_espa_common \
 -L$(XML2LIB) -lxml2 \
 -lm
```

### Verification Data

### User Manual

### Product Guide


## Release Notes
  * Modified the Level-2 pixel QA generator so that the the clear bit remains
    ON if the cirrus confidence is high or the terrain occlusion is set. These
    changes will have an affect only on the L8 products, since the cirrus and
    terrain occlusion bits in the Level-1 QA band only apply for L8.
