top_builddir = ../..

include ../../config.mk

all: SQLRClient.$(LIBEXT) oracle$(EXE)

clean:
	$(RM) oracle$(EXE) *.$(LIBEXT) *.pdb

SQLRClient.$(LIBEXT):
	$(CP) ..\..\src\api\cs\SQLRClient.$(LIBEXT) .

oracle$(EXE): oracle.cs
	csc /out:$@ oracle.cs /lib:..\..\src\api\cs /reference:SQLRClient.dll
