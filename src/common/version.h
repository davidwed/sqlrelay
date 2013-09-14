// Copyright (c) 1999-2000  David Muse
// See the file COPYING for more information

// handle -version
if (argc==2 && (!charstring::compare(argv[1],"-version") ||
			!charstring::compare(argv[1],"--version"))) {
	stdoutput.printf("SQL Relay version: %s\n",SQLR_VERSION);
	process::exit(0);
}
