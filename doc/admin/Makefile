top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html

SRCS = configguide.wt \
	configreference.wt \
	installing.wt \
	installingpkg.wt \
	oraclentier.wt \
	prerequisites.wt \
	running.wt \
	tuning.wt \
	tlscert.wt \
	tlsca.wt \
	createtlscert.wt

OBJS = configguide.html \
	configreference.html \
	installing.html \
	installingpkg.html \
	oraclentier.html \
	prerequisites.html \
	running.html \
	tuning.html \
	tlscert.html \
	tlsca.html \
	createtlscert.html

.wt.html:
	wt -i $< -o $@ -xhtml xhtml.conf

all: $(OBJS)

clean:
	$(RM) $(OBJS)
