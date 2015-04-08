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

all: SQLRClient.dll SQLRClient.dll.config

clean:
	$(RM) *.dll *.pdb SQLRClient.dll.config

SQLRClient.dll.config: SQLRClient.dll.config.in
	$(REPLACE) @SQLR_VERSION@ $(SQLR_VERSION) SQLRClient.dll.config.in > SQLRClient.dll.config

SQLRClient.dll: $(SRCS)
	$(CSC) $(CSCFLAGS) /optimize /target:library /out:$@ $(SRCS)

install: $(INSTALLLIB)

installlib:
	$(MKINSTALLDIRS) $(monodir)
	$(CP) SQLRClient.dll $(monodir)
	$(CP) SQLRClient.dll.config $(monodir)

installdll:
	$(MKINSTALLDIRS) $(monodir)
	$(CP) SQLRClient.dll $(monodir)

uninstall: $(UNINSTALLLIB)

uninstalllib:
	$(RMTREE) $(monodir)

uninstalldll:
	$(RM) $(monodir)/SQLRClient.dll*
