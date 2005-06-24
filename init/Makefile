top_builddir = ..

include ../config.mk

.PHONY: install uninstall

UID = $(shell id | grep "uid=0" )

ifneq ($(UID),)
initroot = $(DESTDIR)
else
ifneq ($(DESTDIR),)
initroot = $(DESTDIR)
else
initroot = $(prefix)
endif
endif

install:
ifeq ($(UNAME),Linux)
	if ( test -d "/etc/rc.d/init.d" -a -d "/etc/sysconfig" -a -d "/var/lock/subsys" ); then \
		$(MKINSTALLDIRS) $(initroot)/etc/rc.d/init.d; \
		$(MKINSTALLDIRS) $(initroot)/etc/sysconfig; \
		$(INSTALL) -m 755 redhat/init.d/sqlrelay $(initroot)/etc/rc.d/init.d; \
		$(INSTALL) -m 644 redhat/sysconfig/sqlrelay $(initroot)/etc/sysconfig; \
	else \
		if ( test -r "/etc/rc.d/rc.inet2" -a -d "/etc/rc.d" -a -d "/var/lock/subsys" ); then \
			$(MKINSTALLDIRS) $(initroot)/etc/rc.d; \
			$(INSTALL) -m 755 slackware/rc.d/rc.sqlrelay $(initroot)/etc/rc.d; \
			$(INSTALL) -m 644 slackware/sqlrelay $(initroot)/etc; \
		else \
			if ( test -r "/etc/init.d" -a -d "/var/lock" ); then \
				$(MKINSTALLDIRS) $(initroot)/etc/init.d; \
				$(INSTALL) -m 755 debian/init.d/sqlrelay $(initroot)/etc/init.d; \
				$(INSTALL) -m 644 debian/sqlrelay $(initroot)/etc; \
			fi; \
		fi; \
	fi
endif
ifeq ($(UNAME),NetBSD)
	$(MKINSTALLDIRS) $(initroot)/etc/rc.d
	$(INSTALL) -m 755 netbsd/rc.d/sqlrelay $(initroot)/etc/rc.d
	$(INSTALL) -m 644 netbsd/sqlrelay $(initroot)/etc
endif
ifeq ($(UNAME),FreeBSD)
	$(MKINSTALLDIRS) $(initroot)/usr/local/etc/rc.d
	$(INSTALL) -m 755 freebsd/rc.d/sqlrelay $(initroot)/usr/local/etc/rc.d
	$(INSTALL) -m 644 freebsd/sqlrelay $(initroot)/etc
endif
ifeq ($(UNAME),SCO_SV)
	$(MKINSTALLDIRS) $(initroot)/etc/init.d
	$(INSTALL) -m 755 openserver/init.d/sqlrelay $(initroot)/etc/init.d
	$(INSTALL) -m 644 openserver/sqlrelay $(initroot)/etc
endif
ifeq ($(UNAME),IRIX)
	# should work for irix too
	$(MKINSTALLDIRS) $(initroot)/etc/init.d
	$(INSTALL) -m 755 openserver/init.d/sqlrelay $(initroot)/etc/init.d
	$(INSTALL) -m 644 openserver/sqlrelay $(initroot)/etc
endif
ifeq ($(UNAME),IRIX64)
	# should work for irix too
	$(MKINSTALLDIRS) $(initroot)/etc/init.d
	$(INSTALL) -m 755 openserver/init.d/sqlrelay $(initroot)/etc/init.d
	$(INSTALL) -m 644 openserver/sqlrelay $(initroot)/etc
endif
ifeq ($(initroot),)
	if ( test -x "/sbin/chkconfig" -a -n "`/sbin/chkconfig -h 2>&1 | grep add`" ); then \
		/sbin/chkconfig --add sqlrelay; \
	fi
endif

uninstall:
	if ( test -x "/sbin/chkconfig" -a -n "`/sbin/chkconfig -h 2>&1 | grep add`" -a -n "`/sbin/chkconfig --list sqlrelay 2> /dev/null`"); then \
		/sbin/chkconfig --del sqlrelay; \
	fi
	$(RM) $(initroot)/etc/rc.d/init.d/sqlrelay
	$(RM) $(initroot)/etc/rc.d/rc.sqlrelay
	$(RM) $(initroot)/etc/init.d/sqlrelay
	$(RM) $(initroot)/usr/local/etc/rc.d/sqlrelay
	$(RM) $(initroot)/etc/sysconfig/sqlrelay
	$(RM) $(initroot)/etc/sqlrelay
