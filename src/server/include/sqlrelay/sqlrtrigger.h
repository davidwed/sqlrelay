// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRTRIGGER_H
#define SQLRTRIGGER_H

#include <sqlrelay/private/sqlrserverdll.h>

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

class sqlrserverconnection;
class sqlrservercursor;

class SQLRSERVER_DLLSPEC sqlrtrigger {
	public:
			sqlrtrigger(xmldomnode *parameters, bool debug);
		virtual	~sqlrtrigger();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree,
					bool before,
					bool success);
	protected:
		xmldomnode	*parameters;
		bool		debug;
};

#endif
