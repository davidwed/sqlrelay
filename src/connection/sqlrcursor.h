// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#ifndef SQLRCURSOR_H
#define SQLRCURSOR_H

#include <defines.h>

enum bindtype {
	NULL_BIND,
	STRING_BIND,
	LONG_BIND,
	DOUBLE_BIND,
	BLOB_BIND,
	CLOB_BIND,
	CURSOR_BIND
};

class bindvar {
	friend class sqlrconnection;
	friend class sqlrcursor;
	private:
		char	*variable;
		short	variablesize;
		union {
			char	*stringval;
			long	longval;
			struct	{
				double		value;
				unsigned short	precision;
				unsigned short	scale;
			} doubleval;
			unsigned short	cursorid;
		} value;
		unsigned long	valuesize;
		bindtype	type;
		short		isnull;
};

class sqlrconnection;

class sqlrcursor {
	friend class sqlrconnection;
	friend class cursormanager;
	public:
			sqlrcursor(sqlrconnection *conn);
		virtual	~sqlrcursor();

	protected:
		// interface definition
		virtual	int	openCursor(int id);
		virtual	int	closeCursor();
		virtual	int	prepareQuery(const char *query, long length);
		virtual	int	inputBindString(const char *variable, 
						unsigned short variablesize,
						const char *value, 
						unsigned short valuesize,
						short *isnull);
		virtual	int	inputBindLong(const char *variable, 
						unsigned short variablesize,
						unsigned long *value);
		virtual	int	inputBindDouble(const char *variable, 
						unsigned short variablesize,
						double *value,
						unsigned short precision,
						unsigned short scale);
		virtual	int	inputBindBlob(const char *variable, 
						unsigned short variablesize,
						const char *value, 
						unsigned long valuesize,
						short *isnull);
		virtual	int	inputBindClob(const char *variable, 
						unsigned short variablesize,
						const char *value, 
						unsigned long valuesize,
						short *isnull);
		virtual	int	outputBindString(const char *variable, 
						unsigned short variablesize,
						char *value,
						unsigned short valuesize,
						short *isnull);
		virtual	int	outputBindBlob(const char *variable, 
						unsigned short variablesize,
						int index,
						short *isnull);
		virtual	int	outputBindClob(const char *variable, 
						unsigned short variablesize,
						int index,
						short *isnull);
		virtual	int	outputBindCursor(const char *variable,
						unsigned short variablesize,
						sqlrcursor *cursor);
		virtual	void	returnOutputBindBlob(int index);
		virtual	void	returnOutputBindClob(int index);
		virtual	void	returnOutputBindCursor(int index);
		virtual void	checkForTempTable(const char *query,
							unsigned long length);
		virtual	int	executeQuery(const char *query, long length,
						unsigned short execute)=0;
		virtual	int	queryIsNotSelect();
		virtual	int	queryIsCommitOrRollback();
		virtual	char	*getErrorMessage(int *liveconnection)=0;
		virtual	void	returnRowCounts()=0;
		virtual	void	returnColumnCount()=0;
		virtual	void	returnColumnInfo()=0;
		virtual	int	noRowsToReturn()=0;
		virtual	int	skipRow()=0;
		virtual	int	fetchRow()=0;
		virtual	void	returnRow()=0;
		virtual	void	cleanUpData();

	public:
		// methods/variables used by derived classes
		stringbuffer	*fakeInputBinds(const char *query);

		int	skipComment(char **ptr, const char *endptr);
		int	skipWhitespace(char **ptr, const char *endptr);
		int	advance(char **ptr, const char *endptr,
						unsigned short steps);

		sqlrconnection	*conn;

	private:
		// methods used internally
		int	handleBinds();
		void	performSubstitution(stringbuffer *buffer, int which);
		void	abort();
		char	*skipWhitespaceAndComments(const char *querybuffer);

		char		querybuffer[MAXQUERYSIZE+1];
		unsigned long	querylength;

		bindvar		inbindvars[MAXVAR];
		unsigned short	inbindcount;
		bindvar		outbindvars[MAXVAR];
		unsigned short	outbindcount;

		int		suspendresultset;
		int		busy;
};

#endif
