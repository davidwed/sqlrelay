top_builddir = ../..

include ../../config.mk

all: local oracle krb tls unicode

clean:
	$(RM) *.exe *.dll *.dll.config *.pdb

local:
	$(CP) ../../src/api/cs/SQLRClient.dll .
	$(CP) ../../src/api/cs/SQLRClient.dll.config .

oracle: oracle.cs
	$(CSC) $(CSCFLAGS) /out:oracle.exe oracle.cs /reference:SQLRClient.dll

krb: krb.cs
	$(CSC) $(CSCFLAGS) /out:krb.exe krb.cs /reference:SQLRClient.dll

tls: tls.cs
	$(CSC) $(CSCFLAGS) /out:tls.exe tls.cs /reference:SQLRClient.dll

unicode: unicode.cs
	$(CSC) $(CSCFLAGS) /out:unicode.exe unicode.cs /reference:SQLRClient.dll
