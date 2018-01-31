top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html

SRCS = \
	oracle-tls.wt \
	oracle-tls-wallet-quirks.wt \
	oracle-tls-selfsigned.wt \
	oracle-tls-casigned.wt \
	sqlrserver-oracle12c-ubuntu1604.wt \
	sqlrclient-ubuntu1604.wt \
	sqlrserver-tls-pfx-sqlserver-windows.wt \
	tls.wt \
	tls-encryption.wt \
	tls-certificate.wt

OBJS = \
	oracle-tls.html \
	oracle-tls-wallet-quirks.html \
	oracle-tls-selfsigned.html \
	oracle-tls-casigned.html \
	sqlrserver-oracle12c-ubuntu1604.html \
	sqlrclient-ubuntu1604.html \
	sqlrserver-tls-pfx-sqlserver-windows.html \
	tls.html \
	tls-encryption.html \
	tls-certificate.html


.wt.html:
	wt -i $< -o $@

all: $(OBJS)

clean:
	$(RM) $(OBJS)
