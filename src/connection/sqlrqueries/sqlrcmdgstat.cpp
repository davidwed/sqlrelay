// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrqueries/sqlrcmdgstat.h>
#include <rudiments/charstring.h>
#include <debugprint.h>

// for time_t, time(), localtime()
#include <time.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

extern "C" {
	sqlrquery	*new_sqlrcmdgstat(xmldomnode *parameters) {
		return new sqlrcmdgstat(parameters);
	}
}

sqlrcmdgstat::sqlrcmdgstat(xmldomnode *parameters) : sqlrquery(parameters) {
	debugFunction();
}

bool sqlrcmdgstat::match(const char *querystring,
				uint32_t querylength) {
	debugFunction();
	return !charstring::compareIgnoringCase(querystring,"sqlrcmd gstat");
}

sqlrquerycursor *sqlrcmdgstat::getCursor(sqlrconnection_svr *sqlrcon) {
	return new sqlrcmdgstatcursor(sqlrcon,parameters);
}

sqlrcmdgstatcursor::sqlrcmdgstatcursor(
		sqlrconnection_svr *sqlrcon,xmldomnode *parameters) :
					sqlrquerycursor(sqlrcon,parameters) {
	rowcount=0;
	currentrow=0;
}

bool sqlrcmdgstatcursor::executeQuery(const char *query, uint32_t length) {

	shmdata	*gs=conn->cont->shm;

	time_t	now=time(NULL);	

	uint32_t	connectedclients=conn->cont->shm->connectedclients;
	if (now/60>gs->peak_connectionsinuse_1min_time/60) {
		gs->peak_connectionsinuse_1min_time=now;
		gs->peak_connectionsinuse_1min=connectedclients;
	}

	int select_1=0, select_5=0, select_15=0;
	int insert_1=0, insert_5=0, insert_15=0;
	int update_1=0, update_5=0, update_15=0;
	int delete_1=0, delete_5=0, delete_15=0;
	int etc_1=0, etc_5=0, etc_15=0;
	int sqlrcmd_1=0, sqlrcmd_5=0, sqlrcmd_15=0;
	for (int j=0; j<60*15; j++) {
		if (now-gs->timestamp[j]<60) {
			select_1+=gs->qps_select[j];
			insert_1+=gs->qps_insert[j];
			update_1+=gs->qps_update[j];
			delete_1+=gs->qps_delete[j];
			etc_1+=gs->qps_etc[j];
			sqlrcmd_1+=gs->qps_custom[j];
		}
		if (now-gs->timestamp[j]<60*5) {
			select_5+=gs->qps_select[j];
			insert_5+=gs->qps_insert[j];
			update_5+=gs->qps_update[j];
			delete_5+=gs->qps_delete[j];
			etc_5+=gs->qps_etc[j];
			sqlrcmd_5+=gs->qps_custom[j];
		}
		if (now-gs->timestamp[j]<60*15) {
			select_15+= gs->qps_select[j];
			insert_15+= gs->qps_insert[j];
			update_15+= gs->qps_update[j];
			delete_15+= gs->qps_delete[j];
			etc_15+= gs->qps_etc[j];
			sqlrcmd_15+= gs->qps_custom[j];

		}
	}

	int32_t qpm_1=select_1+insert_1+update_1+delete_1+etc_1+sqlrcmd_1;
	int32_t	qpm_5=select_5+insert_5+update_5+delete_5+etc_5+sqlrcmd_5;
	int32_t	qpm_15=select_15+insert_15+update_15+
				delete_15+etc_15+sqlrcmd_15;

	time_t	uptime=(now-gs->starttime); 
	if (!uptime) {
		uptime=1;
	}

	rowcount=0;
	char	tmpbuf[GSTAT_VALUE_LEN+1];
	strftime(tmpbuf,GSTAT_VALUE_LEN,"%Y/%m/%d %H:%M:%S",
					localtime(&(gs->starttime)));
	setGSResult("start",tmpbuf,rowcount++);
	setGSResult("uptime",uptime,rowcount++);
	strftime(tmpbuf,GSTAT_VALUE_LEN,"%Y/%m/%d %H:%M:%S",localtime(&now));
	setGSResult("now",tmpbuf,rowcount++);
	setGSResult("access_count",gs->opened_cli_connections,rowcount++);
	setGSResult("query_total",gs->total_queries,rowcount++);
	setGSResult("qpm",gs->total_queries*60/uptime,rowcount++);
	setGSResult("qpm_1",qpm_1,rowcount++);
	setGSResult("qpm_5",qpm_5/5,rowcount++);
	setGSResult("qpm_15",qpm_15/15,rowcount++);
	setGSResult("select_1",select_1,rowcount++);
	setGSResult("select_5",select_5/5,rowcount++);
	setGSResult("select_15",select_15/15,rowcount++);
	setGSResult("insert_1",insert_1,rowcount++);
	setGSResult("insert_5",insert_5/5,rowcount++);
	setGSResult("insert_15",insert_15/15,rowcount++);
	setGSResult("update_1",update_1,rowcount++);
	setGSResult("update_5",update_5/5,rowcount++);
	setGSResult("update_15",update_15/15,rowcount++);
	setGSResult("delete_1",delete_1,rowcount++);
	setGSResult("delete_5",delete_5/5,rowcount++);
	setGSResult("delete_15",delete_15/15,rowcount++);
	setGSResult("etc_1",etc_1,rowcount++);
	setGSResult("etc_5",etc_5/5,rowcount++);
	setGSResult("etc_15",etc_15/15,rowcount++);
	setGSResult("sqlrcmd_1",sqlrcmd_1,rowcount++);
	setGSResult("sqlrcmd_5",sqlrcmd_5/5,rowcount++);
	setGSResult("sqlrcmd_15",sqlrcmd_15/15,rowcount++);
	setGSResult("max_listener",gs->max_listeners,rowcount++);
	setGSResult("max_listener_error",gs->max_listeners_errors,rowcount++);
	setGSResult("busy_listener",
			conn->cont->semset->getValue(10),rowcount++);
	setGSResult("peak_listener",gs->peak_listeners,rowcount++);
	setGSResult("connection",conn->cont->shm->totalconnections,rowcount++);
	setGSResult("session",connectedclients,rowcount++);
	setGSResult("peak_session",gs->peak_connectionsinuse,rowcount++);
	setGSResult("peak_session_1min",
			gs->peak_connectionsinuse_1min,rowcount++);
	strftime(tmpbuf,GSTAT_VALUE_LEN,"%Y/%m/%d %H:%M:%S",
			localtime(&(gs->peak_connectionsinuse_1min_time)));
	setGSResult("peak_session_1min_time",tmpbuf,rowcount++);

	currentrow=0;
	return true;
}

