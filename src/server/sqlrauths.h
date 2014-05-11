// Copyright (c) 2014  David Muse
// See the file COPYING for more information

#ifndef SQLRAUTHS_H
#define SQLRAUTHS_H

#include <sqlrserverdll.h>

#include <rudiments/xmldom.h>
#include <rudiments/linkedlist.h>
#include <rudiments/dynamiclib.h>
#include <sqlrpwdencs.h>
#include <sqlrauth.h>

class SQLRSERVER_DLLSPEC sqlrauthplugin {
	public:
		sqlrauth	*au;
		dynamiclib	*dl;
};

class SQLRSERVER_DLLSPEC sqlrauths {
	public:
			sqlrauths();
			~sqlrauths();

		bool	loadAuthenticators(const char *auths,
						sqlrpwdencs *sqlrpe);
		bool	authenticate(const char *user, const char *password);
	private:
		void	unloadAuthenticators();
		void	loadAuthenticator(xmldomnode *auth,
						sqlrpwdencs *sqlrpe);

		xmldom				*xmld;
		linkedlist< sqlrauthplugin * >	llist;
};

#endif
