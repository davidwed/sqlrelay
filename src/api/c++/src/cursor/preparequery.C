// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>

void sqlrcursor::prepareQuery(const char *query) {
	prepareQuery(query,strlen(query));
}

void sqlrcursor::prepareQuery(const char *query, int length) {
	reexecute=0;
	validatebinds=0;
	resumed=0;
	clearVariables();
	if (copyrefs) {
		initQueryBuffer();
		strcpy(queryptr,query);
	} else {
		queryptr=(char *)query;
	}
	querylen=length;
	if (querylen>MAXQUERYSIZE) {
		querylen=MAXQUERYSIZE;
	}
}

int sqlrcursor::prepareFileQuery(const char *path, const char *filename) {

	// init some variables
	reexecute=0;
	validatebinds=0;
	resumed=0;
	clearVariables();

	// init the fullpath buffer
	if (!fullpath) {
		fullpath=new char[MAXPATHLEN+1];
	}

	// add the path to the fullpath
	int	index=0;
	int	counter=0;
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
		fullpath[0]=(char)NULL;

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
		fullpath[counter]=(char)NULL;

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
		char	*err=new char[32+strlen(fullpath)];
		strcpy(err,"The file ");
		strcat(err,fullpath);
		strcat(err," could not be opened.\n");
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint(err);
			sqlrc->debugPreEnd();
		}
		setError(err);
		delete[] err;

		// set queryptr to NULL so executeQuery won't try to do
		// anything with it in the event that it gets called
		queryptr=NULL;

		return 0;
	}

	initQueryBuffer();

	// read the file into the query buffer
	querylen=queryfile.getSize();
	if (querylen<MAXQUERYSIZE) {
		queryfile.read((unsigned char *)querybuffer,querylen);
		querybuffer[querylen]=(char)NULL;
	} else {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("The query in ");
		sqlrc->debugPrint(fullpath);
		sqlrc->debugPrint(" is too large.\n");
		sqlrc->debugPreEnd();
	}

	queryfile.close();

	return 1;
}

void sqlrcursor::initQueryBuffer() {
	if (!querybuffer) {
		querybuffer=new char[MAXQUERYSIZE+1];
		queryptr=querybuffer;
	}
}

void sqlrcursor::attachToBindCursor(short bindcursorid) {
	prepareQuery("");
	reexecute=1;
	cursorid=bindcursorid;
}
