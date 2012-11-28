// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrqueries/sqlrcmdcstat.h>
#include <rudiments/charstring.h>
#include <debugprint.h>

// for gettimeofday
#include <sys/time.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

extern "C" {
	sqlrquery	*new_sqlrcmdcstat(xmldomnode *parameters) {
		return new sqlrcmdcstat(parameters);
	}
}

sqlrcmdcstat::sqlrcmdcstat(xmldomnode *parameters) : sqlrquery(parameters) {
	debugFunction();
}

bool sqlrcmdcstat::match(const char *querystring,
				uint32_t querylength) {
	debugFunction();
	return !charstring::compareIgnoringCase(querystring,"sqlrcmd cstat");
}

sqlrquerycursor *sqlrcmdcstat::getCursor(sqlrconnection_svr *sqlrcon) {
	return new sqlrcmdcstatcursor(sqlrcon,parameters);
}

sqlrcmdcstatcursor::sqlrcmdcstatcursor(
		sqlrconnection_svr *sqlrcon,xmldomnode *parameters) :
					sqlrquerycursor(sqlrcon,parameters) {
	currentrow=0;
	fieldbuffer=NULL;
	cs=NULL;
}

sqlrcmdcstatcursor::~sqlrcmdcstatcursor() {
	delete[] fieldbuffer;
}

bool sqlrcmdcstatcursor::executeQuery(const char *query, uint32_t length) {
	currentrow=0;
	return true;
}

uint32_t sqlrcmdcstatcursor::colCount() {
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

const char *sqlrcmdcstatcursor::getColumnName(uint32_t col) {
	return (col<9)?colinfo[col].name:NULL;
}

uint16_t sqlrcmdcstatcursor::getColumnType(uint32_t col) {
	return (col<9)?colinfo[col].type:0;
}

uint32_t sqlrcmdcstatcursor::getColumnLength(uint32_t col) {
	return (col<9)?colinfo[col].length:0;
}

uint32_t sqlrcmdcstatcursor::getColumnPrecision(uint32_t col) {
	return (col<9)?colinfo[col].precision:0;
}

uint32_t sqlrcmdcstatcursor::getColumnScale(uint32_t col) {
	return (col<9)?colinfo[col].scale:0;
}

bool sqlrcmdcstatcursor::noRowsToReturn() {
	return false;
}

bool sqlrcmdcstatcursor::fetchRow() {
	while (currentrow<MAXCONNECTIONS) {
		cs=&(conn->cont->shm->connstats[currentrow]);
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

void sqlrcmdcstatcursor::getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null) {
	*field=NULL;
	*fieldlength=0;
	*blob=false;
	*null=false;

	delete[] fieldbuffer;
	fieldbuffer=NULL;

	switch (col) {
		case 0:
			// index -
			// index assigned by listener
			fieldbuffer=charstring::parseNumber(cs->index);
			break;
		case 1:
			// mine -
			// * if the connection is processing this command
			if (cs->index==conn->cont->handoffindex) {
				*field="*";
				*fieldlength=1;
			} else {
				*null=true;
			}
			return;
		case 2:
			// processid -
			// pid of the connection
			fieldbuffer=charstring::parseNumber(cs->processid);
			break;
		case 3:
			// connect -
			// number of client connections
			fieldbuffer=charstring::parseNumber(cs->nconnect);
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
			struct timeval	now;
			gettimeofday(&now,NULL);
			double	statetime=
				((double)(now.tv_sec-
					cs->state_start_tv.tv_sec))+
				((double)(now.tv_usec-
					cs->state_start_tv.tv_usec))/1000000.0;
			fieldbuffer=charstring::parseNumber(statetime,
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

	*field=fieldbuffer;
	*fieldlength=charstring::length(fieldbuffer);
}
