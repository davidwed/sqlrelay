// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef MSQLCONNECTION_H
#define MSQLCONNECTION_H

#define NUM_CONNECT_STRING_VARS 2

#include <sqlrconnection.h>

#include <msql.h>

class msqlconnection;

class msqlcursor : public sqlrcursor {
	friend class msqlconnection;
	private:
			msqlcursor(sqlrconnection *conn);
		int	executeQuery(const char *query, long length,
					unsigned short execute);
		char	*getErrorMessage(int *liveconnection);
		void	returnRowCounts();
		void	returnColumnCount();
		void	returnColumnInfo();
		int	noRowsToReturn();
		int	skipRow();
		int	fetchRow();
		void	returnRow();
		void	cleanUpData(bool freerows, bool freecols,
							bool freebinds);

		m_result	*msqlresult;
		m_field		*msqlfield;
		m_row		msqlrow;
		int		ncols;
		int		nrows;
		int		affectedrows;

		msqlconnection	*msqlconn;
};

class msqlconnection : public sqlrconnection {
	friend class msqlcursor;
	public:
			msqlconnection();
	private:
		int	getNumberOfConnectStringVars();
		void	handleConnectString();
		int	logIn();
		sqlrcursor	*initCursor();
		void	deleteCursor(sqlrcursor *curs);
		void	logOut();
		int	isTransactional();
		int	ping();
		char	*identify();
		unsigned short	autoCommitOn();
		unsigned short	autoCommitOff();
		int	commit();
		int	rollback();

		int	msql;

		char	*host;
		char	*db;

		int	devnull;
};

#endif
