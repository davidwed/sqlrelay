// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/linkedlist.h>
#include <rudiments/regularexpression.h>

class SQLRSERVER_DLLSPEC sqlrrouter_regex : public sqlrrouter {
	public:
			sqlrrouter_regex(xmldomnode *parameters,
							bool debug);
			~sqlrrouter_regex();

		const char	*route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);
	private:
		linkedlist< regularexpression * >	relist;

		const char	*connectionid;

		bool	enabled;
};

sqlrrouter_regex::sqlrrouter_regex(xmldomnode *parameters, bool debug) :
						sqlrrouter(parameters,debug) {
	enabled=charstring::compareIgnoringCase(
			parameters->getAttributeValue("enabled"),"no");

	if (!enabled) {
		return;
	}

	connectionid=parameters->getAttributeValue("connectionid");

	for (xmldomnode *pn=parameters->getFirstTagChild("pattern");
				!pn->isNullNode();
				pn=pn->getNextTagSibling("pattern")) {
		regularexpression	*re=new regularexpression;
		re->compile(parameters->getAttributeValue("pattern"));
		re->study();
		relist.append(re);
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

	for (linkedlistnode< regularexpression *> *rn=relist.getFirst();
							rn; rn=rn->getNext()) {
		if (rn->getValue()->match(sqlrcur->getQueryBuffer())) {
			return connectionid;
		}
	}
	return NULL;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrrouter *new_sqlrrouter_regex(
						xmldomnode *parameters,
						bool debug) {
		return new sqlrrouter_regex(parameters,debug);
	}
}
