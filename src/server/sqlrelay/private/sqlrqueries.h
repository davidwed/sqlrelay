// Copyright (c) 2016  David Muse
// See the file COPYING for more information

	private:
		void	unload();
		void	loadQuery(xmldomnode *logger);

		const char	*libexecdir;

		singlylinkedlist< sqlrqueryplugin * >	llist;
