// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef ORACLETEMPTABLESPRESERVEROWSBYDEFAULT_H
#define ORACLETEMPTABLESPRESERVEROWSBYDEFAULT_H

#include <sqltranslation.h>

class oracletemptablespreserverowsbydefault : public sqltranslation {
	public:
			oracletemptablespreserverowsbydefault(
					sqltranslations *sqlts,
					rudiments::xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldom *querytree);
};

#endif
