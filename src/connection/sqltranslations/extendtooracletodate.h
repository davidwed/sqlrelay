// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef EXTENDTOORACLETODATE_H
#define EXTENDTOORACLETODATE_H

#include <sqltranslation.h>

using namespace rudiments;

class extendtooracletodate : public sqltranslation {
	public:
			extendtooracletodate(sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		bool	translateExtends(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *querynode);
		bool	translateExtend(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *querynode);
};

#endif
