top_builddir = ../../..

include ../../../config.mk

.PHONY: all clean install uninstall

CPPFLAGS = $(BASECPPFLAGS) -I./ -I$(top_builddir)/ -I$(top_builddir)/src/common -I$(top_builddir)/src/util -I$(top_builddir)/src/authenticator -I$(top_builddir)/src/connection $(RUDIMENTSINCLUDES) $(ORACLEINCLUDES) $(SIDINCLUDES)
LIBS = $(ORACLELIBS) -L$(top_builddir)/src/connection -lsqlrconnection -L$(top_builddir)/src/util -lsqlrutil $(RUDIMENTSLIBS) $(EXTRALIBS) $(SIDLIBS) $(LIBDMALLOC) $(LIBEFENCE)
ifneq ($(strip $(ORACLELIBSPATH)),)
ifeq ($(ORACLEUSERPATH),yes)
	RPATH = -R $(ORACLELIBSPATH)
endif
endif
ifneq ($(strip $(libdir)),)
ifeq ($(SERVERUSERPATH),yes)
	RPATH += -R $(libdir)
endif
endif

SRCS= oracle8connection.cpp \
	oracle8sqltranslations.cpp \
	oracle8sqlwriter.cpp \
	main.cpp
LOBJS= oracle8connection.lo \
	oracle8sqltranslations.lo \
	oracle8sqlwriter.lo \
	main.lo

.SUFFIXES: .lo

.cpp.lo:
	$(LIBTOOL) --mode=compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

all: sqlr-connection-oracle8$(EXE)

clean:
	$(LIBTOOL) --mode=clean $(RM) sqlr-connection-oracle8$(EXE)
	$(LIBTOOL) --mode=clean $(RM) *.lo
	$(LIBTOOL) --mode=clean $(RM) *.o
	$(RMTREE) .libs

sqlr-connection-oracle8$(EXE): $(SRCS) $(LOBJS)
	$(LIBTOOL) --mode=link $(CXX) $(ORACLESTATIC) $(LDFLAGS) -o $@ $(LOBJS) $(LIBS) $(RPATH)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(LTINSTALL) sqlr-connection-oracle8$(EXE) $(bindir)

uninstall:
	$(LIBTOOL) --mode=uninstall $(RM) $(bindir)/sqlr-connection-oracle8$(EXE)
