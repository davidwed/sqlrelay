// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection_svr::getDatabaseListCommand(sqlrcursor_svr *cursor) {
	return getListCommand(cursor,0,false);
}

bool sqlrconnection_svr::getTableListCommand(sqlrcursor_svr *cursor) {
	return getListCommand(cursor,1,false);
}

bool sqlrconnection_svr::getColumnListCommand(sqlrcursor_svr *cursor) {
	return getListCommand(cursor,2,true);
}

bool sqlrconnection_svr::getListCommand(sqlrcursor_svr *cursor,
						int which, bool gettable) {

 	dbgfile.debugPrint("connection",2,"getting list command");

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
	}

	// build the appropriate query
	const char	*query=NULL;
	switch (which) {
		case 0:
			query=getDatabaseListQuery(charstring::length(wild));
			break;
		case 1:
			query=getTableListQuery(charstring::length(wild));
			break;
		case 2:
			query=getColumnListQuery(charstring::length(wild));
			break;
	}
	buildListQuery(cursor,query,wild,table);
	delete[] wild;
	delete[] table;

	// set the values that we won'tget from the client
	cursor->inbindcount=0;
	cursor->outbindcount=0;
	sendcolumninfo=SEND_COLUMN_INFO;

	// run it like a normal query, but don't request the query,
	// binds or column info status from the client
	return newQueryInternal(cursor,false);
}

bool sqlrconnection_svr::buildListQuery(sqlrcursor_svr *cursor,
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

void sqlrconnection_svr::escapeParameter(stringbuffer *buffer,
						const char *parameter) {

	if (!parameter) {
		return;
	}

	// escape single quotes and slashes
	for (const char *ptr=parameter; *ptr; ptr++) {
		if (*ptr=='\'' || *ptr=='\\') {
			buffer->append('\\');
		}
		buffer->append(*ptr);
	}
}

const char *sqlrconnection_svr::getDatabaseListQuery(bool wild) {
	return "select 1";
}

const char *sqlrconnection_svr::getTableListQuery(bool wild) {
	return "select 1";
}

const char *sqlrconnection_svr::getColumnListQuery(bool wild) {
	return "select 1";
}
