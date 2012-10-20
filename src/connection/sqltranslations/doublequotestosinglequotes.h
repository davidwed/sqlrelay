// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef DOUBLE_QUOTES_TO_SINGLE_QUOTES_H
#define DOUBLE_QUOTES_TO_SINGLE_QUOTES_H

#include <sqltranslation.h>

class doublequotestosinglequotes : public sqltranslation {
	public:
			doublequotestosinglequotes(sqltranslations *sqlts,
					rudiments::xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldom *querytree);
	private:
		bool	replaceDoubleQuotes(rudiments::xmldomnode *node);
};

#endif
