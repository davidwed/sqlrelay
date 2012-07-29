// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLTRIGGER_H
#define SQLTRIGGER_H

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>
#include <rudiments/dynamiclib.h>

using namespace rudiments;

class sqlrconnection_svr;
class sqlrcursor_svr;

class sqltrigger {
	public:
			sqltrigger(xmldomnode *parameters);
		virtual	~sqltrigger();

		void	attachModule(dynamiclib *dl);

		virtual bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree,
					bool before,
					bool success);
	protected:
		xmldomnode	*parameters;
		dynamiclib	*dl;
};

#endif
