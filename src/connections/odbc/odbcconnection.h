// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef ODBCCCONNECTION_H
#define ODBCCCONNECTION_H

#define FETCH_AT_ONCE		10
#define MAX_SELECT_LIST_SIZE	256
#define MAX_ITEM_BUFFER_SIZE	4096

#define NUM_CONNECT_STRING_VARS 3

#ifdef HAVE_IODBC
	#include <iodbcinst.h>
#endif
#ifdef __CYGWIN__
	#include <windows.h>
	#include <w32api/sql.h>
	#include <w32api/sqlext.h>
	#include <w32api/sqltypes.h>
#else
	#include <sql.h>
	#include <sqlext.h>
	#include <sqltypes.h>
#endif

// note that sqlrconnection.h must be included after sqltypes.h to
// get around a problem with CHAR/xmlChar in gnome-xml
#include <sqlrconnection.h>

#include <config.h>

struct column {
	char	name[MAX_ITEM_BUFFER_SIZE];
	int	namelength;
	int	type;
	int	length;
	int	precision;
	int	scale;
	int	nullable;
	int	primarykey;
	int	unique;
	int	partofkey;
	int	unsignednumber;
	int	zerofill;
	int	binary;
	int	autoincrement;
};

class odbcconnection;

class odbccursor : public sqlrcursor {
	friend class odbcconnection;
	private:
			odbccursor(sqlrconnection *conn);
			~odbccursor();
		bool	prepareQuery(const char *query, long length);
		bool	inputBindString(const char *variable, 
					unsigned short variablesize,
					const char *value, 
					unsigned short valuesize,
					short *isnull);
		bool	inputBindLong(const char *variable, 
					unsigned short variablesize,
					unsigned long *value);
		bool	inputBindDouble(const char *variable, 
					unsigned short variablesize,
					double *value, 
					unsigned short precision,
					unsigned short scale);
		bool	outputBindString(const char *variable, 
					unsigned short variablesize,
					const char *value, 
					unsigned short valuesize,
					short *isnull);
		short	nonNullBindValue();
		short	nullBindValue();
		bool	bindValueIsNull(short isnull);
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


		long		erg;
		SQLHSTMT	stmt;
		long		result;
		SQLSMALLINT	ncols;
		SQLINTEGER 	affectedrows;

// this code is here in case unixodbc ever 
// successfully supports array fetches

/*#ifdef HAVE_UNIXODBC
		char		field[MAX_SELECT_LIST_SIZE]
					[FETCH_AT_ONCE]
					[MAX_ITEM_BUFFER_SIZE];
		SQLINTEGER	indicator[MAX_SELECT_LIST_SIZE]
						[FETCH_AT_ONCE];
#else*/
		char		field[MAX_SELECT_LIST_SIZE]
					[MAX_ITEM_BUFFER_SIZE];
		SQLINTEGER	indicator[MAX_SELECT_LIST_SIZE];
//#endif
		column 		col[MAX_SELECT_LIST_SIZE];

		int		row;
		int		maxrow;
		int		totalrows;
		int		rownumber;

		stringbuffer	*errormsg;

		odbcconnection	*odbcconn;
};

class odbcconnection : public sqlrconnection {
	friend class odbccursor;
	private:
		int	getNumberOfConnectStringVars();
		void	handleConnectString();
		bool	logIn();
		sqlrcursor	*initCursor();
		void	deleteCursor(sqlrcursor *curs);
		void	logOut();
#if (ODBCVER>=0x0300)
		bool	autoCommitOn();
		bool	autoCommitOff();
		bool	commit();
		bool	rollback();
#endif
		bool	ping();
		char	*identify();

		long	erg;
		SQLHENV	env;
		SQLHDBC	dbc;

		char	*dsn;
};

#endif
