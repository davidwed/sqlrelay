// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

bool sqlrcontroller_svr::getDatabaseListCommand(sqlrcursor_svr *cursor) {
 	dbgfile.debugPrint("connection",1,"get db list...");
	bool	retval=getListCommand(cursor,0,false);
 	dbgfile.debugPrint("connection",1,"done getting db list");
	return retval;
}

bool sqlrcontroller_svr::getTableListCommand(sqlrcursor_svr *cursor) {
 	dbgfile.debugPrint("connection",1,"get table list...");
	bool	retval=getListCommand(cursor,1,false);
 	dbgfile.debugPrint("connection",1,"done getting table list");
	return retval;
}

bool sqlrcontroller_svr::getColumnListCommand(sqlrcursor_svr *cursor) {
 	dbgfile.debugPrint("connection",1,"get column list...");
	bool	retval=getListCommand(cursor,2,true);
 	dbgfile.debugPrint("connection",1,"done getting column list");
	return retval;
}

bool sqlrcontroller_svr::getListCommand(sqlrcursor_svr *cursor,
						int which, bool gettable) {

	// clean up any custom query cursors
	if (cursor->customquerycursor) {
		delete cursor->customquerycursor;
		cursor->customquerycursor=NULL;
	}

	// clean up whatever result set the cursor might have been busy with
	cursor->cleanUpData(true,true);

	// get length of wild parameter
	uint32_t	wildlen;
	if (clientsock->read(&wildlen,idleclienttimeout,0)!=sizeof(uint32_t)) {
		dbgfile.debugPrint("connection",2,
			"get list failed: client sent bad wild length");
		return false;
	}

	// bounds checking
	if (wildlen>maxquerysize) {
		dbgfile.debugPrint("connection",2,
			"get list failed: client sent bad wild length");
		return false;
	}

	// read the wild parameter into the buffer
	char	*wild=new char[wildlen+1];
	if (wildlen) {
		if ((uint32_t)(clientsock->read(wild,wildlen,
					idleclienttimeout,0))!=wildlen) {
			dbgfile.debugPrint("connection",2,
				"get list failed: "
				"client sent short wild parameter");
			return false;
		}
	}
	wild[wildlen]='\0';

	// read the table parameter into the buffer
	char	*table=NULL;
	if (gettable) {

		// get length of table parameter
		uint32_t	tablelen;
		if (clientsock->read(&tablelen,
				idleclienttimeout,0)!=sizeof(uint32_t)) {
			dbgfile.debugPrint("connection",2,
				"get list failed: "
				"client sent bad table length");
			return false;
		}

		// bounds checking
		if (tablelen>maxquerysize) {
			dbgfile.debugPrint("connection",2,
				"get list failed: "
				"client sent bad table length");
			return false;
		}

		// read the table parameter into the buffer
		table=new char[tablelen+1];
		if (tablelen) {
			if ((uint32_t)(clientsock->read(table,tablelen,
					idleclienttimeout,0))!=tablelen) {
				dbgfile.debugPrint("connection",2,
					"get list failed: "
					"client sent short table parameter");
				return false;
			}
		}
		table[tablelen]='\0';

		// some apps aren't well behaved, trim spaces off of both sides
		charstring::bothTrim(table);

		// translate table name, if necessary
		if (sqlt) {
			const char	*newname=NULL;
			if (sqlt->getReplacementTableName(NULL,NULL,
							table,&newname)) {
				delete[] table;
				table=charstring::duplicate(newname);
			}
		}
	}

	// set the values that we won't get from the client
	cursor->inbindcount=0;
	cursor->outbindcount=0;
	sendcolumninfo=SEND_COLUMN_INFO;

	// get the list and return it
	bool	result=true;
	if (conn->getListsByApiCalls()) {
		result=getListByApiCall(cursor,which,table,wild);
	} else {
		result=getListByQuery(cursor,which,table,wild);
	}

	// clean up
	delete[] wild;
	delete[] table;

	return result;
}

bool sqlrcontroller_svr::getListByApiCall(sqlrcursor_svr *cursor,
						int which,
						const char *table,
						const char *wild) {

	// initialize flags andbuffers
	bool	success=false;

	// get the appropriate list
	switch (which) {
		case 0:
			success=conn->getDatabaseList(cursor,wild);
			break;
		case 1:
			success=conn->getTableList(cursor,wild);
			break;
		case 2:
			success=conn->getColumnList(cursor,table,wild);
			break;
	}

	// if an error occurred...
	if (!success) {
		cursor->errorMessage(cursor->error,
					maxerrorlength,
					&(cursor->errorlength),
					&(cursor->errnum),
					&(cursor->liveconnection));
		returnError(cursor,!cursor->liveconnection);

		// this is actually OK, only return false on a network error
		return true;
	}

	// indicate that no error has occurred
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);

	// send the client the id of the 
	// cursor that it's going to use
	clientsock->write(cursor->id);

	// tell the client that this is not a
	// suspended result set
	clientsock->write((uint16_t)NO_SUSPENDED_RESULT_SET);

	// if the query processed ok then send a result set header and return...
	returnResultSetHeader(cursor);
	if (!returnResultSetData(cursor)) {
		return false;
	}
	return true;
}

bool sqlrcontroller_svr::getListByQuery(sqlrcursor_svr *cursor,
						int which,
						const char *table,
						const char *wild) {

	// build the appropriate query
	const char	*query=NULL;
	bool		havewild=charstring::length(wild);
	switch (which) {
		case 0:
			query=conn->getDatabaseListQuery(havewild);
			break;
		case 1:
			query=conn->getTableListQuery(havewild);
			break;
		case 2:
			query=conn->getColumnListQuery(havewild);
			break;
	}

	// FIXME: this can fail
	buildListQuery(cursor,query,wild,table);

	// run it like a normal query, but don't request the query,
	// binds or column info status from the client
	return handleQueryOrBindCursor(cursor,false,false,false);
}

bool sqlrcontroller_svr::buildListQuery(sqlrcursor_svr *cursor,
						const char *query,
						const char *wild,
						const char *table) {

	// clean up buffers to avoid SQL injection
	stringbuffer	wildbuf;
	escapeParameter(&wildbuf,wild);
	stringbuffer	tablebuf;
	escapeParameter(&tablebuf,table);

	// bounds checking
	cursor->querylength=charstring::length(query)+
					wildbuf.getStringLength()+
					tablebuf.getStringLength();
	if (cursor->querylength>maxquerysize) {
		return false;
	}

	// fill the query buffer and update the length
	if (tablebuf.getStringLength()) {
		snprintf(cursor->querybuffer,maxquerysize+1,
			query,tablebuf.getString(),wildbuf.getString());
	} else {
		snprintf(cursor->querybuffer,maxquerysize+1,
					query,wildbuf.getString());
	}
	cursor->querylength=charstring::length(cursor->querybuffer);
	return true;
}

void sqlrcontroller_svr::escapeParameter(stringbuffer *buffer,
						const char *parameter) {

	if (!parameter) {
		return;
	}

	// escape single quotes
	for (const char *ptr=parameter; *ptr; ptr++) {
		if (*ptr=='\'') {
			buffer->append('\'');
		}
		buffer->append(*ptr);
	}
}
