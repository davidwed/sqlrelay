// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrutil.h>
#include <rudiments/sys.h>
#include <rudiments/process.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/directory.h>
#include <rudiments/file.h>
#include <rudiments/stdio.h>
#include <config.h>
#include <defaults.h>
#include <version.h>

const char *programs[]={
	"sqlr-listener-",
	"sqlr-connection-",
	"sqlr-scaler-",
	"sqlr-cachemanager",
	NULL
};

const char *suffixes[]={".pid",".",".pid",".pid",NULL};

static void helpmessage(const char *progname) {
	stdoutput.printf(
		"%s is the shutdown program for the %s server processes.\n"
		"\n"
		"The %s program stops %s-listener, %s-connection, and %s-scaler processes.\n"
		"\n"
		"When run with the -id argument, %s stops processes for the specified instance.  When run with no -id argument, %s stops all running %s-listener, %s-connection, and %s-scaler processes.\n"
		"\n"
		"Usage: %s [OPTIONS]\n"
		"\n"
		"Options:\n"
		CONFIG
		LOCALSTATEDIR
		"\n"
		"Examples:\n"
		"\n"
		"Stop instance \"myinst\" who's pid files are found in the default location.\n"
		"	%s -id myinst\n"
		"\n"
		"Stop instance \"myinst\" who's pid files are found under /opt/myapp/var\n"
		"	%s -localstatedir /opt/myapp/var -id myinst\n"
		"\n"
		"Stop all running instances who's pid files are found in the default location.\n"
		"	%s\n"
		"\n"
		"Stop all running instances who's pid files are found under /opt/myapp/var\n"
		"	%s -localstatedir /opt/myapp/var\n"
		"\n",
		progname,SQL_RELAY,progname,SQLR,SQLR,SQLR,
		progname,progname,SQLR,SQLR,SQLR,
		progname,progname,progname,progname,progname);
}

int main(int argc, const char **argv) {

	version(argc,argv);
	help(argc,argv);

	// process the command line
	sqlrcmdline	cmdl(argc,argv);

	// get the id
	const char	*id=cmdl.getValue("-id");
	size_t		idlen=charstring::length(id);

	// get the pid directory
	sqlrpaths	sqlrpth(&cmdl);

	// open the pid directory
	directory	dir;
	if (!dir.open(sqlrpth.getPidDir())) {
		stdoutput.printf("failed to open pid directory %s\n",
							sqlrpth.getPidDir());
		process::exit(1);
	}

	// some useful string buffers
	stringbuffer	match;
	stringbuffer	fqp;

	// run through the programs that we want to kill
	const char **prog=NULL;
	const char **suffix=NULL;
	for (prog=programs,suffix=suffixes; *prog; prog++, suffix++) {

		// don't kill the cachemanager if an id was given
		if (idlen && !charstring::compare(*prog,"sqlr-cachemanager")) {
			break;
		}

		// rewind the directory
		dir.rewind();

		// reset the file to match
		match.clear();
		match.append(*prog);
		if (idlen) {
			match.append(id);
			match.append(*suffix);
		}

		// look through the files in the directory...
		for (;;) {

			// get a file
			const char	*file=dir.read();
			if (!file) {
				break;
			}

			// skip the file if it doesn't match what
			// we're looking for
			if ((idlen &&
				(((*suffix)[1]=='\0' &&
					charstring::compare(file,
						match.getString(),
						match.getStringLength())) ||
				((*suffix)[1]=='p' &&
					charstring::compare(file,
						match.getString())))) ||
				charstring::compare(file,
						match.getString(),
						match.getStringLength())) {
				continue;
			}

			// build the fully qualified path name of the pid file
			fqp.clear();
			fqp.append(sqlrpth.getPidDir());
			fqp.append(file);

			// skip the pid file if it's not readable
			if (!file::readable(fqp.getString())) {
				continue;
			}
			
			// get the pid from the file
			char		*pidstr=
					file::getContents(fqp.getString());
			uint64_t	pid=
					charstring::toInteger(pidstr);

			// kill the process
			stdoutput.printf("killing process %lld\n",pid);
			if (process::sendSignal(pid,SIGINT)) {
				stdoutput.printf("   success\n");
			} else {
				stdoutput.printf("   failed\n");
				file::remove(fqp.getString());
			}
		}
	}

	process::exit(0);
}
