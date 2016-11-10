// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/linkedlist.h>
#include <rudiments/regularexpression.h>

class SQLRSERVER_DLLSPEC sqlrrouter_regex : public sqlrrouter {
	public:
			sqlrrouter_regex(sqlrservercontroller *cont,
						xmldomnode *parameters);
			~sqlrrouter_regex();

		const char	*route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);
	private:
		linkedlist< regularexpression * >	relist;

		const char	*connectionid;

		bool	enabled;

		bool	debug;
};

sqlrrouter_regex::sqlrrouter_regex(sqlrservercontroller *cont,
					xmldomnode *parameters) :
						sqlrrouter(cont,parameters) {
	debug=cont->getConfig()->getDebugRouters();
	enabled=charstring::compareIgnoringCase(
			parameters->getAttributeValue("enabled"),"no");
	if (!enabled && debug) {
		stdoutput.printf("	disabled\n");
		return;
	}

	connectionid=parameters->getAttributeValue("connectionid");

	for (xmldomnode *pn=parameters->getFirstTagChild("pattern");
				!pn->isNullNode();
				pn=pn->getNextTagSibling("pattern")) {

		const char	*pattern=pn->getAttributeValue("pattern");
		if (debug) {
			stdoutput.printf("	pattern: \"%s\"\n",pattern);
		}

		regularexpression	*re=new regularexpression;
		re->compile(pattern);
		re->study();
		relist.append(re);
	}
	if (debug && !relist.getLength()) {
		stdoutput.printf("	WARNING! no patterns found\n");
	}
}

sqlrrouter_regex::~sqlrrouter_regex() {
	for (linkedlistnode< regularexpression *> *rn=relist.getFirst();
							rn; rn=rn->getNext()) {
		delete rn->getValue();
	}
}

const char *sqlrrouter_regex::route(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur) {
	if (!enabled) {
		return NULL;
	}

	const char	*query=sqlrcur->getQueryBuffer();
	for (linkedlistnode< regularexpression *> *rn=relist.getFirst();
							rn; rn=rn->getNext()) {
		if (rn->getValue()->match(query)) {
			if (debug) {
				stdoutput.printf("\nrouting query:\n"
							"	%s\nto: %s\n",
							query,connectionid);
			}
			return connectionid;
		}
	}
	return NULL;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrrouter *new_sqlrrouter_regex(
						sqlrservercontroller *cont,
						xmldomnode *parameters) {
		return new sqlrrouter_regex(cont,parameters);
	}
}
