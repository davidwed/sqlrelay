top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html.in

SRCS = configguide.wt
OBJS = configguide.html.in

.wt.html.in:
	wt -c $< -o $@ -xhtml xhtml.conf

all: $(OBJS)

clean:
	$(RM) $(OBJS)
