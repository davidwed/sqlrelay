require "mkmf"

drive = File::PATH_SEPARATOR == ";" ? /\A\w:/ : /\A/
print "\n"
print "arch = "
print CONFIG["arch"]
print "\n"
print "sitearch = "
print CONFIG["sitearch"]
print "\n"
print "ruby_version = "
begin
print Config::CONFIG["ruby_version"]
rescue
print CONFIG["ruby_version"]
end
print "\n"
print "prefix = "
print CONFIG["prefix"].sub(drive, "")
print "\n"
print "exec_prefix = "
print CONFIG["exec_prefix"].sub(drive, "")
print "\n"
print "libdir = "
print $libdir.sub(drive, "")
print "\n"
print "rubylibdir = "
print $rubylibdir.sub(drive, "")
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
print "CFLAGS = -I. -I$(topdir) -I$(hdrdir) -I$(hdrdir)/$(sitearch) -I$(srcdir) -I$(hdrdir)/../$(arch)/ruby-$(ruby_version)"
print "\n"
print "CPPFLAGS = "
print $CFLAGS
print "\n\n"
print "all:\n"
print "	echo $(CPPFLAGS) $(CFLAGS)\n"
