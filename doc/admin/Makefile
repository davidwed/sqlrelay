top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html

SRCS = configguide.wt \
	configreference.wt \
	installing.wt \
	oraclentier.wt \
	prerequisites.wt \
	router.wt \
	running.wt \
	tuning.wt

OBJS = configguide.html \
	configreference.html \
	installing.html \
	oraclentier.html \
	prerequisites.html \
	router.html \
	running.html \
	tuning.html

.wt.html:
	wt -c $< -o $@ -xhtml xhtml.conf

all: $(OBJS)

clean:
	$(RM) $(OBJS)
