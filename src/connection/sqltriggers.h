// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLTRIGGERS_H
#define SQLTRIGGERS_H

#include <rudiments/xmldom.h>
#include <rudiments/linkedlist.h>
#include <rudiments/dynamiclib.h>
#include <sqltrigger.h>

class sqlrconnection_svr;
class sqlrcursor_svr;

class sqltriggerplugin {
	public:
		sqltrigger		*tr;
		rudiments::dynamiclib	*dl;
};

class sqltriggers {
	public:
			sqltriggers();
			~sqltriggers();

		bool	loadTriggers(const char *triggers);
		void	runBeforeTriggers(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						rudiments::xmldom *querytree);
		void	runAfterTriggers(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						rudiments::xmldom *querytree,
						bool success);
	private:
		void		unloadTriggers();
		void		loadTrigger(rudiments::xmldomnode *trigger,
					rudiments::linkedlist< sqltriggerplugin *> *list);
		void		runTriggers(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldom *querytree,
					rudiments::linkedlist< sqltriggerplugin * > *list,
					bool before,
					bool success);

		rudiments::xmldom				*xmld;
		rudiments::linkedlist< sqltriggerplugin * >	beforetriggers;
		rudiments::linkedlist< sqltriggerplugin * >	aftertriggers;
};

#endif
