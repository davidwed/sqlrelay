include ../config.mk

top_srcdir = ..
INSTALL = $(top_srcdir)/install-sh -c

install:
	$(top_srcdir)/mkinstalldirs $(bindir)
	$(INSTALL) backupschema $(bindir)
	$(INSTALL) sqlr-stop $(bindir)
	$(INSTALL) sqlrclient-config $(bindir)
	$(INSTALL) sqlrclientwrapper-config $(bindir)

uninstall:
	$(RM) $(bindir)/backupschema
	$(RM) $(bindir)/sqlr-stop
	$(RM) $(bindir)/sqlr-kill
	$(RM) $(bindir)/killsqlr
	$(RM) $(bindir)/sqlrclient-config
	$(RM) $(bindir)/sqlrclientwrapper-config
