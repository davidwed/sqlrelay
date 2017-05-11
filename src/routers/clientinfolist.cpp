// Copyright (c) 2016  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/linkedlist.h>
#include <rudiments/regularexpression.h>

class SQLRSERVER_DLLSPEC sqlrrouter_clientinfolist : public sqlrrouter {
	public:
			sqlrrouter_clientinfolist(sqlrservercontroller *cont,
						sqlrrouters *rs,
						xmldomnode *parameters);
			~sqlrrouter_clientinfolist();

		const char	*route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char **err,
						int64_t *errn);
		bool	routeEntireSession();
	private:
		const char	*connid;

		regularexpression	**clientinfos;
		uint64_t		clientinfocount;

		bool	enabled;

		bool	debug;
};

sqlrrouter_clientinfolist::sqlrrouter_clientinfolist(sqlrservercontroller *cont,
						sqlrrouters *rs,
						xmldomnode *parameters) :
					sqlrrouter(cont,rs,parameters) {
	clientinfos=NULL;

	debug=cont->getConfig()->getDebugRouters();
	enabled=charstring::compareIgnoringCase(
			parameters->getAttributeValue("enabled"),"no");
	if (!enabled && debug) {
		stdoutput.printf("	disabled\n");
		return;
	}

	connid=parameters->getAttributeValue("connectionid");

	// this is faster than running through the xml over and over
	clientinfocount=parameters->getChildCount();
	clientinfos=new regularexpression *[clientinfocount];
	xmldomnode *clientinfo=parameters->getFirstTagChild("clientinfo");
	for (uint64_t i=0; i<clientinfocount; i++) {
		clientinfos[i]=new regularexpression(
				clientinfo->getAttributeValue("pattern"));
		clientinfos[i]->study();
		clientinfo=clientinfo->getNextTagSibling("clientinfo");
	}
}

sqlrrouter_clientinfolist::~sqlrrouter_clientinfolist() {
	for (uint64_t i=0; i<clientinfocount; i++) {
		delete clientinfos[i];
	}
	delete[] clientinfos;
}

const char *sqlrrouter_clientinfolist::route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char **err,
						int64_t *errn) {
	if (!enabled) {
		return NULL;
	}

	// get the clientinfo
	const char	*clientinfo=sqlrcon->cont->getClientInfo();
	if (charstring::isNullOrEmpty(clientinfo)) {
		if (debug) {
			stdoutput.printf("routing null/empty client info\n");
		}
		return NULL;
	}

	// run through the clientinfo array...
	for (uint64_t i=0; i<clientinfocount; i++) {

		// if the clientinfo matches...
		if (clientinfos[i]->match(clientinfo)) {
			if (debug) {
				stdoutput.printf("routing client info "
						"\"%s\" to %s\n",
						clientinfo,connid);
			}
			return connid;
		}
	}
	return NULL;
}

bool sqlrrouter_clientinfolist::routeEntireSession() {
	return true;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrrouter *new_sqlrrouter_clientinfolist(
						sqlrservercontroller *cont,
						sqlrrouters *rs,
						xmldomnode *parameters) {
		return new sqlrrouter_clientinfolist(cont,rs,parameters);
	}
}
