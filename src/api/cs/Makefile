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
	$(RM) *.snk *.il *.dll.in *.dll *.mdb AssemblyInfo.cs

AssemblyInfo.cs: AssemblyInfo.cs.in
	$(REPLACE) @SQLR_VERSION@ $(SQLR_VERSION) AssemblyInfo.cs.in > AssemblyInfo.cs

SQLRClient.dll: $(SRCS)
	$(CSC) $(CSCFLAGS) /optimize /target:library /out:$@ $(SRCS)

# in case we ever want to sign the dll
#SQLRClient.dll.in: $(SRCS)
#	$(CSC) $(CSCFLAGS) /optimize /target:library /out:$@ $(SRCS)

#keyPair.snk:
#	$(SN) -k keyPair.snk

#SQLRClient.il: SQLRClient.dll.in keyPair.snk
#	$(ILDASM) SQLRClient.dll.in $(ILDASMOUT)SQLRClient.il

#SQLRClient.dll: SQLRClient.il
#	$(ILASM) SQLRClient.il /dll /key=keyPair.snk

install: $(INSTALLLIB)

installdll:
	$(MKINSTALLDIRS) $(dotnetdir)
	$(CP) SQLRClient.dll $(dotnetdir)

installlib:
	$(MKINSTALLDIRS) $(dotnetdir)
	$(CP) SQLRClient.dll $(dotnetdir)
	$(CP) SQLRClient.dll.config $(dotnetdir)
	$(CHMOD) 755 $(dotnetdir)/SQLRClient.dll

uninstall:
	$(RM) $(dotnetdir)/SQLRClient.dll*
