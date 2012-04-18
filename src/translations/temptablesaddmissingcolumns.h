// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef TEMPTABLESADDMISSINGCOLUMNS_H
#define TEMPTABLESADDMISSINGCOLUMNS_H

#include <sqltranslation.h>

using namespace rudiments;

class temptablesaddmissingcolumns : public sqltranslation {
	public:
			temptablesaddmissingcolumns(
						sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
};

#endif
