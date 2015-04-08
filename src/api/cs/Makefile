top_builddir = ../../..

include ../../../config.mk

SRCS = SQLRConnection.cs \
	SQLRCursor.cs \
	SQLRelayCommand.cs \
	SQLRelayConnection.cs \
	SQLRelayDataAdapter.cs \
	SQLRelayDataReader.cs \
	SQLRelayException.cs \
	SQLRelayParameter.cs \
	SQLRelayParameterCollection.cs \
	SQLRelayTransaction.cs \
	SQLRelayType.cs \
	AssemblyInfo.cs

all: SQLRClient.dll

clean:
	$(RM) *.dll *.pdb SQLRConnection.cs SQLRCursor.cs

SQLRConnection.cs: SQLRConnection.cs.in
	#$(REPLACE) @LIBEXT@ $(SOSUFFIX) SQLRConnection.cs.in > SQLRConnection.cs
	$(REPLACE) @LIBEXT@ dll SQLRConnection.cs.in > SQLRConnection.cs

SQLRCursor.cs: SQLRCursor.cs.in
	#$(REPLACE) @LIBEXT@ $(SOSUFFIX) SQLRCursor.cs.in > SQLRCursor.cs
	$(REPLACE) @LIBEXT@ dll SQLRCursor.cs.in > SQLRCursor.cs

SQLRClient.dll: $(SRCS)
	#csc /optimize /target:library /out:$@ $(SRCS)
	mcs -pkg:dotnet /optimize /target:library /out:$@ $(SRCS)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(CP) SQLRClient.dll $(bindir)

uninstall:
	$(RM) $(bindir)/SQLRClient.dll
