include ../config.mk

top_srcdir = ..
INSTALL = $(top_srcdir)/install-sh -c

install:
	$(top_srcdir)/mkinstalldirs $(mandir)
	$(top_srcdir)/mkinstalldirs $(mandir)/man1
	$(top_srcdir)/mkinstalldirs $(mandir)/man8
	$(INSTALL) -m 644 man1/fields.1 $(mandir)/man1
	$(INSTALL) -m 644 man1/query.1 $(mandir)/man1
	$(INSTALL) -m 644 man1/query.py.1 $(mandir)/man1
	$(INSTALL) -m 644 man1/sqlr-config-gtk.1 $(mandir)/man1
	$(INSTALL) -m 644 man1/sqlrsh.1 $(mandir)/man1
	$(INSTALL) -m 644 man8/sqlr-cachemanager.8 $(mandir)/man8
	$(INSTALL) -m 644 man8/sqlr-connection.8 $(mandir)/man8
	$(INSTALL) -m 644 man8/sqlr-ipclean.8 $(mandir)/man8
	$(INSTALL) -m 644 man8/sqlr-listener.8 $(mandir)/man8
	$(INSTALL) -m 644 man8/sqlr-scaler.8 $(mandir)/man8
	$(INSTALL) -m 644 man8/sqlr-start.8 $(mandir)/man8
	$(INSTALL) -m 644 man8/sqlr-stop.8 $(mandir)/man8

uninstall:
	$(RM) $(mandir)/man1/fields.1
	$(RM) $(mandir)/man1/query.1
	$(RM) $(mandir)/man1/query.py.1
	$(RM) $(mandir)/man1/sqlr-config-gtk.1
	$(RM) $(mandir)/man1/sqlrsh.1
	$(RM) $(mandir)/man8/sqlr-cachemanager.8
	$(RM) $(mandir)/man8/sqlr-connection.8
	$(RM) $(mandir)/man8/sqlr-ipclean.8
	$(RM) $(mandir)/man8/sqlr-listener.8
	$(RM) $(mandir)/man8/sqlr-scaler.8
	$(RM) $(mandir)/man8/sqlr-start.8
	$(RM) $(mandir)/man8/sqlr-stop.8

