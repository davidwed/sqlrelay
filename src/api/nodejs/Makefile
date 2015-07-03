top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

all: build

clean:
	$(RM) binding.gyp package.json
	$(NODEGYP) clean

build: binding.gyp sqlrelay.cpp package.json
	$(NODEGYP) -v configure build

binding.gyp: binding_in.gyp
	$(REPLACE) @NODEJSCFLAGS@ "$(NODEJSCFLAGS)" binding_in.gyp > temp.gyp
	$(REPLACE) @NODEJSLIBS@ "$(NODEJSLIBS)" temp.gyp > binding.gyp
	$(RM) temp.gyp

package.json: package_in.json
	$(REPLACE) @SQLR_VERSION@ "$(SQLR_VERSION)" package_in.json > package.json

install:
	$(MKINSTALLDIRS) $(DESTDIR)$(NODEMODULEDIR)/sqlrelay
	$(CP) build/Release/sqlrelay.node $(DESTDIR)$(NODEMODULEDIR)/sqlrelay
	$(CP) package.json $(DESTDIR)$(NODEMODULEDIR)/sqlrelay
	$(CP) index.js $(DESTDIR)$(NODEMODULEDIR)/sqlrelay

uninstall:
	$(RMTREE) $(DESTDIR)$(NODEMODULEDIR)/sqlrelay
