top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html

SRCS = sqlrserver-oracle12c-ubuntu1604.wt \
	sqlrclient-ubuntu1604.wt \
	sqlrserver-tls-pfx-sqlserver-windows.wt

OBJS = sqlrserver-oracle12c-ubuntu1604.html \
	sqlrclient-ubuntu1604.html \
	sqlrserver-tls-pfx-sqlserver-windows.html


.wt.html:
	wt -i $< -o $@

all: $(OBJS)

clean:
	$(RM) $(OBJS)
