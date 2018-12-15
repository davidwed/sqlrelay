// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

	private:
		void	unload();
		void	loadFilter(domnode *filter,
				singlylinkedlist< sqlrfilterplugin * > *list);
		bool	run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				sqlrparser *sqlrp,
				const char *query,
				const char **err,
				int64_t *errn,
				singlylinkedlist< sqlrfilterplugin * > *list);

		sqlrfiltersprivate	*pvt;
