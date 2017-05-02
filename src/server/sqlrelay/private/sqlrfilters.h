// Copyright (c) 2016  David Muse
// See the file COPYING for more information

	private:
		void	unload();
		void	loadFilter(xmldomnode *filter,
				singlylinkedlist< sqlrfilterplugin * > *list);
		bool	run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				sqlrparser *sqlrp,
				const char *query,
				const char **err,
				int64_t *errn,
				singlylinkedlist< sqlrfilterplugin * > *list);

		sqlrfiltersprivate	*pvt;
