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

all: SQLRClient.$(LIBEXT)

clean:
	$(RM) *.$(LIBEXT) *.pdb

SQLRClient.$(LIBEXT): $(SRCS)
	csc /optimize /target:library /out:$@ $(SRCS)

install:
	$(MKINSTALLDIRS) $(bindir)
	$(CP) SQLRClient.$(LIBEXT) $(bindir)

uninstall:
	$(RM) $(bindir)/SQLRClient.$(LIBEXT)
