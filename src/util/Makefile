include ../../config.mk

top_srcdir = ../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../ -I../common -I../libsqlrutil $(RUDIMENTSINCLUDES)

SRCS = cmdline.C debugfile.C tempdir.C sqlrconfigfile.C authenticator.C
OBJS = cmdline.o debugfile.o tempdir.o sqlrconfigfile.o authenticator.o

all:
	$(MAKE) libsqlrutil.a; \
	if ( test "$(MINGW32)" != "yes" -a "$(CYGWIN)" != "yes" -a "$(UWIN)" != "yes" ); then \
		$(MAKE) libsqlrutil.so; \
	else \
		$(MAKE) libsqlrutil.dll; \
	fi

clean:
	$(RM) *.o *.a *.so

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

libsqlrutil.a: $(SRCS) $(OBJS)
	$(STATICAR) $@ $(OBJS)

libsqlrutil.so: $(SRCS) $(OBJS)
	$(DYNAMICAR) $@ $(OBJS)

libsqlrutil.dll: cmdline.C cmdline.o
	dllwrap --export-all-symbols --output-def libsqlrutil.def --output-exp libsqlrutil.exp --output-lib libsqlrutil.lib --dllname $@ $(OBJS) -lstdc++ $(RUDIMENTSLIBS)
	$(RM) libsqlrutil.def
	$(RM) libsqlrutil.exp

install:
	$(top_srcdir)/mkinstalldirs $(libdir)
	$(INSTALL) -m 644 libsqlrutil.a $(libdir); \
	if ( test "$(MINGW32)" != "yes" -a "$(CYGWIN)" != "yes" -a "$(UWIN)" != "yes" ); then \
		$(RM) $(libdir)/libsqlrutil.so*; \
		$(INSTALL) libsqlrutil.so $(libdir)/libsqlrutil.so.$(SQLR_VERSION); \
		cd $(libdir); \
		$(SYMLINK) libsqlrutil.so.$(SQLR_VERSION) libsqlrutil.so; \
	else \
		$(INSTALL) libsqlrutil.dll $(libdir); \
		$(INSTALL) libsqlrutil.lib $(libdir); \
	fi

uninstall:
	$(RM) $(libdir)/libsqlrutil.so*
	$(RM) $(libdir)/libsqlrutil.a
	$(RM) $(libdir)/libsqlrutil.dll
	$(RM) $(libdir)/libsqlrutil.lib
