top_builddir = ..

include ../config.mk

.PHONY: install uninstall

install:
	$(MKINSTALLDIRS) $(mandir)
	$(MKINSTALLDIRS) $(mandir)/man1
	$(MKINSTALLDIRS) $(mandir)/man8
	$(INSTALL) -m 644 man1/sqlr-fields.1 $(mandir)/man1
	$(INSTALL) -m 644 man1/sqlr-query.1 $(mandir)/man1
	$(INSTALL) -m 644 man1/sqlrsh.1 $(mandir)/man1
	$(INSTALL) -m 644 man8/sqlr-cachemanager.8 $(mandir)/man8
	$(INSTALL) -m 644 man8/sqlr-connection.8 $(mandir)/man8
	$(INSTALL) -m 644 man8/sqlr-listener.8 $(mandir)/man8
	$(INSTALL) -m 644 man8/sqlr-scaler.8 $(mandir)/man8
	$(INSTALL) -m 644 man8/sqlr-start.8 $(mandir)/man8
	$(INSTALL) -m 644 man8/sqlr-stop.8 $(mandir)/man8

uninstall:
	$(RM) $(mandir)/man1/fields.1
	$(RM) $(mandir)/man1/query.1
	$(RM) $(mandir)/man1/query.py.1
	$(RM) $(mandir)/man1/sqlr-fields.1
	$(RM) $(mandir)/man1/sqlr-query.1
	$(RM) $(mandir)/man1/sqlrsh.1
	$(RM) $(mandir)/man8/sqlr-cachemanager.8
	$(RM) $(mandir)/man8/sqlr-connection.8
	$(RM) $(mandir)/man8/sqlr-listener.8
	$(RM) $(mandir)/man8/sqlr-scaler.8
	$(RM) $(mandir)/man8/sqlr-start.8
	$(RM) $(mandir)/man8/sqlr-stop.8

