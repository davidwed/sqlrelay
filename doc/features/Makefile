top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html

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
	languages.html \
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

all: $(OBJS)

clean:
	$(RM) $(OBJS)
