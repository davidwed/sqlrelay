// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#ifndef SQLRCRUD_H
#define SQLRCRUD_H

#include <sqlrelay/private/sqlrcrudincludes.h>

/** The sqlrcrud class provides a generic CRUD (create, read, update, and
 *  delete) interface to SQL Relay.
 *
 *  A typical invocation would be:
 *
 *  sqlrconnection	con(...);
 *  sqlrcursor		cur(&con);
 *  sqlrcrud		crud;
 *
 *  crud.setSqlrConnection(&con);
 *  crud.setSqlrCursor(&cur);
 *  crud.setTable("mytable");
 *  crud.setIdSequence("mytable_ids");
 *
 *  crud.buildQueries();
 *
 *  crud.doRead(...);
 *
 *  sqlrresultsettable	*t=getResultSetTable();
 *
 *  for (...) {
 *  	... process result set ...
 *  }
 *
 *  Various methods such as doRead(), doUpdate(), and doDelete() take a
 *  "criteria" argument.  This should be an XML or JSON string conforming to
 *  one of the following formats, that will be used to construct the where
 *  clause for these operations:
 *
 *  XML:
 *  ... describe format here ... 
 *
 *  JSON:
 *  ... describe format here ... 
 *
 *  The doRead() method also takes a "sort" argument.  This should be an XML or
 *  JSON string conforming to one of the following formats, that will be used
 *  to construct the order-by clause for these operations:
 *
 *  XML:
 *  ... describe format here ... 
 *
 *  JSON:
 *  ... describe format here ... 
 *
 *
 *  The class also provides methods for overriding the template queries that
 *  are automatically built by buildQueries() as well as any primary key or
 *  autoincrement columns detected by buildQueries().
 *
 *  This class might be used in an MVC application, in which a tableview
 *  requests various CRUD operations, optionally providing JSON-formatted
 *  criteria and sort parameters.
 */
class SQLRCLIENT_DLLSPEC sqlrcrud : public object {
	public:
		/** Creates an instance of the sqlrcrud class. */
		sqlrcrud();

		/** Destroys this instance of the sqlrcrud class. */
		~sqlrcrud();


		/** Sets the instance of sqlrconnection for this
		 *  instance to use. */
		void	setSqlrConnection(sqlrconnection *con);

		/** Sets the instance of sqlrcursor for this instance to use. */
		void	setSqlrCursor(sqlrcursor *cur);


		/** Sets the table that this instance operates on. */
		void	setTable(const char *table);

		/** Sets the sequence to generate new ids from during create
		 *  operations (insert queries). */
		void	setIdSequence(const char *idsequence);

		/** Returns the table set by setTable(). */
		const char	*getTable();

		/** Returns the sequence set by setSequence(). */
		const char	*getIdSequence();


		/** Uses the table set by setTable() and sequence set by
		 *  setIdSequence() to construct template CRUD queries (insert,
		 *  select, update, and delete queries).  Also detects the
		 *  primary key and autoincrement columns, if the table has
		 *  any. */
		bool	buildQueries();


		/** Indicates which column is the primary key of the table
		 *  set by setTable().
		 *
		 *  May be used to override the primary key column detected by
		 *  buildQueries() or to set one if none was detected.  Note
		 *  that subsequent calls to buildQueries() will override the
		 *  primary key set by this method if one was detected in the
		 *  table.
		 *
		 *  Defaults to NULL.  Remains NULL if a call to buildQueries()
		 *  doesn't detect a primary key in the table.  If set to NULL
		 *  then the table will be presumed not to have a primary
		 *  key. */
		void	setPrimaryKeyColumn(const char *primarykey);

		/** Indicates which column is the autoincrement column of the
		 *  table set by setTable().
		 *
		 *  May be used to override the autoincrement column detected by
		 *  buildQueries() or to set one if none was detected.  Note
		 *  that subsequent calls to buildQueries() will override the
		 *  autoincrement column set by this method if one was detected
		 *  in the table.
		 *
		 *  Defaults to NULL.  Remains NULL if a call to buildQueries()
		 *  doesn't detect a autoincrement column in the table.  If set
		 *  to NULL then the table will be presumed not to have a
		 *  primary key. */
		void	setAutoIncrementColumn(const char *autoinc);

