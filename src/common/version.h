// Copyright (c) 1999-2000  David Muse
// See the file COPYING for more information

// handle -version
if (argc==2 && (!rudiments::charstring::compare(argv[1],"-version") ||
			!rudiments::charstring::compare(argv[1],"--version"))) {
	stdoutput.printf("SQL Relay version: %s\n",SQLR_VERSION);
	process::exit(0);
}
