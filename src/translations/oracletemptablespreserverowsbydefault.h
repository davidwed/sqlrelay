// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef ORACLETEMPTABLESPRESERVEROWSBYDEFAULT_H
#define ORACLETEMPTABLESPRESERVEROWSBYDEFAULT_H

#include <sqltranslation.h>

using namespace rudiments;

class oracletemptablespreserverowsbydefault : public sqltranslation {
	public:
			oracletemptablespreserverowsbydefault(
						sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
};

#endif
