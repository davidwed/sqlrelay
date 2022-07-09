// Copyright (c) 1999-2018 David Muse
// See the COPYING file for more information.

#ifndef SQLRRESULTSETCOLLECTIONS_H
#define SQLRRESULTSETCOLLECTIONS_H

#include <sqlrelay/private/sqlrcollectionsincludes.h>

class SQLRCLIENT_DLLSPEC sqlrscalar : public scalarcollection<const char *> {
	public:
		/** Creates an empty instance of the sqlrscalar class. */
		sqlrscalar();

		/** Creates an instance of the sqlrscalar class and
		 *  sets the cursor used by the instance to "cursor". */
		sqlrscalar(sqlrcursor *cursor);

		/** Creates an instance of the sqlrscalar class.
		 *  Sets the cursor used by the instance to "cursor", the
		 *  row that the instance returns fields from to "row", and the
		 *  column that the instance returns fields from to "column". */
		sqlrscalar(sqlrcursor *cursor, uint64_t row, uint64_t column);

		/** Deletes this intance of the sqlrscalar class. */
		~sqlrscalar();

		/** Returns true. */
		bool	getIsReadOnly() const;

		/** Sets the cursor used by this instance to "cursor". */
		void	setCursor(sqlrcursor *cursor);

		/** Sets the row that this instance returns fields from
		 *  to "row". */
		void	setRow(uint64_t row);

		/** Sets the column that this instance returns fields from
		 *  to "col". */
		void	setColumn(uint32_t col);

		/** Returns the value stored in this intance.  Returns NULL or
		 *  0 if no value has been stored. */
		const char	*getValue() const;

		#include <sqlrelay/private/sqlrscalar.h>
};

class SQLRCLIENT_DLLSPEC sqlrlistnode : public listnode<const char *> {
	public:
		/** Return the value (field) stored in the node. */
		const char	*getValue() const;

		/** Returns the previous node in the sqlrlist or NULL
		 *  if this node is the first node in the list. */
		listnode<const char *>	*getPrevious() const;

		/** Returns the next node in the sqlrlist or NULL
		 * if this node is the last node in the list. */
		listnode<const char *>	*getNext() const;

		#include <sqlrelay/private/sqlrlistnode.h>
};

class SQLRCLIENT_DLLSPEC sqlrrowlist : public listcollection<const char *> {
	public:
		/** Creates an empty instance of the sqlrrowlist class. */
		sqlrrowlist();

		/** Creates an instance of the sqlrrowlist class and sets the
		 *  cursor used by the instance to "cursor". */
		sqlrrowlist(sqlrcursor *cursor);

		/** Creates an instance of the sqlrrowlist class.  Sets the
		 *  cursor used by the instance to "cursor" and the row that
		 *  the instance returns fields from to "row". */
		sqlrrowlist(sqlrcursor *cursor, uint64_t row);

		/** Deletes this intance of the sqlrrowlist class. */
		~sqlrrowlist();

		/** Sets the cursor used by this instance to "cursor". */
		void	setCursor(sqlrcursor *cursor);

		/** Sets the row that this instance returns fields from
		 *  to "row". */
		void	setRow(uint64_t row);

		/** Returns true. */
		bool	getIsReadOnly() const;

		/** Returns true. */
		bool	getIsBlockBased() const;

		/** Returns 1. */
		uint64_t	getBlockSize() const;

		/** Returns the number of nodes in the sqlrrowlist. */
		uint64_t	getLength() const;

		/** Returns the first node in the sqlrrowlist. */
		listnode<const char *>	*getFirst() const;

		/** Returns the node after "node" or NULL if this node is the
		 *  last node in the list. "node" is presumed to be in the
		 *  list. */
		listnode<const char *>	*getNext(
					listnode<const char *> *node) const;

		/** Returns a pointer to the first listnode
		 *  containing "value" or NULL if "value" was not found. */
		listnode<const char *>	*find(const char *value) const;

		/** Returns a pointer to the first listnode
		 *  after "startnode" containing "value" or NULL
		 *  if "value" was not found. */
		listnode<const char *>	*find(listnode<const char *> *startnode,
						const char *value) const;
	
		#include <sqlrelay/private/sqlrrowlist.h>
};

