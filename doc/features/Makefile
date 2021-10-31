top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html

SRCS = connectionpooling.wt \
	databases.wt \
	dbabstractionlayers.wt \
	ha.wt \
	languages.wt \
	multirowfetches.wt \
	nativeprotocol.wt \
	proxying.wt \
	queryfiltering.wt \
	querytranslation.wt \
	queryrouting.wt \
	resultsetcaching.wt \
	schedules.wt \
	substitutionandbindvars.wt \
	suspendedtx.wt \
	throttling.wt

OBJS = connectionpooling.html \
	databases.html \
	dbabstractionlayers.html \
	ha.html \
	languages.html \
	multirowfetches.html \
	nativeprotocol.html \
	proxying.html \
	queryfiltering.html \
	querytranslation.html \
	queryrouting.html \
	resultsetcaching.html \
	schedules.html \
	substitutionandbindvars.html \
	suspendedtx.html \
	throttling.html

.wt.html:
	wt -i $< -o $@ -xhtml xhtml.conf

all: $(OBJS)

clean:
	$(RM) $(OBJS)
