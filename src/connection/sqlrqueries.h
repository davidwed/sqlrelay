// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRQUERIES_H
#define SQLRQUERIES_H

#include <rudiments/xmldom.h>
#include <rudiments/linkedlist.h>
#include <rudiments/dynamiclib.h>
#include <sqlrquery.h>

class sqlrconnection_svr;
class sqlrcursor_svr;

class sqlrqueryplugin {
	public:
		sqlrquery		*qr;
		rudiments::dynamiclib	*dl;
};

class sqlrqueries {
	public:
			sqlrqueries();
			~sqlrqueries();

		bool	loadQueries(const char *queries);
		void	initQueries(sqlrconnection_svr *sqlrcon);
		sqlrquery	*match(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *querystring);
	private:
		void		unloadQueries();
		void		loadQuery(rudiments::xmldomnode *logger);

		rudiments::xmldom				*xmld;
		rudiments::linkedlist< sqlrqueryplugin * >	llist;
};

#endif
