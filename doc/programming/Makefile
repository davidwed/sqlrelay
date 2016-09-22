top_builddir = ../..

include ../../config.mk

# documentation
.PHONY: doc parts html cleanparts

all:

doc: parts html cleanparts

parts:
	cd parts; \
	rm -f *.html; \
	for i in `/bin/ls *.*`; \
	do \
		BASENAME=`/bin/basename $$i`; \
		echo "processing $$BASENAME"; \
		gvim $$i +":runtime! syntax/2html.vim" +"colorscheme default" +":x! ./$$BASENAME.html" +"q!"; \
		sleep 2; \
	done
	for i in `/bin/ls parts/*.html`; \
	do \
		sed -i -e "/<html>/d" -e "/<head>/d" -e "/<title>.*<\/title>/d" -e "/<\!DOCTYPE.*/d" -e "/<script.*>/d" -e "/<\!--.*/d" -e "/.*-->/d" -e "/<\/script>.*/d" -e "/<meta.*/d" -e "/<\/head>/d" -e "/<body.*/d" -e "/<\/body>/d" -e "/<\/html>/d" $$i; \
	done

html:
	for file in `ls *.in`; \
	do \
		$(top_builddir)/insertparts.pl $$file > `basename $$file .in`; \
	done

cleanparts:
	rm -f parts/*.html