class SQLRCLIENT_DLLSPEC sqlrrowdictionary :
		public dictionarycollection<const char *, const char *> {
	public:
		/** Creates an instance of the sqlrrowdictionary class. */
		sqlrrowdictionary();

		/** Creates an instance of the sqlrrowdictionary class and
		 *  sets the cursor used by the instance to "cursor". */
		sqlrrowdictionary(sqlrcursor *cursor);

		/** Creates an instance of the sqlrrowdictionary class.
		 *  Sets the cursor used by the instance to "cursor" and the
		 *  row that the instance returns fields from to "row". */
		sqlrrowdictionary(sqlrcursor *cursor, uint64_t row);

		/** Deletes this intance of the sqlrrowdictionary class. */
		~sqlrrowdictionary();

		/** Sets the cursor used by this instance to "cursor". */
		void	setCursor(sqlrcursor *cursor);

		/** Sets the row that this instance returns fields from
		 *  to "row". */
		void	setRow(uint64_t row);

		/** Returns true. */
		bool	getIsReadOnly() const;

		/** Sets "value" to the value (field) associated with
		 *  "key" (column name).
		 *
		 *  Returns true on success or false if "key" wasn't
		 *  found. */
		bool	getValue(const char *key, const char **value) const;

		/** Returns the value (field) associated with "key"
		 *  (column name) or NULL if "key" wasn't found (was an invalid
		 *  column name).  Note that there is no way to distinguish
		 *  between failure to find "key" and a valid value of NULL
		 *  associated with "key". */
		const char	*getValue(const char *key) const;

		/** Sets "k" to the key (column name) associated with "key"
		 *  (also the column name).  Returns true on success or false
		 *  if "key" wasn't found (invalid column name). */
		bool	getKey(const char *key, const char **k) const;

		/** Returns the key (column name) associated with "key" (also
		 *  column name) or NULL if "key" (also the column name) wasn't
		 *  found (was an invalid column name).  Note that there is no
		 *  way to distinguish between failure to find "key" and a
		 *  valid key (column name) of NULL associated with "key" (also
		 *  column name). */
		const char	*getKey(const char *key) const;

		/** Returns a list of the keys (column names) in the
		 *  dictionary. */
		linkedlist<const char *>	*getKeys() const;

		/** Returns the number of key/value (column name/field) pairs
		 *  in the dictionary. */
		uint64_t	getLength() const;
	
		#include <sqlrelay/private/sqlrrowdictionary.h>
};

class SQLRCLIENT_DLLSPEC sqlrresultsetlist :
				public listcollection<const char *> {
	public:
		/** Creates an empty instance of the sqlrresultsetlist
		 *  class. */
		sqlrresultsetlist();

		/** Creates an instance of the sqlrresultsetlist class and
		 *  sets the cursor used by the instance to "cursor". */
		sqlrresultsetlist(sqlrcursor *cursor);

		/** Creates an instance of the sqlrresultsetlist class.
		 *  Sets the cursor used by the instance to "cursor" and the
		 *  column that the instance returns fields from to "col". */
		sqlrresultsetlist(sqlrcursor *cursor, uint64_t col);

		/** Deletes this intance of the sqlrresultsetlist
		 *  class. */
		~sqlrresultsetlist();

		/** Sets the cursor used by this instance to "cursor". */
		void	setCursor(sqlrcursor *cursor);

		/** Sets the column that this instance returns fields from
		 *  to "col". */
		void	setColumn(uint32_t col);

		/** Returns true. */
		bool	getIsReadOnly() const;

		/** Returns true. */
		bool	getIsBlockBased() const;

		/** Returns the result set buffer size of the cursor used by
 		 *  this intance.  Returns 0 if the cursor is configured to
 		 *  fetch all rows at once. */
		uint64_t	getBlockSize() const;

		/** Returns the number of nodes in the sqlrresultsetlist. */
		uint64_t	getLength() const;

		/** Returns the first node in the sqlrresultsetlist. */
		listnode<const char *>	*getFirst() const;

		/** Returns the node after "node" or NULL if this node is the
		 *  last node in the list. "node" is presumed to be in the
		 *  list. */
		listnode<const char *>	*getNext(
					listnode<const char *> *node) const;

		/** Returns a pointer to the first listnode
		 *  containing "value" or NULL if "value" was not found. */
		listnode<const char *>	*find(const char *value) const;

		/** Returns a pointer to the first listnode
		 *  after "startnode" containing "value" or NULL
		 *  if "value" was not found. */
		listnode<const char *>	*find(listnode<const char *> *startnode,
						const char *value) const;
	
		#include <sqlrelay/private/sqlrresultsetlist.h>
};

