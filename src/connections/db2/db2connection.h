// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef DB2CCONNECTION_H
#define DB2CCONNECTION_H

#define FETCH_AT_ONCE		10
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	4096

#define NUM_CONNECT_STRING_VARS 5

#include <sqlrconnection.h>

#include <sqlcli1.h>

struct column {
	char	name[MAX_ITEM_BUFFER_SIZE];
	int	namelength;
	int	type;
	int	length;
	int	precision;
	int	scale;
};

class db2connection;

class db2cursor : public sqlrcursor {
	friend class db2connection;
	public:
			db2cursor(sqlrconnection *conn);
			~db2cursor();
	private:
		int	prepareQuery(const char *query, long length);
		int	inputBindString(const char *variable, 
					unsigned short variablesize,
					const char *value, 
					unsigned short valuesize,
					short *isnull);
		int	inputBindLong(const char *variable, 
					unsigned short variablesize,
					unsigned long *value);
		int	inputBindDouble(const char *variable, 
					unsigned short variablesize,
					double *value, 
					unsigned short precision,
					unsigned short scale);
		int	outputBindString(const char *variable, 
					unsigned short variablesize,
					char *value, 
					unsigned short valuesize,
					short *isnull);
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

		long		erg;
		SQLHSTMT	stmt;
		long		result;
		SQLSMALLINT	ncols;
		SQLINTEGER 	affectedrows;
		char		field[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE]
					[MAX_ITEM_BUFFER_SIZE];
		SQLINTEGER	indicator[MAX_SELECT_LIST_SIZE]
						[FETCH_AT_ONCE];
#if (DB2VERSION==8)
		SQLUSMALLINT	rowstat[FETCH_AT_ONCE];
#endif
		column 		col[MAX_SELECT_LIST_SIZE];

		int		row;
		int		maxrow;
		int		totalrows;
		int		rownumber;

		stringbuffer	*errormsg;

		db2connection	*db2conn;
};

class db2connection : public sqlrconnection {
	friend class db2cursor;
	private:
		int	getNumberOfConnectStringVars();
		void	handleConnectString();
		int	logIn();
		sqlrcursor	*initCursor();
		void	deleteCursor(sqlrcursor *curs);
		void	logOut();
		short	nullBindValue();
		int	bindValueIsNull(short isnull);
		unsigned short	autoCommitOn();
		unsigned short	autoCommitOff();
		int	commit();
		int	rollback();
		char	*pingQuery();
		char	*identify();

		SQLHENV		env;
		long		erg;
		SQLHDBC		dbc;

		char		*server;
		int		autocommit;
};

#endif
