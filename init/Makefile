top_builddir = ..

include ../config.mk

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
		$(CP) rc.sqlrelay $(initroot)/etc/rc.d/init.d/sqlrelay; \
		$(CHMOD) 755 $(initroot)/etc/rc.d/init.d/sqlrelay; \
		$(CP) rc.sqlrcachemanager $(initroot)/etc/rc.d/init.d/sqlrcachemanager; \
		$(CHMOD) 755 $(initroot)/etc/rc.d/init.d/sqlrcachemanager; \
	elif ( test -d "/etc/init.d" ); \
	then \
		$(MKINSTALLDIRS) $(initroot)/etc/init.d; \
		$(CP) rc.sqlrelay $(initroot)/etc/init.d/sqlrelay; \
		$(CHMOD) 755 $(initroot)/etc/init.d/sqlrelay; \
		$(CP) rc.sqlrcachemanager $(initroot)/etc/init.d/sqlrcachemanager; \
		$(CHMOD) 755 $(initroot)/etc/init.d/sqlrcachemanager; \
	elif ( test -d "/etc/rc.d" ); \
	then \
		$(MKINSTALLDIRS) $(initroot)/etc/rc.d; \
		if ( test -r "/etc/rc.conf" ); \
		then \
			$(CP) rc.sqlrelay $(initroot)/etc/rc.d/sqlrelay; \
			$(CHMOD) 755 $(initroot)/etc/rc.d/sqlrelay; \
			$(CP) rc.sqlrcachemanager $(initroot)/etc/rc.d/sqlrcachemanager; \
			$(CHMOD) 755 $(initroot)/etc/rc.d/sqlrcachemanager; \
		else \
			$(CP) rc.sqlrelay $(initroot)/etc/rc.d/rc.sqlrelay; \
			$(CHMOD) 755 $(initroot)/etc/rc.d/rc.sqlrelay; \
			$(CP) rc.sqlrcachemanager $(initroot)/etc/rc.d/rc.sqlrcachemanager; \
			$(CHMOD) 755 $(initroot)/etc/rc.d/rc.sqlrcachemanager; \
		fi; \
	elif ( test -d "/usr/pkg/etc/rc.d" ); \
	then \
		$(MKINSTALLDIRS) $(initroot)/usr/pkg/etc/rc.d; \
		$(CP) rc.sqlrelay $(initroot)/usr/pkg/etc/rc.d/sqlrelay; \
		$(CHMOD) 755 $(initroot)/usr/pkg/etc/rc.d/sqlrelay; \
		$(CP) rc.sqlrcachemanager $(initroot)/usr/pkg/etc/rc.d/sqlrcachemanager; \
		$(CHMOD) 755 $(initroot)/usr/pkg/etc/rc.d/sqlrcachemanager; \
	elif ( test -d "/Library/LaunchDaemons" ); \
	then \
		$(MKINSTALLDIRS) $(initroot)/Library/LaunchDaemons; \
		$(CP) rc.sqlrelay $(bindir)/bin/rc.sqlrelay; \
		$(CHMOD) 755 $(bindir)/bin/rc.sqlrelay; \
		$(CP) rc.sqlrcachemanager $(bindir)/bin/rc.sqlrcachemanager; \
		$(CHMOD) 755 $(bindir)/bin/rc.sqlrcachemanager; \
		$(CP) com.firstworks.sqlrelay.plist $(initroot)/Library/LaunchDaemons; \
		$(CHMOD) 644 $(initroot)/Library/LaunchDaemons/com.firstworks.sqlrelay.plist; \
		$(CP) com.firstworks.sqlrcachemanager.plist $(initroot)/Library/LaunchDaemons; \
		$(CHMOD) 644 $(initroot)/Library/LaunchDaemons/com.firstworks.sqlrcachemanager.plist; \
	fi
