top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html.in

SRCS = configguide.wt \
	configreference.wt \
	installing.wt

OBJS = configguide.html.in \
	configreference.html.in \
	installing.html.in

.wt.html.in:
	wt -c $< -o $@ -xhtml xhtml.conf

all: $(OBJS)

cleanin:
	#$(RM) *.in

clean:
	$(RM) $(OBJS)
