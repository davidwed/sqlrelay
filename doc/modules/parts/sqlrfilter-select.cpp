#include <sqlrelay/sqlrserver.h>

class SQLRSERVER_DLLSPEC select : public sqlrfilter {
	public:
			select(sqlrfilters *sqlrfs,
					xmldomnode *parameters,
					bool debug);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char *query);
};

select::select(sqlrfilters *sqlrfs,
			xmldomnode *parameters,
			bool debug) : sqlrfilter(sqlrfs,parameters,debug) {
}

bool select::run(sqlrserverconnection *sqlrcon,
				sqlrservercursor *sqlrcur,
				const char *query) {
	return charstring::compareIgnoringCase(query,"select",6);
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrfilter *new_sqlrfilter_select(
							sqlrfilters *sqlrfs,
							xmldomnode *parameters,
							bool debug) {
		return new select(sqlrfs,parameters,debug);
	}
}
