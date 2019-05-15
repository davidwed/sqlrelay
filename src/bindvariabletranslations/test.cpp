// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class SQLRSERVER_DLLSPEC sqlrbindvariabletranslation_test :
					public sqlrbindvariabletranslation {
	public:
			sqlrbindvariabletranslation_test(
					sqlrservercontroller *cont,
					sqlrbindvariabletranslations *rs,
					domnode *parameters);
			~sqlrbindvariabletranslation_test();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur);
};

sqlrbindvariabletranslation_test::
	sqlrbindvariabletranslation_test(
				sqlrservercontroller *cont,
				sqlrbindvariabletranslations *rs,
				domnode *parameters) :
		sqlrbindvariabletranslation(cont,rs,parameters) {
}

sqlrbindvariabletranslation_test::
	~sqlrbindvariabletranslation_test() {
}

bool sqlrbindvariabletranslation_test::run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur) {
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrbindvariabletranslation
			*new_sqlrbindvariabletranslation_test(
					sqlrservercontroller *cont,
					sqlrbindvariabletranslations *rs,
					domnode *parameters) {
		return new sqlrbindvariabletranslation_test(cont,rs,parameters);
	}
}
