include ../../config.mk

all:
	cd c++; $(MAKE) all
	cd c; $(MAKE) all
	if ( test -n "$(HAVE_PYTHON)" ); then \
		cd python; $(MAKE) all; \
	fi
	if ( test -n "$(HAVE_PERL)" ); then \
		cd perl/SQLRConnection; $(MAKE) -f Makefile.master all; \
		cd ../..; \
		cd perl/SQLRCursor; $(MAKE) -f Makefile.master all; \
		cd ../..; \
		cd perl/DBD; $(MAKE) -f Makefile.master all; \
	fi
	if ( test -n "$(HAVE_RUBY)" ); then \
		cd ruby; $(MAKE) -f Makefile.master all; \
	fi
	if ( test -n "$(HAVE_PHP)" ); then \
		cd php; $(MAKE) all; \
	fi
	if ( test -n "$(HAVE_JAVA)" ); then \
		cd java; $(MAKE) all; \
	fi

clean:
	cd c++; $(MAKE) clean
	cd c; $(MAKE) clean
	if ( test -n "$(HAVE_PYTHON)" ); then \
		cd python; $(MAKE) clean; \
	fi
	if ( test -n "$(HAVE_PERL)" ); then \
		cd perl/SQLRConnection; $(MAKE) -f Makefile.master clean; \
		cd ../..; \
		cd perl/SQLRCursor; $(MAKE) -f Makefile.master clean; \
		cd ../..; \
		cd perl/DBD; $(MAKE) -f Makefile.master clean; \
	fi
	if ( test -n "$(HAVE_RUBY)" ); then \
		cd ruby; $(MAKE) -f Makefile.master clean; \
	fi
	if ( test -n "$(HAVE_PHP)" ); then \
		cd php; $(MAKE) clean; \
	fi
	if ( test -n "$(HAVE_JAVA)" ); then \
		cd java; $(MAKE) clean; \
	fi

rebuild:
	cd c++; $(MAKE) rebuild
	cd c; $(MAKE) rebuild
	if ( test -n "$(HAVE_PYTHON)" ); then \
		cd python; $(MAKE) rebuild; \
	fi
	if ( test -n "$(HAVE_PERL)" ); then \
		cd perl/SQLRConnection; $(MAKE) -f Makefile.master rebuild; \
		cd ../..; \
		cd perl/SQLRCursor; $(MAKE) -f Makefile.master rebuild; \
		cd ../..; \
		cd perl/DBD; $(MAKE) -f Makefile.master rebuild; \
	fi
	if ( test -n "$(HAVE_RUBY)" ); then \
		cd ruby; $(MAKE) -f Makefile.master rebuild; \
	fi
	if ( test -n "$(HAVE_PHP)" ); then \
		cd php; $(MAKE) rebuild; \
	fi
	if ( test -n "$(HAVE_JAVA)" ); then \
		cd java; $(MAKE) rebuild; \
	fi

install:
	cd c++; $(MAKE) install
	cd c; $(MAKE) install
	if ( test -n "$(HAVE_PYTHON)" ); then \
		cd python; $(MAKE) install; \
	fi
	if ( test -n "$(HAVE_RUBY)" ); then \
		cd ruby; $(MAKE) -f Makefile.master install; \
	fi
	if ( test -n "$(HAVE_PHP)" ); then \
		cd php; $(MAKE) install; \
	fi
	if ( test -n "$(HAVE_PERL)" ); then \
		cd perl/SQLRConnection; $(MAKE) -f Makefile.master install; \
		cd ../..; \
		cd perl/SQLRCursor; $(MAKE) -f Makefile.master install; \
		cd ../..; \
		cd perl/DBD; $(MAKE) -f Makefile.master install; \
	fi
	if ( test -n "$(HAVE_JAVA)" ); then \
		cd java; $(MAKE) install; \
	fi
	cd zope; $(MAKE) install

uninstall:
	cd c++; $(MAKE) uninstall
	cd c; $(MAKE) uninstall
	if ( test -n "$(HAVE_PYTHON)" ); then \
		cd python; $(MAKE) uninstall; \
	fi
	if ( test -n "$(HAVE_RUBY)" ); then \
		cd ruby; $(MAKE) -f Makefile.master uninstall; \
	fi
	if ( test -n "$(HAVE_PHP)" ); then \
		cd php; $(MAKE) uninstall; \
	fi
	if ( test -n "$(HAVE_PERL)" ); then \
		cd perl/SQLRConnection; $(MAKE) -f Makefile.master uninstall; \
		cd ../..; \
		cd perl/SQLRCursor; $(MAKE) -f Makefile.master uninstall; \
		cd ../..; \
		cd perl/DBD; $(MAKE) -f Makefile.master uninstall; \
	fi
	if ( test -n "$(HAVE_JAVA)" ); then \
		cd java; $(MAKE) uninstall; \
	fi
	cd zope; $(MAKE) uninstall
