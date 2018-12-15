// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class SQLRSERVER_DLLSPEC sqlrresultsetrowtranslation_test :
					public sqlrresultsetrowtranslation {
	public:
			sqlrresultsetrowtranslation_test(
					sqlrservercontroller *cont,
					sqlrresultsetrowtranslations *rs,
					domnode *parameters);
			~sqlrresultsetrowtranslation_test();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames,
					const char ***fields,
					uint64_t **fieldlengths);
};

sqlrresultsetrowtranslation_test::
	sqlrresultsetrowtranslation_test(
				sqlrservercontroller *cont,
				sqlrresultsetrowtranslations *rs,
				domnode *parameters) :
		sqlrresultsetrowtranslation(cont,rs,parameters) {
}

sqlrresultsetrowtranslation_test::
	~sqlrresultsetrowtranslation_test() {
}

bool sqlrresultsetrowtranslation_test::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames,
					const char ***fields,
					uint64_t **fieldlengths) {
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrresultsetrowtranslation
			*new_sqlrresultsetrowtranslation_test(
					sqlrservercontroller *cont,
					sqlrresultsetrowtranslations *rs,
					domnode *parameters) {
		return new sqlrresultsetrowtranslation_test(cont,rs,parameters);
	}
}
