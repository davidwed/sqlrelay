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
		bool	executeQuery(const char *query,
					long length,
					bool execute);
		char	*getErrorMessage(bool *liveconnection);
		void	returnRowCounts();
		void	returnColumnCount();
		void	returnColumnInfo();
		bool	noRowsToReturn();
		bool	skipRow();
		bool	fetchRow();
		void	returnRow();
		void	cleanUpData(bool freeresult, bool freebinds);

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
		bool	logIn();
		sqlrcursor	*initCursor();
		void	deleteCursor(sqlrcursor *curs);
		void	logOut();
		bool	isTransactional();
		bool	ping();
		char	*identify();
		bool	autoCommitOn();
		bool	autoCommitOff();
		bool	commit();
		bool	rollback();

		int	msql;

		char	*host;
		char	*db;

		int	devnull;
};

#endif
