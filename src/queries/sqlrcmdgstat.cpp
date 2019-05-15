// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/sys.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>
#include <datatypes.h>
#include <config.h>

// for time_t, time(), localtime()
#include <time.h>

class SQLRSERVER_DLLSPEC sqlrquery_sqlrcmdgstat : public sqlrquery {
	public:
			sqlrquery_sqlrcmdgstat(sqlrservercontroller *cont,
							sqlrqueries *qs,
							domnode *parameters);
		bool	match(const char *querystring, uint32_t querylength);
		sqlrquerycursor	*newCursor(sqlrserverconnection *conn,
							uint16_t id);
};

#define GSTAT_KEY_LEN		40
#define GSTAT_VALUE_LEN		40
#define GSTAT_ROW_COUNT_MAX	60

struct gs_result_row {
	char	key[GSTAT_KEY_LEN+1];
	char	value[GSTAT_VALUE_LEN+1];
};

class sqlrquery_sqlrcmdgstatcursor : public sqlrquerycursor {
	public:
			sqlrquery_sqlrcmdgstatcursor(
						sqlrserverconnection *sqlrcon,
						sqlrquery *q,
						domnode *parameters,
						uint16_t id);

		bool		executeQuery(const char *query,
						uint32_t length);
		uint32_t	colCount();
		const char	*getColumnName(uint32_t col);
		uint16_t	getColumnType(uint32_t col);
		uint32_t	getColumnLength(uint32_t col);
		uint32_t	getColumnPrecision(uint32_t col);
		uint32_t	getColumnScale(uint32_t col);
		bool		noRowsToReturn();
		bool		fetchRow(bool *error);
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob, bool *null);
	private:
		void	setGSResult(const char *key,
					int32_t value, uint16_t i);
		void	setGSResult(const char *key,
					const char *value, uint16_t i);

		uint64_t	rowcount;
		uint64_t	currentrow;

		gs_result_row	gs_resultset[GSTAT_ROW_COUNT_MAX];

};

sqlrquery_sqlrcmdgstat::sqlrquery_sqlrcmdgstat(sqlrservercontroller *cont,
						sqlrqueries *qs,
						domnode *parameters) :
						sqlrquery(cont,qs,parameters) {
	debugFunction();
}

bool sqlrquery_sqlrcmdgstat::match(const char *querystring,
					uint32_t querylength) {
	debugFunction();
	return !charstring::compareIgnoringCase(querystring,"sqlrcmd gstat");
}

sqlrquerycursor *sqlrquery_sqlrcmdgstat::newCursor(
						sqlrserverconnection *sqlrcon,
						uint16_t id) {
	return new sqlrquery_sqlrcmdgstatcursor(sqlrcon,this,
						getParameters(),id);
}

sqlrquery_sqlrcmdgstatcursor::sqlrquery_sqlrcmdgstatcursor(
					sqlrserverconnection *sqlrcon,
					sqlrquery *q,
					domnode *parameters,
					uint16_t id) :
				sqlrquerycursor(sqlrcon,q,parameters,id) {
	rowcount=0;
	currentrow=0;
}

