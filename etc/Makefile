top_builddir = ..

include ../config.mk

install:
	$(MKINSTALLDIRS) $(sysconfdir)
	$(MKINSTALLDIRS) $(sysconfdir)/sqlrelay.conf.d
	$(CP) sqlrelay.conf.example $(sysconfdir)
	$(CHMOD) 644 $(sysconfdir)/sqlrelay.conf.example
	$(CP) sqlrelay.dtd $(sysconfdir)
	$(CHMOD) 644 $(sysconfdir)/sqlrelay.dtd
	$(CP) sqlrelay.xsd $(sysconfdir)
	$(CHMOD) 644 $(sysconfdir)/sqlrelay.xsd
	$(CP) sqlrelay.conf.example $(EXAMPLEDIR)/sqlrelay.conf
	$(CHMOD) 644 $(EXAMPLEDIR)/sqlrelay.conf

uninstall:
	$(RM) $(sysconfdir)/sqlrelay.conf.example \
		$(sysconfdir)/sqlrelay.xsd \
		$(sysconfdir)/sqlrelay.dtd \
		$(EXAMPLEDIR)/sqlrelay.conf
