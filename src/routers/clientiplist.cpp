// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class SQLRSERVER_DLLSPEC sqlrrouter_clientiplist : public sqlrrouter {
	public:
		sqlrrouter_clientiplist(sqlrservercontroller *cont,
						sqlrrouters *rs,
						domnode *parameters);
		~sqlrrouter_clientiplist();

		const char	*route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char **err,
						int64_t *errn);
		bool	routeEntireSession();
	private:
		bool	match(const char *ip, const char *pattern);

		const char	*connid;

		const char	**clientips;
		uint64_t	clientipcount;
};

sqlrrouter_clientiplist::sqlrrouter_clientiplist(sqlrservercontroller *cont,
						sqlrrouters *rs,
						domnode *parameters) :
					sqlrrouter(cont,rs,parameters) {
	clientips=NULL;

	connid=parameters->getAttributeValue("connectionid");

	// this is faster than running through the xml over and over
	clientipcount=parameters->getChildCount();
	clientips=new const char *[clientipcount];
	domnode *clientip=parameters->getFirstTagChild("client");
	for (uint64_t i=0; i<clientipcount; i++) {
		clientips[i]=clientip->getAttributeValue("ip");
		clientip=clientip->getNextTagSibling("client");
	}
}

sqlrrouter_clientiplist::~sqlrrouter_clientiplist() {
	delete[] clientips;
}

const char *sqlrrouter_clientiplist::route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char **err,
						int64_t *errn) {

	debugStart("route");

	// get the clientip
	const char	*clientip=sqlrcon->cont->getClientAddr();
	if (charstring::isNullOrEmpty(clientip)) {
		debugWrite("routing null/empty client ip");
		debugEnd();
		return NULL;
	}

	// run through the clientip array...
	for (uint64_t i=0; i<clientipcount; i++) {

		// if the clientip matches...
		if (match(clientip,clientips[i])) {
			debugWrite("routing client ip \"%s\" to %s",
							clientip,connid);
			debugEnd();
			return connid;
		}
	}

	debugEnd();

	return NULL;
}

bool sqlrrouter_clientiplist::match(const char *ip, const char *pattern) {

	for (uint16_t i=0; i<4; i++) {

		debugWrite("%d: ip=%s  pattern=%s\n",i,ip,pattern);

		// handle wildcards
		if (!charstring::compare(pattern,"*")) {
			debugWrite("%s matches wildcard %s...",ip,pattern);
			break;
		}
		if (!charstring::compare(pattern,"*.",2)) {
			debugWrite("%s matches wildcard %s...",ip,pattern);
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

			uint64_t	i=
				charstring::convertToUnsignedInteger(ip);
			bool		inrange=
			(charstring::convertToUnsignedInteger(start)<=i &&
				charstring::convertToUnsignedInteger(end)>=i);

			delete[] chunk;

			if (!inrange) {
				debugWrite("%s doesn't match %s...",ip,pattern);
				return false;
			}

			debugWrite("%s matches range %s...",ip,pattern);

			pattern=dot+1;
			ip=charstring::findFirst(ip,'.')+1;

			continue;
		}

		delete[] chunk;

		// handle individual numbers
		if (charstring::convertToUnsignedInteger(pattern)==
				charstring::convertToUnsignedInteger(ip)) {

			debugWrite("%s matches individual %s...",ip,pattern);

			pattern=charstring::findFirst(pattern,'.')+1;
			ip=charstring::findFirst(ip,'.')+1;

			continue;
		}

		debugWrite("%s doesn't match %s...",ip,pattern);
		return false;
	}

	debugWrite("match found");
	return true;
}

bool sqlrrouter_clientiplist::routeEntireSession() {
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrrouter *new_sqlrrouter_clientiplist(
						sqlrservercontroller *cont,
						sqlrrouters *rs,
						domnode *parameters) {
		return new sqlrrouter_clientiplist(cont,rs,parameters);
	}
}
