// Copyright (c) 2016  David Muse
// See the file COPYING for more information

	private:
		void	unload();
		void	loadTranslation(xmldomnode *translation);

		bool	getReplacementName(
				dictionary< sqlrdatabaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *oldname,
				const char **newname);
		bool	removeReplacement(
				dictionary< sqlrdatabaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *oldname);

		sqlrtranslationsprivate	*pvt;

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
		dictionary< sqlrdatabaseobject *, char * >	temptablemap;
		dictionary< sqlrdatabaseobject *, char * >	tempindexmap;
