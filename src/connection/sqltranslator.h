// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#ifndef SQLTRANSLATOR_H
#define SQLTRANSLATOR_H

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

using namespace rudiments;

class sqltranslator {
	public:
			sqltranslator();
		virtual	~sqltranslator();

		virtual bool	loadRules(const char *rules);
		virtual bool	applyRules(xmldom *querytree);
	protected:
		virtual bool	applyRulesToQuery(xmldomnode *query);
		virtual bool	nativizeDatatypes(xmldomnode *query,
							xmldomnode *rule);
		virtual bool	convertDatatypes(xmldomnode *query,
							xmldomnode *rule);
		virtual bool	trimColumnsComparedToStringBinds(
							xmldomnode *query,
							xmldomnode *rule);

		// helper methods
		xmldomnode	*newNode(xmldomnode *parentnode,
						const char *type);
		xmldomnode	*newNode(xmldomnode *parentnode,
						const char *type,
						const char *value);
		xmldomnode	*newNodeAfter(xmldomnode *parentnode,
						xmldomnode *node,
						const char *type);
		xmldomnode	*newNodeAfter(xmldomnode *parentnode,
						xmldomnode *node,
						const char *type,
						const char *value);
		void		setAttribute(xmldomnode *node,
						const char *name,
						const char *value);

		xmldomnode	*rulesnode;
	private:
		xmldom		*xmld;
		xmldom		*tree;
};

#endif
