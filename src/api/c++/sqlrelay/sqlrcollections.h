// Copyright (c) 1999-2018 David Muse
// See the COPYING file for more information.

#ifndef SQLRRESULTSETCOLLECTIONS_H
#define SQLRRESULTSETCOLLECTIONS_H

#include <sqlrelay/private/sqlrcollectionsincludes.h>

class SQLRCLIENT_DLLSPEC sqlrrowlinkedlistnode :
				public listnode<const char *> {
	public:
		/** Return the value stored in the node. */
		const char	*getValue() const;

		/** Returns the previous node in the listcollection or NULL
		 *  if this node is the first node in the list. */
		listnode<const char *>	*getPrevious() const;

		/** Returns the next node in the listcollection or NULL
		 * if this node is the last node in the list. */
		listnode<const char *>	*getNext() const;

		#include <sqlrelay/private/sqlrrowlinkedlistnode.h>
};

/** Read-only. */
class SQLRCLIENT_DLLSPEC sqlrrowlinkedlist :
				public listcollection<const char *> {
	public:
		/** Creates an empty instance of the sqlrrowlinkedlist class. */
		sqlrrowlinkedlist();

		/** Creates an instance of the sqlrrowlinkedlist class and
		 *  sets "cursor" to the cursor used by the class. */
		sqlrrowlinkedlist(sqlrcursor *cursor);

		/** Creates an instance of the sqlrrowlinkedlist class.
		 *  Sets "cursor" to the cursor used by the class and "row"
		 *  to the row that the class returns fields from. */
		sqlrrowlinkedlist(sqlrcursor *cursor, uint64_t row);

		/** Deletes this intance of the sqlrrowlinkedlist class. */
		~sqlrrowlinkedlist();

		/** Sets the cursor used by this instance to "cursor". */
		void	setCursor(sqlrcursor *cursor);

		/** Sets the row used by this instance to "row". */
		void	setRow(uint64_t row);

		/** Returns true. */
		bool		getIsReadOnly() const;

		/** Returns true. */
		bool		getIsBlockBased() const;

		/** Returns 1. */
		uint64_t	getBlockSize() const;

		/** Creates a new listnode containing "value" and
		 *  prepends it to the listcollection. */
		void	prepend(const char *value);

		/** Prepends already created listnode "node" to the
		 *  listcollection. */
		void	prepend(listnode<const char *> *node);

		/** Creates a new listnode containing "value" and
		 *  appends it to the listcollection. */
		void	append(const char *value);

		/** Appends already created listnode "node" to the
		 *  listcollection. */
		void	append(listnode<const char *> *node);

		/** Creates a new listnode containing "value" and
		 *  inserts it into the listcollection before "node". */
		void	insertBefore(listnode<const char *> *node,
							const char *value);

		/** Inserts already created listnode "newnode" into the
		 *  listcollection before "node". */
		void	insertBefore(listnode<const char *> *node,
					listnode<const char *> *newnode);

		/** Creates a new listnode containing "value" and
		 *  inserts it into the listcollection after "node". */
		void	insertAfter(listnode<const char *> *node,
							const char *value);

		/** Inserts already created listnode "newnode" into the
		 *  listcollection after "node". */
		void	insertAfter(listnode<const char *> *node,
					listnode<const char *> *newnode);

		/** Moves node "nodetomove" to the position before "node" in
		 *  the listcollection. */
		void	moveBefore(listnode<const char *> *node,
					listnode<const char *> *nodetomove);

		/** Moves node "nodetomove" to the position after "node" in
		 *  the listcollection. */
		void	moveAfter(listnode<const char *> *node,
					listnode<const char *> *nodetomove);

		/** Detaches "node" from the list. */
		void	detach(listnode<const char *> *node);

		/** Deletes the first listnode containing "value".
		 *
		 *  The value stored in the listnode is only
		 *  deleted if setManageValues(true) or
		 *  setManageArrayValues(true) has been called.
		 * 
		 *  Note that this operation requires a search and is expensive
		 *  in both execution time and code size.
		 *
		 *  Returns true on success and false on failure. */
		bool	remove(const char *value);

		/** Deletes all listnodes containing "value".
		 *
		 *  The value stored in each listnode is only
		 *  deleted if setManageValues(true) or
		 *  setManageArrayValues(true) has been called.
		 * 
		 *  Note that this operation requires a search and is expensive
		 *  in both execution time and code size.
		 * 
		 *  Returns true on success and false on failure. */
		bool	removeAll(const char *value);

		/** Removes listnode "node" from the listcollection.
		 *
		 *  The value stored in the listnode is only
		 *  deleted if setManageValues(true) or
		 *  setManageArrayValues(true) has been called.
		 * 
		 *  Note that this operation does not require a search and is
		 *  far less expensive than the remove(value) operation and
		 *  removeAll().
		 *
		 *  Returns true on success and false on failure. */
		bool	remove(listnode<const char *> *node);

		/** Returns the number of nodes in the listcollection. */
		uint64_t	getLength() const;

		/** Returns the first node in the listcollection. */
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

		/** Sorts the listcollection in ascending order using a modified
		 *  insertion sort algorithm.  This sort is slower than
		 *  heapSort() but uses no additional memory. */
		void	insertionSort();

		/** Sorts the listcollection in ascending order using a heap
		 *  sort algorithm.  This sort is faster than heapSort() but
		 *  uses additional memory in proportion to the size of the
		 *  list. */
		void	heapSort();

		/** Deletes all listnodes currently in the listcollection. */
		void	clear();

		/** Prints out a representation of the listcollection. */
		void	print() const;

		/** Prints out a representation of the first "count"
		 *  nodes of the listcollection. */
		void	print(uint64_t count) const;
	
		#include <sqlrelay/private/sqlrrowlinkedlist.h>
};

