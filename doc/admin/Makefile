top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html

SRCS = configguide.wt \
	configreference.wt \
	installing.wt \
	oraclentier.wt \
	prerequisites.wt \
	running.wt \
	tuning.wt \
	tlscert.wt

OBJS = configguide.html \
	configreference.html \
	installing.html \
	oraclentier.html \
	prerequisites.html \
	running.html \
	tuning.html \
	tlscert.html

.wt.html:
	wt -i $< -o $@ -xhtml xhtml.conf

all: $(OBJS)

clean:
	$(RM) $(OBJS)
