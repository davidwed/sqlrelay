// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef DROPTABLEAUTOINCREMENTORACLE_H
#define DROPTABLEAUTOINCREMENTORACLE_H

#include <sqltrigger.h>

class droptableautoincrementoracle : public sqltrigger {
	public:
			droptableautoincrementoracle(
					rudiments::xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldom *querytree,
					bool before,
					bool success);
	private:
		bool	dropSequences(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *database,
						const char *schema,
						const char *tablename);
		bool	dropSequence(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *sequencename);
		bool	deleteSequence(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *sequencename);
};

#endif
