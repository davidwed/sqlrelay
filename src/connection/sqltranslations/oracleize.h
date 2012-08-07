// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef ORACLEIZE_H
#define ORACLEIZE_H

#include <sqltranslation.h>

using namespace rudiments;

class oracleize : public sqltranslation {
	public:
			oracleize(sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
};

#endif
