// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#ifndef SQLRCRUD_H
#define SQLRCRUD_H

#include <sqlrelay/private/sqlrcrudincludes.h>

/** The sqlrcrud class provides a generic CRUD (create, read, update, and
 *  delete) interface to SQL Relay.
 *
 *  A typical invocation of a mycrud class which implements mvccrud, from
 *  within the dao tier of an MVC application, would be something like:
 *
 *
 *  // initialize crud
 *  sqlrconnection	*con=new sqlrconnection(...);
 *  sqlrcursor		*cur=new sqlrcursor(con);
 *  sqlrcrud		*crud=new sqlrcrud();
 *
 *  crud->setSqlrConnection(con);
 *  crud->setSqlrCursor(cur);
 *  crud->setTable("mytable");
 *  crud->setIdSequence("mytable_ids");
 *  crud->buildQueries();
 *
 *  // read data
 *  crud->doRead(...);
 *
 *  // return results via instance of mvcresults
 *  mvcr->setSuccess();
 *  mvcr->attachData("myresults","table",crud->getResultSetTable());
 *  mvcr->getWatebasket()->attach(crud);
 *  mvcr->getWatebasket()->attach(con);
 *  mvcr->getWatebasket()->attach(cur);
 *
 *
 *  Various methods such as doRead(), doUpdate(), and doDelete() take a
 *  "criteria" argument.  This should be a JSON string in jsonlogic format
 *  (http://jsonlogic.com), and will be used to construct the where clause for
 *  these operations:
 *
 *    { "and" : [
 *      { "=" : [
 *        { "var" : "col1" },
 *        1
 *      ] },
 *      { "!=" : [
 *        { "var" : "col2" },
 *        "one"
 *      ] }
 *    ] }
 *
 *  The doRead() method also takes a "sort" argument.  This should be a JSON
 *  string conforming to the following format, and will be used to construct
 *  the order-by clause for these operations:
 *
 *    {
 *      "col1" : "asc",
 *      "col2" : "asc",
 *      "col3" : "desc"
 *    }
 *
 *  The class also provides methods for overriding the template queries that
 *  are automatically built by buildQueries() as well as any primary key or
 *  autoincrement columns detected by buildQueries().
 */
class SQLRCLIENT_DLLSPEC sqlrcrud : public mvccrud {
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


		/** Sets the table that this instance operates on.
		 *
		 *  Also sets the sequence to generate new ids from during
		 *  create operations to "table"_ids, if the id sequence is
		 *  NULL, either because setIdSequence() hasn't been called,
		 *  or was called with NULL. */
		void	setTable(const char *table);

		/** Sets the sequence to generate new ids from during create
		 *  operations (insert queries). */
		void	setIdSequence(const char *idsequence);

		/** Returns the table set by setTable(). */
		const char	*getTable();

		/** Returns the sequence set by setSequence() or setTable().
		 *
		 *  Note that this may return NULL if:
		 *  * neither setIdSequence() nor setTable() were ever called
		 *  * setTable() was called, but then setIdSequence(NULL)
		 *    was called later
		 */
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
		 *  buildQueries() or overridden by setPrimaryKeyColumn().
		 *
		 *  Note that this may return NULL if:
		 *  * neither buildQueries() nor setPrimaryKeyColumn() was ever
		 *    called
		 *  * buildQueries() was called, but didn't detect a
		 *    primary key column and setPrimaryKeyColumn() was never
		 *    called
		 *  * buildQueries() was called, and it detected a primary key
		 *    column, but then setPrimaryKeyColumn(NULL) was called
		 *    later
		 */
		const char	*getPrimaryKeyColumn();

		/** Returns the autoincrement column as either determined by
		 *  buildQueries() or overridden by setAutoIncrementColumn().
		 *
		 *  Note that this may return NULL if:
		 *  * neither buildQueries() nor setAutoincrementColumn() was
		 *    ever called
		 *  * buildQueries() was called, but didn't detect an
		 *    autoincrement column and setAutoIncrementColumn() was
		 *    never called
		 *  * buildQueries() was called, and it detected an
		 *    autoincrement column, but then
		 *    setAutoIncrementColumn(NULL) was called later
		 */
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


