// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>

int	sqlrcursor::skipAndFetch(int rowtoget) {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Skipping and Fetching\n");
		if (rowtoget>-1) {
			sqlrc->debugPrint("	row to get: ");
			sqlrc->debugPrint((long)rowtoget);
			sqlrc->debugPrint("\n");
		}
		sqlrc->debugPreEnd();
	}

	// if we're stepping through the result set, we can possibly 
	// skip a big chunk of it...
	if (!skipRows(rowtoget)) {
		return -1;
	}

	// tell the connection how many rows to send
	fetchRows();
	return 1;
}

void	sqlrcursor::fetchRows() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Fetching ");
		sqlrc->debugPrint(rsbuffersize);
		sqlrc->debugPrint(" rows\n");
		sqlrc->debugPreEnd();
	}

	// if we're reading from a cached result set, do nothing
	if (cachesource && cachesourceind) {
		return;
	}

	// otherwise, send to the connection the number of rows to send back
	sqlrc->write((unsigned long)rsbuffersize);
}

int	sqlrcursor::skipRows(int rowtoget) {

	// if we're reading from a cached result set we have to manually skip
	if (cachesource && cachesourceind) {

		// if rowtoget is -1 then don't skip,
		// otherwise skip to the next block of rows
		if (rowtoget==-1) {
			return 1;
		} else {
			rowcount=rowtoget-(rowtoget%rsbuffersize);
		}

		// get the row offset from the index
		cachesourceind->setPositionRelativeToBeginning(
				13+sizeof(long)+(rowcount*sizeof(long)));
		long	rowoffset;
		if (cachesourceind->read(&rowoffset)!=sizeof(long)) {
			setError("The cache file index appears to be corrupt.");
			return 0;
		}

		// skip to that offset in the cache file
		cachesource->setPositionRelativeToBeginning(rowoffset);
		return 1;
	}

	// calculate how many rows to skip unless we're buffering the entire
	// result set or caching the result set
	unsigned long	skip=0;
	if (rsbuffersize && !cachedest && rowtoget>-1) {
		skip=(long)((rowtoget-(rowtoget%rsbuffersize))-rowcount); 
		rowcount=rowcount+skip;
	}
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Skipping ");
		sqlrc->debugPrint((long)skip);
		sqlrc->debugPrint(" rows\n");
		sqlrc->debugPreEnd();
	}

	// if we're reading from a connection, send the connection the 
	// number of rows to skip
	sqlrc->write(skip);
	return 1;
}
