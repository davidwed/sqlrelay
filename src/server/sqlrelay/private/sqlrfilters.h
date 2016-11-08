// Copyright (c) 2016  David Muse
// See the file COPYING for more information

	private:
		void	unloadFilters();
		void	loadFilter(xmldomnode *filter);

		const char	*libexecdir;
		bool		debug;

		singlylinkedlist< sqlrfilterplugin * >	tlist;
