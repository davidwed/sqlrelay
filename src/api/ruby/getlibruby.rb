require "mkmf"
drive = File::PATH_SEPARATOR == ";" ? /\A\w:/ : /\A/
print "\n"
print "RUBY_INSTALL_NAME = "
print CONFIG["RUBY_INSTALL_NAME"]
print "\n"
print "RUBY_SO_NAME = "
print CONFIG["RUBY_SO_NAME"]
print "\n"
print "LIBRUBY = "
print CONFIG["LIBRUBY"]
print "\n"
print "all:\n"
print " echo $(LIBRUBY) \n"
