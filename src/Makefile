include ../config.mk

all:
	cd util; $(MAKE) all
	cd listener; $(MAKE) all
	cd connection; $(MAKE) all
	cd connections; $(MAKE) all
	cd start; $(MAKE) all
	cd cachemanager; $(MAKE) all
	cd scaler; $(MAKE) all
	if ( test -n "$(HAVE_GTK)" ); then \
		cd config; $(MAKE) all; cd ..; \
	fi
	cd api; $(MAKE) all
	cd cmdline; $(MAKE) all

clean:
	cd util; $(MAKE) clean
	cd listener; $(MAKE) clean
	cd connection; $(MAKE) clean
	cd connections; $(MAKE) clean
	cd start; $(MAKE) clean
	cd cachemanager; $(MAKE) clean
	cd scaler; $(MAKE) clean
	if ( test -n "$(HAVE_GTK)" ); then \
		cd config; $(MAKE) clean; cd ..; \
	fi
	cd api; $(MAKE) clean
	cd cmdline; $(MAKE) clean

rebuild:
	cd util; $(MAKE) rebuild
	cd listener; $(MAKE) rebuild
	cd connection; $(MAKE) rebuild
	cd cachemanager; $(MAKE) rebuild
	cd start; $(MAKE) rebuild
	cd scaler; $(MAKE) rebuild
	if ( test -n "$(HAVE_GTK)" ); then \
		cd config; $(MAKE) rebuild; cd ..; \
	fi
	cd api; $(MAKE) rebuild
	cd cmdline; $(MAKE) rebuild

install:
	cd util; $(MAKE) install
	cd listener; $(MAKE) install
	cd connection; $(MAKE) install
	cd connections; $(MAKE) install
	cd cachemanager; $(MAKE) install
	cd start; $(MAKE) install
	cd scaler; $(MAKE) install
	if ( test -n "$(HAVE_GTK)" ); then \
		cd config; $(MAKE) install; cd ..; \
	fi
	cd api; $(MAKE) install
	cd cmdline; $(MAKE) install

uninstall:
	cd util; $(MAKE) uninstall
	cd listener; $(MAKE) uninstall
	cd connection; $(MAKE) uninstall
	cd connections; $(MAKE) uninstall
	cd cachemanager; $(MAKE) uninstall
	cd start; $(MAKE) uninstall
	cd scaler; $(MAKE) uninstall
	if ( test -n "$(HAVE_GTK)" ); then \
		cd config; $(MAKE) uninstall; cd ..; \
	fi
	cd cmdline; $(MAKE) uninstall
	cd api; $(MAKE) uninstall
