// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef CREATETABLEAUTOINCREMENTORACLE_H
#define CREATETABLEAUTOINCREMENTORACLE_H

#include <sqltrigger.h>

using namespace rudiments;

class createtableautoincrementoracle : public sqltrigger {
	public:
			createtableautoincrementoracle(xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree,
					bool before,
					bool success);
	private:
		bool	createSequenceAndTrigger(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *tablename,
						const char *columnname);
		bool	runQuery(sqlrconnection_svr *sqlrcon,
						const char *query,
						uint32_t length);
};

#endif
