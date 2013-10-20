top_builddir = ..

include ../config.mk

.PHONY: install uninstall

UID = $(shell id | grep "uid=0" )

ifneq ($(initscript_prefix),)
ifneq ($(DESTDIR),)
initroot = $(DESTDIR)$(initscript_prefix)
else
initroot = $(initscript_prefix)
endif
else
ifneq ($(UID),)
initroot = $(DESTDIR)
else
ifneq ($(DESTDIR),)
initroot = $(DESTDIR)
else
initroot = $(prefix)
endif
endif
endif

all:

install:
	if ( test -d "/etc/rc.d/init.d" ); \
	then \
		$(MKINSTALLDIRS) $(initroot)/etc/rc.d/init.d; \
		$(INSTALL) -m 755 rc.sqlrelay $(initroot)/etc/rc.d/init.d/sqlrelay; \
	elif ( test -d "/etc/init.d" ); \
	then \
		$(MKINSTALLDIRS) $(initroot)/etc/init.d; \
		$(INSTALL) -m 755 rc.sqlrelay $(initroot)/etc/init.d/sqlrelay; \
	elif ( test -d "/etc/rc.d" ); \
	then \
		$(MKINSTALLDIRS) $(initroot)/etc/rc.d; \
		if ( test -r "/etc/rc.conf" ); \
		then \
			$(INSTALL) -m 755 rc.sqlrelay $(initroot)/etc/rc.d/sqlrelay; \
		else \
			$(INSTALL) -m 755 rc.sqlrelay $(initroot)/etc/rc.d/rc.sqlrelay; \
		fi; \
	fi
	if ( test -d "/etc/sysconfig" ); \
	then \
		$(MKINSTALLDIRS) $(initroot)/etc/sysconfig; \
		$(INSTALL) -m 644 sqlrelay $(initroot)/etc/sysconfig/sqlrelay; \
	else \
		$(MKINSTALLDIRS) $(initroot)/etc; \
		$(INSTALL) -m 644 sqlrelay $(initroot)/etc/sqlrelay; \
	fi
ifeq ($(initroot),)
	if ( test -x "/sbin/chkconfig" ); \
	then \
		/sbin/chkconfig --add sqlrelay; \
	elif ( test -x "/usr/sbin/update-rc.d" ); \
	then \
		/usr/sbin/update-rc.d sqlrelay defaults; \
	fi
endif

uninstall:
	if ( test -x "/sbin/chkconfig" -a -n "`/sbin/chkconfig --list sqlrelay 2> /dev/null`"); then \
		/sbin/chkconfig --del sqlrelay; \
	fi
	$(RM) $(initroot)/etc/rc.d/init.d/sqlrelay
	$(RM) $(initroot)/etc/rc.d/rc.sqlrelay
	$(RM) $(initroot)/etc/init.d/sqlrelay
	$(RM) $(initroot)/usr/local/etc/rc.d/sqlrelay
	if ( test -x "/usr/sbin/update-rc.d" ); \
	then \
		/usr/sbin/update-rc.d sqlrelay remove; \
	fi
	$(RM) $(initroot)/etc/sysconfig/sqlrelay
	$(RM) $(initroot)/etc/sqlrelay
