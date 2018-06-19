// Copyright (c) 2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

class SQLRSERVER_DLLSPEC sqlrresultsetheadertranslation_test :
					public sqlrresultsetheadertranslation {
	public:
			sqlrresultsetheadertranslation_test(
					sqlrservercontroller *cont,
					sqlrresultsetheadertranslations *rs,
					domnode *parameters);
			~sqlrresultsetheadertranslation_test();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames,
					const char ***fields,
					uint64_t **fieldlengths);
};

sqlrresultsetheadertranslation_test::
	sqlrresultsetheadertranslation_test(
				sqlrservercontroller *cont,
				sqlrresultsetheadertranslations *rs,
				domnode *parameters) :
		sqlrresultsetheadertranslation(cont,rs,parameters) {
}

sqlrresultsetheadertranslation_test::
	~sqlrresultsetheadertranslation_test() {
}

bool sqlrresultsetheadertranslation_test::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames,
					const char ***fields,
					uint64_t **fieldlengths) {
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrresultsetheadertranslation
			*new_sqlrresultsetheadertranslation_test(
					sqlrservercontroller *cont,
					sqlrresultsetheadertranslations *rs,
					domnode *parameters) {
		return new sqlrresultsetheadertranslation_test(
						cont,rs,parameters);
	}
}
