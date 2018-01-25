top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html

SRCS = sqlrserver-oracle12c-ubuntu1604.wt

OBJS = sqlrserver-oracle12c-ubuntu1604.html

.wt.html:
	wt -i $< -o $@

all: $(OBJS)

clean:
	$(RM) $(OBJS)
