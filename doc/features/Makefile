top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html .html.in

SRCS = connectionpooling.wt \
	databases.wt \
	dbabstractionlayers.wt \
	dropinreplacementlibs.wt \
	ha.wt \
	languages.wt \
	multirowfetches.wt \
	proxying.wt \
	queryfiltering.wt \
	queryrouting.wt \
	resultsetcaching.wt \
	schedules.wt \
	substitutionandbindvars.wt \
	suspendedtx.wt \
	throttling.wt

OBJS = connectionpooling.html \
	databases.html \
	dbabstractionlayers.html \
	dropinreplacementlibs.html \
	ha.html \
	languages.html.in \
	multirowfetches.html \
	proxying.html \
	queryfiltering.html \
	queryrouting.html \
	resultsetcaching.html \
	schedules.html \
	substitutionandbindvars.html \
	suspendedtx.html \
	throttling.html

.wt.html:
	wt -c $< -o $@ -xhtml xhtml.conf

.wt.html.in:
	wt -c $< -o $@ -xhtml xhtml.conf

all: $(OBJS)
	$(MAKE) html
	$(MAKE) cleanin

clean:
	$(RM) $(OBJS)

cleanin:
	$(RM) *.in