		/** Indicates that the read (select) query contains a partial
		 *  where clause.  This affects the behavior of doRead() when
		 *  it builds a where clause from "criteria". */
		void	setReadQueryContainsPartialWhere(
						bool containspartial);

		/** Indicates that the read (select) query contains a partial
		 *  order-by clause.  This affects the behavior of doRead()
		 *  when it builds an order-by clause from the "sort". */
		void	setReadQueryContainsPartialOrderBy(
						bool containspartial);

		/** Indicates that the update query contains a partial where
		 *  clause.  This affects the behavior of doUpdate() when
		 *  it builds a where clause from "criteria". */
		void	setUpdateQueryContainsPartialWhere(
						bool containspartial);

		/** Indicates that the delete query contains a partial where
		 *  clause.  This affects the behavior of doDelete() when
		 *  it builds a where clause from "criteria". */
		void	setDeleteQueryContainsPartialWhere(
						bool containspartial);

		/** Returns true if the read (select) query contains a partial
		 *  where clause and false otherwise. */
		bool	getReadQueryContainsPartialWhere();

		/** Returns true if the read (select) query contains a partial
		 *  order-by clause and false otherwise. */
		bool	getReadQueryContainsPartialOrderBy();

		/** Returns true if the update query contains a partial where
		 *  clause and false otherwise. */
		bool	getUpdateQueryContainsPartialWhere();

		/** Returns true if the delete query contains a partial where
		 *  clause and false otherwise. */
		bool	getDeleteQueryContainsPartialWhere();


		/** Executes the create (insert) query as either built by
		 *  buildQueries() or overridden by setCreateQuery().
		 *
		 *  "columns" should contain the set of columns that
		 *  corresponding elements of "values" will be inserted into.
		 *
		 *  "types" should contain the corresponding data type for each
		 *  value:
		 *  * "n" for numeric
		 *  * "t" for true
		 *  * "f" for false
		 *  * "u" for null
		 *  * "s" (or any other value) for string
		 *  Otherwise "types" may be null, and the data type will be 
		 *  derived as "s", "n", or "u" from the value.
		 *
		 *  Note that if getAutoIncrementColumn() returns non-NULL
		 *  (see getAutoIncrementColumn() for why this may be) then any
		 *  value specified for that column will be overridden, such
		 *  that the column will auto-increment.  If
		 *  getAutoIncrementColumn() returns NULL (see
		 *  getAutoIncrementColumn() for why this may be) then any
		 *  value specified for the autoincrement column will not be
		 *  overridden.
		 *
		 *  Note that if getPrimaryKeyColumn() and getIdSequence() both
		 *  return non-NULL (see getPrimaryKeyColumn() and
		 *  getIdSequence() for why this may be) then any value
		 *  specified for that column will be overridden, such that the
		 *  column will be populated from the id sequence.  If either of
		 *  getPrimaryKeyColumn() or getIdSequence() return NULL (see
		 *  getPrimaryKeyColumn() and getIdSequence() for why this may
		 *  be) then any value specified for the primary column will not
		 *  be overridden.
		 *
		 *  Returns true on success and false on error.  On error, the
		 *  code and message can be retrieved using getErrorCode() and
		 *  getErrorMessage(). */
		bool	doCreate(const char * const *columns,
					const char * const *values,
					const char * const *types);

