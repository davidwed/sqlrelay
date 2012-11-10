// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRLOGGER_H
#define SQLRLOGGER_H

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

class sqlrconnection_svr;
class sqlrcursor_svr;

class sqlrlogger {
	public:
			sqlrlogger(rudiments::xmldomnode *parameters);
		virtual	~sqlrlogger();

		virtual bool	init(sqlrconnection_svr *sqlrcon);
		virtual bool	run(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur);
	protected:
		rudiments::xmldomnode	*parameters;
};

#endif
