// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#ifndef SQLRPARSER
#define SQLRPARSER

#include <sqlrelay/private/sqlrserverdll.h>

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>
#include <rudiments/stringbuffer.h>

class SQLRSERVER_DLLSPEC sqlrparser {
	public:
			sqlrparser();
		virtual	~sqlrparser();

		virtual	bool	parse(const char *query);
		virtual	void	useTree(xmldom *tree);
		virtual	xmldom	*getTree();
		virtual	xmldom	*detachTree();

		virtual	bool	write(stringbuffer *output);
		virtual	bool	write(xmldomnode *node,
					stringbuffer *output,
					bool omitsiblings);
		virtual	bool	write(xmldomnode *node, 
					stringbuffer *output);
};

#endif
