// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

	private:
		void	unload();
		void	loadTranslation(domnode *translation);

		sqlrdatabaseobject *createDatabaseObject(
						const char *database,
						const char *schema,
						const char *object,
						const char *dependency);

		void	setReplacementName(
				dictionary< sqlrdatabaseobject *, char *> *dict,
				sqlrdatabaseobject *oldobject,
				const char *newobject);
		bool	getReplacementName(
				dictionary< sqlrdatabaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *oldobject,
				const char **newobject);
		bool	removeReplacement(
				dictionary< sqlrdatabaseobject *, char *> *dict,
				const char *database,
				const char *schema,
				const char *oldobject);

		sqlrtranslationsprivate	*pvt;