ifeq ($(initroot),)
	if ( test -x "/sbin/chkconfig" ); \
	then \
		/sbin/chkconfig --add sqlrelay; \
		/sbin/chkconfig --add sqlrcachemanager; \
	elif ( test -x "/usr/sbin/update-rc.d" ); \
	then \
		/usr/sbin/update-rc.d sqlrelay defaults; \
		/usr/sbin/update-rc.d sqlrcachemanager defaults; \
	elif ( test -x "/bin/launchctl" ); \
	then \
		/bin/launchctl load /Library/LaunchDaemons/com.firstworks.sqlrelay.plist; \
		/bin/launchctl load /Library/LaunchDaemons/com.firstworks.sqlrcachemanager.plist; \
	elif ( test -x "/etc/init.d/sqlrelay" ); \
	then \
		if ( test -d "/etc/rc0.d" ); \
		then \
			cd /etc/rc0.d; \
			ln -s ../init.d/sqlrelay K15sqlrelay; \
			ln -s ../init.d/sqlrcachemanager K15sqlrcachemanager; \
		fi; \
		if ( test -d "/etc/rc1.d" ); \
		then \
			cd /etc/rc1.d; \
			ln -s ../init.d/sqlrelay K15sqlrelay; \
			ln -s ../init.d/sqlrcachemanager K15sqlrcachemanager; \
		fi; \
		if ( test -d "/etc/rc2.d" ); \
		then \
			cd /etc/rc2.d; \
			ln -s ../init.d/sqlrelay S85sqlrelay; \
			ln -s ../init.d/sqlrcachemanager S85sqlrcachemanager; \
		fi; \
		if ( test -d "/etc/rc3.d" ); \
		then \
			cd /etc/rc3.d; \
			ln -s ../init.d/sqlrelay S85sqlrelay; \
			ln -s ../init.d/sqlrcachemanager S85sqlrcachemanager; \
		fi; \
		if ( test -d "/etc/rc4.d" ); \
		then \
			cd /etc/rc4.d; \
			ln -s ../init.d/sqlrelay S85sqlrelay; \
			ln -s ../init.d/sqlrcachemanager S85sqlrcachemanager; \
		fi; \
		if ( test -d "/etc/rc5.d" ); \
		then \
			cd /etc/rc5.d; \
			ln -s ../init.d/sqlrelay S85sqlrelay; \
			ln -s ../init.d/sqlrcachemanager S85sqlrcachemanager; \
		fi; \
		if ( test -d "/etc/rc6.d" ); \
		then \
			cd /etc/rc6.d; \
			ln -s ../init.d/sqlrelay K15sqlrelay; \
			ln -s ../init.d/sqlrcachemanager K15sqlrcachemanager; \
		fi; \
	fi
endif

uninstall:
	if ( test -x "/sbin/chkconfig" -a -n \
		"`/sbin/chkconfig --list sqlrelay 2> /dev/null`"); \
	then \
		/sbin/chkconfig --del sqlrelay; \
	fi
	if ( test -x "/sbin/chkconfig" -a -n \
		"`/sbin/chkconfig --list sqlrcachemanager 2> /dev/null`"); \
	then \
		/sbin/chkconfig --del sqlrcachemanager; \
	fi
	$(RM) $(initroot)/etc/rc.d/init.d/sqlrelay \
		$(initroot)/etc/rc.d/init.d/sqlrcachemanager \
		$(initroot)/etc/rc.d/rc.sqlrelay \
		$(initroot)/etc/rc.d/rc.sqlrcachemanager \
		$(initroot)/etc/init.d/sqlrelay \
		$(initroot)/etc/init.d/sqlrcachemanager \
		$(initroot)/rc.d/sqlrelay \
		$(initroot)/rc.d/sqlrcachemanager \
		$(initroot)/usr/pkg/etc/rc.d/sqlrelay \
		$(initroot)/usr/pkg/etc/rc.d/sqlrcachemanager \
		$(initroot)/usr/local/etc/rc.d/sqlrelay \
		$(initroot)/usr/local/etc/rc.d/sqlrcachemanager
	if ( test -x "/usr/sbin/update-rc.d" ); \
	then \
		/usr/sbin/update-rc.d sqlrelay remove; \
		/usr/sbin/update-rc.d sqlrcachemanager remove; \
	elif ( test -x "/bin/launchctl" ); \
	then \
		/bin/launchctl unload /Library/LaunchDaemons/com.firstworks.sqlrelay.plist; \
		/bin/launchctl unload /Library/LaunchDaemons/com.firstworks.sqlrcachemanager.plist; \
	elif ( test ! -x "/sbin/chkconfig" ); \
	then \
		$(RM) /etc/rc0.d/*sqlrelay \
			/etc/rc1.d/*sqlrelay \
			/etc/rc2.d/*sqlrelay \
			/etc/rc3.d/*sqlrelay \
			/etc/rc4.d/*sqlrelay \
			/etc/rc5.d/*sqlrelay \
			/etc/rc6.d/*sqlrelay \
			/etc/rc0.d/*sqlrcachemanager \
			/etc/rc1.d/*sqlrcachemanager \
			/etc/rc2.d/*sqlrcachemanager \
			/etc/rc3.d/*sqlrcachemanager \
			/etc/rc4.d/*sqlrcachemanager \
			/etc/rc5.d/*sqlrcachemanager \
			/etc/rc6.d/*sqlrcachemanager; \
	fi
	$(RM) $(initroot)/Library/LaunchDaemons/com.firstworks.sqlrelay.plist \
		$(initroot)/Library/LaunchDaemons/com.firstworks.sqlrcachemanager.plist \
		$(initroot)/etc/sysconfig/sqlrelay \
		$(initroot)/etc/sqlrelay
