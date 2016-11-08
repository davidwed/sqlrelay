// Copyright (c) 2016  David Muse
// See the file COPYING for more information

	private:
		void	unload();
		void	loadResultSetTranslation(
					xmldomnode *resultsettranslation);

		const char	*libexecdir;
		bool		debug;

		singlylinkedlist< sqlrresultsettranslationplugin * >	tlist;
