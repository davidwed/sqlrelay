// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>
#include <defines.h>
#include <datatypes.h>

int sqlrcursor::processResultSet(int rowtoget) {

	// start caching the result set
	if (cacheon) {
		startCaching();
	}

	// parse the columninfo and data
	int	success=1;

	// skip and fetch here if we're not reading from a cached result set
	// this way, everything gets done in 1 round trip
	if (!cachesource) {
		success=skipAndFetch(firstrowindex+rowtoget);
	}

	// get data back from the server
	if (success>0 && (success=noError())>0 &&
			((cachesource && cachesourceind) ||
			((!cachesource && !cachesourceind)  && 
				(success=getCursorId()) && 
				(success=getSuspended())>0)) &&
			(success=parseColumnInfo())>0 && 
			(success=parseOutputBinds())>0) {

		// skip and fetch here if we're reading from a cached result set
		if (cachesource) {
			success=skipAndFetch(firstrowindex+rowtoget);
		}

		// parse the data
		if (success>-1) {
			success=parseData();
		}
	}

	// handle error responses
	if (success==0) {
		getErrorFromServer();
		return 0;
	} else if (success==-1) {
		clearResultSet();
		setError("Failed to execute the query and/or process the result set.\n A query, bind variable or bind value could be too large, there could be too \n many bind variables, or a network error may have ocurred.");
		sqlrc->endSession();
		return 0;
	}
	return 1;
}

int sqlrcursor::noError() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Checking For An Error... ");
		sqlrc->debugPreEnd();
	}

	// get a flag indicating whether there's been an error or not
	unsigned short	success;
	if (getShort(&success)!=sizeof(unsigned short)) {
		return -1;
	}
	if (success==NO_ERROR) {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("none.\n");
			sqlrc->debugPreEnd();
		}
		cacheNoError();
		return 1;
	} else if (success==ERROR) {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("error!!!\n");
			sqlrc->debugPreEnd();
		}
		return 0;
	}
	return -1;
}

int sqlrcursor::getCursorId() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Getting Cursor ID...\n");
		sqlrc->debugPreEnd();
	}
	if (sqlrc->read(&cursorid)!=sizeof(unsigned short)) {
		return 0;
	}
	havecursorid=true;
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Cursor ID: ");
		sqlrc->debugPrint((long)cursorid);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}
	return 1;
}

int sqlrcursor::getSuspended() {

	// see if the result set of that cursor is actually suspended
	unsigned short	suspendedresultset;
	if (sqlrc->read(&suspendedresultset)!=sizeof(unsigned short)) {
		return -1;
	}

	if (suspendedresultset==SUSPENDED_RESULT_SET) {

		// If it was suspended the server will send the index of the 
		// last row from the previous result set.
		// Initialize firstrowindex and rowcount from this index.
		sqlrc->read(&firstrowindex);
		rowcount=firstrowindex+1;
	
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Previous result set was ");
	       		sqlrc->debugPrint("suspended at row index: ");
			sqlrc->debugPrint((long)firstrowindex);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}
		return 1;

	} else if (suspendedresultset==NO_SUSPENDED_RESULT_SET) {

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Previous result set was ");
	       		sqlrc->debugPrint("not suspended.\n");
			sqlrc->debugPreEnd();
		}
		return 1;

	}
	return 0;
}

void sqlrcursor::getErrorFromServer() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Getting Error From Server\n");
		sqlrc->debugPreEnd();
	}

	// get the length of the error string
	unsigned short	length;
	if (getShort(&length)!=sizeof(unsigned short)) {
		error=new char[77];
		strcpy(error,"There was an error, but the connection died trying to retrieve it.  Sorry.");
	} else {
		// get the error string
		error=new char[length+1];
		sqlrc->read(error,length);
		error[length]=(char)NULL;
	}
	
	handleError();
}

void sqlrcursor::setError(const char *err) {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Setting Error\n");
		sqlrc->debugPreEnd();
	}
	error=strdup(err);
	handleError();
}

void sqlrcursor::handleError() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint(error);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	endofresultset=1;

	cacheError();
	finishCaching();
}

int sqlrcursor::fetchRowIntoBuffer(int row) {

	// if we getting the entire result set at once, then the result set 
	// buffer index is the requested row-firstrowindex
	if (!rsbuffersize) {
		if (row<(int)rowcount && row>=(int)firstrowindex) {
			return row-firstrowindex;
		}
		return -1;
	}

	// but, if we're not getting the entire result set at once
	// and if the requested row is not in the current range, 
	// fetch more data from the connection
	while (row>=(int)(firstrowindex+rsbuffersize) && !endofresultset) {

		if (sqlrc->connected || (cachesource && cachesourceind)) {

			clearRows();

			// if we're not fetching from a cached result set,
			// tell the server to send one 
			if (!cachesource && !cachesourceind) {
				sqlrc->write((unsigned short)FETCH_RESULT_SET);
				sqlrc->write(cursorid);
			}

			if (skipAndFetch(row)==-1 || parseData()==-1) {
				return -1;
			}

		} else {
			return -1;
		}
	}

	// return the buffer index corresponding to the requested row
	// or -1 if the requested row is past the end of the result set
	if (row<rowcount) {
		return row%rsbuffersize;
	}
	return -1;
}

int sqlrcursor::getShort(unsigned short *integer) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (cachesource && cachesourceind) {
		return cachesource->read(integer);
	} else {
		return sqlrc->read(integer);
	}
}

int sqlrcursor::getLong(unsigned long *integer) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (cachesource && cachesourceind) {
		return cachesource->read(integer);
	} else {
		return sqlrc->read(integer);
	}
}

int sqlrcursor::getString(char *string, int size) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (cachesource && cachesourceind) {
		return cachesource->read(string,size);
	} else {
		return sqlrc->read(string,size);
	}
}
