// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRQUERY_H
#define SQLRQUERY_H

#include <sqlrelay/private/sqlrserverdll.h>

#include <sqlrelay/sqlrcursor.h>
#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

// for return values of columnTypeFormat
#include <datatypes.h>

class sqlrquerycursor;

class SQLRSERVER_DLLSPEC sqlrquery {
	public:
			sqlrquery(xmldomnode *parameters);
		virtual	~sqlrquery();

		virtual bool	match(const char *querystring,
						uint32_t querylength);
		virtual sqlrquerycursor	*getCursor(sqlrconnection_svr *sqlrcon);
	protected:
		xmldomnode	*parameters;
};

class SQLRSERVER_DLLSPEC sqlrquerycursor : public sqlrcursor_svr {
	public:
			sqlrquerycursor(sqlrconnection_svr *conn,
					xmldomnode *parameters);
		virtual	~sqlrquerycursor();
		virtual sqlrquerytype_t	queryType(const char *query,
							uint32_t length);
	protected:
		xmldomnode	*parameters;
};

#endif
