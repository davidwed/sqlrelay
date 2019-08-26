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
			~sqlrrouter_regex();

		const char	*route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char **err,
						int64_t *errn);
	private:
		linkedlist< regularexpression * >	relist;

		const char	*connid;

		bool	enabled;

		bool	debug;
};

sqlrrouter_regex::sqlrrouter_regex(sqlrservercontroller *cont,
						sqlrrouters *rs,
						domnode *parameters) :
					sqlrrouter(cont,rs,parameters) {
	debug=cont->getConfig()->getDebugRouters();
	enabled=!charstring::isNo(parameters->getAttributeValue("enabled"));
	if (!enabled && debug) {
		stdoutput.printf("	disabled\n");
		return;
	}

	connid=parameters->getAttributeValue("connectionid");

	for (domnode *pn=parameters->getFirstTagChild("pattern");
				!pn->isNullNode();
				pn=pn->getNextTagSibling("pattern")) {

		const char	*pattern=pn->getAttributeValue("pattern");
		if (debug) {
			stdoutput.printf("	pattern: \"%s\"\n",pattern);
		}

		regularexpression	*re=new regularexpression;
		re->setPattern(pattern);
		re->study();
		relist.append(re);
	}
	if (debug && !relist.getLength()) {
		stdoutput.printf("	WARNING! no patterns found\n");
	}
}

sqlrrouter_regex::~sqlrrouter_regex() {
	relist.clearAndDelete();
}

const char *sqlrrouter_regex::route(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					const char **err,
					int64_t *errn) {

	if (!enabled || !sqlrcon || !sqlrcur) {
		return NULL;
	}

	if (debug) {
		stdoutput.printf("		route {\n");
	}

	const char	*query=sqlrcur->getQueryBuffer();
	for (linkedlistnode< regularexpression *> *rn=relist.getFirst();
							rn; rn=rn->getNext()) {
		if (rn->getValue()->match(query)) {
			if (debug) {
				stdoutput.printf("			"
							"routing query:\n"
							"		"
							"	%s\n"
							"		"
							"	to: %s\n"
							"		}\n",
							query,connid);
			}
			return connid;
		}
	}

	if (debug) {
		stdoutput.printf("		}\n");
	}
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