		/** Returns the primary key column as either determined by
		 *  buildQueries() or overridden by setPrimaryKeyColumn(). */
		const char	*getPrimaryKeyColumn();

		/** Returns the autoincrement column as either determined by
		 *  buildQueries() or overridden by setAutoIncrementColumn(). */
		const char	*getAutoIncrementColumn();

		/** Sets the create (insert) query template to "createquery".
		 *
		 *  May be used to override the query built by buidQueries().
		 *  Note that subsequent calls to buildQueries() will override
		 *  the query set by this method.
		 *
		 *  This query should contain substitution variables $(COLUMNS)
		 *  and $(VALUES), into which columns and values will be placed.
		 *
		 *  For example:
		 *  	insert into mytable ($(COLUMNS)) values ($(VALUES))
		 *
		 *  Defaults to NULL and remains NULL until set by buildQuery()
		 *  or a call to this method. */
		void	setCreateQuery(const char *createquery);

		/** Sets the read (select) query template to "readquery".
		 *
		 *  May be used to override the query built by buidQueries().
		 *  Note that subsequent calls to buildQueries() will override
		 *  the query set by this method.
		 *
		 *  This query should contain substitution variables $(WHERE)
		 *  and $(ORDERBY), into which where and order-by clauses will
		 *  be placed.
		 *
		 *  For example:
		 *  	select * from mytable $(WHERE) $(ORDERBY)
		 *
		 *  Defaults to NULL and remains NULL until set by buildQuery()
		 *  or a call to this method. */
		void	setReadQuery(const char *readquery);

		/** Sets the update query template to "updatequery".
		 *
		 *  May be used to override the query built by buidQueries().
		 *  Note that subsequent calls to buildQueries() will override
		 *  the query set by this method.
		 *
		 *  This query should contain substitution variables $(SET)
		 *  and $(WHERE), into which set and where clauses will be
		 *  placed.
		 *
		 *  For example:
		 *  	update mytable set $(SET) $(WHERE)
		 *
		 *  Defaults to NULL and remains NULL until set by buildQuery()
		 *  or a call to this method. */
		void	setUpdateQuery(const char *updatequery);

		/** Sets the delete query template to "deletequery".
		 *
		 *  May be used to override the query built by buidQueries().
		 *  Note that subsequent calls to buildQueries() will override
		 *  the query set by this method.
		 *
		 *  This query should contain substitution variable $(WHERE),
		 *  into which the where clause will be placed.
		 *
		 *  For example:
		 *  	delete from mytable $(WHERE)
		 *
		 *  Defaults to NULL and remains NULL until set by buildQuery()
		 *  or a call to this method. */
		void	setDeleteQuery(const char *deletequery);

		/** Returns the create (insert) query as either built by
		 *  buildQueries() or overridden by setCreateQuery(). */
		const char	*getCreateQuery();

		/** Returns the read (select) query as either built by
		 *  buildQueries() or overridden by setReadQuery(). */
		const char	*getReadQuery();

		/** Returns the update query as either built by buildQueries()
		 *  or overridden by setUpdateQuery(). */
		const char	*getUpdateQuery();

		/** Returns the delete query as either built by buildQueries()
		 *  or overridden by setDeleteQuery(). */
		const char	*getDeleteQuery();


		/** Executes the create (insert) query as either built by
		 *  buildQueries() or overridden by setCreateQuery(),
		 *  substituting "columns" into $(COLUMNS) and "values" into
		 *  $(VALUES) as appropriate.
		 *
		 *  Returns true on success and false on error.  On error, the
		 *  code and message can be retrieved using getErrorCode() and
		 *  getErrorMessage(). */
		bool	doCreate(const char * const *columns,
					const char * const *values);

