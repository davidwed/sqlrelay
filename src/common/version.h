// Copyright (c) 1999-2000  David Muse
// See the file COPYING for more information

// handle -version
if (argc==2 && (!charstring::compare(argv[1],"-version") ||
		!charstring::compare(argv[1],"--version"))) {
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
