top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html

SRCS = configguide.wt \
	configreference.wt \
	configure_mysql_frontend.wt \
	installing.wt \
	installingpkg.wt \
	oraclentier.wt \
	prerequisites.wt \
	running.wt \
	tuning.wt \
	tlscert.wt \
	tlsca.wt

OBJS = configguide.html \
	configreference.html \
	configure_mysql_frontend.html \
	installing.html \
	installingpkg.html \
	oraclentier.html \
	prerequisites.html \
	running.html \
	tuning.html \
	tlscert.html \
	tlsca.html

.wt.html:
	wt -i $< -o $@ -xhtml xhtml.conf

all: $(OBJS)

clean:
	$(RM) $(OBJS)
