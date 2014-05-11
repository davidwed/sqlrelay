// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrpwdenc.h>
#include <rudiments/charstring.h>
#include <rudiments/crypt.h>

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

	// the first two characters of the result string
	// are the salt, so don't include them, if possible
	char	*encrypted=crypt::encrypt(value,
					parameters->getAttributeValue("salt"));
	if (charstring::length(encrypted)<2) {
		return encrypted;
	}
	char	*retval=charstring::duplicate(encrypted+2);
	delete[] encrypted;
	return retval;
}

extern "C" {
	sqlrpwdenc *new_crypt(xmldomnode *parameters) {
		return new crypt_pwdenc(parameters);
	}
}
