// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/process.h>
#include <rudiments/datetime.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>
#include <datatypes.h>

class SQLRSERVER_DLLSPEC sqlrquery_sqlrcmdcstat : public sqlrquery {
	public:
			sqlrquery_sqlrcmdcstat(sqlrservercontroller *cont,
							sqlrqueries *qs,
							domnode *parameters);
		bool	match(const char *querystring, uint32_t querylength);
		sqlrquerycursor	*newCursor(sqlrserverconnection *conn,
							uint16_t id);
};

class sqlrquery_sqlrcmdcstatcursor : public sqlrquerycursor {
	public:
			sqlrquery_sqlrcmdcstatcursor(
						sqlrserverconnection *sqlrcon,
						sqlrquery *q,
						domnode *parameters,
						uint16_t id);
			~sqlrquery_sqlrcmdcstatcursor();

		bool		executeQuery(const char *query,
						uint32_t length);
		uint32_t	colCount();
		const char	*getColumnName(uint32_t col);
		uint16_t	getColumnType(uint32_t col);
		uint32_t	getColumnLength(uint32_t col);
		uint32_t	getColumnPrecision(uint32_t col);
		uint32_t	getColumnScale(uint32_t col);
		uint16_t	getColumnIsNullable(uint32_t col);
		bool		noRowsToReturn();
		bool		fetchRow(bool *error);
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob, bool *null);
	private:
		uint64_t	currentrow;
		char		*fieldbuffer[9];

		sqlrconnstatistics	*cs;

};

sqlrquery_sqlrcmdcstat::sqlrquery_sqlrcmdcstat(sqlrservercontroller *cont,
						sqlrqueries *qs,
						domnode *parameters) :
						sqlrquery(cont,qs,parameters) {
	debugFunction();
}

bool sqlrquery_sqlrcmdcstat::match(const char *querystring,
					uint32_t querylength) {
	debugFunction();
	return !charstring::compareIgnoringCase(querystring,"sqlrcmd cstat");
}

sqlrquerycursor *sqlrquery_sqlrcmdcstat::newCursor(
					sqlrserverconnection *sqlrcon,
					uint16_t id) {
	return new sqlrquery_sqlrcmdcstatcursor(sqlrcon,this,
						getParameters(),id);
}

sqlrquery_sqlrcmdcstatcursor::sqlrquery_sqlrcmdcstatcursor(
					sqlrserverconnection *sqlrcon,
					sqlrquery *q,
					domnode *parameters,
					uint16_t id) :
				sqlrquerycursor(sqlrcon,q,parameters,id) {
	currentrow=0;
	for (uint16_t i=0; i<9; i++) {
		fieldbuffer[i]=NULL;
	}
	cs=NULL;
}

sqlrquery_sqlrcmdcstatcursor::~sqlrquery_sqlrcmdcstatcursor() {
	for (uint16_t i=0; i<9; i++) {
		delete[] fieldbuffer[i];
	}
}

bool sqlrquery_sqlrcmdcstatcursor::executeQuery(const char *query,
						uint32_t length) {
	currentrow=0;
	return true;
}

uint32_t sqlrquery_sqlrcmdcstatcursor::colCount() {
	return 9;
}

struct colinfo_t {
	const char	*name;
	uint16_t	type;
	uint32_t	length;
	uint32_t	precision;
	uint32_t	scale;
};

static struct colinfo_t colinfo[]={
	{"INDEX",NUMBER_DATATYPE,10,10,0},
	{"MINE",VARCHAR2_DATATYPE,1,0,0},
	{"PROCESSID",NUMBER_DATATYPE,10,10,0},
	{"CONNECT",NUMBER_DATATYPE,12,12,0},
	{"STATE",VARCHAR2_DATATYPE,25,0,0},
	{"STATE_TIME",NUMBER_DATATYPE,12,12,2},
	{"CLIENT_ADDR",VARCHAR2_DATATYPE,24,0,0},
	{"CLIENT_INFO",VARCHAR2_DATATYPE,STATCLIENTINFOLEN-1,0,0},
	{"SQL_TEXT",VARCHAR2_DATATYPE,STATSQLTEXTLEN-1,0,0}
};

