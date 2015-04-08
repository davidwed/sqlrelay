top_builddir = ../..

include ../../config.mk

all: oracle

clean:
	$(RM) oracle.exe *.dll *.dll.config *.pdb

oracle: oracle.cs
	$(CP) ../../src/api/cs/SQLRClient.dll .
	$(CP) ../../src/api/cs/SQLRClient.dll.config .
	$(CSC) $(CSCFLAGS) /out:oracle.exe oracle.cs /reference:SQLRClient.dll
