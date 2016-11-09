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
		memorypool	*temptablepool;
		memorypool	*tempindexpool;
		dictionary< sqlrdatabaseobject *, char * >	temptablemap;
		dictionary< sqlrdatabaseobject *, char * >	tempindexmap;