		/** Executes the create (insert) query as either built by
		 *  buildQueries() or overridden by setCreateQuery().
		 *
		 *  Keys of "kvp" and values of "kvp" should be set to the
		 *  column/value pairs to be inserted.  The data type of each
		 *  value will be derived as "s", "n", or "u" from the value.
		 *
		 *  Note that if getAutoIncrementColumn() returns non-NULL
		 *  (see getAutoIncrementColumn() for why this may be) then any
		 *  value specified for that column will be overridden, such
		 *  that the column will auto-increment.  If
		 *  getAutoIncrementColumn() returns NULL (see
		 *  getAutoIncrementColumn() for why this may be) then any
		 *  value specified for the autoincrement column will not be
		 *  overridden.
		 *
		 *  Note that if getPrimaryKeyColumn() and getIdSequence() both
		 *  return non-NULL (see getPrimaryKeyColumn() and
		 *  getIdSequence() for why this may be) then any value
		 *  specified for that column will be overridden, such that the
		 *  column will be populated from the id sequence.  If either of
		 *  getPrimaryKeyColumn() or getIdSequence() return NULL (see
		 *  getPrimaryKeyColumn() and getIdSequence() for why this may
		 *  be) then any value specified for the primary column will not
		 *  be overridden.
		 *
		 *  Returns true on success and false on error.  On error, the
		 *  code and message can be retrieved using getErrorCode() and
		 *  getErrorMessage(). */
		bool	doCreate(dictionary<const char *, const char *> *kvp);

		/** Executes the create (insert) query as either built by
		 *  buildQueries() or overridden by setCreateQuery(),
		 *
		 *  "j" should be a jsondom containing 1 object:
		 *
		 *  "data" should be a JSON object consisting of the
		 *  column/value pairs to be inserted.
		 *
		 *  Note that if getAutoIncrementColumn() returns non-NULL
		 *  (see getAutoIncrementColumn() for why this may be) then any
		 *  value specified for that column will be overridden, such
		 *  that the column will auto-increment.  If
		 *  getAutoIncrementColumn() returns NULL (see
		 *  getAutoIncrementColumn() for why this may be) then any
		 *  value specified for the autoincrement column will not be
		 *  overridden.
		 *
		 *  Note that if getPrimaryKeyColumn() and getIdSequence() both
		 *  return non-NULL (see getPrimaryKeyColumn() and
		 *  getIdSequence() for why this may be) then any value
		 *  specified for that column will be overridden, such that the
		 *  column will be populated from the id sequence.  If either of
		 *  getPrimaryKeyColumn() or getIdSequence() return NULL (see
		 *  getPrimaryKeyColumn() and getIdSequence() for why this may
		 *  be) then any value specified for the primary column will not
		 *  be overridden.
		 *
		 *  Returns true on success and false on error.  On error, the
		 *  code and message can be retrieved using getErrorCode() and
		 *  getErrorMessage(). */
		bool	doCreate(jsondom *j);

		/** Executes the read (select) query as either built by
		 *  buildQueries() or overridden by setReadQuery().
		 *
		 *  "criteria" should be a JSON string representing the
		 *  criteria that will be used to build the where clause,
		 *  conforming to the format described in the class description.
		 *
		 *  "sort" should be a JSON string representing the criteria
		 *  that will be used to build the order-by clause, conforming
		 *  to the format described in the class description.
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

		/** Executes the read (select) query as either built by
		 *  buildQueries() or overridden by setReadQuery().
		 *
		 *  "j" should be a jsondom containing 3 objects:
		 *
		 *  "criteria" should be a JSON object representing the
		 *  criteria that will be used to build the where clause,
		 *  conforming to the format described in the class description.
		 *
		 *  "sort" should be a JSON object representing the criteria
		 *  that will be used to build the order-by clause, conforming
		 *  to the format described in the class description.
		 *
		 *  "skip" should be a number indicating how many rows to skip
		 *  immediately (useful for paging).
		 *
		 *  Returns true on success and false on error.  On error, the
		 *  code and message can be retrieved using getErrorCode() and
		 *  getErrorMessage(). */
		bool	doRead(jsondom *j);

