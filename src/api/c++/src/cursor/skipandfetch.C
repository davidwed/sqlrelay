// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>

bool sqlrcursor::skipAndFetch(bool getallrows, uint64_t rowtoget) {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Skipping and Fetching\n");
		if (!getallrows) {
			sqlrc->debugPrint("	row to get: ");
			sqlrc->debugPrint((int64_t)rowtoget);
			sqlrc->debugPrint("\n");
		}
		sqlrc->debugPreEnd();
	}

	// if we're stepping through the result set, we can possibly 
	// skip a big chunk of it...
	if (!skipRows(getallrows,rowtoget)) {
		return false;
	}

	// tell the connection how many rows to send
	fetchRows();

	sqlrc->flushWriteBuffer();
	return true;
}

void sqlrcursor::fetchRows() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Fetching ");
		sqlrc->debugPrint((int64_t)rsbuffersize);
		sqlrc->debugPrint(" rows\n");
		sqlrc->debugPreEnd();
	}

	// if we're reading from a cached result set, do nothing
	if (cachesource && cachesourceind) {
		return;
	}

	// otherwise, send to the connection the number of rows to send back
	sqlrc->cs->write(rsbuffersize);
}

bool sqlrcursor::skipRows(bool getallrows, uint64_t rowtoget) {

	// if we're reading from a cached result set we have to manually skip
	if (cachesource && cachesourceind) {

		// skip to the next block of rows
		if (getallrows) {
			return true;
		} else {
			rowcount=rowtoget-(rowtoget%rsbuffersize);
		}

		// get the row offset from the index
		cachesourceind->setPositionRelativeToBeginning(
				13+sizeof(int32_t)+(rowcount*sizeof(int64_t)));
		int64_t	rowoffset;
		if (cachesourceind->read(&rowoffset)!=sizeof(int64_t)) {
			setError("The cache file index appears to be corrupt.");
			return false;
		}

		// skip to that offset in the cache file
		cachesource->setPositionRelativeToBeginning(rowoffset);
		return true;
	}

	// calculate how many rows to skip unless we're buffering the entire
	// result set or caching the result set
	uint64_t	skip=0;
	if (rsbuffersize && !cachedest && !getallrows) {
		skip=(rowtoget-(rowtoget%rsbuffersize))-rowcount; 
		rowcount=rowcount+skip;
	}
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Skipping ");
		sqlrc->debugPrint((int64_t)skip);
		sqlrc->debugPrint(" rows\n");
		sqlrc->debugPreEnd();
	}

	// if we're reading from a connection, send the connection the 
	// number of rows to skip
	sqlrc->cs->write(skip);
	return true;
}
