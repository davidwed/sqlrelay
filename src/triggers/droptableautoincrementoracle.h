// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef DROPTABLEAUTOINCREMENTORACLE_H
#define DROPTABLEAUTOINCREMENTORACLE_H

#include <sqltrigger.h>

using namespace rudiments;

class droptableautoincrementoracle : public sqltrigger {
	public:
			droptableautoincrementoracle(xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree,
					bool before,
					bool success);
	private:
		bool	dropSequences(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *tablename);
		bool	dropSequence(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *sequencename);
		bool	deleteSequence(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *sequencename);
};

#endif
