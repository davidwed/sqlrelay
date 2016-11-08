// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/crypt.h>

class SQLRSERVER_DLLSPEC sqlrpwdenc_crypt : public sqlrpwdenc {
	public:
			sqlrpwdenc_crypt(xmldomnode *parameters);
		bool	oneWay();
		char	*encrypt(const char *value);
};

sqlrpwdenc_crypt::sqlrpwdenc_crypt(xmldomnode *parameters) :
					sqlrpwdenc(parameters) {
}

bool sqlrpwdenc_crypt::oneWay() {
	return true;
}

char *sqlrpwdenc_crypt::encrypt(const char *value) {

	// the first two characters of the result string
	// are the salt, so don't include them, if possible
	char	*encrypted=crypt::encrypt(value,getParameters()->
						getAttributeValue("salt"));
	if (charstring::length(encrypted)<2) {
		return encrypted;
	}
	char	*retval=charstring::duplicate(encrypted+2);
	delete[] encrypted;
	return retval;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrpwdenc *new_sqlrpwdenc_crypt(
						xmldomnode *parameters) {
		return new sqlrpwdenc_crypt(parameters);
	}
}
