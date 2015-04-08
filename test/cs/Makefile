top_builddir = ../..

include ../../config.mk

all: SQLRClient.dll oracle$(EXE)

clean:
	$(RM) oracle$(EXE) *.dll *.dll.config *.pdb

SQLRClient.dll:
	$(CP) ../../src/api/cs/SQLRClient.dll .
	$(CP) ../../src/api/cs/SQLRClient.dll.config .

oracle$(EXE): oracle.cs
	#csc /out:$@ oracle.cs /lib:..\..\src\api\cs /reference:SQLRClient.dll
	mcs -pkg:dotnet /out:$@ oracle.cs /lib:../../src/api/cs /reference:SQLRClient.dll
