require "mkmf"
drive = File::PATH_SEPARATOR == ";" ? /\A\w:/ : /\A/
print "\n"
print "arch = "
print CONFIG["arch"]
print "\n"
print "ruby_version = "
print Config::CONFIG["ruby_version"]
print "\n"
print "prefix = "
print with_destdir(CONFIG["prefix"].sub(drive, ""))
print "\n"
print "exec_prefix = "
print with_destdir(CONFIG["exec_prefix"].sub(drive, ""))
print "\n"
print "libdir = "
print with_destdir($libdir.sub(drive, ""))
print "\n"
print "rubylibdir = "
print with_destdir($rubylibdir.sub(drive, ""))
print "\n"
print "topdir = "
print $topdir
print "\n"
print "hdrdir = "
print $hdrdir
print "\n"
print "srcdir = "
print $srcdir
print "\n"
print "CFLAGS = -I. -I$(topdir) -I$(hdrdir) -I$(srcdir) "
print "\n"
print "CPPFLAGS = "
print $CFLAGS
print "\n\n"
print "all:\n"
print "	echo $(CPPFLAGS) $(CFLAGS)\n"
