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

install:
ifeq ($(UNAME),Linux)
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
		$(INSTALL) -m 755 rc.sqlrelay $(initroot)/etc/rc.d/rc.sqlrelay; \
	fi
	if ( test -d "/etc/sysconfig" ); \
	then \
		$(MKINSTALLDIRS) $(initroot)/etc/sysconfig; \
		$(INSTALL) -m 644 sqlrelay $(initroot)/etc/sysconfig/sqlrelay; \
	else \
		$(MKINSTALLDIRS) $(initroot)/etc; \
		$(INSTALL) -m 644 sqlrelay $(initroot)/etc/sqlrelay; \
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
