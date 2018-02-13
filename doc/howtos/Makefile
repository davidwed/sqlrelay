top_builddir = ../..

include ../../config.mk

.SUFFIXES: .wt .html

SRCS = \
	oracle-tls.wt \
	oracle-tls-wallet-quirks.wt \
	oracle-tls-selfsigned.wt \
	oracle-tls-casigned.wt \
	freetds-mssql-tls.wt \
	ubuntu1604-sqlrserver-oracle12c.wt \
	ubuntu1604-sqlrclient.wt \
	ubuntu1604-datagrip.wt \
	sqlrserver-tls-pfx-sqlserver-windows.wt \
	tls-encryption.wt \
	tls-certificate.wt

OBJS = \
	oracle-tls.html \
	oracle-tls-wallet-quirks.html \
	oracle-tls-selfsigned.html \
	oracle-tls-casigned.html \
	freetds-mssql-tls.html \
	ubuntu1604-sqlrserver-oracle12c.html \
	ubuntu1604-sqlrclient.html \
	ubuntu1604-datagrip.html \
	sqlrserver-tls-pfx-sqlserver-windows.html \
	tls-encryption.html \
	tls-certificate.html


.wt.html:
	wt -i $< -o $@

all: $(OBJS)

clean:
	$(RM) $(OBJS)
