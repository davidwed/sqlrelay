include ../config.mk

top_srcdir = ..
INSTALL = $(top_srcdir)/install-sh -c

install:
ifeq ($(UNAME),Linux)
	if ( test -d "/etc/rc.d/init.d" -a -d "/etc/sysconfig" -a -d "/var/lock/subsys" ); then \
		$(INSTALL) -m 755 redhat/init.d/sqlrelay /etc/rc.d/init.d; \
		$(INSTALL) -m 644 redhat/sysconfig/sqlrelay /etc/sysconfig; \
	fi; \
	if ( test -r "/etc/rc.d/rc.inet2" -a -d "/etc/rc.d" -a -d "/var/lock/subsys" ); then \
		$(INSTALL) -m 755 slackware/rc.d/rc.sqlrelay /etc/rc.d; \
		$(INSTALL) -m 644 slackware/sqlrelay /etc; \
	fi; \
	if ( test -r "/etc/init.d" -a -d "/var/lock" ); then \
		$(INSTALL) -m 755 debian/init.d/sqlrelay /etc/init.d; \
		$(INSTALL) -m 644 debian/sqlrelay /etc; \
	fi
endif
ifeq ($(UNAME),NetBSD)
	$(INSTALL) -m 755 netbsd/rc.d/sqlrelay /etc/rc.d
	$(INSTALL) -m 644 netbsd/sqlrelay /etc
endif
ifeq ($(UNAME),FreeBSD)
	$(INSTALL) -m 755 freebsd/rc.d/sqlrelay /usr/local/etc/rc.d
	$(INSTALL) -m 644 freebsd/sqlrelay /etc
endif
ifeq ($(UNAME),SCO_SV)
	$(INSTALL) -m 755 openserver/init.d/sqlrelay /etc/init.d
	$(INSTALL) -m 644 openserver/sqlrelay /etc
endif
	if ( test -x "/sbin/chkconfig" ); then \
		/sbin/chkconfig --add sqlrelay; \
	fi

uninstall:
	if ( test -x "/sbin/chkconfig" -a -n "`/sbin/chkconfig --list sqlrelay 2> /dev/null`"); then \
		/sbin/chkconfig --del sqlrelay; \
	fi
	$(RM) /etc/rc.d/init.d/sqlrelay
	$(RM) /etc/rc.d/rc.sqlrelay
	$(RM) /etc/init.d/sqlrelay
	$(RM) /usr/local/etc/rc.d/sqlrelay
	$(RM) /etc/sysconfig/sqlrelay
	$(RM) /etc/sqlrelay
