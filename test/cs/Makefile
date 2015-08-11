top_builddir = ../..

include ../../config.mk

all: local oracle

clean:
	$(RM) *.exe *.dll *.dll.config *.pdb

local:
	$(CP) ../../src/api/cs/SQLRClient.dll .
	$(CP) ../../src/api/cs/SQLRClient.dll.config .

oracle: oracle.cs
	$(CSC) $(CSCFLAGS) /out:oracle.exe oracle.cs /reference:SQLRClient.dll
