// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class SQLRSERVER_DLLSPEC sqlrmoduledata_test : public sqlrmoduledata {
	public:
			sqlrmoduledata_test(domnode *parameters);
			~sqlrmoduledata_test();
};

sqlrmoduledata_test::sqlrmoduledata_test(domnode *parameters) :
					sqlrmoduledata(parameters) {
}

sqlrmoduledata_test::
	~sqlrmoduledata_test() {
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrmoduledata
			*new_sqlrmoduledata_test(domnode *parameters) {
		return new sqlrmoduledata_test(parameters);
	}
}
