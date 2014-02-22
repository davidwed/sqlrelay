// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRQUERIES_H
#define SQLRQUERIES_H

#include <sqlrserverdll.h>

#include <rudiments/xmldom.h>
#include <rudiments/linkedlist.h>
#include <rudiments/dynamiclib.h>
#include <sqlrquery.h>

class sqlrconnection_svr;
class sqlrcursor_svr;

class SQLRSERVER_DLLSPEC sqlrqueryplugin {
	public:
		sqlrquery	*qr;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrqueries {
	public:
			sqlrqueries();
			~sqlrqueries();

		bool		loadQueries(const char *queries);
		sqlrquerycursor	*match(sqlrconnection_svr *sqlrcon,
						const char *querystring,
						uint32_t querylength);
	private:
		void		unloadQueries();
		void		loadQuery(xmldomnode *logger);

		xmldom				*xmld;
		linkedlist< sqlrqueryplugin * >	llist;
};

#endif
