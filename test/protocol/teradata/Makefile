top_builddir = ../../..

include ../../../config.mk

all: teradata.class

teradata.class: teradata.java
	$(JAVAC) $<

clean:
	$(RM) *.lo *.o *.obj *.class
	$(RMTREE) .libs
