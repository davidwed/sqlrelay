include ../../../config.mk

install:
	if ( test -n "$(HAVE_ZOPE)" -a -n "$(HAVE_PYTHON)" ); then \
		$(RMTREE) $(ZOPEDIR)/ZSQLRelayDA; \
		$(abs_srcdir)/mkinstalldirs $(ZOPEDIR); \
		cp -r ZSQLRelayDA $(ZOPEDIR); \
		cp -r $(PYTHONDIR)/site-packages/SQLRelay $(ZOPEDIR)/ZSQLRelayDA; \
	else \
		cat install.Makefile; \
	fi

uninstall:
	if ( test -n "$(HAVE_ZOPE)" -a -n "$(HAVE_PYTHON)" ); then \
		$(RMTREE) $(ZOPEDIR)/ZSQLRelayDA; \
	else \
		cat uninstall.Makefile; \
	fi
