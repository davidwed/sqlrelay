// Copyright (c) 1999-2000  David Muse
// See the file COPYING for more information

// handle -version
if (argc==2 && (!strcmp(argv[1],"-version") || !strcmp(argv[1],"--version"))) {
	printf("SQL Relay version: %s\n",SQLR_VERSION);
	exit(0);
}
