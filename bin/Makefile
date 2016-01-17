top_builddir = ..

include ../config.mk

install: $(INSTALLCONFIG)

install-clientconfig:
	$(MKINSTALLDIRS) $(bindir)
	$(CP) sqlrclient-config $(bindir)/$(SQLR)client-config
	$(CHMOD) 755 $(bindir)/$(SQLR)client-config
	$(CP) sqlrclientwrapper-config $(bindir)/$(SQLR)clientwrapper-config
	$(CHMOD) 755 $(bindir)/$(SQLR)clientwrapper-config

install-serverconfig:
	$(MKINSTALLDIRS) $(bindir)
	$(CP) sqlrserver-config $(bindir)/$(SQLR)server-config
	$(CHMOD) 755 $(bindir)/$(SQLR)server-config

uninstall:
	$(RM) $(bindir)/$(SQLR)client-config \
		$(bindir)/$(SQLR)clientwrapper-config \
		$(bindir)/$(SQLR)server-config \
		$(bindir)/sqlrclient-config \
		$(bindir)/sqlrclientwrapper-config \
		$(bindir)/sqlrserver-config
