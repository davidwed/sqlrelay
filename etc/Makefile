include ../config.mk

INSTALL = $(abs_srcdir)/install-sh -c

install:
	$(abs_srcdir)/mkinstalldirs $(sysconfdir)
	$(INSTALL) -m 644 sqlrelay.conf.example $(sysconfdir)
	$(INSTALL) -m 644 sqlrelay.dtd $(sysconfdir)

uninstall:
	$(RM) $(sysconfdir)/sqlrelay.conf.example
	$(RM) $(sysconfdir)/sqlrelay.dtd
