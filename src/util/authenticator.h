// Copyright (c) 2001  David Muse
// See the file COPYING for more information

#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <sqlrconfigfile.h>

class authenticator {

	public:
			authenticator(sqlrconfigfile *cfgfile);
			~authenticator();
		int	authenticate(const char *user, const char *password);

	private:
		list< usercontainer * >	userlist;
		unsigned long		usercount;
		char			**users;
		char			**passwords;
};

#endif
