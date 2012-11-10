// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLTRANSLATION_H
#define SQLTRANSLATION_H

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

class sqlrconnection_svr;
class sqlrcursor_svr;
class sqltranslations;

class sqltranslation {
	public:
			sqltranslation(sqltranslations *sqlts,
					rudiments::xmldomnode *parameters);
		virtual	~sqltranslation();

		virtual bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldom *querytree);
	protected:
		sqltranslations		*sqlts;
		rudiments::xmldomnode	*parameters;
};

#endif
