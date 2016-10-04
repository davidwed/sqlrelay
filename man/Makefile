top_builddir = ..

include ../config.mk

all:

install:
	$(MKINSTALLDIRS) $(mandir)
	$(MKINSTALLDIRS) $(mandir)/man1
	$(MKINSTALLDIRS) $(mandir)/man8
	$(CP) man1/sqlrsh.1 $(mandir)/man1
	$(CHMOD) 644 $(mandir)/man1/sqlrsh.1
	$(CP) man1/sqlr-export.1 $(mandir)/man1
	$(CHMOD) 644 $(mandir)/man1/sqlr-export.1
	$(CP) man1/sqlr-import.1 $(mandir)/man1
	$(CHMOD) 644 $(mandir)/man1/sqlr-import.1
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
	$(CP) man8/sqlr-status.8 $(mandir)/man8
	$(CHMOD) 644 $(mandir)/man8/sqlr-status.8
	$(CP) man8/sqlr-pwdenc.8 $(mandir)/man8
	$(CHMOD) 644 $(mandir)/man8/sqlr-pwdenc.8

uninstall:
	$(RM) $(mandir)/man1/sqlrsh.1 \
		$(mandir)/man1/sqlr-export.1 \
		$(mandir)/man1/sqlr-import.1 \
		$(mandir)/man8/sqlr-cachemanager.8 \
		$(mandir)/man8/sqlr-connection.8 \
		$(mandir)/man8/sqlr-listener.8 \
		$(mandir)/man8/sqlr-scaler.8 \
		$(mandir)/man8/sqlr-start.8 \
		$(mandir)/man8/sqlr-stop.8 \
		$(mandir)/man8/sqlr-status.8 \
		$(mandir)/man8/sqlr-pwdenc.8 \
		$(mandir)/man1/fields.1 \
		$(mandir)/man1/query.1 \
		$(mandir)/man1/query.py.1 \
		$(mandir)/man1/sqlr-fields.1 \
		$(mandir)/man1/sqlr-query.1

man:
	help2man --source="SQL Relay" --no-info --libtool ../src/cmdline/sqlrsh > man1/sqlrsh.1
	help2man --source="SQL Relay" --no-info --libtool ../src/cmdline/sqlr-export > man1/sqlr-export.1
	help2man --source="SQL Relay" --no-info --libtool ../src/cmdline/sqlr-import > man1/sqlr-import.1
	help2man --source="SQL Relay" --no-info --section=8 --libtool ../src/server/sqlr-cachemanager > man8/sqlr-cachemanager.8
	help2man --source="SQL Relay" --no-info --section=8 --libtool ../src/server/sqlr-connection > man8/sqlr-connection.8
	help2man --source="SQL Relay" --no-info --section=8 --libtool ../src/server/sqlr-listener > man8/sqlr-listener.8
	help2man --source="SQL Relay" --no-info --section=8 --libtool ../src/server/sqlr-scaler > man8/sqlr-scaler.8
	help2man --source="SQL Relay" --no-info --section=8 --libtool ../src/server/sqlr-start > man8/sqlr-start.8
	help2man --source="SQL Relay" --no-info --section=8 --libtool ../src/server/sqlr-stop > man8/sqlr-stop.8
	help2man --source="SQL Relay" --no-info --section=8 --libtool ../src/server/sqlr-status > man8/sqlr-status.8
	help2man --source="SQL Relay" --no-info --section=8 --libtool ../src/server/sqlr-pwdenc > man8/sqlr-pwdenc.8
