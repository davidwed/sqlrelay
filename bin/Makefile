top_builddir = ..

include ../config.mk

install: $(INSTALLCONFIG)

install-clientconfig:
	$(MKINSTALLDIRS) $(bindir)
	$(CP) sqlrclient-config $(bindir)
	$(CHMOD) 755 $(bindir)/sqlrclient-config
	$(CP) sqlrclientwrapper-config $(bindir)
	$(CHMOD) 755 $(bindir)/sqlrclientwrapper-config

install-serverconfig:
	$(MKINSTALLDIRS) $(bindir)
	$(CP) sqlrserver-config $(bindir)
	$(CHMOD) 755 $(bindir)/sqlrserver-config

uninstall:
	$(RM) $(bindir)/sqlrclient-config \
		$(bindir)/sqlrclientwrapper-config \
		$(bindir)/sqlrserver-config
