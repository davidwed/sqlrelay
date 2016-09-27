top_builddir = ../..

include ../../config.mk

# documentation
.PHONY: doc parts html cleanparts

.SUFFIXES: .wt .html

SRCS = ado.net.wt \
	c++.wt \
	binds.wt \
	erlang.wt \
	odbc.wt \
	jdbc.wt \
	precisionscale.wt

OBJS = ado.net.html \
	c++.html \
	binds.html \
	erlang.html \
	odbc.html \
	jdbc.html \
	precisionscale.html

.wt.html:
	wt -c $< -o $@ -xhtml xhtml.conf

all: $(OBJS)

clean:
	$(RM) $(OBJS)
