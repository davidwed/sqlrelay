// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRQUERY_H
#define SQLRQUERY_H

#include <sqlrelay/private/sqlrserverdll.h>

#include <sqlrelay/sqlrservercursor.h>
#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

class sqlrquerycursor;

class SQLRSERVER_DLLSPEC sqlrquery {
	public:
			sqlrquery(xmldomnode *parameters);
		virtual	~sqlrquery();

		virtual bool	match(const char *querystring,
						uint32_t querylength);
		virtual sqlrquerycursor	*newCursor(	
						sqlrserverconnection *sqlrcon,
						uint16_t id);
	protected:
		xmldomnode	*parameters;
};

class SQLRSERVER_DLLSPEC sqlrquerycursor : public sqlrservercursor {
	public:
			sqlrquerycursor(sqlrserverconnection *conn,
					xmldomnode *parameters,
					uint16_t id);
		virtual	~sqlrquerycursor();
		virtual sqlrquerytype_t	queryType(const char *query,
							uint32_t length);
		bool	isCustomQuery();
	protected:
		xmldomnode	*parameters;
};

#endif
