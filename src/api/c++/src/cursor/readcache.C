// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>

bool sqlrcursor::openCachedResultSet(const char *filename) {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Opening cached result set: ");
		sqlrc->debugPrint(filename);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	if (!endofresultset) {
		abortResultSet();
	}
	clearResultSet();

	cached=true;
	endofresultset=false;

	// create the index file name
	char	*indexfilename=new char[strlen(filename)+5];
	sprintf(indexfilename,"%s.ind",filename);

	// open the file
	cachesource=new file();
	cachesourceind=new file();
	if ((cachesource->open(filename,O_RDWR|O_EXCL)) &&
		(cachesourceind->open(indexfilename,O_RDWR|O_EXCL))) {

		// don't need this anymore
		delete[] indexfilename;

		// initialize firstrowindex and rowcount
		firstrowindex=0;
		rowcount=firstrowindex;

		// make sure it's a cache file and skip the ttl
		char		magicid[13];
		unsigned long	longvar;
		if (getString(magicid,13)==13 &&
			!strncmp(magicid,"SQLRELAYCACHE",13) &&
			getLong(&longvar)==sizeof(unsigned long)) {

			// process the result set
			return processResultSet(firstrowindex+rsbuffersize-1);
		} else {

			// if the test above failed, the file is either not
			// a cache file or is corrupt
			stringbuffer	errstr;
			errstr.append("File ");
			errstr.append(filename);
			errstr.append(" is either corrupt");
			errstr.append(" or not a cache file.");
			setError(errstr.getString());
		}

	} else {

		// if we couldn't open the file, set the error message
		stringbuffer	errstr;
		errstr.append("Couldn't open ");
		errstr.append(filename);
		errstr.append(" and ");
		errstr.append(indexfilename);
		setError(errstr.getString());
	}

	// don't need this anymore
	delete[] indexfilename;

	// if we fell through to here, then an error has ocurred
	clearCacheSource();
	return false;
}

void sqlrcursor::clearCacheSource() {
	if (cachesource) {
		cachesource->close();
		delete cachesource;
		cachesource=NULL;
	}
	if (cachesourceind) {
		cachesourceind->close();
		delete cachesourceind;
		cachesourceind=NULL;
	}
}
