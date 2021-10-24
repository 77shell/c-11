#
# Delta Electronics, INC
# TPSBU, IESBG
# 
# 2011-Aug-22
# Max <max.yang@delta.com.tw>
#
# To build *csu*
# 

DBG=y


ifneq ($(DBG),y)
DEBUG=
OPTIMIZE=-O2
PROFILE=
else
DEBUG=-g
OPTIMIZE=-O0
PROFILE=-pg
endif


include ./Makefile.inc


INCLUDEPATH += -iquote$(SRC_ROOT)/include
LIBSPATH = -L$(SRC_ROOT)/ \
	-L/usr/lib -Wl,-rpath,/usr/local/lib


CXXFLAGS += $(DEBUG) $(OPTIMIZE) $(PROFILE) $(INCLUDEPATH) $(LIBSPATH) $(LIBFLAGS)
CFLAGS   += -Wextra -Wno-override-init $(DEBUG) $(OPTIMIZE) $(PROFILE) $(INCLUDEPATH) $(LIBSPATH) $(LIBFLAGS)

COMPILE.cc = $(CXX) $(CXXFLAGS)
COMPILE.c  = $(CC)  $(CFLAGS)


vpath %.cpp .
vpath %.c .

BINS := hello \
	list \
	initializer_list \
	exception_type \
	exception \
	exception_13_5_2 \
	cond_var \
	catch_signals \
	unordered_map

.PHONY: impl_pattern
impl_pattern: impl_pattern.cpp impl_pattern_test.cpp
	g++ -o$@ $^

.PHONY: impl
impl: impl.cpp impl.h impl_test.cpp
	g++ -std=c++14 $^ -o $@

.PHONY: all
all: $(BINS)

#receiver: receiver.o
#	$(CXX) -o $@ $< -lcan

deque: deque.cpp
	$(COMPILE.cc) -o $@ $< -pthread

sha256: sha256.cpp
	$(COMPILE.cc) -o $@ $< -lssl -lcrypto -I~/src/csu.linux/x86/gcc-9.3.0-17/usr/local/include -L~/csu.linux/x86/gcc-9.3.0-17/usr/local/lib64

zip: zip.cpp
	$(COMPILE.cc) -o $@ $< -lzip

unzip: unzip.cpp
	$(COMPILE.cc) -o$@ $< -lzip

cond_var: cond_var.cpp
	$(CXX) -std=c++14 -pthread -o$@ $^

%: %.c
	$(COMPILE.c) -o $@ $< $(LIB)

%: %.cpp
	$(COMPILE.cc) -o$@ $< $(LIB)

.PHONY: clean
clean:
	rm -rf *.o $(csu502) db_para.*
	find . -regextype posix-egrep -iregex '.*\.#.*' | xargs rm
	find . -iregex .*~$ | xargs rm
	rm $(BINS) *.o *~ -rf


.PHONY: TAGS
TAGS:
	rm -rf TAGS
	find . -regextype posix-egrep -iregex '.*\.(cpp|c|h)' > search.log
	perl filter.pl search.log | xargs etags -a
	rm -rf search.log
