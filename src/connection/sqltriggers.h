// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLTRIGGERS_H
#define SQLTRIGGERS_H

#include <rudiments/xmldom.h>
#include <rudiments/linkedlist.h>
#include <rudiments/dynamiclib.h>
#include <sqltrigger.h>

using namespace rudiments;

class sqlrconnection_svr;
class sqlrcursor_svr;

class sqltriggerplugin {
	public:
		sqltrigger	*tr;
		dynamiclib	*dl;
};

class sqltriggers {
	public:
			sqltriggers();
			~sqltriggers();

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
					linkedlist< sqltriggerplugin *> *list);
		void		runTriggers(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree,
					linkedlist< sqltriggerplugin * > *list,
					bool before,
					bool success);

		xmldom				*xmld;
		linkedlist< sqltriggerplugin * >	beforetriggers;
		linkedlist< sqltriggerplugin * >	aftertriggers;
};

#endif