void sqlrcmdgstatcursor::setGSResult(const char *key,
					int32_t value, uint16_t i) {
	charstring::copy(gs_resultset[i].key,key,GSTAT_KEY_LEN);
	gs_resultset[i].key[GSTAT_KEY_LEN]='\0';
	snprintf(gs_resultset[i].value,GSTAT_VALUE_LEN,"%d",value);
	gs_resultset[i].value[GSTAT_VALUE_LEN]='\0';
}

void sqlrcmdgstatcursor::setGSResult(const char *key,
					const char *value, uint16_t i) {
	charstring::copy(gs_resultset[i].key,key,GSTAT_KEY_LEN);
	gs_resultset[i].key[GSTAT_KEY_LEN]='\0';
	charstring::copy(gs_resultset[i].value,value,GSTAT_VALUE_LEN);
	gs_resultset[i].value[GSTAT_VALUE_LEN]='\0';
}

uint32_t sqlrcmdgstatcursor::colCount() {
	return 2;
}

struct colinfo_t {
	const char	*name;
	uint16_t	type;
	uint32_t	length;
	uint32_t	precision;
	uint32_t	scale;
};

static struct colinfo_t colinfo[]={
	{"KEY",VARCHAR2_DATATYPE,GSTAT_KEY_LEN,0,0},
	{"VALUE",VARCHAR2_DATATYPE,GSTAT_VALUE_LEN,0,0}
};

const char *sqlrcmdgstatcursor::getColumnName(uint32_t col) {
	return (col<2)?colinfo[col].name:NULL;
}

uint16_t sqlrcmdgstatcursor::getColumnType(uint32_t col) {
	return (col<2)?colinfo[col].type:0;
}

uint32_t sqlrcmdgstatcursor::getColumnLength(uint32_t col) {
	return (col<2)?colinfo[col].length:0;
}

uint32_t sqlrcmdgstatcursor::getColumnPrecision(uint32_t col) {
	return (col<2)?colinfo[col].precision:0;
}

uint32_t sqlrcmdgstatcursor::getColumnScale(uint32_t col) {
	return (col<2)?colinfo[col].scale:0;
}

bool sqlrcmdgstatcursor::noRowsToReturn() {
	return false;
}

bool sqlrcmdgstatcursor::fetchRow() {
	if (currentrow<rowcount) {
		currentrow++;
		return true;
	}
	return false;
}

void sqlrcmdgstatcursor::getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null) {
	if (col==0) {
		*field=gs_resultset[currentrow-1].key;
	} else if (col==1) {
		*field=gs_resultset[currentrow-1].value;
	} else {
		*field=NULL;
	}
	*fieldlength=charstring::length(*field);
	*blob=false;
	*null=false;
}
