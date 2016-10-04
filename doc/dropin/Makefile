top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html

SRCS = mysql.wt \
	postgresql.wt

OBJS = mysql.html \
	postgresql.html

.wt.html:
	wt -i $< -o $@ -xhtml xhtml.conf

all: $(OBJS)

clean:
	$(RM) $(OBJS)
