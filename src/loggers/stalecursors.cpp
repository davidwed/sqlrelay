// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/process.h>
#include <rudiments/charstring.h>
#include <rudiments/datetime.h>
#include <rudiments/stringbuffer.h>

class SQLRSERVER_DLLSPEC sqlrlogger_stalecursors : public sqlrlogger {
	public:
			sqlrlogger_stalecursors(sqlrloggers *ls,
						domnode *parameters);
			~sqlrlogger_stalecursors();

		bool	init(sqlrlistener *sqlrl, sqlrserverconnection *sqlrcon);
		bool	run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrevent_t event,
					const char *info);
	private:
		const char	*host;
		uint16_t	port;
		const char	*socket;
		const char	*user;
		const char	*password;

		sqlrconnection	*sqlrclientcon;
		sqlrcursor	*insertsqlrcur;
		sqlrcursor	*updatesqlrcur;
		sqlrcursor	*deletesqlrcur;

		bool		enabled;

		pid_t		pid;

		datetime	dt;
		stringbuffer	dtstr;
};

sqlrlogger_stalecursors::sqlrlogger_stalecursors(sqlrloggers *ls,
						domnode *parameters) :
						sqlrlogger(ls,parameters) {

	host=parameters->getAttributeValue("host");
	port=charstring::toInteger(parameters->getAttributeValue("port"));
	socket=parameters->getAttributeValue("socket");
	user=parameters->getAttributeValue("user");
	password=parameters->getAttributeValue("password");

	enabled=!charstring::isNo(parameters->getAttributeValue("enabled"));

	sqlrclientcon=NULL;
	insertsqlrcur=NULL;
	updatesqlrcur=NULL;
	deletesqlrcur=NULL;
}

sqlrlogger_stalecursors::~sqlrlogger_stalecursors() {
	delete sqlrclientcon;
	delete insertsqlrcur;
	delete updatesqlrcur;
	delete deletesqlrcur;
}

bool sqlrlogger_stalecursors::init(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon) {

	if (!enabled) {
		return true;
	}

	// don't do anything for the listener
	if (!sqlrcon) {
		return true;
	}

	// get the pid
	pid=process::getProcessId();

	// create connection/cursors
	sqlrclientcon=new sqlrconnection(host,port,socket,user,password,0,1);
//sqlrclientcon->debugOn();
	insertsqlrcur=new sqlrcursor(sqlrclientcon);
	updatesqlrcur=new sqlrcursor(sqlrclientcon);
	deletesqlrcur=new sqlrcursor(sqlrclientcon);

	// create the table (ignore result in case it already exists)
	deletesqlrcur->sendQuery("create table stalecursors "
				"(instance varchar(256),"
				"connection_id varchar(256), "
				"connection_pid int, "
				"cursor_id int, "
				"most_recent_query varchar(256), "
				"most_recent_query_timestamp varchar(256))");
	deletesqlrcur->sendQuery("delete from stalecursors");

	// initial insert
	insertsqlrcur->prepareQuery("insert into "
				"	stalecursors "
				"values "
				"	(:instance,"
				"	:connection_id,"
				"	:connection_pid,"
				"	:cursor_id,"
				"	null,"
				"	null)");

	// update
	updatesqlrcur->prepareQuery("update "
				"	stalecursors "
				"set "
				"	most_recent_query="
				"		:most_recent_query,"
				"	most_recent_query_timestamp="
				"		:most_recent_query_timestamp "
				"where "
				"	instance=:instance "
				"	and "
				"	connection_id=:connection_id "
				"	and "
				"	connection_pid=:connection_pid "
				"	and "
				"	cursor_id=:cursor_id");

	// delete
	deletesqlrcur->prepareQuery("delete from "
				"	stalecursors "
				"where "
				"	instance=:instance "
				"	and "
				"	connection_id=:connection_id "
				"	and "
				"	connection_pid=:connection_pid "
				"	and "
				"	cursor_id=:cursor_id");

	return true;
}

bool sqlrlogger_stalecursors::run(sqlrlistener *sqlrl,
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					sqlrlogger_loglevel_t level,
					sqlrevent_t event,
					const char *info) {

	if (!enabled) {
		return true;
	}

	// don't do anything for the listener
	if (!sqlrcon) {
		return true;
	}

	if (event==SQLREVENT_CURSOR_OPEN) {

		sqlrclientcon->begin();

		// insert row
		insertsqlrcur->inputBind("instance",
					sqlrcon->cont->getId());
		insertsqlrcur->inputBind("connection_id",
					sqlrcon->cont->getConnectionId());
		insertsqlrcur->inputBind("connection_pid",pid);
		insertsqlrcur->inputBind("cursor_id",sqlrcur->getId());
		insertsqlrcur->executeQuery();

		sqlrclientcon->commit();

	} else if (event==SQLREVENT_QUERY) {

		sqlrclientcon->begin();

		// build the date string
		dt.getSystemDateAndTime();
		dtstr.clear();
		dtstr.writeFormatted("%04d-%02d-%02d %02d:%02d:%02d",
				dt.getYear(),dt.getMonth(),dt.getDayOfMonth(),
				dt.getHour(),dt.getMinutes(),dt.getSeconds());

		// update row
		updatesqlrcur->inputBind("most_recent_query",
					sqlrcur->getQueryBuffer());
		updatesqlrcur->inputBind("most_recent_query_timestamp",
					dtstr.getString());
		updatesqlrcur->inputBind("instance",
					sqlrcon->cont->getId());
		updatesqlrcur->inputBind("connection_id",
					sqlrcon->cont->getConnectionId());
		updatesqlrcur->inputBind("connection_pid",pid);
		updatesqlrcur->inputBind("cursor_id",sqlrcur->getId());
		updatesqlrcur->executeQuery();

		sqlrclientcon->commit();

	} else if (event==SQLREVENT_CURSOR_CLOSE) {

		sqlrclientcon->begin();

		// delete row
		deletesqlrcur->inputBind("instance",
					sqlrcon->cont->getId());
		deletesqlrcur->inputBind("connection_id",
					sqlrcon->cont->getConnectionId());
		deletesqlrcur->inputBind("connection_pid",pid);
		deletesqlrcur->inputBind("cursor_id",sqlrcur->getId());
		deletesqlrcur->executeQuery();

		sqlrclientcon->commit();
	}

	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrlogger *new_sqlrlogger_stalecursors(
						sqlrloggers *ls,
						domnode *parameters) {
		return new sqlrlogger_stalecursors(ls,parameters);
	}
}
