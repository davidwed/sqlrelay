top_builddir = ../../..

include ../../../config.mk

install:
	if ( test -n "$(HAVE_ZOPE)" -a -n "$(HAVE_PYTHON)" ); then \
		$(RMTREE) $(DESTDIR)$(ZOPEDIR)/ZSQLRelayDA; \
		$(MKINSTALLDIRS) $(DESTDIR)$(ZOPEDIR); \
		cp -r ZSQLRelayDA $(DESTDIR)$(ZOPEDIR); \
		cp -r $(PYTHONDIR)/site-packages/SQLRelay $(DESTDIR)$(ZOPEDIR)/ZSQLRelayDA; \
	else \
		cat install.Makefile; \
	fi

uninstall:
	if ( test -n "$(HAVE_ZOPE)" -a -n "$(HAVE_PYTHON)" ); then \
		$(RMTREE) $(DESTDIR)$(ZOPEDIR)/ZSQLRelayDA; \
	else \
		cat uninstall.Makefile; \
	fi
