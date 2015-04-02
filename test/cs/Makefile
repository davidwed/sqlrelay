top_builddir = ../..

include ../../config.mk

all: SQLRClient.$(LIBEXT) SQLRAdapterTest$(EXE) SQLRCLientTest$(EXE)

clean:
	$(RM) SQLRAdapterTest$(EXE) SQLRCLientTest$(EXE) *.$(LIBEXT) *.pdb

SQLRClient.$(LIBEXT):
	$(CP) ..\..\src\api\cs\SQLRClient.$(LIBEXT) .

SQLRClientTest$(EXE): SQLRClientTest.cs
	csc /out:$@ SQLRClientTest.cs /lib:..\..\src\api\cs /reference:SQLRClient.dll

SQLRAdapterTest$(EXE): SQLRAdapterTest.cs
	csc /out:$@ SQLRAdapterTest.cs /lib:..\..\src\api\cs /reference:SQLRClient.dll
