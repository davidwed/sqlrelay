// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef ORACLEIZE_H
#define ORACLEIZE_H

#include <sqltranslation.h>

class oracleize : public sqltranslation {
	public:
			oracleize(sqltranslations *sqlts,
					rudiments::xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldom *querytree);
};

#endif
