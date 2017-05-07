// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/linkedlist.h>

class SQLRSERVER_DLLSPEC sqlrrouter_clientiplist : public sqlrrouter {
	public:
			sqlrrouter_clientiplist(sqlrservercontroller *cont,
						sqlrrouters *rs,
						xmldomnode *parameters,
						const char **connectionids,
						sqlrconnection **connections,
						uint16_t connectioncount);
			~sqlrrouter_clientiplist();

		const char	*route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);
		bool		routeEntireSession();
	private:
		bool	match(const char *ip, const char *pattern);

		const char	*connectionid;

		const char	**clientips;
		uint64_t	clientipcount;

		bool	enabled;

		bool	debug;
};

sqlrrouter_clientiplist::sqlrrouter_clientiplist(sqlrservercontroller *cont,
						sqlrrouters *rs,
						xmldomnode *parameters,
						const char **connectionids,
						sqlrconnection **connections,
						uint16_t connectioncount) :
					sqlrrouter(cont,rs,parameters,
							connectionids,
							connections,
							connectioncount) {
	clientips=NULL;

	debug=cont->getConfig()->getDebugRouters();
	enabled=charstring::compareIgnoringCase(
			parameters->getAttributeValue("enabled"),"no");
	if (!enabled && debug) {
		stdoutput.printf("	disabled\n");
		return;
	}

	connectionid=parameters->getAttributeValue("connectionid");

	// this is faster than running through the xml over and over
	clientipcount=parameters->getChildCount();
	clientips=new const char *[clientipcount];
	xmldomnode *clientip=parameters->getFirstTagChild("client");
	for (uint64_t i=0; i<clientipcount; i++) {
		clientips[i]=clientip->getAttributeValue("ip");
		clientip=clientip->getNextTagSibling("client");
	}
}

sqlrrouter_clientiplist::~sqlrrouter_clientiplist() {
	delete[] clientips;
}

const char *sqlrrouter_clientiplist::route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur) {
	if (!enabled) {
		return NULL;
	}

	// get the clientip
	const char	*clientip=sqlrcon->cont->getClientAddr();
	if (charstring::isNullOrEmpty(clientip)) {
		if (debug) {
			stdoutput.printf("routing client ip "
					"(null/empty) to -1\n");
		}
		return "-1";
	}

	// run through the clientip array...
	for (uint64_t i=0; i<clientipcount; i++) {

		// if the clientip matches...
		if (match(clientip,clientips[i])) {
			if (debug) {
				stdoutput.printf("routing client ip "
							"\"%s\" to %s\n",
							clientip,connectionid);
			}
			return connectionid;
		}
	}
	return NULL;
}

bool sqlrrouter_clientiplist::match(const char *ip, const char *pattern) {

	if (debug) {
		stdoutput.printf("\n");
	}

	for (uint16_t i=0; i<4; i++) {

		if (debug) {
			stdoutput.printf("%d: ip=%s  pattern=%s\n",
							i,ip,pattern);
		}

		// handle wildcards
		if (!charstring::compare(pattern,"*")) {
			if (debug) {
				stdoutput.printf("	"
						"%s matches "
						"wildcard %s...\n",
						ip,pattern);
			}
			break;
		}
		if (!charstring::compare(pattern,"*.",2)) {
			if (debug) {
				stdoutput.printf("	"
						"%s matches "
						"wildcard %s...\n",
						ip,pattern);
			}
			pattern=pattern+2;
			ip=charstring::findFirst(ip,'.')+1;
			continue;
		}

		// handle dashed ranges
		const char	*dot=charstring::findFirstOrEnd(pattern,'.');
		char	*chunk=charstring::duplicate(pattern,dot-pattern);
		char	*dash=charstring::findFirst(chunk,'-');
		if (dash) {


			const char	*start=chunk;
			const char	*end=dash+1;

			uint64_t	i=charstring::toUnsignedInteger(ip);
			bool		inrange=
				(charstring::toUnsignedInteger(start)<=i &&
					charstring::toUnsignedInteger(end)>=i);

			delete[] chunk;

			if (!inrange) {
				if (debug) {
					stdoutput.printf("	"
							"%s doesn't "
							"match %s...\n",
							ip,pattern);
				}
				return false;
			}

			if (debug) {
				stdoutput.printf("	"
						"%s matches "
						"range %s...\n",
						ip,pattern);
			}

			pattern=dot+1;
			ip=charstring::findFirst(ip,'.')+1;

			continue;
		}

		delete[] chunk;

		// handle individual numbers
		if (charstring::toUnsignedInteger(pattern)==
				charstring::toUnsignedInteger(ip)) {

			if (debug) {
				stdoutput.printf("	"
						"%s matches "
						"individual %s...\n",
						ip,pattern);
			}

			pattern=charstring::findFirst(pattern,'.')+1;
			ip=charstring::findFirst(ip,'.')+1;

			continue;
		}

		if (debug) {
			stdoutput.printf("	%s doesn't match %s...\n",
								ip,pattern);
		}
		return false;
	}

	if (debug) {
		stdoutput.printf("match found\n");
	}
	return true;
}

bool sqlrrouter_clientiplist::routeEntireSession() {
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrrouter *new_sqlrrouter_clientiplist(
						sqlrservercontroller *cont,
						sqlrrouters *rs,
						xmldomnode *parameters,
						const char **connectionids,
						sqlrconnection **connections,
						uint16_t connectioncount) {
		return new sqlrrouter_clientiplist(cont,rs,parameters,
							connectionids,
							connections,
							connectioncount);
	}
}
