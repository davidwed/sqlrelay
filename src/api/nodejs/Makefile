top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

all: build

clean:
	$(RM) binding.gyp
	$(NODEGYP) clean

build: binding.gyp sqlrelay.cpp
	$(NODEGYP) configure build

binding.gyp: binding_in.gyp
	$(REPLACE) @top_builddir@ "$(top_builddir)" binding_in.gyp > binding.gyp

install: $(INSTALLLIB)

installdll:

installlib:

uninstall: $(UNINSTALLLIB)

uninstalldll:

uninstalllib:
