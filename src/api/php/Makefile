include ../../../config.mk

top_builddir = ../../..
INSTALL = $(top_builddir)/install-sh -c

CPPFLAGS = $(BASECPPFLAGS) -I./ -I../c++/include $(RUDIMENTSINCLUDES) $(PHPINCLUDES)

.SUFFIXES: .o

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -DCOMPILE_DL=1 -c $< -o $@

all:
	if ( test ! -r sql_relay.so ); then \
		cp phpincludes1.h phpincludes.h; \
		$(MAKE) sql_relay.so 2> errors1; \
		if ( test ! -r sql_relay.so ); then \
			echo; \
			echo; \
			echo "	Hmm, looks like that didn't work."; \
			echo "	Let's try something different..."; \
			echo; \
			echo; \
			cp phpincludes2.h phpincludes.h; \
			$(MAKE) sql_relay.so 2> errors2; \
			if ( test ! -r sql_relay.so ); then \
				echo "first try errors : "; \
				cat errors1; \
				echo; \
				echo "second try errors : "; \
				cat errors2; \
				echo; \
			fi \
		fi \
	fi; \
	$(RM) errors1; \
	$(RM) errors2

clean:
	$(RM) *.so
	$(RM) *.o
	$(RM) phpincludes.h
	$(RM) errors1
	$(RM) errors2

rebuild: clean all

sql_relay.so: sql_relay.C sql_relay.o
	$(DYNAMICAR) $@ sql_relay.o -L../c++/src/.libs -lsqlrclient $(RUDIMENTSLIBS)

install:
	$(top_builddir)/mkinstalldirs $(PHPEXTDIR)
	$(INSTALL) -s sql_relay.so $(PHPEXTDIR)

uninstall:
	$(RM) $(PHPEXTDIR)/sql_relay.so