/** Read-only. */
class SQLRCLIENT_DLLSPEC sqlrrowdictionary :
				public dictionarycollection<const char *,
								const char *> {
	public:
		sqlrrowdictionary();
		sqlrrowdictionary(sqlrcursor *cursor);
		~sqlrrowdictionary();

		void	setCursor(sqlrcursor *cursor);
		void	setRow(uint64_t row);

		bool	getIsReadOnly() const;

		bool	getValue(const char *key, const char **value) const;
		const char	*getValue(const char *key) const;

		bool	getKey(const char *key, const char **k) const;
		const char	*getKey(const char *key) const;

		linkedlist<const char *>	*getKeys() const;

		uint64_t	getLength() const;

		void	print() const;
	
		#include <sqlrelay/private/sqlrrowdictionary.h>
};

/** Read-only, block-based, sequential-access. */
class SQLRCLIENT_DLLSPEC sqlrresultsettable :
				public tablecollection<const char *> {
	public:
		sqlrresultsettable();
		sqlrresultsettable(sqlrcursor *cursor);
		~sqlrresultsettable();

		void	setCursor(sqlrcursor *cursor);

		bool	getIsReadOnly() const;
		bool	getIsBlockBased() const;
		bool	getIsSequentialAccess() const;

		const char	*getColumnName(uint64_t col) const;
		uint64_t	getColCount() const;

		const char	*getValue(uint64_t row,
						uint64_t col) const;
		const char	*getValue(uint64_t row,
						const char *colname) const;

		uint64_t	getRowCount() const;
		uint64_t	getBlockSize() const;
		bool		getAllRowsAvailable() const;
	
		#include <sqlrelay/private/sqlrresultsettable.h>
};

class SQLRCLIENT_DLLSPEC sqlrresultsetdomnode : public cursordomnode {
	public:
		sqlrresultsetdomnode(dom *dom,
					domnode *nullnode,
					sqlrcursor *sqlrcur);
		sqlrresultsetdomnode(dom *dom,
					domnode *nullnode,
					const char *ns,
					sqlrcursor *sqlrcur);
		~sqlrresultsetdomnode();

		void		setSqlrCursor(sqlrcursor *sqlrcur);
		sqlrcursor	*getSqlrCursor();

		domnodetype	getType() const;
		const char	*getName() const;
		const char	*getValue() const;
		domnode		*getParent() const;
		uint64_t	getPosition() const;
		domnode		*getPreviousSibling() const;
		domnode		*getNextSibling() const;
		uint64_t	getChildCount() const;
		domnode		*getFirstChild() const;
		uint64_t	getAttributeCount() const;
		domnode		*getAttribute(const char *name) const;
		domnode		*getAttributeIgnoringCase(
						const char *name) const;
		domnode		*getAttribute(uint64_t position) const;
		bool		isNullNode() const;

		#include <sqlrelay/private/sqlrresultsetdomnode.h>
};

#endif