		/** Executes the update query as either built by buildQueries()
		 *  or overridden by setUpdateQuery().
		 *
		 *  "columns" and "values" should be set to the column/value
		 *  pairs to be updated.  "types" should be set to the
		 *  corresponding data type for each value:
		 *  * "n" for numeric
		 *  * "t" for true
		 *  * "f" for false
		 *  * "u" for null
		 *  * "s" (or any other value) for string
		 *  Otherwise "types" may be null, and the data type will be
		 *  derived as "s", "n", or "u" from the value.
		 *
		 *  "criteria" should be a JSON string representing the
		 *  criteria that will be used to build the where clause,
		 *  conforming to the format described in the class description.
		 *
		 *  Note that if getAutoIncrementColumn() returns non-NULL
		 *  (see getAutoIncrementColumn() for why this may be) then any
		 *  value specified for that column will be overridden, such
		 *  that the column will auto-increment.  If
		 *  getAutoIncrementColumn() returns NULL (see
		 *  getAutoIncrementColumn() for why this may be) then any
		 *  value specified for the autoincrement column will not be
		 *  overridden.
		 *
		 *  Note that if getPrimaryKeyColumn() and getIdSequence() both
		 *  return non-NULL (see getPrimaryKeyColumn() and
		 *  getIdSequence() for why this may be) then any value
		 *  specified for that column will be overridden, such that the
		 *  column will be populated from the id sequence.  If either of
		 *  getPrimaryKeyColumn() or getIdSequence() return NULL (see
		 *  getPrimaryKeyColumn() and getIdSequence() for why this may
		 *  be) then any value specified for the primary column will not
		 *  be overridden.
		 *
		 *  Returns true on success and false on error.  On error, the
		 *  code and message can be retrieved using getErrorCode() and
		 *  getErrorMessage(). */
		bool	doUpdate(const char * const * columns,
					const char * const *values,
					const char * const *types,
					const char *criteria);

		/** Executes the update query as either built by buildQueries()
		 *  or overridden by setUpdateQuery().
		 *
		 *  Keys of "kvp" and values of "kvp" should be set to the
		 *  column/value pairs to be updated.  The data type of each
		 *  value will be derived as "s", "n", or "u" from the value.
		 *
		 *  "criteria" should be a JSON string representing the
		 *  criteria that will be used to build the where clause,
		 *  conforming to the format described in the class description.
		 *
		 *  Note that if getAutoIncrementColumn() returns non-NULL
		 *  (see getAutoIncrementColumn() for why this may be) then any
		 *  value specified for that column will be overridden, such
		 *  that the column will auto-increment.  If
		 *  getAutoIncrementColumn() returns NULL (see
		 *  getAutoIncrementColumn() for why this may be) then any
		 *  value specified for the autoincrement column will not be
		 *  overridden.
		 *
		 *  Note that if getPrimaryKeyColumn() and getIdSequence() both
		 *  return non-NULL (see getPrimaryKeyColumn() and
		 *  getIdSequence() for why this may be) then any value
		 *  specified for that column will be overridden, such that the
		 *  column will be populated from the id sequence.  If either of
		 *  getPrimaryKeyColumn() or getIdSequence() return NULL (see
		 *  getPrimaryKeyColumn() and getIdSequence() for why this may
		 *  be) then any value specified for the primary column will not
		 *  be overridden.
		 *
		 *  Returns true on success and false on error.  On error, the
		 *  code and message can be retrieved using getErrorCode() and
		 *  getErrorMessage(). */
		bool	doUpdate(dictionary<const char *, const char *> *kvp,
							const char *criteria);

		/** Executes the update query as either built by buildQueries()
		 *  or overridden by setUpdateQuery().
		 *
		 *  "j" should be a jsondom containing 2 objects:
		 *
		 *  "criteria" should be a JSON object representing the
		 *  criteria that will be used to build the where clause,
		 *  conforming to the format described in the class description.
		 *
		 *  "data" should be a JSON object consisting of the
		 *  column/value pairs to be updated.
		 *
		 *  Note that if getAutoIncrementColumn() returns non-NULL
		 *  (see getAutoIncrementColumn() for why this may be) then any
		 *  value specified for that column will be overridden, such
		 *  that the column will auto-increment.  If
		 *  getAutoIncrementColumn() returns NULL (see
		 *  getAutoIncrementColumn() for why this may be) then any
		 *  value specified for the autoincrement column will not be
		 *  overridden.
		 *
		 *  Note that if getPrimaryKeyColumn() and getIdSequence() both
		 *  return non-NULL (see getPrimaryKeyColumn() and
		 *  getIdSequence() for why this may be) then any value
		 *  specified for that column will be overridden, such that the
		 *  column will be populated from the id sequence.  If either of
		 *  getPrimaryKeyColumn() or getIdSequence() return NULL (see
		 *  getPrimaryKeyColumn() and getIdSequence() for why this may
		 *  be) then any value specified for the primary column will not
		 *  be overridden.
		 *
		 *  Returns true on success and false on error.  On error, the
		 *  code and message can be retrieved using getErrorCode() and
		 *  getErrorMessage(). */
		bool	doUpdate(jsondom *j);

