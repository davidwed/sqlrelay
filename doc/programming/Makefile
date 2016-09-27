top_builddir = ../..

include ../../config.mk

# documentation
.PHONY: doc parts html cleanparts

.SUFFIXES: .wt .html

SRCS = ado.net.wt \
	c.wt \
	c++.wt \
	binds.wt \
	erlang.wt \
	java.wt \
	jdbc.wt \
	odbc.wt \
	precisionscale.wt

OBJS = ado.net.html \
	c.html \
	c++.html \
	binds.html \
	erlang.html \
	java.html \
	jdbc.html \
	odbc.html \
	precisionscale.html

.wt.html:
	wt -c $< -o $@ -xhtml xhtml.conf

all: $(OBJS)

clean:
	$(RM) $(OBJS)
