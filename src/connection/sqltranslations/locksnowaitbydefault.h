// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef LOCKSNOWAITBYDEFAULT_H
#define LOCKSNOWAITBYDEFAULT_H

#include <sqltranslation.h>

using namespace rudiments;

class locksnowaitbydefault : public sqltranslation {
	public:
			locksnowaitbydefault(sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
};

#endif
