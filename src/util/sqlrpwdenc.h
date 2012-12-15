// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRPWDENC_H
#define SQLRPWDENC_H

#include <rudiments/xmldomnode.h>

class sqlrpwdenc {
	public:
			sqlrpwdenc(const char *id);
		virtual	~sqlrpwdenc();
		virtual const char	*getId();
		virtual	bool	oneWay();
		virtual	char	*encrypt(const char *value,
					rudiments::xmldomnode *parameters);
		virtual	char	*decrypt(const char *value,
					rudiments::xmldomnode *parameters);
	protected:
		char	*id;
};

#endif
