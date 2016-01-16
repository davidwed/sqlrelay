top_builddir = ..

include ../config.mk

install:
	$(MKINSTALLDIRS) $(mandir)
	$(MKINSTALLDIRS) $(mandir)/man1
	$(MKINSTALLDIRS) $(mandir)/man8
	$(CP) man1/sqlrsh.1 $(mandir)/man1
	$(CHMOD) 644 $(mandir)/man1/sqlrsh.1
	$(CP) man8/sqlr-cachemanager.8 $(mandir)/man8
	$(CHMOD) 644 $(mandir)/man8/sqlr-cachemanager.8
	$(CP) man8/sqlr-connection.8 $(mandir)/man8
	$(CHMOD) 644 $(mandir)/man8/sqlr-connection.8
	$(CP) man8/sqlr-listener.8 $(mandir)/man8
	$(CHMOD) 644 $(mandir)/man8/sqlr-listener.8
	$(CP) man8/sqlr-scaler.8 $(mandir)/man8
	$(CHMOD) 644 $(mandir)/man8/sqlr-scaler.8
	$(CP) man8/sqlr-start.8 $(mandir)/man8
	$(CHMOD) 644 $(mandir)/man8/sqlr-start.8
	$(CP) man8/sqlr-stop.8 $(mandir)/man8
	$(CHMOD) 644 $(mandir)/man8/sqlr-stop.8

uninstall:
	$(RM) $(mandir)/man1/sqlrsh.1 \
		$(mandir)/man8/sqlr-cachemanager.8 \
		$(mandir)/man8/sqlr-connection.8 \
		$(mandir)/man8/sqlr-listener.8 \
		$(mandir)/man8/sqlr-scaler.8 \
		$(mandir)/man8/sqlr-start.8 \
		$(mandir)/man8/sqlr-stop.8 \
		$(mandir)/man1/fields.1 \
		$(mandir)/man1/query.1 \
		$(mandir)/man1/query.py.1 \
		$(mandir)/man1/sqlr-fields.1 \
		$(mandir)/man1/sqlr-query.1

man:
	help2man --source="SQL Relay" --no-info sqlrsh > man1/sqlrsh.1
