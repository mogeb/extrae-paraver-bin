setenv MPI_HOME 
setenv PAPI_HOME /usr/
setenv LIBXML2_HOME 
setenv DYNINST_HOME /usr/local/
setenv DWARF_HOME /usr/
setenv ELF_HOME /usr/
setenv LIBERTY_HOME 
setenv BFD_HOME 
setenv UNWIND_HOME /usr
setenv EXTRAE_CC gcc
setenv EXTRAE_CFLAGS "-g -O2 -fno-optimize-sibling-calls -Wall -W"
setenv EXTRAE_CXX g++
setenv EXTRAE_CXXFLAGS "-g -O2 -Wall -W"
setenv PACKAGE_NAME Extrae
setenv PACKAGE_BUGREPORT tools@bsc.es
setenv CONFIGURE_LINE " $ ./configure --prefix=/home/mogeb/paraver/extrae --without-mpi --disable-posix-clock --with-unwind-libs=/usr/lib/x86_64-linux-gnu/ --with-unwind-headers=/usr/include --with-unwind=/usr/lib/x86_64-linux-gnu/ --with-dyninst-libs=/usr/local/lib --with-dyninst=/usr/local --with-dwarf=/usr --with-dwarf-headers=/usr/include --with-dwarf-libs=/usr/lib --with-elf=/usr --with-papi=/usr"
