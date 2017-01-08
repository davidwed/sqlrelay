// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <rudiments/dynamiclib.h>


// types...


// structs...


// function pointers...


// date/time macros...


// constants...


// dlopen infrastructure...
static dynamiclib	lib;
static const char	*module="db2";
static const char	*libname="lib....so";
static const char	*pathnames[]={
	NULL
};

static bool openOnDemand() {

	// buffer to store any errors we might get
	stringbuffer	err;

	// look for the library
	stringbuffer	libfilename;
	const char	**path=pathnames;
	while (*path) {
		libfilename.clear();
		libfilename.append(*path)->append('/')->append(libname);
		if (file::readable(libfilename.getString())) {
			break;
		}
		path++;
	}
	if (!*path) {
		err.append("\nFailed to load ")->append(module);
		err.append(" libraries.\n");
		err.append(libname)->append(" was not found in any "
						"of these paths:\n");
		path=pathnames;
		while (*path) {
			err.append('	')->append(*path)->append('\n');
			path++;
		}
		stdoutput.write(err.getString());
		return false;
	}

	// open the library
	if (!lib.open(libfilename.getString(),true,true)) {
		goto error;
	}

	// get the functions we need

	// success
	return true;

	// error
error:
	char	*error=lib.getError();
	err.append("\nFailed to load ")->append(module);
	err.append(" libraries on-demand.\n");
	err.append(error)->append('\n');
	delete[] error;
	return false;
}
