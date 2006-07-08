top_builddir = ..

include ../config.mk

.PHONY: install uninstall

install:
	$(MKINSTALLDIRS) $(sysconfdir)
	$(INSTALL) -m 644 sqlrelay.conf.example $(sysconfdir)
	$(INSTALL) -m 644 sqlrelay.dtd $(sysconfdir)
	$(INSTALL) -m 644 sqlrelay.conf.example $(EXAMPLEDIR)/sqlrelay.conf

uninstall:
	$(RM) $(sysconfdir)/sqlrelay.conf.example
	$(RM) $(sysconfdir)/sqlrelay.dtd
	$(RM) $(EXAMPLEDIR)/sqlrelay.conf
