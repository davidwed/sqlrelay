// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

	private:
		void	unload();
		sqlrtriggerplugin	*loadTrigger(domnode *trigger);
		void	run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				singlylinkedlist< sqlrtriggerplugin * > *list,
				bool before,
				bool *success);

		sqlrtriggersprivate	*pvt;
