include ../../config.mk

top_srcdir = ../..
INSTALL = $(top_srcdir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../../ -I../common -I../util $(RUDIMENTSINCLUDES)

SRCS = sqlrconnection.C connection/connectioncmdline.C connection/ipc.C connection/listenercomm.C connection/scalercomm.C connection/unixsocketseqfile.C sqlrcursor.C
OBJS = sqlrconnection.o connection/connectioncmdline.o connection/ipc.o connection/listenercomm.o connection/scalercomm.o connection/unixsocketseqfile.o sqlrcursor.o
DEBUGOBJS = sqlrconnection_debug.o connection/connectioncmdline_debug.o connection/ipc_debug.o connection/listenercomm_debug.o connection/scalercomm_debug.o connection/unixsocketseqfile_debug.o sqlrcursor_debug.o

all:
	$(MAKE) libsqlrconnection.a; \
	$(MAKE) libsqlrconnection_debug.a; \
	if ( test "$(MINGW32)" != "yes" -a "$(CYGWIN)" != "yes" -a "$(UWIN)" != "yes" ); then \
		$(MAKE) libsqlrconnection.so; \
		$(MAKE) libsqlrconnection_debug.so; \
	else \
		$(MAKE) libsqlrconnection.dll; \
		$(MAKE) libsqlrconnection_debug.dll; \
	fi

clean:
	$(RM) *.o */*.o *.p *.a *.so

rebuild: clean all

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

sqlrconnection_debug.o: sqlrconnection.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

connection/connectioncmdline_debug.o: connection/connectioncmdline.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

connection/ipc_debug.o: connection/ipc.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

connection/listenercomm_debug.o: connection/listenercomm.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

connection/scalercomm_debug.o: connection/scalercomm.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

connection/unixsocketseqfile_debug.o: connection/unixsocketseqfile.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

sqlrcursor_debug.o: sqlrcursor.C
	$(CXX) -DSERVER_DEBUG $(DEBUGCXXFLAGS) $(CPPFLAGS) -c $< -o $@

libsqlrconnection.a: $(SRCS) $(OBJS)
	$(STATICAR) $@ $(OBJS)

libsqlrconnection_debug.a: $(SRCS) $(DEBUGOBJS)
	$(STATICAR) $@ $(DEBUGOBJS)

libsqlrconnection.so: $(SRCS) $(OBJS)
	$(DYNAMICAR) $@ $(OBJS)

libsqlrconnection_debug.so: $(SRCS) $(DEBUGOBJS)
	$(DYNAMICAR) $@ $(DEBUGOBJS)

libsqlrconnection.dll: $(SRCS) $(OBJS)
	dllwrap --export-all-symbols --output-def libsqlrconnection.def --output-exp libsqlrconnection.exp --output-lib libsqlrconnection.lib --dllname $@ $(OBJS) -lstdc++ $(RUDIMENTSLIBS)
	$(RM) libsqlrconnection.def
	$(RM) libsqlrconnection.exp

libsqlrconnection_debug.dll: $(SRCS) $(DEBUGOBJS)
	dllwrap --export-all-symbols --output-def libsqlrconnection.def --output-exp libsqlrconnection.exp --output-lib libsqlrconnection.lib --dllname $@ $(DEBUGOBJS) -lstdc++ $(RUDIMENTSLIBS)
	$(RM) libsqlrconnection.def
	$(RM) libsqlrconnection.exp

install:
	$(top_srcdir)/mkinstalldirs $(libdir)
	$(INSTALL) -m 644 libsqlrconnection.a $(libdir); \
	if ( test "$(MINGW32)" != "yes" -a "$(CYGWIN)" != "yes" -a "$(UWIN)" != "yes" ); then \
		$(RM) $(libdir)/libsqlrconnection.so*; \
		$(RM) $(libdir)/libsqlrconnection_debug.so*; \
		$(INSTALL) libsqlrconnection.so $(libdir)/libsqlrconnection.so.$(SQLR_VERSION); \
		$(INSTALL) libsqlrconnection_debug.so $(libdir)/libsqlrconnection_debug.so.$(SQLR_VERSION); \
		cd $(libdir); \
		$(SYMLINK) libsqlrconnection.so.$(SQLR_VERSION) libsqlrconnection.so; \
		$(SYMLINK) libsqlrconnection_debug.so.$(SQLR_VERSION) libsqlrconnection_debug.so; \
	else \
		$(INSTALL) libsqlrconnection_debug.dll $(libdir); \
		$(INSTALL) libsqlrconnection_debug.lib $(libdir); \
	fi

uninstall:
	$(RM) $(libdir)/libsqlrconnection.so*
	$(RM) $(libdir)/libsqlrconnection_debug.so*
	$(RM) $(libdir)/libsqlrconnection.a
	$(RM) $(libdir)/libsqlrconnection_debug.a
	$(RM) $(libdir)/libsqlrconnection.dll
	$(RM) $(libdir)/libsqlrconnection_debug.dll
	$(RM) $(libdir)/libsqlrconnection.lib
	$(RM) $(libdir)/libsqlrconnection_debug.lib
