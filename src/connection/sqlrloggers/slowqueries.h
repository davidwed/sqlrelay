// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SLOWQUERIES_H
#define SLOWQUERIES_H

#include <sqlrlogger.h>
#include <rudiments/file.h>

class slowqueries : public sqlrlogger {
	public:
			slowqueries(rudiments::xmldomnode *parameters);
			~slowqueries();

		bool	init(sqlrconnection_svr *sqlrcon);
		bool	run(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur);

	private:
		char		*querylogname;
		rudiments::file	querylog;
		uint64_t	sec;
		uint64_t	usec;
};

#endif
