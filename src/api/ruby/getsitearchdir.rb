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
print "archdir = "
print with_destdir($archdir.sub(drive, ""))
print "\n"
print "sitedir = "
print with_destdir($sitedir.sub(drive, ""))
print "\n"
print "sitelibdir = "
print with_destdir($sitelibdir.sub(drive, ""))
print "\n"
print "sitearchdir = "
print with_destdir($sitearchdir.sub(drive, ""))
print "\n\n"
print "all:\n"
print "	echo $(sitearchdir)\n"
