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
	public:
			sqlrcursor(sqlrconnection *conn);
		virtual	~sqlrcursor();

		// interface definition
		virtual	bool	openCursor(int id);
		virtual	bool	closeCursor();

		virtual	bool	prepareQuery(const char *query, long length);
		virtual	bool	inputBindString(const char *variable, 
						unsigned short variablesize,
						const char *value, 
						unsigned short valuesize,
						short *isnull);
		virtual	bool	inputBindLong(const char *variable, 
						unsigned short variablesize,
						unsigned long *value);
		virtual	bool	inputBindDouble(const char *variable, 
						unsigned short variablesize,
						double *value,
						unsigned short precision,
						unsigned short scale);
		virtual	bool	inputBindBlob(const char *variable, 
						unsigned short variablesize,
						const char *value, 
						unsigned long valuesize,
						short *isnull);
		virtual	bool	inputBindClob(const char *variable, 
						unsigned short variablesize,
						const char *value, 
						unsigned long valuesize,
						short *isnull);
		virtual	bool	outputBindString(const char *variable, 
						unsigned short variablesize,
						char *value,
						unsigned short valuesize,
						short *isnull);
		virtual	bool	outputBindBlob(const char *variable, 
						unsigned short variablesize,
						int index,
						short *isnull);
		virtual	bool	outputBindClob(const char *variable, 
						unsigned short variablesize,
						int index,
						short *isnull);
		virtual	bool	outputBindCursor(const char *variable,
						unsigned short variablesize,
						sqlrcursor *cursor);
		virtual	void	returnOutputBindBlob(int index);
		virtual	void	returnOutputBindClob(int index);
		virtual	void	returnOutputBindCursor(int index);
		virtual void	checkForTempTable(const char *query,
							unsigned long length);
		virtual	bool	executeQuery(const char *query,
							long length,
							bool execute)=0;
		virtual	bool	queryIsNotSelect();
		virtual	bool	queryIsCommitOrRollback();
		virtual	char	*getErrorMessage(bool *liveconnection)=0;
		virtual	void	returnRowCounts()=0;
		virtual	void	returnColumnCount()=0;
		virtual	void	returnColumnInfo()=0;
		virtual	bool	noRowsToReturn()=0;
		virtual	bool	skipRow()=0;
		virtual	bool	fetchRow()=0;
		virtual	void	returnRow()=0;
		virtual	void	cleanUpData(bool freerows, bool freecols,
								bool freebinds);

	protected:
		regularexpression	createtemplower;
		regularexpression	createtempupper;

		// methods/variables used by derived classes
		stringbuffer	*fakeInputBinds(const char *query);

		bool	skipComment(char **ptr, const char *endptr);
		bool	skipWhitespace(char **ptr, const char *endptr);
		bool	advance(char **ptr, const char *endptr,
						unsigned short steps);

		sqlrconnection	*conn;

	private:
		// methods used internally
		bool	handleBinds();
		void	performSubstitution(stringbuffer *buffer, int index);
		void	abort();
		char	*skipWhitespaceAndComments(const char *querybuffer);

		char		querybuffer[MAXQUERYSIZE+1];
		unsigned long	querylength;

		bindvar		inbindvars[MAXVAR];
		unsigned short	inbindcount;
		bindvar		outbindvars[MAXVAR];
		unsigned short	outbindcount;

		bool		suspendresultset;
		bool		busy;
		int		id;
};

#endif
