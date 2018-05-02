top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html

SRCS = ado.net.wt \
	c.wt \
	c++.wt \
	cs.wt \
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
	python.wt \
	ruby.wt \
	tcl.wt

OBJS = ado.net.html \
	c.html \
	c++.html \
	cs.html \
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
	python.html \
	ruby.html \
	tcl.html

.wt.html:
	wt -i $< -o $@ -xhtml xhtml.conf

all: $(OBJS)

clean:
	$(RM) $(OBJS)
