// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class SQLRSERVER_DLLSPEC sqlrresultsetrowblocktranslation_test :
				public sqlrresultsetrowblocktranslation {
	public:
		sqlrresultsetrowblocktranslation_test(
					sqlrservercontroller *cont,
					domnode *parameters);
		~sqlrresultsetrowblocktranslation_test();
		bool	setRow(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						uint32_t colcount,
						const char * const *fieldnames,
						const char **fields,
						uint64_t *fieldlengths,
						bool *lobs,
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
						bool **lobs,
						bool **nulls);
};

sqlrresultsetrowblocktranslation_test::
	sqlrresultsetrowblocktranslation_test(
				sqlrservercontroller *cont,
				domnode *parameters) :
		sqlrresultsetrowblocktranslation(cont,parameters) {
}

sqlrresultsetrowblocktranslation_test::
	~sqlrresultsetrowblocktranslation_test() {
}

bool sqlrresultsetrowblocktranslation_test::setRow(
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames,
					const char **fields,
					uint64_t *fieldlengths,
					bool *lobs,
					bool *nulls) {
	return true;
}

bool sqlrresultsetrowblocktranslation_test::run(
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char * const *fieldnames) {
	return true;
}

bool sqlrresultsetrowblocktranslation_test::getRow(
					sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char ***fields,
					uint64_t **fieldlengths,
					bool **lobs,
					bool **nulls) {
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrresultsetrowblocktranslation
			*new_sqlrresultsetrowblocktranslation_test(
					sqlrservercontroller *cont,
					domnode *parameters) {
		return new sqlrresultsetrowblocktranslation_test(
							cont,parameters);
	}
}
