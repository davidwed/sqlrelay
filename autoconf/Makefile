top_builddir = ..

include ../config.mk

install:
	$(MKINSTALLDIRS) $(acdir)
	$(CP) *.m4 $(acdir)
	$(CHMOD) 644 $(acdir)/*.m4

uninstall:
	$(RMTREE) $(acdir)
