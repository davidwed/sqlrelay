// Copyright (c) 2001  David Muse
// See the file COPYING for more information

#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <sqlrconfigfile.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class authenticator {

	public:
			authenticator(sqlrconfigfile *cfgfile);
			~authenticator();
		bool	authenticate(const char *user, const char *password);

	private:
		linkedlist< usercontainer * >	userlist;
		unsigned long			usercount;
		char			**users;
		char			**passwords;
};

#endif
