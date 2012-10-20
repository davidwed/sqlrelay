// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef LOCKSNOWAITBYDEFAULT_H
#define LOCKSNOWAITBYDEFAULT_H

#include <sqltranslation.h>

class matchestolike : public sqltranslation {
	public:
			matchestolike(sqltranslations *sqlts,
					rudiments::xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldom *querytree);
	private:
		bool	replaceMatchesWithLike(rudiments::xmldomnode *node);
		void	wrap(rudiments::xmldomnode *node);
};

#endif
