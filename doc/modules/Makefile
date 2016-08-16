top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html.in

SRCS = auths.wt \
	filters.wt \
	loggers.wt

OBJS = auths.html.in \
	filters.html.in \
	loggers.html.in

.wt.html.in:
	wt -c $< -o $@ -xhtml xhtml.conf

all: $(OBJS)
	$(MAKE) html
	#$(MAKE) cleanin

clean:
	$(RM) $(OBJS)
	$(MAKE) cleanin

cleanin:
	$(RM) *.in
