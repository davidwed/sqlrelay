top_builddir = ..

include ../config.mk

.PHONY: install uninstall

install:
	$(MKINSTALLDIRS) $(sysconfdir)
	$(INSTALL) -m 644 sqlrelay.conf.example $(sysconfdir)
	$(INSTALL) -m 644 sqlrelay.dtd $(sysconfdir)

uninstall:
	$(RM) $(sysconfdir)/sqlrelay.conf.example
	$(RM) $(sysconfdir)/sqlrelay.dtd
