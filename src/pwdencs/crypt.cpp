// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrpwdenc.h>
#include <rudiments/charstring.h>
#include <rudiments/crypt.h>

using namespace rudiments;

class crypt_pwdenc : public sqlrpwdenc {
	public:
			crypt_pwdenc(xmldomnode *parameters);
		bool	oneWay();
		char	*encrypt(const char *value);
};

crypt_pwdenc::crypt_pwdenc(xmldomnode *parameters) : sqlrpwdenc(parameters) {
}

bool crypt_pwdenc::oneWay() {
	return true;
}

char *crypt_pwdenc::encrypt(const char *value) {
	return crypt::encrypt(value,parameters->getAttributeValue("salt"));
}

extern "C" {
	sqlrpwdenc *new_crypt(xmldomnode *parameters) {
		return new crypt_pwdenc(parameters);
	}
}
