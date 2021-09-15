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
else
 DEBUG=-g
 OPTIMIZE=-O0
endif


include ./Makefile.inc


INCLUDEPATH += -iquote$(SRC_ROOT)/include
LIBSPATH = -L$(SRC_ROOT)/ \
	-L/usr/lib -Wl,-rpath,/usr/local/lib

CPPFLAGS += $(FLAGS) $(DEBUG) $(OPTIMIZE) $(INCLUDEPATH) $(LIBSPATH) $(LIBFLAGS)
CFLAGS   += $(FLAGS) -Wextra -Wno-override-init $(DEBUG) $(OPTIMIZE) $(INCLUDEPATH) $(LIBSPATH) $(LIBFLAGS)

COMPILE.cc = $(CXX) $(CPPFLAGS)
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
	catch_signals


.PHONY: impl
impl: impl_pattern.cpp impl_pattern_test.cpp
	g++ -o$@ $^

.PHONY: all
all: $(BINS)

#receiver: receiver.o
#	$(CXX) -o $@ $< -lcan

sha256: sha256.c
	$(COMPILE.c) -o $@ $< -lcrypto

zip: zip.cpp
	$(COMPILE.cc) -o $@ $< -lzip

unzip: unzip.c
	$(COMPILE.c) -o$@ $< -lzip

cond_var: cond_var.cpp
	$(CXX) -std=c++14 -pthread -o$@ $^

%: %.c
	$(COMPILE.c) -o $@ $< $(LIB)

%: %.cpp
	$(COMPILE.cc) -o$@ $< $(LIB)

.PHONY: impl
impl: impl.cpp impl.h impl_test.cpp
	g++ -std=c++14 $^ -o $@

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
