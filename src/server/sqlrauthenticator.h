// Copyright (c) 2001  David Muse
// See the file COPYING for more information

#ifndef SQLRAUTHENTICATOR_H
#define SQLRAUTHENTICATOR_H

#include <sqlrserverdll.h>

#include <sqlrconfigfile.h>
#include <sqlrpwdencs.h>

class SQLRSERVER_DLLSPEC sqlrauthenticator {

	public:
			sqlrauthenticator(sqlrconfigfile *cfgfile,
						sqlrpwdencs *sqlrpe);
			~sqlrauthenticator();
		bool	authenticate(const char *user, const char *password);

	private:
		linkedlist< usercontainer * >	userlist;

		uint32_t	usercount;
		char		**users;
		char		**passwords;
		char		**passwordencryptions;

		sqlrpwdencs	*sqlrpe;
};

#endif
