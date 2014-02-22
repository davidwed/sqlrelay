// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLTRANSLATION_H
#define SQLTRANSLATION_H

#include <sqlrserverdll.h>

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

class sqlrconnection_svr;
class sqlrcursor_svr;
class sqltranslations;

class SQLRSERVER_DLLSPEC sqltranslation {
	public:
			sqltranslation(sqltranslations *sqlts,
					xmldomnode *parameters);
		virtual	~sqltranslation();

		virtual bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	protected:
		sqltranslations		*sqlts;
		xmldomnode		*parameters;
};

#endif
