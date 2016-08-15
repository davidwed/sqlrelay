top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html.in

SRCS = configguide.wt \
	configreference.wt \
	installing.wt \
	oraclentier.wt \
	prerequisites.wt \
	running.wt \
	tuning.wt

OBJS = configguide.html.in \
	configreference.html.in \
	installing.html.in \
	oraclentier.html.in \
	prerequisites.html.in \
	running.html.in \
	tuning.html.in

.wt.html.in:
	wt -c $< -o $@ -xhtml xhtml.conf

all: $(OBJS)
	$(MAKE) html
	$(MAKE) cleanin

cleanin:
	$(RM) *.in

clean:
	$(RM) $(OBJS)
