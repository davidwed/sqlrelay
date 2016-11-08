// Copyright (c) 2016  David Muse
// See the file COPYING for more information

	private:
		void	unload();
		void	loadAuth(xmldomnode *auth, sqlrpwdencs *sqlrpe);

		const char	*libexecdir;
		bool		debug;

		singlylinkedlist< sqlrauthplugin * >	llist;
