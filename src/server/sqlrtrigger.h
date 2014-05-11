// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRTRIGGER_H
#define SQLRTRIGGER_H

#include <sqlrserverdll.h>

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

class sqlrconnection_svr;
class sqlrcursor_svr;

class SQLRSERVER_DLLSPEC sqlrtrigger {
	public:
			sqlrtrigger(xmldomnode *parameters);
		virtual	~sqlrtrigger();

		virtual bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree,
					bool before,
					bool success);
	protected:
		xmldomnode	*parameters;
};

#endif
