// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#ifndef SQLTRANSLATOR_H
#define SQLTRANSLATOR_H

#include <sqlrserverdll.h>

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>
#include <rudiments/memorypool.h>
#include <rudiments/dictionary.h>
#include <rudiments/dynamiclib.h>
#include <sqltranslation.h>

class sqlrconnection_svr;
class sqlrcursor_svr;

class SQLRSERVER_DLLSPEC databaseobject {
	public:
		const char	*database;
		const char	*schema;
		const char	*object;
		const char	*dependency;
};

class SQLRSERVER_DLLSPEC sqltranslationplugin {
	public:
		sqltranslation	*tr;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqltranslations {
	public:
			sqltranslations();
			~sqltranslations();

		bool	loadTranslations(const char *translations);
		bool	runTranslations(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldom *querytree);

		bool	getReplacementTableName(const char *database,
						const char *schema,
						const char *oldname,
						const char **newname);
		bool	getReplacementIndexName(const char *database,
						const char *schema,
						const char *oldname,
						const char **newname);
		databaseobject *createDatabaseObject(
						memorypool *pool,
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
		void	loadTranslation(xmldomnode *translation);

		bool	getReplacementName(
				dictionary< databaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *oldname,
				const char **newname);
		bool	removeReplacement(
				dictionary< databaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *oldname);
		
		xmldom	*xmld;
		xmldom	*tree;

		linkedlist< sqltranslationplugin * >	tlist;


	public:
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
		xmldomnode	*newNodeBefore(xmldomnode *parentnode,
							xmldomnode *node,
							const char *type);
		xmldomnode	*newNodeBefore(xmldomnode *parentnode,
							xmldomnode *node,
							const char *type,
							const char *value);
		void		setAttribute(xmldomnode *node,
							const char *name,
							const char *value);
		bool		isString(const char *value);

		memorypool	*temptablepool;
		memorypool	*tempindexpool;
		dictionary< databaseobject *, char * >	temptablemap;
		dictionary< databaseobject *, char * >	tempindexmap;
};

#endif
