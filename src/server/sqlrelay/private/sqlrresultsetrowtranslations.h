// Copyright (c) 2016  David Muse
// See the file COPYING for more information

	private:
		void	unload();
		void	loadResultSetRowTranslation(
					xmldomnode *resultsetrowtranslation);

		const char	*libexecdir;
		bool		debug;

		singlylinkedlist< sqlrresultsetrowtranslationplugin * >	tlist;
