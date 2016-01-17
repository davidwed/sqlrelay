top_builddir = ..

include ../config.mk

install:
	$(MKINSTALLDIRS) $(sysconfdir)/sqlrelay.conf.d
	$(MKINSTALLDIRS) $(EXAMPLEDIR)
	$(CP) sqlrelay.conf $(EXAMPLEDIR)/sqlrelay.conf
	$(CHMOD) 644 $(EXAMPLEDIR)/sqlrelay.conf

uninstall:
	$(RM) $(EXAMPLEDIR)/sqlrelay.conf \
		$(sysconfdir)/sqlrelay.xsd \
		$(sysconfdir)/sqlrelay.dtd \
		$(sysconfdir)/sqlrelay.conf.example \
		$(datadir)/examples/sqlrelay.conf
	-$(RMDIR) $(sysconfdir)/sqlrelay.conf.d 2> /dev/null
	-$(RMDIR) $(EXAMPLEDIR) 2> /dev/null
	-$(RMDIR) $(datadir)/$(SQLRELAY) 2> /dev/null
	-$(RMDIR) $(datadir)/examples 2> /dev/null
