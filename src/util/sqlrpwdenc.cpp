// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrpwdenc.h>
#include <rudiments/charstring.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

sqlrpwdenc::sqlrpwdenc(const char *id) {
	id=charstring::duplicate(id);
}

sqlrpwdenc::~sqlrpwdenc() {
	delete[] id;
}

const char *sqlrpwdenc::getId() {
	return id;
}

bool sqlrpwdenc::oneWay() {
	return false;
}

char *sqlrpwdenc::encrypt(const char *value, xmldomnode *parameters) {
	return NULL;
}

char *sqlrpwdenc::decrypt(const char *value, xmldomnode *parameters) {
	return NULL;
}
