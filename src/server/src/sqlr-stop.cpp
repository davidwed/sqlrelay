// Copyright (c) 2014  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>
#include <sqlrelay/sqlrutil.h>
#include <rudiments/sys.h>
#include <rudiments/process.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/directory.h>
#include <rudiments/file.h>
#include <rudiments/signalclasses.h>
#include <rudiments/stdio.h>

const char *programs[]={
	"sqlr-listener-",
	"sqlr-connection-",
	"sqlr-scaler-",
	"sqlr-cachemanager",
	NULL
};

const char *suffixes[]={"",".","","",NULL};

int main(int32_t argc, const char **argv) {

	#include <version.h>

	// are we running on windows
	bool	iswindows=!charstring::compareIgnoringCase(
				sys::getOperatingSystemName(),"Windows");

	// process the command line
	sqlrcmdline	cmdl(argc,argv);

	// get the id
	const char	*id=cmdl.getValue("-id");
	size_t		idlen=charstring::length(id);

	// get the pid directory
	sqlrtempdir	tmpdir(&cmdl);
	stringbuffer	piddir;
	piddir.append(tmpdir.getString());
	piddir.append((iswindows)?'\\':'/');
	piddir.append("pids");

	// open the pid directory
	directory	dir;
	if (!dir.open(piddir.getString())) {
		stdoutput.printf("failed to open pid directory %s\n",
							piddir.getString());
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
				(((*suffix)[0]=='.' &&
					charstring::compare(file,
						match.getString(),
						match.getStringLength())) ||
				((*suffix)[0]=='\0' &&
					charstring::compare(file,
						match.getString())))) ||
				charstring::compare(file,
						match.getString(),
						match.getStringLength())) {
				continue;
			}

			// build the fully qualified path name of the pid file
			fqp.clear();
			fqp.append(piddir.getString());
			fqp.append((iswindows)?'\\':'/');
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
			if (signalmanager::sendSignal(pid,SIGINT)) {
				stdoutput.printf("   success\n");
			} else {
				stdoutput.printf("   failed\n");
				file::remove(fqp.getString());
			}
		}
	}

	process::exit(0);
}
