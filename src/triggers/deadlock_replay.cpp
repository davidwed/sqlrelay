// Copyright (c) 1999-2018  David Muse
// All rights reserved

#include <sqlrelay/sqlrserver.h>

class SQLRSERVER_DLLSPEC sqlrtrigger_deadlock_replay : public sqlrtrigger {
	public:
			sqlrtrigger_deadlock_replay(
					sqlrservercontroller *cont,
					sqlrtriggers *ts,
					domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					bool before,
					bool success);

	private:
		bool	logQuery(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);
		bool	replayLog(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);
};

sqlrtrigger_deadlock_replay::sqlrtrigger_deadlock_replay(
					sqlrservercontroller *cont,
					sqlrtriggers *ts,
					domnode *parameters) :
					sqlrtrigger(cont,ts,parameters) {
}

bool sqlrtrigger_deadlock_replay::run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						bool before,
						bool success) {
	return (before)?logQuery(sqlrcon,sqlrcur):replayLog(sqlrcon,sqlrcur);
}

bool sqlrtrigger_deadlock_replay::logQuery(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur) {

	// if we're not currently replaying the log...
	// then log the query...
	return true;
}

bool sqlrtrigger_deadlock_replay::replayLog(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur) {

	// if we got a deadlock...
	// then replay the log...
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC
	sqlrtrigger	*new_sqlrtrigger_deadlock_replay(
						sqlrservercontroller *cont,
						sqlrtriggers *ts,
						domnode *parameters) {

		return new sqlrtrigger_deadlock_replay(cont,ts,parameters);
	}
}
