// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRTRANSLATION_H
#define SQLRTRANSLATION_H

#include <sqlrelay/private/sqlrserverdll.h>

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

class sqlrserverconnection;
class sqlrservercursor;
class sqlrtranslations;

class SQLRSERVER_DLLSPEC sqlrtranslation {
	public:
			sqlrtranslation(sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug);
		virtual	~sqlrtranslation();

		virtual bool	usesTree();

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query,
					stringbuffer *translatedquery);

		virtual bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
	protected:
		sqlrtranslations	*sqlts;
		xmldomnode		*parameters;
		bool			debug;
};

#endif
