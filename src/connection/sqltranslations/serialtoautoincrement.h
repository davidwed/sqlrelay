// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SERIALTOAUTOINCREMENT_H
#define SERIALTOAUTOINCREMENT_H

#include <sqltranslation.h>

using namespace rudiments;

class serialtoautoincrement : public sqltranslation {
	public:
			serialtoautoincrement(sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
};

#endif