/** Read-only, block-based, sequential-access. */
class SQLRCLIENT_DLLSPEC sqlrresultsettable :
				public tablecollection<const char *> {
	public:
		/** Creates an instance of the sqlrresultsettable class. */
		sqlrresultsettable();

		/** Creates an instance of the sqlrresultsettable class and
		 *  sets the cursor used by the instance to "cursor". */
		sqlrresultsettable(sqlrcursor *cursor);

		/** Deletes this intance of the sqlrresultsettable class. */
		~sqlrresultsettable();

		/** Sets the cursor used by this instance to "cursor". */
		void	setCursor(sqlrcursor *cursor);

		/** Returns true. */
		bool	getIsReadOnly() const;

		/** Returns true. */
		bool	getIsBlockBased() const;

		/** Returns the result set buffer size of the cursor used by
 		 *  this intance.  Returns 0 if the cursor is configured to
 		 *  fetch all rows at once. */
		uint64_t	getBlockSize() const;

		/** Returns true. */
		bool	getIsSequentialAccess() const;

		/** Returns the name of column "col". */
		const char	*getColumnName(uint64_t col) const;

		/** Returns the current number of columns in the table. */
		uint64_t	getColumnCount() const;

		/** Returns the value at "row", "col".  Returns NULL or 0 if
		 *  there is no value at that address. */
		const char	*getValue(uint64_t row,
						uint64_t col) const;

		/** Returns the value at "row", "colname".  Returns NULL or 0
		 *  if there is no value at that address. */
		const char	*getValue(uint64_t row,
						const char *colname) const;

		/** Returns the current number of rows in the table.  May
		 *  increase as new blocks of rows are fetched. */
		uint64_t	getRowCount() const;

		/** Returns true when the last block of rows has been
		 *  fetched. */
		bool	getAllRowsAvailable() const;
	
		#include <sqlrelay/private/sqlrresultsettable.h>
};

class SQLRCLIENT_DLLSPEC sqlrresultsetdomnode : public cursordomnode {
	public:
		/** Creates a new node, intializes its member variables to
 		 *  NULL, and sets the cursor used by the instance to
 		 *  "cursor". */
		sqlrresultsetdomnode(dom *dom,
					domnode *nullnode,
					sqlrcursor *cursor);

		/** Creates a new node, intializes its member variables to
 		 *  NULL, and sets the cursor used by the instance to
 		 *  "cursor". */
		sqlrresultsetdomnode(dom *dom,
					domnode *nullnode,
					const char *ns,
					sqlrcursor *cursor);

		/** Deletes the node, all attribute nodes, and all child
		 *  nodes, recursively. */
		~sqlrresultsetdomnode();

 		/** Sets the cursor used by this instance to "cursor". */
		void		setCursor(sqlrcursor *cursor);

		/** Returns the cursor used by this intance. */
		sqlrcursor	*getCursor();

		/** Returns the type of the node. */
		domnodetype	getType() const;

		/** Returns the name of the node. */
		const char	*getName() const;

		/** Returns the value of node. */
		const char	*getValue() const;

		/** Returns a pointer to the parent node or the
		 *  nullnode if none exists. */
		domnode		*getParent() const;

		/** Returns the position of the node, relative to its
		 *  siblings. */
		uint64_t	getPosition() const;

		/** Returns a pointer to the previous sibling
		 *  node or the nullnode if none exists. */
		domnode		*getPreviousSibling() const;

		/** Returns a pointer to the next sibling node
		 *  or the nullnode if none exists. */
		domnode		*getNextSibling() const;

		/** Returns the number of immediate child nodes. */
		uint64_t	getChildCount() const;

		/** Returns the first child node or the nullnode
		 *  if no children are found. */
		domnode		*getFirstChild() const;

		/** Returns the number of attributes. */
		uint64_t	getAttributeCount() const;

		/** Returns the attribute named "name"
		 *  or the nullnode if not found. */
		domnode		*getAttribute(const char *name) const;

		/** Returns the attribute named "name" (ignoring case)
		 *  or the nullnode if not found. */
		domnode		*getAttributeIgnoringCase(
						const char *name) const;

		/** Returns the attribute node at index
		 *  "position" or the nullnode if not found. */
		domnode		*getAttribute(uint64_t position) const;

		/** Returns true if this node is the special
		 *  nullnode and false otherwise. */
		bool		isNullNode() const;

		#include <sqlrelay/private/sqlrresultsetdomnode.h>
};

#endif
