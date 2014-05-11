// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRTRIGGERS_H
#define SQLRTRIGGERS_H

#include <sqlrelay/sqlrserverdll.h>

#include <rudiments/xmldom.h>
#include <rudiments/linkedlist.h>
#include <rudiments/dynamiclib.h>
#include <sqlrelay/sqlrtrigger.h>

class sqlrconnection_svr;
class sqlrcursor_svr;

class SQLRSERVER_DLLSPEC sqlrtriggerplugin {
	public:
		sqlrtrigger	*tr;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrtriggers {
	public:
			sqlrtriggers();
			~sqlrtriggers();

		bool	loadTriggers(const char *triggers);
		void	runBeforeTriggers(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldom *querytree);
		void	runAfterTriggers(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						xmldom *querytree,
						bool success);
	private:
		void		unloadTriggers();
		void		loadTrigger(xmldomnode *trigger,
					linkedlist< sqlrtriggerplugin *> *list);
		void		runTriggers(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree,
					linkedlist< sqlrtriggerplugin * > *list,
					bool before,
					bool success);

		xmldom					*xmld;
		linkedlist< sqlrtriggerplugin * >	beforetriggers;
		linkedlist< sqlrtriggerplugin * >	aftertriggers;
};

#endif