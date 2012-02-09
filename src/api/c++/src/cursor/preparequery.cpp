// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>

void sqlrcursor::prepareQuery(const char *query) {
	prepareQuery(query,charstring::length(query));
}

void sqlrcursor::prepareQuery(const char *query, uint32_t length) {
	reexecute=false;
	validatebinds=false;
	resumed=false;
	clearVariables();
	querylen=length;
	if (copyrefs) {
		initQueryBuffer(querylen);
		charstring::copy(querybuffer,query,querylen);
		querybuffer[querylen]='\0';
	} else {
		queryptr=query;
	}
}

bool sqlrcursor::prepareFileQuery(const char *path, const char *filename) {

	// init some variables
	reexecute=false;
	validatebinds=false;
	resumed=false;
	clearVariables();

	// init the fullpath buffer
	if (!fullpath) {
		fullpath=new char[MAXPATHLEN+1];
	}

	// add the path to the fullpath
	uint32_t	index=0;
	uint32_t	counter=0;
	if (path) {
		while (path[index] && counter<MAXPATHLEN) {
			fullpath[counter]=path[index];
			index++;
			counter++;
		}

		// add the "/" to the fullpath
		if (counter<=MAXPATHLEN) {
			fullpath[counter]='/';
			counter++;
		}
	}

	// add the file to the fullpath
	index=0;
	while (filename[index] && counter<MAXPATHLEN) {
		fullpath[counter]=filename[index];
		index++;
		counter++;
	}

	// handle a filename that's too long
	if (counter>MAXPATHLEN) {

		// sabotage the file name so it can't be opened
		fullpath[0]='\0';

		// debug info
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("File name ");
			if (path) {
				sqlrc->debugPrint((char *)path);
				sqlrc->debugPrint("/");
			}
			sqlrc->debugPrint((char *)filename);
			sqlrc->debugPrint(" is too long.");
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

	} else {

		// terminate the string
		fullpath[counter]='\0';

		// debug info
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("File: ");
			sqlrc->debugPrint(fullpath);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}
	}

	// open the file
	file	queryfile;
	if (!queryfile.open(fullpath,O_RDONLY)) {

		// set the error
		char	*err=new char[32+charstring::length(fullpath)];
		charstring::append(err,"The file ");
		charstring::append(err,fullpath);
		charstring::append(err," could not be opened.\n");
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint(err);
			sqlrc->debugPreEnd();
		}
		setError(err);

		// set queryptr to NULL so executeQuery won't try to do
		// anything with it in the event that it gets called
		queryptr=NULL;

		delete[] err;

		return false;
	}

	initQueryBuffer(queryfile.getSize());

	// read the file into the query buffer
	querylen=queryfile.getSize();
	queryfile.read((unsigned char *)querybuffer,querylen);
	querybuffer[querylen]='\0';

	queryfile.close();

	return true;
}

void sqlrcursor::initQueryBuffer(uint32_t querylength) {
	delete[] querybuffer;
	querybuffer=new char[querylength+1];
	queryptr=querybuffer;
}

void sqlrcursor::attachToBindCursor(uint16_t bindcursorid) {
	prepareQuery("");
	reexecute=true;
	cursorid=bindcursorid;
}
