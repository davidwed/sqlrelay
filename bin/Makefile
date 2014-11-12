top_builddir = ..

include ../config.mk

.PHONY: install uninstall

install:
	$(MKINSTALLDIRS) $(bindir)
	$(INSTALL) sqlrclient-config $(bindir)
	$(INSTALL) sqlrclientwrapper-config $(bindir)
	$(INSTALL) sqlrserver-config $(bindir)

uninstall:
	$(RM) $(bindir)/sqlr-kill
	$(RM) $(bindir)/killsqlr
	$(RM) $(bindir)/sqlrclient-config
	$(RM) $(bindir)/sqlrclientwrapper-config
	$(RM) $(bindir)/sqlrserver-config
	$(RM) $(bindir)/sqlrconnection-config
