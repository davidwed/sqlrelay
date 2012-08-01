// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef LOCKSNOWAITBYDEFAULT_H
#define LOCKSNOWAITBYDEFAULT_H

#include <sqltranslation.h>

using namespace rudiments;

class matchestolike : public sqltranslation {
	public:
			matchestolike(sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		bool	replaceMatchesWithLike(xmldomnode *node);
		void	wrap(xmldomnode *node);
};

#endif
