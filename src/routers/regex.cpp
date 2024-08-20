// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/linkedlist.h>
#include <rudiments/regularexpression.h>

class SQLRSERVER_DLLSPEC sqlrrouter_regex : public sqlrrouter {
	public:
		sqlrrouter_regex(sqlrservercontroller *cont,
						sqlrrouters *rs,
						domnode *parameters);

		const char	*route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char **err,
						int64_t *errn);
	private:
		linkedlist< regularexpression * >	relist;

		const char	*connid;
};

sqlrrouter_regex::sqlrrouter_regex(sqlrservercontroller *cont,
						sqlrrouters *rs,
						domnode *parameters) :
					sqlrrouter(cont,rs,parameters) {
	relist.setManageValues(true);

	connid=parameters->getAttributeValue("connectionid");

	debugStart("patterns");
	for (domnode *pn=parameters->getFirstTagChild("pattern");
				!pn->isNullNode();
				pn=pn->getNextTagSibling("pattern")) {

		const char	*pattern=pn->getAttributeValue("pattern");
		debugWrite("pattern: \"%s\"",pattern);

		regularexpression	*re=new regularexpression;
		re->setPattern(pattern);
		re->study();
		relist.append(re);
	}
	if (!relist.getCount()) {
		debugWrite("WARNING! no patterns found");
	}
	debugEnd();
}

const char *sqlrrouter_regex::route(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char **err,
					int64_t *errn) {

	if (!sqlrcon || !sqlrcur) {
		return NULL;
	}

	debugWrite("route");

	const char	*query=sqlrcur->getQueryBuffer();
	for (listnode< regularexpression *> *rn=relist.getFirst();
							rn; rn=rn->getNext()) {
		if (rn->getValue()->match(query)) {
			debugWrite("routing query:");
			debugWrite("%s",query);
			debugWrite("to: %s",connid);
			return connid;
		}
	}

	debugEnd();

	return NULL;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrrouter *new_sqlrrouter_regex(
						sqlrservercontroller *cont,
						sqlrrouters *rs,
						domnode *parameters) {
		return new sqlrrouter_regex(cont,rs,parameters);
	}
}
