// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>
#include <rudiments/des.h>

class SQLRSERVER_DLLSPEC sqlrpwdenc_des : public sqlrpwdenc {
	public:
			sqlrpwdenc_des(domnode *parameters, bool debug);
		bool	oneWay();
		char	*encrypt(const char *value);

		class des	d;
};

sqlrpwdenc_des::sqlrpwdenc_des(domnode *parameters, bool debug) :
						sqlrpwdenc(parameters,debug) {
}

bool sqlrpwdenc_des::oneWay() {
	return true;
}

char *sqlrpwdenc_des::encrypt(const char *value) {

	d.setIv((const unsigned char *)
			getParameters()->getAttributeValue("salt"),
			d.getIvSize());

	d.append((const unsigned char *)value,
			charstring::length(value));

	const char	*encrypted=(const char *)d.getEncryptedData();

	// the first two characters of the result string are the salt,
	// so don't include them in the result, if possible
	return (charstring::length(encrypted)<2)?
			charstring::duplicate(encrypted):
			charstring::duplicate(encrypted+2);
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrpwdenc *new_sqlrpwdenc_des(
						domnode *parameters,
						bool debug) {
		return new sqlrpwdenc_des(parameters,debug);
	}
}