		/** Executes the read (select) query as either built by
		 *  buildQueries() or overridden by setReadQuery().
		 *
		 *  "criteria" should be an XML or JSON string representing
		 *  the criteria that will be used to build the where clause,
		 *  conforming to the format described in the class description.
		 *
		 *  "sort" should be an XML or JSON string representing
		 *  the criteria that will be used to build the order-by clause,
		 *  conforming to the format described in the class description.
		 *
		 *  "skip" indicates how many rows to skip immediately (useful
		 *  for paging).
		 *
		 *  Returns true on success and false on error.  On error, the
		 *  code and message can be retrieved using getErrorCode() and
		 *  getErrorMessage(). */
		bool	doRead(const char *criteria,
					const char *sort,
					uint64_t skip);

		/** Executes the update query as either built by buildQueries()
		 *  or overridden by setUpdateQuery().
		 *
		 *  "columns" and "values" should be set to the column/value
		 *  pairs to be updated.
		 *
		 *  "criteria" should be an XML or JSON string representing
		 *  the criteria that will be used to build the where clause,
		 *  conforming to the format described in the class description.
		 *
		 *  Returns true on success and false on error.  On error, the
		 *  code and message can be retrieved using getErrorCode() and
		 *  getErrorMessage(). */
		bool	doUpdate(const char * const * columns,
					const char * const *values,
					const char *criteria);

		/** Executes the delete query as either built by buildQueries()
		 *  or overridden by setDeleteQuery().
		 *
		 *  "criteria" should be an XML or JSON string representing
		 *  the criteria that will be used to build the where clause,
		 *  conforming to the format described in the class description.
		 *
		 *  Returns true on success and false on error.  On error, the
		 *  code and message can be retrieved using getErrorCode() and
		 *  getErrorMessage(). */
		bool	doDelete(const char *criteria);

		/** Returns whatever error message may have been set by the
		 *  most recent failed method call. */
		const char	*getErrorMessage();

		/** Returns whatever error code may have been set by the
		 *  most recent failed method call. */
		int64_t		getErrorCode();

		/** Returns the number of rows affected by the most recent
		 *  doCreate(), doUpdate(), or doDelete() call, or 0 if
		 *  the most recent call was to doRead(). */
		uint64_t	getAffectedRows();

		/** Returns an instance of sqlrscalar, representing the first
		 *  field of the first row of the result set if doRead() was
		 *  most recently called, or an empty sqlrscalar if doCreate(),
		 *  doUpdate(), or doDelete() was most recently called. */
		const sqlrscalar	*getScalar();

		/** Returns an instance of sqlrrowlinkedlist, representing the
		 *  first row of the result set if doRead() was most recently
		 *  called, or an empty sqlrrowlinkedlist if doCreate(),
		 *  doUpdate(), or doDelete() was most recently called. */
		const sqlrrowlinkedlist	*getRowLinkedList();

		/** Returns an instance of sqlrrowdictionary, representing the
		 *  first row of the result set if doRead() was most recently
		 *  called, or an empty sqlrrowdictionary if doCreate(),
		 *  doUpdate(), or doDelete() was most recently called. */
		const sqlrrowdictionary	*getRowDictionary();

		/** Returns an instance of sqlrrowdictionary, representing the
		 *  first column of each row of the result set if doRead() was
		 *  most recently called, or an empty sqlrresultsetlinkedlist
		 *  if doCreate(), doUpdate(), or doDelete() was most recently
		 *  called. */
		const sqlrresultsetlinkedlist	*getResultSetLinkedList();

		/** Returns an instance of sqlresultsettable, representing the
		 *  result set if doRead() was most recently called, or an
		 *  empty sqlrresultsettable if doCreate(), doUpdate(), or
		 *  doDelete() was most recently called. */
		const sqlrresultsettable	*getResultSetTable();

	#include <sqlrelay/private/sqlrcrud.h>
};

#endif
