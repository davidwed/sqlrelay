// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#ifndef SQLRCRUD_H
#define SQLRCRUD_H

#include <sqlrelay/private/sqlrcrudincludes.h>

class SQLRCLIENT_DLLSPEC sqlrcrud : public object {
	public:
		sqlrcrud();
		~sqlrcrud();

		void	setSqlrConnection(sqlrconnection *con);
		void	setSqlrCursor(sqlrcursor *cur);

		void	setTable(const char *table);
		void	setPrimaryKeyColumn(const char *primarykey);
		void	setAutoIncrementColumn(const char *autoinc);
		void	setColumns(const char * const *columns);

		const char	*getTable();
		const char	*getPrimaryKeyColumn();
		const char	*getAutoIncrementColumn();
		const char * const *getColumns();

		bool	buildQueries();

		void	setCreateQuery(const char *createquery);
		void	setReadQuery(const char *readquery);
		void	setUpdateQuery(const char *updatequery);
		void	setDeleteQuery(const char *deletequery);

		const char	*getCreateQuery();
		const char	*getReadQuery();
		const char	*getUpdateQuery();
		const char	*getDeleteQuery();

		bool	doCreate(const char * const *columns,
				const char * const *values);

		bool	doRead(const char * const *criteria,
				const char * const *sort,
				uint64_t skip, uint64_t fetch);

		bool	doUpdate(const char * const * columns,
				const char * const *values,
				const char * const *criteria);

		bool	doDelete(const char * const *criteria);

		const char	*getErrorMessage();
		int64_t		getErrorCode();
		uint64_t	getAffectedRows();

		sqlrscalar		*getScalar();
		sqlrrowlinkedlist	*getRowLinkedList();
		sqlrrowdictionary	*getRowDictionary();
		sqlrresultsetlinkedlist	*getResultSetLinkedList();
		sqlrresultsettable	*getResultSetTable();

	#include <sqlrelay/private/sqlrcrud.h>
};

#endif