bool sqlrquery_sqlrcmdgstatcursor::executeQuery(const char *query,
							uint32_t length) {

	sqlrshm	*gs=conn->cont->getShm();

	time_t	now=time(NULL);	

	uint32_t	connectedclients=gs->connectedclients;
	if (now/60>gs->peak_connectedclients_1min_time/60) {
		gs->peak_connectedclients_1min_time=now;
		gs->peak_connectedclients_1min=connectedclients;
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
			etc_1+=gs->qps_create[j]+
				gs->qps_drop[j]+
				gs->qps_alter[j]+
				gs->qps_etc[j];
			sqlrcmd_1+=gs->qps_custom[j];
		}
		if (now-gs->timestamp[j]<60*5) {
			select_5+=gs->qps_select[j];
			insert_5+=gs->qps_insert[j];
			update_5+=gs->qps_update[j];
			delete_5+=gs->qps_delete[j];
			etc_5+=gs->qps_create[j]+
				gs->qps_drop[j]+
				gs->qps_alter[j]+
				gs->qps_etc[j];
			sqlrcmd_5+=gs->qps_custom[j];
		}
		if (now-gs->timestamp[j]<60*15) {
			select_15+= gs->qps_select[j];
			insert_15+= gs->qps_insert[j];
			update_15+= gs->qps_update[j];
			delete_15+= gs->qps_delete[j];
			etc_15+=gs->qps_create[j]+
				gs->qps_drop[j]+
				gs->qps_alter[j]+
				gs->qps_etc[j];
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
	setGSResult("busy_listener",gs->forked_listeners,rowcount++);
	setGSResult("peak_listener",gs->peak_listeners,rowcount++);
	setGSResult("connection",gs->totalconnections,rowcount++);
	setGSResult("session",connectedclients,rowcount++);
	setGSResult("peak_session",gs->peak_connectedclients,rowcount++);
	setGSResult("peak_session_1min",
			gs->peak_connectedclients_1min,rowcount++);
	strftime(tmpbuf,GSTAT_VALUE_LEN,"%Y/%m/%d %H:%M:%S",
			localtime(&(gs->peak_connectedclients_1min_time)));
	setGSResult("peak_session_1min_time",tmpbuf,rowcount++);
	setGSResult("sqlr_version",SQLR_VERSION,rowcount++);
	setGSResult("rudiments_version",sys::getRudimentsVersion(),rowcount++);
#if defined(__DATE__) && defined(__TIME__)
	setGSResult("module_compiled", __DATE__ " " __TIME__, rowcount++);
#endif

	currentrow=0;
	return true;
}

void sqlrquery_sqlrcmdgstatcursor::setGSResult(const char *key,
						int32_t value, uint16_t i) {
	if (i>=GSTAT_ROW_COUNT_MAX) {
		return;
	}
	charstring::copy(gs_resultset[i].key,key,GSTAT_KEY_LEN);
	gs_resultset[i].key[GSTAT_KEY_LEN]='\0';
	charstring::printf(gs_resultset[i].value,GSTAT_VALUE_LEN,"%d",value);
	gs_resultset[i].value[GSTAT_VALUE_LEN]='\0';
}

void sqlrquery_sqlrcmdgstatcursor::setGSResult(const char *key,
						const char *value, uint16_t i) {
	if (i>=GSTAT_ROW_COUNT_MAX) {
		return;
	}
	charstring::copy(gs_resultset[i].key,key,GSTAT_KEY_LEN);
	gs_resultset[i].key[GSTAT_KEY_LEN]='\0';
	charstring::copy(gs_resultset[i].value,value,GSTAT_VALUE_LEN);
	gs_resultset[i].value[GSTAT_VALUE_LEN]='\0';
}

uint32_t sqlrquery_sqlrcmdgstatcursor::colCount() {
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

const char *sqlrquery_sqlrcmdgstatcursor::getColumnName(uint32_t col) {
	return (col<2)?colinfo[col].name:NULL;
}

uint16_t sqlrquery_sqlrcmdgstatcursor::getColumnType(uint32_t col) {
	return (col<2)?colinfo[col].type:0;
}

uint32_t sqlrquery_sqlrcmdgstatcursor::getColumnLength(uint32_t col) {
	return (col<2)?colinfo[col].length:0;
}

uint32_t sqlrquery_sqlrcmdgstatcursor::getColumnPrecision(uint32_t col) {
	return (col<2)?colinfo[col].precision:0;
}

uint32_t sqlrquery_sqlrcmdgstatcursor::getColumnScale(uint32_t col) {
	return (col<2)?colinfo[col].scale:0;
}

bool sqlrquery_sqlrcmdgstatcursor::noRowsToReturn() {
	return false;
}

bool sqlrquery_sqlrcmdgstatcursor::fetchRow(bool *error) {
	*error=false;
	if (currentrow<rowcount) {
		currentrow++;
		return true;
	}
	return false;
}

void sqlrquery_sqlrcmdgstatcursor::getField(uint32_t col,
						const char **field,
						uint64_t *fieldlength,
						bool *blob,
						bool *null) {
	if ((currentrow-1)>=GSTAT_ROW_COUNT_MAX) {
		*field=NULL;
		*fieldlength=0;
		*blob=false;
		*null=true;
		return;
	}
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

extern "C" {
	SQLRSERVER_DLLSPEC sqlrquery *new_sqlrquery_sqlrcmdgstat(
						sqlrservercontroller *cont,
						sqlrqueries *qs,
						domnode *parameters) {
		return new sqlrquery_sqlrcmdgstat(cont,qs,parameters);
	}
}
