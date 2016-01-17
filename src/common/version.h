// Copyright (c) 1999-2016  David Muse
// See the file COPYING for more information

#include <rudiments/charstring.h>
#include <rudiments/stdio.h>
#include <rudiments/file.h>
#include <rudiments/process.h>

static void version(int argc, const char **argv) {

	if (argc!=2 || (charstring::compare(argv[1],"-version") &&
			charstring::compare(argv[1],"--version"))) {
		return;
	}

	stdoutput.printf("%s %s\n",argv[0],SQLR_VERSION);
	stdoutput.write("\n"
		"Copyright (C) 1999-2016 David Muse\n"
		"This is free software; see the source for copying "
		"conditions.  There is NO\n"
		"warranty; not even for MERCHANTABILITY or "
		"FITNESS FOR A PARTICULAR PURPOSE.\n"
		"\n"
		"Written by David Muse.\n");

	process::exit(0);
}

static void helpmessage(const char *progname);

static void help(int argc, const char **argv) {

	if (argc!=2 || (charstring::compare(argv[1],"-help") &&
			charstring::compare(argv[1],"--help"))) {
		return;
	}

	// strip off any leading directories
	char	*progname=file::basename(argv[0]);

	// strip off any libtool prefixes
	if (!charstring::compare(progname,"lt-",3)) {
		char	*tmp=charstring::duplicate(progname+3);
		delete[] progname;
		progname=tmp;
	}

	helpmessage(progname);

	delete[] progname;

	process::exit(0);
}

#define CONNECTIONOPTIONS \
"Connection options:\n" \
"	-host host		Host name or IP address of the server to\n" \
"				connect to.\n" \
"\n" \
"	-port port		Port to connect to.\n" \
"\n" \
"	-socket socket		Local unix socket file name to connect to.\n" \
"				Can be used instead of host/port for making\n" \
"				connections to local servers.\n" \
"\n" \
"	-user user		User name to authenticate with.\n" \
"\n" \
"	-password password	Password to authenticate with.\n" \
"\n" \
"Alternate connection options:\n" \
"	-config file		Override the default config file with \"file\".\n" \
"\n" \
"	-id instanceid		Id of an instance in the config file from which\n" \
"				to derive connection info and credentials.\n" \
"\n"

#define CONFIGID \
"	-config file		Override the default config file with \"file\".\n" \
"\n" \
"	-id instanceid		Id of an instance in the config file.\n" \
"\n"

#define SERVEROPTIONS \
CONFIGID \
"	-localstatedir dir 	Override the default directory for keeping\n" \
"				working or stateful files with \"dir\".\n" \
"\n"

#define DISABLECRASHHANDLER \
"	-disable-crash-handler	Disables SIGSEGV handler, useful for debugging.\n" \
"\n"

#define REPORTBUGS "Report bugs to <david.muse@firstworks.com>\n"
