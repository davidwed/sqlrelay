// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <rudiments/charstring.h>
#include <rudiments/stdio.h>
#include <rudiments/file.h>
#include <rudiments/process.h>
#include <rudiments/sys.h>

static void version(int argc, const char **argv) {

	if (argc!=2 || (charstring::compare(argv[1],"-version") &&
			charstring::compare(argv[1],"--version"))) {
		return;
	}

	stdoutput.printf("%s %s\n\n",argv[0],SQLR_VERSION);
	stdoutput.printf("Rudiments version: %s\n",sys::getRudimentsVersion());
#if defined(__DATE__) && defined(__TIME__)
	stdoutput.printf("Compiled: %s %s\n",__DATE__,__TIME__);
#endif
	stdoutput.write("\n"
		"Copyright (c) 1999-2018 David Muse\n"
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
"	-user user		User name to auth with.\n" \
"\n" \
"	-password password	Password to auth with.\n" \
"\n" \
"Alternate connection options:\n" \
"	-config config		Override the default configuration with the\n" \
"				specified configuration.\n" \
"\n" \
"	-id instanceid		Derive connection info and credentials from the\n" \
"				specified instance, as defined in the\n" \
"				configuration.\n" \
"\n" \
"\n" \
"	-krb			Use Kerberos authentication and encryption.\n" \
"\n" \
"	-krbservice svc		Use the specified kerberos service.\n" \
"\n" \
"	-krbmech mech		Use the specified kerberos mechanism.\n" \
"\n" \
"	-krbflags flags		Use the specified kerberos flags,\n" \
"				comma-separated.\n" \
"\n" \
"\n" \
"	-tls			Use TLS/SSL authentication and encrpyiton.\n" \
"\n" \
"	-tlsversion version	Use the specified TLS/SSL version.\n" \
"\n" \
"	-tlscert file		Use the specified certificate chain file.\n" \
"				This file should contain the client's\n" \
"				certificate, private key, and signing\n" \
"				certificates, as appropriate.\n" \
"				On Windows systems, this must be a .pfx file.\n" \
"				On non-Windows systems, a variety of file\n" \
"				formats are supported.\n" \
"\n" \
"	-tlspassword pwd	Use the specified password to acess the private\n" \
"				key in the file specified by -tlscert.\n" \
"\n" \
"	-tlsciphers \"list\"	Allow the specified list of ciphers.  The\n" \
"				list should be quoted and the ciphers should be\n" \
"				separated by spaces.\n" \
"\n" \
"	-tlsvalidate (no|ca|ca+host|ca+domain)\n" \
"				Certificate validation option.\n" \
"				\"no\" - Don't validate the server's certificate.\n" \
"				\"ca\" - Validate that the server's certificate\n" \
"				was signed by a trusted certificate authority.\n" \
"				\"ca+host\" - Perform \"ca\" validation and also\n" \
"				validate that one of the subject alternate names\n" \
"				(or common name if no SANs are present) in the \n" \
"				certificate matches the host parameter.\n" \
"				(Falls back to \"ca\" validation when a unix\n" \
"				socket is used.)\n" \
"				\"ca+domain\" - Perform \"ca\" validation and also\n" \
"				validate that the domain name of one of the\n" \
"				subject alternate naames (or common name if no\n" \
"				SANs are present) in the certificate matches\n" \
"				the domain name of the host parameter.\n" \
"				(Falls back to \"ca\" validation when a unix\n" \
"				socket is used.)\n" \
"\n" \
"	-tlsca file		Use the specified certificate authority file\n" \
"				when validating the server's certificate.  Or,\n" \
"				if \"file\" is a directory, then use all\n" \
"				certificate authority files found in that\n" \
"				directory when validating the server's\n" \
"				certifictate.\n" \
"\n" \
"	-tlsdepth depth 	Set the maximum certificate chain validation\n" \
"				depth to the specified depth.\n" \
"\n"

#define CONFIG \
"	-config config		Override the default configuration with the\n" \
"				specified configuration.\n" \
"\n"

#define CONFIGID \
CONFIG \
"	-id instanceid		Id of an instance, as defined in the\n" \
"				configuration.\n" \
"\n"

#define LOCALSTATEDIR \
"	-localstatedir dir 	Override the default directory for keeping\n" \
"				pid files, sockets, and other working or\n" \
"				stateful files with the specified\n" \
"				directory.\n" \
"\n"

#define SERVEROPTIONS \
CONFIGID \
LOCALSTATEDIR

#define DISABLECRASHHANDLER \
"	-disable-crash-handler	Disable the built-in crash handler.\n" \
"				Useful for debugging.\n" \
"\n"

#define BACKTRACE \
"	-backtrace dir		Generate a backtrace in the specified\n" \
"				directory on abnormal termination.\n" \
"				Useful for debugging.\n" \
"\n"

#define BACKTRACECHILDREN \
"	-backtrace dir		Instructs the program to spawn\n" \
"				sqlr-connection processes with the same\n" \
"				backtrace option and argument.\n" \
"\n"
