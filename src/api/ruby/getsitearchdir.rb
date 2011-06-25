require "mkmf"
drive = File::PATH_SEPARATOR == ";" ? /\A\w:/ : /\A/
print "arch = "
print CONFIG["arch"]
print "\n"
print "sitearch = "
print CONFIG["sitearch"]
print "\n"
print "ruby_version = "
print Config::CONFIG["ruby_version"]
print "\n"
print "prefix = "
print CONFIG["prefix"].sub(drive, "").sub("$(DESTDIR)","")
print "\n"
print "exec_prefix = "
print CONFIG["exec_prefix"].sub(drive, "").sub("$(DESTDIR)","")
print "\n"
print "libdir = "
print $libdir.sub(drive, "").sub("$(DESTDIR)","")
print "\n"
print "rubylibdir = "
print $rubylibdir.sub(drive, "").sub("$(DESTDIR)","")
print "\n"
print "archdir = "
print $archdir.sub(drive, "").sub("$(DESTDIR)","")
print "\n"
print "sitedir = "
print $sitedir.sub(drive, "").sub("$(DESTDIR)","")
print "\n"
print "_fc_sitedir = "
print $sitedir.sub(drive, "").sub("$(DESTDIR)","")
print "\n"
print "sitelibdir = "
print $sitelibdir.sub(drive, "").sub("$(DESTDIR)","")
print "\n"
print "sitearchdir = "
print $sitearchdir.sub(drive, "").sub("$(DESTDIR)","")
print "\n\n"
print "all:\n"
print "	echo $(sitearchdir)\n"
