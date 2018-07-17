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
					domnode *parameters);
			~sqlrresultsetrowtranslation_test();
		bool	setRow(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char * const *fieldnames,
						const char **fields,
						uint64_t *fieldlengths,
						bool *blobs,
						bool *nulls);
		bool	run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char * const *fieldnames);
		bool	getRow(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char ***fields,
						uint64_t **fieldlengths,
						bool **blobs,
						bool **nulls);
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

bool sqlrresultsetrowtranslation_test::setRow(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char * const *fieldnames,
						const char **fields,
						uint64_t *fieldlengths,
						bool *blobs,
						bool *nulls) {
	return true;
}

bool sqlrresultsetrowtranslation_test::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames) {
	return true;
}

bool sqlrresultsetrowtranslation_test::getRow(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char ***fields,
						uint64_t **fieldlengths,
						bool **blobs,
						bool **nulls) {
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
