// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRTRIGGERS_H
#define SQLRTRIGGERS_H

#include <sqlrelay/private/sqlrserverdll.h>

#include <rudiments/xmldom.h>
#include <rudiments/singlylinkedlist.h>
#include <rudiments/dynamiclib.h>
#include <sqlrelay/sqlrtrigger.h>

class sqlrserverconnection;
class sqlrservercursor;

class SQLRSERVER_DLLSPEC sqlrtriggerplugin {
	public:
		sqlrtrigger	*tr;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrtriggers {
	public:
			sqlrtriggers(bool debug);
			~sqlrtriggers();

		bool	loadTriggers(const char *triggers);
		void	runBeforeTriggers(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldom *querytree);
		void	runAfterTriggers(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						xmldom *querytree,
						bool success);
	private:
		void		unloadTriggers();
		void		loadTrigger(xmldomnode *trigger,
					singlylinkedlist< sqlrtriggerplugin *>
					*list);
		void		runTriggers(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree,
					singlylinkedlist< sqlrtriggerplugin * >
					*list,
					bool before,
					bool success);

		xmldom					*xmld;
		bool					debug;
		singlylinkedlist< sqlrtriggerplugin * >	beforetriggers;
		singlylinkedlist< sqlrtriggerplugin * >	aftertriggers;
};

#endif
