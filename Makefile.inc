SRC_ROOT=${HOME}/src/c++11
INCLUDEPATH = -I$(SRC_ROOT)/usr/include -iquote$(SRC_ROOT)/include
CPPFLAGS = -Wall -D_GNU_SOURCE -std=gnu++11 -fPIC
#LIBFLAG = -L/usr/local/lib/ -L/usr/lib -Wl,-rpath,/usr/local/lib