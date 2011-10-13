// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrcursor::getDatabaseList(const char *wild) {
	return getList(GETDBLIST,NULL,wild);
}

bool sqlrcursor::getTableList(const char *wild) {
	return getList(GETTABLELIST,NULL,wild);
}

bool sqlrcursor::getColumnList(const char *table, const char *wild) {
	return getList(GETCOLUMNLIST,(table)?table:"",wild);
}

bool sqlrcursor::getList(uint16_t command,
			const char *table, const char *wild) {

	reexecute=false;
	validatebinds=false;
	resumed=false;
	clearVariables();

	if (!endofresultset) {
		abortResultSet();
	}
	clearResultSet();

	if (!sqlrc->openSession()) {
		return false;
	}

	cached=false;
	endofresultset=false;

	// tell the server we want to get a db list
	sqlrc->cs->write(command);

	// tell the server whether we'll need a cursor or not
	sendCursorStatus();

	// send the wild parameter
	uint32_t	len=charstring::length(wild);
	sqlrc->cs->write(len);
	if (len) {
		sqlrc->cs->write(wild,len);
	}

	// send the table parameter
	if (table) {
		len=charstring::length(table);
		sqlrc->cs->write(len);
		if (len) {
			sqlrc->cs->write(table,len);
		}
	}

	sqlrc->flushWriteBuffer();

	// process the result set
	bool	retval=true;
	if (rsbuffersize) {
		if (!processResultSet(false,rsbuffersize-1)) {
			retval=false;
		}
	} else {
		if (!processResultSet(true,0)) {
			retval=false;
		}
	}

	// set up not to re-execute the same query if executeQuery is called
	// again before calling prepareQuery on a new query
	reexecute=false;

	return retval;
}