const char *sqlrquery_sqlrcmdcstatcursor::getColumnName(uint32_t col) {
	return (col<9)?colinfo[col].name:NULL;
}

uint16_t sqlrquery_sqlrcmdcstatcursor::getColumnType(uint32_t col) {
	return (col<9)?colinfo[col].type:0;
}

uint32_t sqlrquery_sqlrcmdcstatcursor::getColumnLength(uint32_t col) {
	return (col<9)?colinfo[col].length:0;
}

uint32_t sqlrquery_sqlrcmdcstatcursor::getColumnPrecision(uint32_t col) {
	return (col<9)?colinfo[col].precision:0;
}

uint32_t sqlrquery_sqlrcmdcstatcursor::getColumnScale(uint32_t col) {
	return (col<9)?colinfo[col].scale:0;
}

uint16_t sqlrquery_sqlrcmdcstatcursor::getColumnIsNullable(uint32_t col) {
	return (col==7 || col==8)?1:0;
}

bool sqlrquery_sqlrcmdcstatcursor::noRowsToReturn() {
	return false;
}

bool sqlrquery_sqlrcmdcstatcursor::fetchRow(bool *error) {
	*error=false;
	while (currentrow<MAXCONNECTIONS) {
		cs=&(conn->cont->getShm()->connstats[currentrow]);
		currentrow++;
		if (cs->processid) {
			return true;
		}
	}
	return false;
}

static const char * const statenames[]={
	"NOT_AVAILABLE",
	"INIT",
	"WAIT_FOR_AVAIL_DB",
	"WAIT_CLIENT",
	"SESSION_START",
	"GET_COMMAND",
	"PROCESS_SQL",
	"PROCESS_SQLCMD",
	"RETURN_RESULT_SET",
	"END_SESSION",
	"ANNOUNCE_AVAILABILITY",
	"WAIT_SEMAPHORE"
};

void sqlrquery_sqlrcmdcstatcursor::getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null) {
	*field=NULL;
	*fieldlength=0;
	*blob=false;
	*null=false;

	delete[] fieldbuffer[col];
	fieldbuffer[col]=NULL;

	switch (col) {
		case 0:
			// index -
			// index in sqlrshm.connstats array
			fieldbuffer[col]=charstring::parseNumber(currentrow-1);
			break;
		case 1:
			// mine -
			// * if the connection is processing this command
			if (cs->processid==(uint32_t)process::getProcessId()) {
				*field="*";
				*fieldlength=1;
			} else {
				*null=true;
			}
			return;
		case 2:
			// processid -
			// pid of the connection
			fieldbuffer[col]=charstring::parseNumber(cs->processid);
			break;
		case 3:
			// connect -
			// number of client connections
			fieldbuffer[col]=charstring::parseNumber(cs->nconnect);
			break;
		case 4:
			// state -
			// internally defined status
			if (cs->state<=WAIT_SEMAPHORE) {
				*field=statenames[cs->state];
				*fieldlength=charstring::length(*field);
				return;
			}
			*null=true;
			return;
		case 5:
			{
			// state_time -
			// seconds the connection has been in its current state
			datetime	dt;
			dt.getSystemDateAndTime();
			double	statetime=
				((double)(dt.getSeconds()-
					cs->statestartsec))+
				((double)(dt.getMicroseconds()-
					cs->statestartusec))/1000000.0;
			fieldbuffer[col]=charstring::parseNumber(statetime,
					colinfo[5].precision,colinfo[5].scale);
			}
			break;
		case 6:
			// client_addr -
			// address of currently connected client
			*field=cs->clientaddr;
			*fieldlength=charstring::length(*field);
			return;
		case 7:
			// client info -
			// client info string
			*field=cs->clientinfo;
			*fieldlength=charstring::length(*field);
			return;
		case 8:
			// sql_text -
			// query currently being executed
			*field=cs->sqltext;
			*fieldlength=charstring::length(*field);
			return;
		default:
			*null=true;
			return;
	}

	*field=fieldbuffer[col];
	*fieldlength=charstring::length(fieldbuffer[col]);
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrquery *new_sqlrquery_sqlrcmdcstat(
						sqlrservercontroller *cont,
						sqlrqueries *qs,
						domnode *parameters) {
		return new sqlrquery_sqlrcmdcstat(cont,qs,parameters);
	}
}
