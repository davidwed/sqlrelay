// Copyright (c) 2001  David Muse
// See the file COPYING for more information

#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <sqlrconfigfile.h>

class authenticator {

	public:
			authenticator(sqlrconfigfile *cfgfile);
			~authenticator();
		bool	authenticate(const char *user, const char *password);

	private:
		rudiments::linkedlist< usercontainer * >	userlist;

		uint32_t	usercount;
		char		**users;
		char		**passwords;
};

#endif
