// Code below Copyright (c) 2012-2016 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/md5.h>
#include <rudiments/charstring.h>

class SQLRSERVER_DLLSPEC sqlrpwenc_md5 : public sqlrpwdenc {
	public:
			sqlrpwenc_md5(xmldomnode *parameters, bool debug);
		bool	oneWay();
		char	*encrypt(const char *value);
};

sqlrpwenc_md5::sqlrpwenc_md5(xmldomnode *parameters, bool debug) :
						sqlrpwdenc(parameters,debug) {
}

bool sqlrpwenc_md5::oneWay() {
	return true;
}

char *sqlrpwenc_md5::encrypt(const char *value) {
	md5	m;
	m.append((const unsigned char *)value,charstring::length(value));
	return charstring::duplicate((const char *)m.getHash());
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrpwdenc *new_sqlrpwdenc_md5(
						xmldomnode *parameters,
						bool debug) {
		return new sqlrpwenc_md5(parameters,debug);
	}
}
