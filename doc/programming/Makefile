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
	nodejs.wt \
	odbc.wt \
	perldbi.wt \
	perl.wt \
	php.wt \
	phppdo.wt \
	precisionscale.wt \
	pythondb.wt \
	python.wt

OBJS = ado.net.html \
	c.html \
	c++.html \
	binds.html \
	erlang.html \
	java.html \
	jdbc.html \
	nodejs.html \
	odbc.html \
	perldbi.html \
	perl.html \
	php.html \
	phppdo.html \
	precisionscale.html \
	pythondb.html \
	python.html

.wt.html:
	wt -c $< -o $@ -xhtml xhtml.conf

all: $(OBJS)

clean:
	$(RM) $(OBJS)
