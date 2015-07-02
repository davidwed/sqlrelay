top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

all: build

clean:
	$(RMTREE) build

build: sqlrelay.cpp
	$(NODEGYP) configure build

install: $(INSTALLLIB)

installdll:

installlib:

uninstall: $(UNINSTALLLIB)

uninstalldll:

uninstalllib:
