// Copyright (c) 1999-2018 David Muse
// See the COPYING file for more information.

#ifndef SQLRRESULTSETDOMNODE_H
#define SQLRRESULTSETDOMNODE_H

#include <sqlrelay/private/sqlrresultsetdomnodeincludes.h>

class sqlrresultsetdomnode : public cursordomnode {
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

	private:

		#include <sqlrelay/private/sqlrresultsetdomnode.h>
};

#endif
