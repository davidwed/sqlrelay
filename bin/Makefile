top_builddir = ..

include ../config.mk

install:
	$(MKINSTALLDIRS) $(bindir)
	$(CP) sqlrclient-config $(bindir)
	$(CHMOD) 755 $(bindir)/sqlrclient-config
	$(CP) sqlrclientwrapper-config $(bindir)
	$(CHMOD) 755 $(bindir)/sqlrclientwrapper-config
	$(CP) sqlrserver-config $(bindir)
	$(CHMOD) 755 $(bindir)/sqlrserver-config

uninstall:
	$(RM) $(bindir)/sqlr-kill \
		$(bindir)/killsqlr \
		$(bindir)/sqlrclient-config \
		$(bindir)/sqlrclientwrapper-config \
		$(bindir)/sqlrserver-config \
		$(bindir)/sqlrconnection-config \
		$(bindir)/backupschema
