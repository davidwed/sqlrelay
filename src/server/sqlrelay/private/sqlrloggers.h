// Copyright (c) 2016  David Muse
// See the file COPYING for more information

	private:
		void		unload();
		void		loadLogger(xmldomnode *logger);

		const char	*libexecdir;

		singlylinkedlist< sqlrloggerplugin * >	llist;
