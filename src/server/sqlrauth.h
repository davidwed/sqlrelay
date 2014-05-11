// Copyright (c) 2014  David Muse
// See the file COPYING for more information

#ifndef SQLRAUTH_H
#define SQLRAUTH_H

#include <sqlrserverdll.h>

#include <sqlrpwdencs.h>
#include <rudiments/xmldomnode.h>

class SQLRSERVER_DLLSPEC sqlrauth {
	public:
			sqlrauth(xmldomnode *parameters,
					sqlrpwdencs *sqlrpe);
		virtual	~sqlrauth();
		virtual	bool	authenticate(const char *user,
						const char *password);
	protected:
		xmldomnode		*parameters;
		sqlrpwdencs		*sqlrpe;
};

#endif
