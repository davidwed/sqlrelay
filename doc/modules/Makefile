top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html.in

SRCS = auths.wt \
	filters.wt \
	loggers.wt \
	notifications.wt \
	pwdencs.wt \
	resultsettranslations.wt \
	translations.wt

OBJS = auths.html.in \
	filters.html.in \
	loggers.html.in \
	notifications.html.in \
	pwdencs.html.in \
	resultsettranslations.html.in \
	translations.html.in

.wt.html.in:
	wt -c $< -o $@ -xhtml xhtml.conf

all: $(OBJS)
	$(MAKE) html
	$(MAKE) cleanin

clean:
	$(RM) $(OBJS)
	$(MAKE) cleanin

cleanin:
	$(RM) *.in
