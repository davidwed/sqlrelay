top_builddir = ../../..

include ../../../config.mk

.SUFFIXES: .lo

all: build/Release/sqlrelay.node

clean:
	$(RM) binding.gyp package.json
	-$(NODEGYP) clean
	$(RMTREE) build
	$(RM) temp1.gyp temp2.gyp

build/Release/sqlrelay.node: binding.gyp sqlrelay.cpp package.json
	$(NODEGYP) $(NODEGYPPYTHON) -v configure build

binding.gyp: binding_in.gyp
	$(REPLACE) @NODEJSINCLUDEDIRS@ "$(NODEJSINCLUDEDIRS)" binding_in.gyp > temp1.gyp
	$(REPLACE) @NODEJSCFLAGS@ "$(NODEJSCFLAGS)" temp1.gyp > temp2.gyp
	$(REPLACE) @NODEJSLIBS@ "$(NODEJSLIBS)" temp2.gyp > binding.gyp
	$(RM) temp1.gyp temp2.gyp

package.json: package_in.json
	$(REPLACE) @SQLR_VERSION@ "$(SQLR_VERSION)" package_in.json > package.json

install:
	$(MKINSTALLDIRS) $(DESTDIR)$(NODEMODULEDIR)/sqlrelay
	$(CP) build/Release/sqlrelay.node $(DESTDIR)$(NODEMODULEDIR)/sqlrelay
	$(CP) package.json $(DESTDIR)$(NODEMODULEDIR)/sqlrelay
	$(CP) index.js $(DESTDIR)$(NODEMODULEDIR)/sqlrelay

uninstall:
	$(RMTREE) $(DESTDIR)$(NODEMODULEDIR)/sqlrelay
