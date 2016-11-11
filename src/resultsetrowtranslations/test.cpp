// Copyright (c) 2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

class SQLRSERVER_DLLSPEC sqlrresultsetrowtranslation_test :
					public sqlrresultsetrowtranslation {
	public:
			sqlrresultsetrowtranslation_test(
					sqlrservercontroller *cont,
					sqlrresultsetrowtranslations *rs,
					xmldomnode *parameters);
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
				xmldomnode *parameters) :
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
					xmldomnode *parameters) {
		return new sqlrresultsetrowtranslation_test(cont,rs,parameters);
	}
}
