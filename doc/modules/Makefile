top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html

SRCS = auths.wt \
	filters.wt \
	loggers.wt \
	notifications.wt \
	pwdencs.wt \
	resultsettranslations.wt \
	routers.wt \
	schedules.wt \
	translations.wt

OBJS = auths.html \
	filters.html \
	loggers.html \
	notifications.html \
	pwdencs.html \
	resultsettranslations.html \
	routers.html \
	schedules.html \
	translations.html

.wt.html:
	wt -c $< -o $@ -xhtml xhtml.conf

all: $(OBJS)

clean:
	$(RM) $(OBJS)
