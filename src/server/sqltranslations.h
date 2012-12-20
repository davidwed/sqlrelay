// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#ifndef SQLTRANSLATOR_H
#define SQLTRANSLATOR_H

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>
#include <rudiments/memorypool.h>
#include <rudiments/dictionary.h>
#include <rudiments/dynamiclib.h>
#include <sqltranslation.h>

class sqlrconnection_svr;
class sqlrcursor_svr;

class databaseobject {
	public:
		const char	*database;
		const char	*schema;
		const char	*object;
		const char	*dependency;
};

class sqltranslationplugin {
	public:
		sqltranslation		*tr;
		rudiments::dynamiclib	*dl;
};

class sqltranslations {
	public:
			sqltranslations();
			~sqltranslations();

		bool	loadTranslations(const char *translations);
		bool	runTranslations(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						rudiments::xmldom *querytree);

		bool	getReplacementTableName(const char *database,
						const char *schema,
						const char *oldname,
						const char **newname);
		bool	getReplacementIndexName(const char *database,
						const char *schema,
						const char *oldname,
						const char **newname);
		databaseobject *createDatabaseObject(
						rudiments::memorypool *pool,
						const char *database,
						const char *schema,
						const char *object,
						const char *dependency);
		bool	removeReplacementTable(const char *database,
						const char *schema,
						const char *table);
		bool	removeReplacementIndex(const char *database,
						const char *schema,
						const char *index);

		void	endSession();
	private:
		void	unloadTranslations();
		void	loadTranslation(rudiments::xmldomnode *translation);

		bool	getReplacementName(
				rudiments::dictionary< databaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *oldname,
				const char **newname);
		bool	removeReplacement(
				rudiments::dictionary< databaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *oldname);
		
		rudiments::xmldom	*xmld;
		rudiments::xmldom	*tree;

		rudiments::linkedlist< sqltranslationplugin * >	tlist;


	public:
		// helper methods
		rudiments::xmldomnode	*newNode(rudiments::xmldomnode *parentnode,
							const char *type);
		rudiments::xmldomnode	*newNode(rudiments::xmldomnode *parentnode,
							const char *type,
							const char *value);
		rudiments::xmldomnode	*newNodeAfter(rudiments::xmldomnode *parentnode,
							rudiments::xmldomnode *node,
							const char *type);
		rudiments::xmldomnode	*newNodeAfter(rudiments::xmldomnode *parentnode,
							rudiments::xmldomnode *node,
							const char *type,
							const char *value);
		rudiments::xmldomnode	*newNodeBefore(rudiments::xmldomnode *parentnode,
							rudiments::xmldomnode *node,
							const char *type);
		rudiments::xmldomnode	*newNodeBefore(rudiments::xmldomnode *parentnode,
							rudiments::xmldomnode *node,
							const char *type,
							const char *value);
		void		setAttribute(rudiments::xmldomnode *node,
							const char *name,
							const char *value);
		bool		isString(const char *value);

		rudiments::memorypool	*temptablepool;
		rudiments::memorypool	*tempindexpool;
		rudiments::dictionary< databaseobject *, char * >	temptablemap;
		rudiments::dictionary< databaseobject *, char * >	tempindexmap;
};

#endif
