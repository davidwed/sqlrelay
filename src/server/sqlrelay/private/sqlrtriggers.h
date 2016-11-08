// Copyright (c) 2016  David Muse
// See the file COPYING for more information

	private:
		void	unload();
		void	loadTrigger(xmldomnode *trigger,
					singlylinkedlist< sqlrtriggerplugin *>
					*list);
		void	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree,
					singlylinkedlist< sqlrtriggerplugin * >
					*list,
					bool before,
					bool success);

		sqlrtriggersprivate	*pvt;
