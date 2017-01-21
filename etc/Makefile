top_builddir = ..

include ../config.mk

install:
	$(MKINSTALLDIRS) $(sysconfdir)/$(SQLRELAY).conf.d
	$(CP) sqlrelay.xsd $(sysconfdir)/$(SQLRELAY).xsd
	$(CHMOD) 644 $(sysconfdir)/$(SQLRELAY).xsd
	$(MKINSTALLDIRS) $(EXAMPLEDIR)
	$(CP) sqlrelay.conf $(EXAMPLEDIR)/$(SQLRELAY).conf
	$(CHMOD) 644 $(EXAMPLEDIR)/$(SQLRELAY).conf

uninstall:
	$(RM) $(EXAMPLEDIR)/$(SQLRELAY).conf \
		$(EXAMPLEDIR)/sqlrelay.conf \
		$(sysconfdir)/sqlrelay.xsd \
		$(sysconfdir)/sqlrelay.dtd \
		$(sysconfdir)/sqlrelay.conf.example \
		$(datadir)/examples/sqlrelay.conf
	-$(RMDIR) $(sysconfdir)/$(SQLRELAY).conf.d 2> /dev/null
	-$(RMDIR) $(sysconfdir)/sqlrelay.conf.d 2> /dev/null
	-$(RMDIR) $(EXAMPLEDIR) 2> /dev/null
	-$(RMDIR) $(datadir)/$(SQLRELAY) 2> /dev/null
	-$(RMDIR) $(datadir)/examples 2> /dev/null
