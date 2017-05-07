// Copyright (c) 2017  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/linkedlist.h>
#include <rudiments/regularexpression.h>

class SQLRSERVER_DLLSPEC sqlrrouter_usedatabase : public sqlrrouter {
	public:
			sqlrrouter_usedatabase(sqlrservercontroller *cont,
						sqlrrouters *rs,
						xmldomnode *parameters,
						const char **connectionids,
						sqlrconnection **connections,
						uint16_t connectioncount);
			~sqlrrouter_usedatabase();

		const char	*route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);
	private:
		bool	enabled;

		bool	debug;
};

sqlrrouter_usedatabase::sqlrrouter_usedatabase(sqlrservercontroller *cont,
						sqlrrouters *rs,
						xmldomnode *parameters,
						const char **connectionids,
						sqlrconnection **connections,
						uint16_t connectioncount) :
					sqlrrouter(cont,rs,parameters,
							connectionids,
							connections,
							connectioncount) {
	debug=cont->getConfig()->getDebugRouters();
	enabled=charstring::compareIgnoringCase(
			parameters->getAttributeValue("enabled"),"no");
	if (!enabled && debug) {
		stdoutput.printf("	disabled\n");
		return;
	}

}

sqlrrouter_usedatabase::~sqlrrouter_usedatabase() {
}

const char *sqlrrouter_usedatabase::route(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur) {
	if (!enabled || !sqlrcon || !sqlrcur) {
		return NULL;
	}

	return NULL;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrrouter *new_sqlrrouter_usedatabase(
						sqlrservercontroller *cont,
						sqlrrouters *rs,
						xmldomnode *parameters,
						const char **connectionids,
						sqlrconnection **connections,
						uint16_t connectioncount) {
		return new sqlrrouter_usedatabase(cont,rs,parameters,
							connectionids,
							connections,
							connectioncount);
	}
}
