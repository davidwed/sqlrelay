// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/charstring.h>
#include <rudiments/error.h>
#include <defines.h>
#include <datatypes.h>

bool sqlrcursor::processResultSet(bool getallrows, uint64_t rowtoget) {

	// start caching the result set
	if (cacheon) {
		startCaching();
	}

	// parse the columninfo and data
	bool	success=true;

	// skip and fetch here if we're not reading from a cached result set
	// this way, everything gets done in 1 round trip
	if (!cachesource) {
		success=skipAndFetch(getallrows,firstrowindex+rowtoget);
	}

	// check for an error
	if (success) {

		uint16_t	err=getErrorStatus();

		if (err!=NO_ERROR_OCCURRED) {

			getErrorFromServer();

			// Don't get the cursor if the error was that there
			// were no cursors available.
			if (charstring::compare(error,NOCURSORSERROR)) {
				getCursorId();
			}

			// if we need to disconnect then end the session
			if (err==ERROR_OCCURRED_DISCONNECT) {
				sqlrc->endSession();
			}
			return false;
		}
	}

	// get data back from the server
	if (success && ((cachesource && cachesourceind) ||
			((!cachesource && !cachesourceind)  && 
				(success=getCursorId()) && 
				(success=getSuspended()))) &&
			(success=parseColumnInfo()) && 
			(success=parseOutputBinds())) {

		// skip and fetch here if we're reading from a cached result set
		if (cachesource) {
			success=skipAndFetch(getallrows,firstrowindex+rowtoget);
		}

		// parse the data
		if (success) {
			success=parseData();
		}
	}

	// if success is false, then some kind of network error occurred,
	// end the session
	if (!success) {
		clearResultSet();
		sqlrc->endSession();
	}
	return success;
}

uint16_t sqlrcursor::getErrorStatus() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Checking For An Error... ");
		sqlrc->debugPreEnd();
	}

	// get a flag indicating whether there's been an error or not
	uint16_t	err;
	if (getShort(&err)!=sizeof(uint16_t)) {
		setError("Failed to determine whether an error occurred or not.\n A network error may have ocurred.");
		return false;
	}

	if (err==NO_ERROR_OCCURRED) {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("none.\n");
			sqlrc->debugPreEnd();
		}
		cacheNoError();
		return NO_ERROR_OCCURRED;
	}

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("error!!!\n");
		sqlrc->debugPreEnd();
	}
	return err;
}

bool sqlrcursor::getCursorId() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Getting Cursor ID...\n");
		sqlrc->debugPreEnd();
	}
	if (sqlrc->cs->read(&cursorid)!=sizeof(uint16_t)) {
		char	*err=error::getErrorString();
		stringbuffer	errstr;
		errstr.append("Failed to get a cursor id.\n "
				"A network error may have ocurred. ");
		errstr.append(err);
		setError(errstr.getString());
		delete[] err;
		return false;
	}
	havecursorid=true;
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Cursor ID: ");
		sqlrc->debugPrint((int64_t)cursorid);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}
	return true;
}

bool sqlrcursor::getSuspended() {

	// see if the result set of that cursor is actually suspended
	uint16_t	suspendedresultset;
	if (sqlrc->cs->read(&suspendedresultset)!=sizeof(uint16_t)) {
		setError("Failed to determine whether the session was suspended or not.\n A network error may have ocurred.");
		return false;
	}

	if (suspendedresultset==SUSPENDED_RESULT_SET) {

		// If it was suspended the server will send the index of the 
		// last row from the previous result set.
		// Initialize firstrowindex and rowcount from this index.
		if (sqlrc->cs->read(&firstrowindex)!=sizeof(uint64_t)) {
			setError("Failed to get the index of the last row of a previously suspended result set.\n A network error may have ocurred.");
			return false;
		}
		rowcount=firstrowindex+1;
	
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Previous result set was ");
	       		sqlrc->debugPrint("suspended at row index: ");
			sqlrc->debugPrint((int64_t)firstrowindex);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

	} else {

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Previous result set was ");
	       		sqlrc->debugPrint("not suspended.\n");
			sqlrc->debugPreEnd();
		}
	}
	return true;
}

void sqlrcursor::getErrorFromServer() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Getting Error From Server\n");
		sqlrc->debugPreEnd();
	}

	bool	networkerror=true;

	// get the error code
	if (getLongLong((uint64_t *)&errorno)==sizeof(uint64_t)) {

		// get the length of the error string
		uint16_t	length;
		if (getShort(&length)==sizeof(uint16_t)) {

			// get the error string
			error=new char[length+1];
			sqlrc->cs->read(error,length);
			error[length]='\0';

			networkerror=false;
		}
	}

	if (networkerror) {
		setError("There was an error, but the connection"
				" died trying to retrieve it.  Sorry.");
	}
	
	handleError();
}

void sqlrcursor::setError(const char *err) {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Setting Error\n");
		sqlrc->debugPreEnd();
	}
	error=charstring::duplicate(err);
	handleError();
}

void sqlrcursor::handleError() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint(error);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	endofresultset=true;

	cacheError();
	finishCaching();
}

bool sqlrcursor::fetchRowIntoBuffer(bool getallrows, uint64_t row,
						uint64_t *rowbufferindex) {

	// if we getting the entire result set at once, then the result set 
	// buffer index is the requested row-firstrowindex
	if (!rsbuffersize) {
		if (row<rowcount && row>=firstrowindex) {
			*rowbufferindex=row-firstrowindex;
			return true;
		}
		return false;
	}

	// but, if we're not getting the entire result set at once
	// and if the requested row is not in the current range, 
	// fetch more data from the connection
	while (row>=(firstrowindex+rsbuffersize) && !endofresultset) {

		if (sqlrc->connected || (cachesource && cachesourceind)) {

			clearRows();

			// if we're not fetching from a cached result set,
			// tell the server to send one 
			if (!cachesource && !cachesourceind) {
				sqlrc->cs->write((uint16_t)FETCH_RESULT_SET);
				sqlrc->cs->write(cursorid);
			}

			if (!skipAndFetch(getallrows,row) || !parseData()) {
				return false;
			}

		} else {
			return false;
		}
	}

	// return the buffer index corresponding to the requested row
	// or -1 if the requested row is past the end of the result set
	if (row<rowcount) {
		*rowbufferindex=row%rsbuffersize;
		return true;
	}
	return false;
}

int32_t sqlrcursor::getShort(uint16_t *integer) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (cachesource && cachesourceind) {
		return cachesource->read(integer);
	} else {
		return sqlrc->cs->read(integer);
	}
}

int32_t sqlrcursor::getLong(uint32_t *integer) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (cachesource && cachesourceind) {
		return cachesource->read(integer);
	} else {
		return sqlrc->cs->read(integer);
	}
}

int32_t sqlrcursor::getLongLong(uint64_t *integer) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (cachesource && cachesourceind) {
		return cachesource->read(integer);
	} else {
		return sqlrc->cs->read(integer);
	}
}

int32_t sqlrcursor::getString(char *string, int32_t size) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (cachesource && cachesourceind) {
		return cachesource->read(string,size);
	} else {
		return sqlrc->cs->read(string,size);
	}
}

int32_t sqlrcursor::getDouble(double *value) {

	// if the result set is coming from a cache file, read from
	// the file, if not, read from the server
	if (cachesource && cachesourceind) {
		return cachesource->read(value);
	} else {
		return sqlrc->cs->read(value);
	}
}
