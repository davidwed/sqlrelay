top_builddir = ..

include ../config.mk

.SUFFIXES: .wt .html

SRCS = index.wt faq.wt
OBJS = index.html faq.html

.wt.html:
	wt -i $< -o $@ -xhtml xhtml.conf

all: $(OBJS)
	cd admin $(AND) $(MAKE)
	cd dropin $(AND) $(MAKE)
	cd features $(AND) $(MAKE)
	cd programming $(AND) $(MAKE)

install:
	$(MKINSTALLDIRS) $(docdir)
	$(MKINSTALLDIRS) $(docdir)/admin
	$(MKINSTALLDIRS) $(docdir)/api
	$(MKINSTALLDIRS) $(docdir)/api/ado.net/html
	$(MKINSTALLDIRS) $(docdir)/api/c/html
	$(MKINSTALLDIRS) $(docdir)/api/c++/html
	$(MKINSTALLDIRS) $(docdir)/api/cs/html
	$(MKINSTALLDIRS) $(docdir)/api/perl
	$(MKINSTALLDIRS) $(docdir)/api/php/media
	$(MKINSTALLDIRS) $(docdir)/api/php/sql_relay
	$(MKINSTALLDIRS) $(docdir)/api/python
	$(MKINSTALLDIRS) $(docdir)/api/ruby/doc/classes/SQLRConnection.src
	$(MKINSTALLDIRS) $(docdir)/api/ruby/doc/classes/SQLRCursor.src
	$(MKINSTALLDIRS) $(docdir)/api/ruby/doc/files
	$(MKINSTALLDIRS) $(docdir)/api/ruby/doc/images
	$(MKINSTALLDIRS) $(docdir)/api/ruby/doc/js
	$(MKINSTALLDIRS) $(docdir)/api/java
	$(MKINSTALLDIRS) $(docdir)/api/tcl
	$(MKINSTALLDIRS) $(docdir)/api/erlang/html
	$(MKINSTALLDIRS) $(docdir)/api/nodejs/html
	$(MKINSTALLDIRS) $(docdir)/dropin
	$(MKINSTALLDIRS) $(docdir)/features
	$(MKINSTALLDIRS) $(docdir)/images
	$(MKINSTALLDIRS) $(docdir)/js
	$(MKINSTALLDIRS) $(docdir)/css
	$(MKINSTALLDIRS) $(docdir)/programming
	$(CP) *.html $(docdir)
	$(CHMOD) 644 $(docdir)/*.html
	$(CP) admin/*.html $(docdir)/admin
	$(CHMOD) 644 $(docdir)/admin/*.html
	$(CP) css/*.css $(docdir)/css
	$(CHMOD) 644 $(docdir)/css/*.css
	$(CP) api/ado.net/html/*.html $(docdir)/api/ado.net/html
	$(CHMOD) 644 $(docdir)/api/ado.net/html/*.html
	$(CP) api/ado.net/html/*.css $(docdir)/api/ado.net/html
	$(CHMOD) 644 $(docdir)/api/ado.net/html/*.css
	$(CP) api/ado.net/html/*.png $(docdir)/api/ado.net/html
	$(CHMOD) 644 $(docdir)/api/ado.net/html/*.png
	$(CP) api/ado.net/html/*.js $(docdir)/api/ado.net/html
	$(CHMOD) 644 $(docdir)/api/ado.net/html/*.js
	$(CP) api/c/html/*.html $(docdir)/api/c/html
	$(CHMOD) 644 $(docdir)/api/c/html/*.html
	$(CP) api/c/html/*.css $(docdir)/api/c/html
	$(CHMOD) 644 $(docdir)/api/c/html/*.css
	$(CP) api/c/html/*.png $(docdir)/api/c/html
	$(CHMOD) 644 $(docdir)/api/c/html/*.png
	$(CP) api/c/html/*.js $(docdir)/api/c/html
	$(CHMOD) 644 $(docdir)/api/c/html/*.js
	$(CP) api/c++/html/*.html $(docdir)/api/c++/html
	$(CHMOD) 644 $(docdir)/api/c++/html/*.html
	$(CP) api/c++/html/*.css $(docdir)/api/c++/html
	$(CHMOD) 644 $(docdir)/api/c++/html/*.css
	$(CP) api/c++/html/*.png $(docdir)/api/c++/html
	$(CHMOD) 644 $(docdir)/api/c++/html/*.png
	$(CP) api/c++/html/*.js $(docdir)/api/c++/html
	$(CHMOD) 644 $(docdir)/api/c++/html/*.js
	$(CP) api/cs/html/*.html $(docdir)/api/cs/html
	$(CHMOD) 644 $(docdir)/api/cs/html/*.html
	$(CP) api/cs/html/*.css $(docdir)/api/cs/html
	$(CHMOD) 644 $(docdir)/api/cs/html/*.css
	$(CP) api/cs/html/*.png $(docdir)/api/cs/html
	$(CHMOD) 644 $(docdir)/api/cs/html/*.png
	$(CP) api/cs/html/*.js $(docdir)/api/cs/html
	$(CHMOD) 644 $(docdir)/api/cs/html/*.js
	$(CP) api/perl/perl.html $(docdir)/api/perl
	$(CHMOD) 644 $(docdir)/api/perl/perl.html
	$(CP) api/perl/Connection.html $(docdir)/api/perl
	$(CHMOD) 644 $(docdir)/api/perl/Connection.html
	$(CP) api/perl/Cursor.html $(docdir)/api/perl
	$(CHMOD) 644 $(docdir)/api/perl/Cursor.html
	$(CP) api/perl/SQLRelay.html $(docdir)/api/perl
	$(CHMOD) 644 $(docdir)/api/perl/SQLRelay.html
	$(CP) api/php/*.html $(docdir)/api/php
	$(CHMOD) 644 $(docdir)/api/php/*.html
	$(CP) api/php/media/*.css $(docdir)/api/php/media
	$(CHMOD) 644 $(docdir)/api/php/media/*.css
	$(CP) api/php/sql_relay/*.html $(docdir)/api/php/sql_relay
	$(CHMOD) 644 $(docdir)/api/php/sql_relay/*.html
	$(CP) api/python/*.html $(docdir)/api/python
	$(CHMOD) 644 $(docdir)/api/python/*.html
	$(CP) api/ruby/doc/*.html $(docdir)/api/ruby/doc
	$(CHMOD) 644 $(docdir)/api/ruby/doc/*.html
	$(CP) api/ruby/doc/*.css $(docdir)/api/ruby/doc
	$(CHMOD) 644 $(docdir)/api/ruby/doc/*.css
	$(CP) api/ruby/doc/classes/*.html $(docdir)/api/ruby/doc/classes
	$(CHMOD) 644 $(docdir)/api/ruby/doc/classes/*.html
	$(CP) api/ruby/doc/classes/SQLRConnection.src/*.html $(docdir)/api/ruby/doc/classes/SQLRConnection.src
	$(CHMOD) 644 $(docdir)/api/ruby/doc/classes/SQLRConnection.src/*.html
	$(CP) api/ruby/doc/classes/SQLRCursor.src/*.html $(docdir)/api/ruby/doc/classes/SQLRCursor.src
	$(CHMOD) 644 $(docdir)/api/ruby/doc/classes/SQLRCursor.src/*.html
	$(CP) api/ruby/doc/files/*.html $(docdir)/api/ruby/doc/files
	$(CHMOD) 644 $(docdir)/api/ruby/doc/files/*.html
	$(CP) api/ruby/doc/images/*.png $(docdir)/api/ruby/doc/images
	$(CHMOD) 644 $(docdir)/api/ruby/doc/images/*.png
	$(CP) api/ruby/doc/js/*.js $(docdir)/api/ruby/doc/js
	$(CHMOD) 644 $(docdir)/api/ruby/doc/js/*.js
	$(CP) api/java/*.html $(docdir)/api/java
	$(CHMOD) 644 $(docdir)/api/java/*.html
	$(CP) api/java/package-list $(docdir)/api/java
	$(CHMOD) 644 $(docdir)/api/java/package-list
	$(CP) api/java/*.css $(docdir)/api/java
	$(CHMOD) 644 $(docdir)/api/java/*.css
	$(MKINSTALLDIRS) $(docdir)/api/java/com
	$(MKINSTALLDIRS) $(docdir)/api/java/com/firstworks
	$(MKINSTALLDIRS) $(docdir)/api/java/com/firstworks/sqlrelay
	$(CP) api/java/com/firstworks/sqlrelay/*.html $(docdir)/api/java/com/firstworks/sqlrelay
	$(CHMOD) 644 $(docdir)/api/java/com/firstworks/sqlrelay/*.html
	$(MKINSTALLDIRS) $(docdir)/api/java/resources
	$(CP) api/java/resources/*.gif $(docdir)/api/java/resources
	$(CHMOD) 644 $(docdir)/api/java/resources/*.gif
	$(CP) api/tcl/*.html $(docdir)/api/tcl
	$(CHMOD) 644 $(docdir)/api/tcl/*.html
	$(CP) api/tcl/*.txt $(docdir)/api/tcl
	$(CHMOD) 644 $(docdir)/api/tcl/*.txt
	$(CP) api/erlang/html/edoc-info $(docdir)/api/erlang/html
	$(CHMOD) 644 $(docdir)/api/erlang/html/edoc-info
	$(CP) api/erlang/html/*.png $(docdir)/api/erlang/html
	$(CHMOD) 644 $(docdir)/api/erlang/html/*.png
	$(CP) api/erlang/html/*.html $(docdir)/api/erlang/html
	$(CHMOD) 644 $(docdir)/api/erlang/html/*.html
	$(CP) api/erlang/html/*.css $(docdir)/api/erlang/html
	$(CHMOD) 644 $(docdir)/api/erlang/html/*.css
	$(CP) api/nodejs/html/*.html $(docdir)/api/nodejs/html
	$(CHMOD) 644 $(docdir)/api/nodejs/html/*.html
	$(CP) api/nodejs/html/*.css $(docdir)/api/nodejs/html
	$(CHMOD) 644 $(docdir)/api/nodejs/html/*.css
	$(CP) api/nodejs/html/*.png $(docdir)/api/nodejs/html
	$(CHMOD) 644 $(docdir)/api/nodejs/html/*.png
	$(CP) api/nodejs/html/*.js $(docdir)/api/nodejs/html
	$(CHMOD) 644 $(docdir)/api/nodejs/html/*.js
	$(CP) dropin/*.html $(docdir)/dropin
	$(CHMOD) 644 $(docdir)/dropin/*.html
	$(CP) features/*.html $(docdir)/features
	$(CHMOD) 644 $(docdir)/features/*.html
	$(CP) images/*.png $(docdir)/images
	$(CHMOD) 644 $(docdir)/images/*.png
	$(CP) images/*.PNG $(docdir)/images
	$(CHMOD) 644 $(docdir)/images/*.PNG
	$(CP) js/*.js $(docdir)/js
	$(CHMOD) 644 $(docdir)/js/*.js
	$(CP) programming/*.html $(docdir)/programming
	$(CHMOD) 644 $(docdir)/programming/*.html

uninstall:
	$(RMTREE) $(docdir)

clean:
	$(RM) $(OBJS)
	cd admin $(AND) $(MAKE) clean
	cd dropin $(AND) $(MAKE) clean
	cd features $(AND) $(MAKE) clean
	cd programming $(AND) $(MAKE) clean
	cd howtos $(AND) $(MAKE) clean
