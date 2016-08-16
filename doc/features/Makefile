top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html .html.in

SRCS = connectionpooling.wt \
	databases.wt \
	dbabstractionlayers.wt \
	dropinreplacementlibs.wt \
	ha.wt \
	languages.wt

OBJS = connectionpooling.html \
	databases.html \
	dbabstractionlayers.html \
	dropinreplacementlibs.html \
	ha.html \
	languages.html.in

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
