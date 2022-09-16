// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/crypt.h>

class SQLRSERVER_DLLSPEC sqlrpwdenc_crypt : public sqlrpwdenc {
	public:
			sqlrpwdenc_crypt(domnode *parameters, bool debug);
		bool	oneWay();
		char	*encrypt(const char *value);

		crypt	c;
};

sqlrpwdenc_crypt::sqlrpwdenc_crypt(domnode *parameters, bool debug) :
						sqlrpwdenc(parameters,debug) {
}

bool sqlrpwdenc_crypt::oneWay() {
	return true;
}

char *sqlrpwdenc_crypt::encrypt(const char *value) {

	c.setIv((const unsigned char *)
			getParameters()->getAttributeValue("salt"),
			c.getIvSize());

	c.append((const unsigned char *)value,
			charstring::length(value));

	const char	*encrypted=(const char *)c.getEncryptedData();

	// the first two characters of the result string are the salt,
	// so don't include them in the result, if possible
	return (charstring::length(encrypted)<2)?
			charstring::duplicate(encrypted):
			charstring::duplicate(encrypted+2);
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrpwdenc *new_sqlrpwdenc_crypt(
						domnode *parameters,
						bool debug) {
		return new sqlrpwdenc_crypt(parameters,debug);
	}
}