		/** Executes the delete query as either built by buildQueries()
		 *  or overridden by setDeleteQuery().
		 *
		 *  "criteria" should be a JSON string representing the
		 *  criteria that will be used to build the where clause,
		 *  conforming to the format described in the class description.
		 *
		 *  Returns true on success and false on error.  On error, the
		 *  code and message can be retrieved using getErrorCode() and
		 *  getErrorMessage(). */
		bool	doDelete(const char *criteria);

		/** Executes the delete query as either built by buildQueries()
		 *  or overridden by setDeleteQuery().
		 *
		 *  "j" should be a jsondom containing 1 object:
		 *
		 *  "criteria" should be a JSON object representing the
		 *  criteria that will be used to build the where clause,
		 *  conforming to the format described in the class description.
		 *
		 *  Returns true on success and false on error.  On error, the
		 *  code and message can be retrieved using getErrorCode() and
		 *  getErrorMessage(). */
		bool	doDelete(jsondom *j);

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

		/** Returns an instance of scalarcollection, containing the
		 *  number of affected rows if doCreate(), doUpdate(), or
		 *  doDelete() was most recently called or an empty scalar if
		 *  doRead() was most recently called. */
		scalarcollection<uint64_t>	*getAffectedRowsScalar();

		/** Returns an instance of listcollection with a single element,
		 *  containing the affected rows doCreate(), doUpdate(), or
		 *  doDelete() was most recently called or an empty list if
		 *  doRead() was most recently called. */
		listcollection<uint64_t>	*getAffectedRowsList();

		/** Returns an instance of dictionarycollection with a single
		 *  element, containing the affected rows doCreate(),
		 *  doUpdate(), or doDelete() was most recently called or an
		 *  empty dictionary if doRead() was most recently called. */
		dictionarycollection<const char *, uint64_t>
					*getAffectedRowsDictionary();

		/** Returns an instance of tablecollection with a single
		 *  field, containing the affected rows doCreate(),
		 *  doUpdate(), or doDelete() was most recently called or an
		 *  empty table if doRead() was most recently called. */
		tablecollection<uint64_t>	*getAffectedRowsTable();

		/** Returns an instance of sqlrscalar, representing the first
		 *  field of the first row of the result set if doRead() was
		 *  most recently called, or an empty sqlrscalar if doCreate(),
		 *  doUpdate(), or doDelete() was most recently called. */
		scalarcollection<const char *>	*getFirstFieldScalar();

		/** Returns an instance of sqlrrowlist, representing the
		 *  first row of the result set if doRead() was most recently
		 *  called, or an empty sqlrrowlist if doCreate(), doUpdate(),
		 *  or doDelete() was most recently called. */
		listcollection<const char *>	*getFirstRowList();

		/** Returns an instance of sqlrrowdictionary, representing the
		 *  first row of the result set if doRead() was most recently
		 *  called, or an empty sqlrrowdictionary if doCreate(),
		 *  doUpdate(), or doDelete() was most recently called. */
		dictionarycollection<const char *, const char *>
						*getFirstRowDictionary();

		/** Returns an instance of sqlrrowdictionary, representing the
		 *  first column of each row of the result set if doRead() was
		 *  most recently called, or an empty sqlrresultsetlist if
		 *  doCreate(), doUpdate(), or doDelete() was most recently
		 *  called. */
		listcollection<const char *>	*getFirstColumnList();

		/** Returns an instance of sqlresultsettable, representing the
		 *  result set if doRead() was most recently called, or an
		 *  empty sqlrresultsettable if doCreate(), doUpdate(), or
		 *  doDelete() was most recently called. */
		tablecollection<const char *>	*getResultSetTable();

	#include <sqlrelay/private/sqlrcrud.h>
};

#endif
