// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLTRIGGERS_H
#define SQLTRIGGERS_H

#include <rudiments/xmldom.h>
#include <rudiments/linkedlist.h>
#include <sqltrigger.h>

using namespace rudiments;

class sqlrconnection_svr;
class sqlrcursor_svr;

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
		sqltrigger	*loadTrigger(xmldomnode *trigger);
		void		runTriggers(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree,
					linkedlist< sqltrigger * > *list,
					bool before,
					bool success);

		xmldom				*xmld;
		linkedlist< sqltrigger * >	beforetriggers;
		linkedlist< sqltrigger * >	aftertriggers;
};

#endif
