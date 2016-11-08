// Copyright (c) 2016  David Muse
// See the file COPYING for more information

	private:
		void		unload();
		void		loadSchedule(xmldomnode *schedule);

		const char	*libexecdir;
		bool		debug;

		singlylinkedlist< sqlrscheduleplugin * >	llist;
