top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html.in

SRCS = configguide.wt \
	configreference.wt \
	installing.wt \
	oraclentier.wt

OBJS = configguide.html.in \
	configreference.html.in \
	installing.html.in \
	oraclentier.html.in

.wt.html.in:
	wt -c $< -o $@ -xhtml xhtml.conf

all: $(OBJS)

cleanin:
	#$(RM) *.in

clean:
	$(RM) $(OBJS)
