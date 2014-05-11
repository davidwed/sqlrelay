// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRTRANSLATION_H
#define SQLRTRANSLATION_H

#include <sqlrelay/sqlrserverdll.h>

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

class sqlrconnection_svr;
class sqlrcursor_svr;
class sqlrtranslations;

class SQLRSERVER_DLLSPEC sqlrtranslation {
	public:
			sqlrtranslation(sqlrtranslations *sqlts,
					xmldomnode *parameters);
		virtual	~sqlrtranslation();

		virtual bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	protected:
		sqlrtranslations	*sqlts;
		xmldomnode		*parameters;
};

#endif
