// Copyright (c) 2000-2005  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrconfigfile.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/environment.h>
#include <rudiments/directory.h>
#include <rudiments/sys.h>
#include <rudiments/stdio.h>

#include <defines.h>
#include <defaults.h>

sqlrconfigfile::sqlrconfigfile() : xmlsax() {
	addresses=NULL;
	addresscount=0;
	port=0;
	unixport=charstring::duplicate("");
	listenoninet=false;
	listenonunix=false;
	dbase=charstring::duplicate(DEFAULT_DBASE);
	connections=charstring::toInteger(DEFAULT_CONNECTIONS);
	maxconnections=0;
	maxqueuelength=charstring::toInteger(DEFAULT_MAXQUEUELENGTH);
	growby=charstring::toInteger(DEFAULT_GROWBY);
	ttl=charstring::toInteger(DEFAULT_TTL);
	maxsessioncount=charstring::toInteger(DEFAULT_MAXSESSIONCOUNT);
	endofsession=charstring::duplicate(DEFAULT_ENDOFSESSION);
	endofsessioncommit=!charstring::compare(endofsession,"commit");
	sessiontimeout=charstring::toUnsignedInteger(DEFAULT_SESSIONTIMEOUT);
	runasuser=charstring::duplicate(DEFAULT_RUNASUSER);
	runasgroup=charstring::duplicate(DEFAULT_RUNASGROUP);
	cursors=charstring::toInteger(DEFAULT_CURSORS);
	maxcursors=charstring::toInteger(DEFAULT_CURSORS);
	cursorsgrowby=charstring::toInteger(DEFAULT_CURSORS_GROWBY);
	authtier=charstring::duplicate(DEFAULT_AUTHTIER);
	authonconnection=charstring::compare(authtier,"database");
	authondatabase=!charstring::compare(authtier,"database");
	sessionhandler=charstring::duplicate(DEFAULT_SESSION_HANDLER);
	handoff=charstring::duplicate(DEFAULT_HANDOFF);
	allowedips=charstring::duplicate(DEFAULT_DENIEDIPS);
	deniedips=charstring::duplicate(DEFAULT_DENIEDIPS);
	debug=charstring::duplicate(DEFAULT_DEBUG);
	debugtranslations=charstring::contains(debug,"translations");
	debugtriggers=charstring::contains(debug,"triggers");
	debugbindtranslations=charstring::contains(debug,"bindtranslations");
	maxclientinfolength=charstring::toInteger(DEFAULT_MAXCLIENTINFOLENGTH);
	maxquerysize=charstring::toInteger(DEFAULT_MAXQUERYSIZE);
	maxbindcount=charstring::toInteger(DEFAULT_MAXBINDCOUNT);
	maxbindnamelength=charstring::toInteger(DEFAULT_MAXBINDNAMELENGTH);
	maxstringbindvaluelength=charstring::toInteger(
					DEFAULT_MAXSTRINGBINDVALUELENGTH);
	maxlobbindvaluelength=charstring::toInteger(
					DEFAULT_MAXLOBBINDVALUELENGTH);
	maxerrorlength=charstring::toInteger(DEFAULT_MAXERRORLENGTH);
	idleclienttimeout=charstring::toInteger(DEFAULT_IDLECLIENTTIMEOUT);
	currentuser=NULL;
	currentconnect=NULL;
	connectioncount=0;
	metrictotal=0;
	maxlisteners=charstring::toInteger(DEFAULT_MAXLISTENERS);
	listenertimeout=charstring::toUnsignedInteger(DEFAULT_LISTENERTIMEOUT);
	reloginatstart=!charstring::compare(DEFAULT_RELOGINATSTART,"yes");
	fakeinputbindvariables=!charstring::compare(
					DEFAULT_FAKEINPUTBINDVARIABLES,"yes");
	translatebindvariables=!charstring::compare(
					DEFAULT_TRANSLATEBINDVARIABLES,"yes");
	currentroute=NULL;
	currenttag=NO_TAG;
	authenticationsdepth=0;
	translationsdepth=0;
	resultsettranslationsdepth=0;
	triggersdepth=0;
	loggersdepth=0;
	queriesdepth=0;
	passwordencryptionsdepth=0;
	isolationlevel=NULL;
	ignoreselectdb=false;
	waitfordowndb=true;
	datetimeformat=NULL;
	dateformat=NULL;
	timeformat=NULL;
	dateddmm=false;
	dateyyyyddmm=false;
	dateyyyyddmmset=false;
	instart=false;
	inend=false;
}

sqlrconfigfile::~sqlrconfigfile() {

	for (uint64_t adrind=0; adrind<addresscount; adrind++) {
		delete[] addresses[adrind];
	}
	delete[] addresses;

	delete[] dbase;
	delete[] unixport;
	delete[] endofsession;
	delete[] runasuser;
	delete[] runasgroup;
	delete[] authtier;
	delete[] sessionhandler;
	delete[] handoff;
	delete[] allowedips;
	delete[] deniedips;
	delete[] debug;
	delete[] isolationlevel;
	delete[] datetimeformat;
	delete[] dateformat;
	delete[] timeformat;

	for (usernode *un=userlist.getFirst(); un; un=un->getNext()) {
		delete un->getValue();
	}

	for (connectstringnode *csn=connectstringlist.getFirst();
						csn; csn=csn->getNext()) {
		delete csn->getValue();
	}

	for (routenode *rn=routelist.getFirst(); rn; rn=rn->getNext()) {
		delete rn->getValue();
	}

	for (linkedlistnode< char * > *ssln=sessionstartqueries.getFirst();
						ssln; ssln=ssln->getNext()) {
		delete[] ssln->getValue();
	}
	for (linkedlistnode< char * > *seln=sessionendqueries.getFirst();
						seln; seln=seln->getNext()) {
		delete[] seln->getValue();
	}
}

const char * const * sqlrconfigfile::getAddresses() {
	return addresses;
}

uint64_t sqlrconfigfile::getAddressCount() {
	return addresscount;
}

uint16_t sqlrconfigfile::getPort() {
	return port;
}

const char *sqlrconfigfile::getUnixPort() {
	return unixport;
}

bool sqlrconfigfile::getListenOnInet() {
	return listenoninet;
}

bool sqlrconfigfile::getListenOnUnix() {
	return listenonunix;
}

const char *sqlrconfigfile::getDbase() {
	return dbase;
}

uint32_t sqlrconfigfile::getConnections() {
	return connections;
}

uint32_t sqlrconfigfile::getMaxConnections() {
	return maxconnections;
}

uint32_t sqlrconfigfile::getMaxQueueLength() {
	return maxqueuelength;
}

uint32_t sqlrconfigfile::getGrowBy() {
	return growby;
}

int32_t sqlrconfigfile::getTtl() {
	return ttl;
}

uint16_t sqlrconfigfile::getMaxSessionCount() {
	return maxsessioncount;
}

bool sqlrconfigfile::getDynamicScaling() {
	return (maxconnections>connections && growby>0 && ttl>-1 &&
		(maxlisteners==-1 || maxqueuelength<=maxlisteners));
}

const char *sqlrconfigfile::getEndOfSession() {
	return endofsession;
}

bool sqlrconfigfile::getEndOfSessionCommit() {
	return endofsessioncommit;
}

uint32_t sqlrconfigfile::getSessionTimeout() {
	return sessiontimeout;
}

const char *sqlrconfigfile::getRunAsUser() {
	return runasuser;
}

const char *sqlrconfigfile::getRunAsGroup() {
	return runasgroup;
}

uint16_t sqlrconfigfile::getCursors() {
	return cursors;
}

uint16_t sqlrconfigfile::getMaxCursors() {
	return maxcursors;
}

uint16_t sqlrconfigfile::getCursorsGrowBy() {
	return cursorsgrowby;
}

const char *sqlrconfigfile::getAuthTier() {
	return authtier;
}

const char *sqlrconfigfile::getSessionHandler() {
	return sessionhandler;
}

const char *sqlrconfigfile::getHandoff() {
	return handoff;
}

bool sqlrconfigfile::getAuthOnConnection() {
	return authonconnection;
}

bool sqlrconfigfile::getAuthOnDatabase() {
	return authondatabase;
}

const char *sqlrconfigfile::getAllowedIps() {
	return allowedips;
}

const char *sqlrconfigfile::getDeniedIps() {
	return deniedips;
}

const char *sqlrconfigfile::getDebug() {
	return debug;
}

bool sqlrconfigfile::getDebugTranslations() {
	return debugtranslations;
}

bool sqlrconfigfile::getDebugTriggers() {
	return debugtriggers;
}

bool sqlrconfigfile::getDebugBindTranslations() {
	return debugbindtranslations;
}

uint64_t sqlrconfigfile::getMaxClientInfoLength() {
	return maxclientinfolength;
}

uint32_t sqlrconfigfile::getMaxQuerySize() {
	return maxquerysize;
}

uint16_t sqlrconfigfile::getMaxBindCount() {
	return maxbindcount;
}

uint16_t sqlrconfigfile::getMaxBindNameLength() {
	return maxbindnamelength;
}

uint32_t sqlrconfigfile::getMaxStringBindValueLength() {
	return maxstringbindvaluelength;
}

uint32_t sqlrconfigfile::getMaxLobBindValueLength() {
	return maxlobbindvaluelength;
}

uint32_t sqlrconfigfile::getMaxErrorLength() {
	return maxerrorlength;
}

int32_t sqlrconfigfile::getIdleClientTimeout() {
	return idleclienttimeout;
}

int64_t sqlrconfigfile::getMaxListeners() {
	return maxlisteners;
}

uint32_t sqlrconfigfile::getListenerTimeout() {
	return listenertimeout;
}

bool sqlrconfigfile::getReLoginAtStart() {
	return reloginatstart;
}

bool sqlrconfigfile::getFakeInputBindVariables() {
	return fakeinputbindvariables;
}

bool sqlrconfigfile::getTranslateBindVariables() {
	return translatebindvariables;
}

const char *sqlrconfigfile::getIsolationLevel() {
	return isolationlevel;
}

bool sqlrconfigfile::getIgnoreSelectDatabase() {
	return ignoreselectdb;
}

bool sqlrconfigfile::getWaitForDownDatabase() {
	return waitfordowndb;
}

const char *sqlrconfigfile::getDateTimeFormat() {
	return datetimeformat;
}

const char *sqlrconfigfile::getDateFormat() {
	return dateformat;
}

const char *sqlrconfigfile::getTimeFormat() {
	return timeformat;
}

bool sqlrconfigfile::getDateDdMm() {
	return dateddmm;
}

bool sqlrconfigfile::getDateYyyyDdMm() {
	return dateyyyyddmm;
}

linkedlist< char * > *sqlrconfigfile::getSessionStartQueries() {
	return &sessionstartqueries;
}

linkedlist< char * > *sqlrconfigfile::getSessionEndQueries() {
	return &sessionendqueries;
}

const char *sqlrconfigfile::getTranslations() {
	return translations.getString();
}

const char *sqlrconfigfile::getResultSetTranslations() {
	return resultsettranslations.getString();
}

const char *sqlrconfigfile::getTriggers() {
	return triggers.getString();
}

const char *sqlrconfigfile::getLoggers() {
	return loggers.getString();
}

const char *sqlrconfigfile::getQueries() {
	return queries.getString();
}

const char *sqlrconfigfile::getPasswordEncryptions() {
	return passwordencryptions.getString();
}

const char *sqlrconfigfile::getAuthentications() {
	return authentications.getString();
}

linkedlist< usercontainer * > *sqlrconfigfile::getUserList() {
	return &userlist;
}

linkedlist< connectstringcontainer * > *sqlrconfigfile::getConnectStringList() {
	return &connectstringlist;
}

connectstringcontainer *sqlrconfigfile::getConnectString(
						const char *connectionid) {
	for (connectstringnode *csn=connectstringlist.getFirst();
						csn; csn=csn->getNext()) {
		if (!charstring::compare(connectionid,
					csn->getValue()->getConnectionId())) {
			return csn->getValue();
		}
	}
	return NULL;
}

uint32_t sqlrconfigfile::getConnectionCount() {
	return connectstringlist.getLength();
}

uint32_t sqlrconfigfile::getMetricTotal() {
	// This is tallied here instead of whenever the parser runs into a
	// metric attribute because people often forget to include metric
	// attributes.  In that case, though each connection has a metric,
	// metrictotal=0, causing no connections to start.
	if (!metrictotal) {
		for (connectstringnode *csn=connectstringlist.getFirst();
						csn; csn=csn->getNext()) {
			metrictotal=metrictotal+csn->getValue()->getMetric();
		}
	}
	return metrictotal;
}

linkedlist< routecontainer * >	*sqlrconfigfile::getRouteList() {
	return &routelist;
}

bool sqlrconfigfile::tagStart(const char *name) {

	// don't do anything if we're already done
	// or have not found the correct id
	if (done || !correctid) {
		return true;
	}

	bool		ok=true;
	const char	*currentname="instance";
	tag		thistag=currenttag;

	// set the current tag, validate structure in the process
	switch(currenttag) {
	
		// Root level, nested (users,connections?,router?)
		case NO_TAG:
			currentname="instance";
			if (!charstring::compare(name,"authentications")) {
				thistag=AUTHENTICATIONS_TAG;
				authentications.clear();
			} else if (!charstring::compare(name,"users")) {
				thistag=USERS_TAG;
			} else if (!charstring::compare(name,"session")) {
				thistag=SESSION_TAG;
			} else if (!charstring::compare(name,"connections")) {
				thistag=CONNECTIONS_TAG;
			} else if (!charstring::compare(name,"router")) {
				thistag=ROUTER_TAG;
			} else if (!charstring::compare(name,"translations")) {
				thistag=TRANSLATIONS_TAG;
				translations.clear();
			} else if (!charstring::compare(name,
						"resultsettranslations")) {
				thistag=RESULTSETTRANSLATIONS_TAG;
				resultsettranslations.clear();
			} else if (!charstring::compare(name,"triggers")) {
				thistag=TRIGGERS_TAG;
				triggers.clear();
			} else if (!charstring::compare(name,"loggers")) {
				thistag=LOGGERS_TAG;
				loggers.clear();
			} else if (!charstring::compare(name,"queries")) {
				thistag=QUERIES_TAG;
				queries.clear();
			} else if (!charstring::compare(name,
						"passwordencryptions")) {
				thistag=PASSWORDENCRYPTIONS_TAG;
				passwordencryptions.clear();
			} else {
				ok=false;
			}
			break;
		
		// Users section, nested (user*)
		case USERS_TAG:
			currentname="users";
			if (!charstring::compare(name,"user")) {
				thistag=USER_TAG;
			} else {
				ok=false;
			}
			break;
		
		// Session section, nested (start,end)
		case SESSION_TAG:
			currentname="session";
			if (!charstring::compare(name,"start")) {
				thistag=START_TAG;
				instart=true;
			} else if (!charstring::compare(name,"end")) {
				thistag=END_TAG;
				inend=true;
			} else {
				ok=false;
			}
			break;
		
		// Start section, nested (runquery*)
		case START_TAG:
			currentname="start";
			if (!charstring::compare(name,"runquery")) {
				thistag=RUNQUERY_TAG;
			} else {
				ok=false;
			}
			break;

		// End section, nested (runquery*)
		case END_TAG:
			currentname="end";
			if (!charstring::compare(name,"runquery")) {
				thistag=RUNQUERY_TAG;
			} else {
				ok=false;
			}
			break;
		
		// Connections section, nested (connection*)
		case CONNECTIONS_TAG:
			currentname="connections";
			if (!charstring::compare(name,"connection")) {
				thistag=CONNECTION_TAG;
			} else {
				ok=false;
			}
			break;
		
		// Router section, nested ((route*|filter*)*)
		case ROUTER_TAG:
			currentname="router";
			if (!charstring::compare(name,"route")) {
				thistag=ROUTE_TAG;
			} else if (!charstring::compare(name,"filter")) {
				thistag=FILTER_TAG;
			} else {
				ok=false;
			}
			break;
		
		// Filter section, nested (query*)
		case FILTER_TAG:
			currentname="filter";
			if (!charstring::compare(name,"query")) {
				thistag=QUERY_TAG;
			} else {
				ok=false;
			}
			break;
		
		// Filter section, nested (query*)
		case ROUTE_TAG:
			currentname="route";
			if (!charstring::compare(name,"query")) {
				thistag=QUERY_TAG;
			} else {
				ok=false;
			}
			break;

		// Leaves, cannot be nested
		case USER_TAG:
			currentname="user";
			ok=false;
			break;
		case CONNECTION_TAG:
			currentname="connection";
			ok=false;
			break;
		case QUERY_TAG:
			currentname="query";
			ok=false;
			break;
		case RUNQUERY_TAG:
			currentname="runquery";
			ok=false;
			break;
		default:
			// Nothing to do
			break;
	}
	
	if (!ok) {
		stderror.printf("unexpected tag <%s> within <%s>\n",
							name,currentname);
		return false;
	}

	// initialize tag data
	switch (thistag) {
		case AUTHENTICATIONS_TAG:
			if (!charstring::compare(name,"authentications")) {
				authenticationsdepth=0;
			} else {
				authenticationsdepth++;
			}
			if (authenticationsdepth) {
				authentications.append(">");
			}
			authentications.append("<");
			authentications.append(name);
			currenttag=thistag;
			break;
		case USER_TAG:
			currentuser=new usercontainer();
			userlist.append(currentuser);
			break;
		case CONNECTION_TAG: {
			currentconnect=new connectstringcontainer();
			connectstringlist.append(currentconnect);
			stringbuffer	connectionid;
			connectionid.append(id)->append("-");
			connectionid.append(connectioncount);
			currentconnect->setConnectionId(
					connectionid.getString());
			connectioncount++;
			}
			break;
		case ROUTER_TAG: {
			currentconnect=new connectstringcontainer();
			connectstringlist.append(currentconnect);
			stringbuffer	connectionid;
			connectionid.append(id)->append("-");
			connectionid.append(connectioncount);
			currentconnect->setConnectionId(
					connectionid.getString());
			connectioncount++;
			currenttag=thistag;
			}
			break;
		case ROUTE_TAG:
		case FILTER_TAG:
			currentroute=new routecontainer();
			currentroute->setIsFilter(thistag==FILTER_TAG);
			currenttag=thistag;
			break;
		case USERS_TAG:
		case CONNECTIONS_TAG:
			currenttag=thistag;
			break;
		case TRANSLATIONS_TAG:
			if (!charstring::compare(name,"translations")) {
				translationsdepth=0;
			} else {
				translationsdepth++;
			}
			if (translationsdepth) {
				translations.append(">");
			}
			translations.append("<");
			translations.append(name);
			currenttag=thistag;
			break;
		case RESULTSETTRANSLATIONS_TAG:
			if (!charstring::compare(name,
						"resultsettranslations")) {
				resultsettranslationsdepth=0;
			} else {
				resultsettranslationsdepth++;
			}
			if (resultsettranslationsdepth) {
				resultsettranslations.append(">");
			}
			resultsettranslations.append("<");
			resultsettranslations.append(name);
			currenttag=thistag;
			break;
		case TRIGGERS_TAG:
			if (!charstring::compare(name,"triggers")) {
				triggersdepth=0;
			} else {
				triggersdepth++;
			}
			if (triggersdepth) {
				triggers.append(">");
			}
			triggers.append("<");
			triggers.append(name);
			currenttag=thistag;
			break;
		case LOGGERS_TAG:
			if (!charstring::compare(name,"loggers")) {
				loggersdepth=0;
			} else {
				loggersdepth++;
			}
			if (loggersdepth) {
				loggers.append(">");
			}
			loggers.append("<");
			loggers.append(name);
			currenttag=thistag;
			break;
		case QUERIES_TAG:
			if (!charstring::compare(name,"queries")) {
				queriesdepth=0;
			} else {
				queriesdepth++;
			}
			if (queriesdepth) {
				queries.append(">");
			}
			queries.append("<");
			queries.append(name);
			currenttag=thistag;
			break;
		case PASSWORDENCRYPTIONS_TAG:
			if (!charstring::compare(name,"passwordencryptions")) {
				passwordencryptionsdepth=0;
			} else {
				passwordencryptionsdepth++;
			}
			if (passwordencryptionsdepth) {
				passwordencryptions.append(">");
			}
			passwordencryptions.append("<");
			passwordencryptions.append(name);
			currenttag=thistag;
			break;
		case SESSION_TAG:
		case START_TAG:
		case END_TAG:
		case RUNQUERY_TAG:
			currenttag=thistag;
			break;
		default:
			// Nothing to do
		break;
	}
	return true;
}

bool sqlrconfigfile::tagEnd(const char *name) {

	// don't do anything if we're already done
	// or have not found the correct id
	if (done || !correctid) {
		return true;
	}

	if (!charstring::compare(name,"instance")) {

		// handle various address, port, socket combos...

		// if nothing was specified, use the
		// default address and port but no socket
		if (!addresses && !port && !unixport[0]) {
			addresses=new char *[1];
			addresses[0]=charstring::duplicate(DEFAULT_ADDRESS);
			addresscount=1;
			port=charstring::toInteger(DEFAULT_PORT);
		} else

		// if a port was specified but no address,
		// use the default address
		if (port && !addresses) {
			addresses=new char *[1];
			addresses[0]=charstring::duplicate(DEFAULT_ADDRESS);
			addresscount=1;
		} else

		// if an address was specified by no port
		// use the default port
		if (!port && addresses) {
			port=charstring::toInteger(DEFAULT_PORT);
		}

		listenoninet=(port)?true:false;
		listenonunix=(unixport[0])?true:false;
	}

	// Close up the current tag
	switch (currenttag) {
		case ROUTER_TAG:
			// Check closing tag, no need, but just in case
			if (!charstring::compare(name,"router")) {
				currenttag=NO_TAG;
			}
			break;
		case AUTHENTICATIONS_TAG:
			if (!charstring::compare(name,"authentications")) {
				currenttag=NO_TAG;
			}
			authentications.append("></");
			authentications.append(name);
			if (!authenticationsdepth) {
				authentications.append(">");
			}
			authenticationsdepth--;
			break;
		case USERS_TAG:
		case CONNECTIONS_TAG:
			// Must check closing tag, we have leaves inside
			if (!charstring::compare(name,"users") ||
				!charstring::compare(name,"connections")) {
				currenttag=NO_TAG;
			}
			break;
		case ROUTE_TAG:
		case FILTER_TAG:
			// Must check closing tag, we have leaves inside
			if (!charstring::compare(name,"route") ||
				!charstring::compare(name,"filter")) {
				currenttag=ROUTER_TAG;
				routecontainer	*existingroute=
					routeAlreadyExists(currentroute);
				if (existingroute) {
					moveRegexList(currentroute,
							existingroute);
					delete currentroute;
				} else {
					routelist.append(currentroute);
				}
			}
			break;
		case TRANSLATIONS_TAG:
			if (!charstring::compare(name,"translations")) {
				currenttag=NO_TAG;
			}
			translations.append("></");
			translations.append(name);
			if (!translationsdepth) {
				translations.append(">");
			}
			translationsdepth--;
			break;
		case RESULTSETTRANSLATIONS_TAG:
			if (!charstring::compare(name,
						"resultsettranslations")) {
				currenttag=NO_TAG;
			}
			resultsettranslations.append("></");
			resultsettranslations.append(name);
			if (!resultsettranslationsdepth) {
				resultsettranslations.append(">");
			}
			resultsettranslationsdepth--;
			break;
		case TRIGGERS_TAG:
			if (!charstring::compare(name,"triggers")) {
				currenttag=NO_TAG;
			}
			triggers.append("></");
			triggers.append(name);
			if (!triggersdepth) {
				triggers.append(">");
			}
			triggersdepth--;
			break;
		case LOGGERS_TAG:
			if (!charstring::compare(name,"loggers")) {
				currenttag=NO_TAG;
			}
			loggers.append("></");
			loggers.append(name);
			if (!loggersdepth) {
				loggers.append(">");
			}
			loggersdepth--;
			break;
		case QUERIES_TAG:
			if (!charstring::compare(name,"queries")) {
				currenttag=NO_TAG;
			}
			queries.append("></");
			queries.append(name);
			if (!queriesdepth) {
				queries.append(">");
			}
			queriesdepth--;
			break;
		case PASSWORDENCRYPTIONS_TAG:
			if (!charstring::compare(name,"passwordencryptions")) {
				currenttag=NO_TAG;
			}
			passwordencryptions.append("></");
			passwordencryptions.append(name);
			if (!passwordencryptionsdepth) {
				passwordencryptions.append(">");
			}
			passwordencryptionsdepth--;
			break;
		case SESSION_TAG:
			currenttag=NO_TAG;
			break;
		case START_TAG:
			instart=false;
			currenttag=SESSION_TAG;
			break;
		case END_TAG:
			inend=false;
			currenttag=SESSION_TAG;
			break;
		case RUNQUERY_TAG:
			currenttag=(instart)?START_TAG:END_TAG;
			break;
		default:
			// just ignore the closing tag
			break;
	}

	// we're done if we've found the right instance at this point
	if (correctid && !charstring::compare((char *)name,"instance")) {
		done=true;
	}
	return true;
}

bool sqlrconfigfile::attributeName(const char *name) {

	// don't do anything if we're already done
	if (done) {
		return true;
	}

	currentattribute=NO_ATTRIBUTE;

	switch (currenttag) {
	
	// Attributes of the <instance> tag
	case NO_TAG:

		if (!charstring::compare(name,"id")) {
			currentattribute=ID_ATTRIBUTE;
		} else if (!charstring::compare(name,"addresses")) {
			currentattribute=ADDRESSES_ATTRIBUTE;
		} else if (!charstring::compare(name,"port")) {
			currentattribute=PORT_ATTRIBUTE;
		} else if (!charstring::compare(name,"socket") ||
				!charstring::compare(name,"unixport")) {
			currentattribute=SOCKET_ATTRIBUTE;
		} else if (!charstring::compare(name,"dbase")) {
			currentattribute=DBASE_ATTRIBUTE;
		} else if (!charstring::compare(name,"connections")) {
			currentattribute=CONNECTIONS_ATTRIBUTE;
		} else if (!charstring::compare(name,"maxconnections")) {
			currentattribute=MAXCONNECTIONS_ATTRIBUTE;
		} else if (!charstring::compare(name,"maxqueuelength")) {
			currentattribute=MAXQUEUELENGTH_ATTRIBUTE;
		} else if (!charstring::compare(name,"growby")) {
			currentattribute=GROWBY_ATTRIBUTE;
		} else if (!charstring::compare(name,"ttl")) {
			currentattribute=TTL_ATTRIBUTE;
		} else if (!charstring::compare(name,"maxsessioncount")) {
			currentattribute=MAXSESSIONCOUNT_ATTRIBUTE;
		} else if (!charstring::compare(name,"endofsession")) {
			currentattribute=ENDOFSESSION_ATTRIBUTE;
		} else if (!charstring::compare(name,"sessiontimeout")) {
			currentattribute=SESSIONTIMEOUT_ATTRIBUTE;
		} else if (!charstring::compare(name,"runasuser")) {
			currentattribute=RUNASUSER_ATTRIBUTE;
		} else if (!charstring::compare(name,"runasgroup")) {
			currentattribute=RUNASGROUP_ATTRIBUTE;
		} else if (!charstring::compare(name,"cursors")) {
			currentattribute=CURSORS_ATTRIBUTE;
		} else if (!charstring::compare(name,"maxcursors")) {
			currentattribute=MAXCURSORS_ATTRIBUTE;
		} else if (!charstring::compare(name,"cursors_growby")) {
			currentattribute=CURSORS_GROWBY_ATTRIBUTE;
		} else if (!charstring::compare(name,"authtier") ||
				!charstring::compare(name,"authentication")) {
			currentattribute=AUTHTIER_ATTRIBUTE;
		} else if (!charstring::compare(name,"sessionhandler")) {
			currentattribute=SESSION_HANDLER_ATTRIBUTE;
		} else if (!charstring::compare(name,"handoff")) {
			currentattribute=HANDOFF_ATTRIBUTE;
		} else if (!charstring::compare(name,"deniedips")) {
			currentattribute=DENIEDIPS_ATTRIBUTE;
		} else if (!charstring::compare(name,"allowedips")) {
			currentattribute=ALLOWEDIPS_ATTRIBUTE;
		} else if (!charstring::compare(name,"debug")) {
			currentattribute=DEBUG_ATTRIBUTE;
		} else if (!charstring::compare(name,"maxclientinfolength")) {
			currentattribute=MAXCLIENTINFOLENGTH_ATTRIBUTE;
		} else if (!charstring::compare(name,"maxquerysize")) {
			currentattribute=MAXQUERYSIZE_ATTRIBUTE;
		} else if (!charstring::compare(name,"maxbindcount")) {
			currentattribute=MAXBINDCOUNT_ATTRIBUTE;
		} else if (!charstring::compare(name,"maxbindnamelength")) {
			currentattribute=MAXBINDNAMELENGTH_ATTRIBUTE;
		} else if (!charstring::compare(name,
						"maxstringbindvaluelength")) {
			currentattribute=MAXSTRINGBINDVALUELENGTH_ATTRIBUTE;
		} else if (!charstring::compare(name,"maxlobbindvaluelength")) {
			currentattribute=MAXLOBBINDVALUELENGTH_ATTRIBUTE;
		} else if (!charstring::compare(name,"maxerrorlength")) {
			currentattribute=MAXERRORLENGTH_ATTRIBUTE;
		} else if (!charstring::compare(name,"idleclienttimeout")) {
			currentattribute=IDLECLIENTTIMEOUT_ATTRIBUTE;
		} else if (!charstring::compare(name,"maxlisteners")) {
			currentattribute=MAXLISTENERS_ATTRIBUTE;
		} else if (!charstring::compare(name,"listenertimeout")) {
			currentattribute=LISTENERTIMEOUT_ATTRIBUTE;
		} else if (!charstring::compare(name,"reloginatstart")) {
			currentattribute=RELOGINATSTART_ATTRIBUTE;
		} else if (!charstring::compare(name,
						"fakeinputbindvariables")) {
			currentattribute=FAKEINPUTBINDVARIABLES_ATTRIBUTE;
		} else if (!charstring::compare(name,
						"translatebindvariables")) {
			currentattribute=TRANSLATEBINDVARIABLES_ATTRIBUTE;
		} else if (!charstring::compare(name,"isolationlevel")) {
			currentattribute=ISOLATIONLEVEL_ATTRIBUTE;
		} else if (!charstring::compare(name,"ignoreselectdatabase")) {
			currentattribute=IGNORESELECTDB_ATTRIBUTE;
		} else if (!charstring::compare(name,"waitfordowndatabase")) {
			currentattribute=WAITFORDOWNDB_ATTRIBUTE;
		} else if (!charstring::compare(name,"datetimeformat")) {
			currentattribute=DATETIMEFORMAT_ATTRIBUTE;
		} else if (!charstring::compare(name,"dateformat")) {
			currentattribute=DATEFORMAT_ATTRIBUTE;
		} else if (!charstring::compare(name,"timeformat")) {
			currentattribute=TIMEFORMAT_ATTRIBUTE;
		} else if (!charstring::compare(name,"dateddmm")) {
			currentattribute=DATEDDMM_ATTRIBUTE;
		} else if (!charstring::compare(name,"dateyyyyddmm")) {
			currentattribute=DATEYYYYDDMM_ATTRIBUTE;
		}
		break;

	case AUTHENTICATIONS_TAG:
		authentications.append(" ")->append(name);
		currentattribute=AUTHENTICATIONS_ATTRIBUTE;
		break;
	
	// Attributes of the <users> and <user> tags
	case USERS_TAG:
	case USER_TAG:
		if (!charstring::compare(name,"user")) {
			currentattribute=USER_ATTRIBUTE;
		} else if (!charstring::compare(name,"password")) {
			currentattribute=PASSWORD_ATTRIBUTE;
		} else if (!charstring::compare(name,"passwordencryption")) {
			currentattribute=PASSWORDENCRYPTION_ATTRIBUTE;
		}
		break;

	// Attributes of the <connection> tag
	case CONNECTIONS_TAG:
	case CONNECTION_TAG:
		if (!charstring::compare(name,"connectionid")) {
			currentattribute=CONNECTIONID_ATTRIBUTE;
		} else if (!charstring::compare(name,"string")) {
			currentattribute=STRING_ATTRIBUTE;
		} else if (!charstring::compare(name,"metric")) {
			currentattribute=METRIC_ATTRIBUTE;
		} else if (!charstring::compare(name,"behindloadbalancer")) {
			currentattribute=BEHINDLOADBALANCER_ATTRIBUTE;
		/*} else if (!charstring::compare(name,"port")) {
			currentattribute=PORT_ATTRIBUTE;
		} else if (!charstring::compare(name,"socket") ||
				!charstring::compare(name,"unixport")) {
			currentattribute=SOCKET_ATTRIBUTE;*/
		} else if (!charstring::compare(name,"passwordencryption")) {
			currentattribute=PASSWORDENCRYPTION_ATTRIBUTE;
		}
		break;

	// Attributes of the <router> tag
	case ROUTER_TAG:
		if (!charstring::compare(name,"connectionid")) {
			currentattribute=CONNECTIONID_ATTRIBUTE;
		} else if (!charstring::compare(name,"string")) {
			currentattribute=STRING_ATTRIBUTE;
		} else if (!charstring::compare(name,"metric")) {
			currentattribute=METRIC_ATTRIBUTE;
		} else if (!charstring::compare(name,"behindloadbalancer")) {
			currentattribute=BEHINDLOADBALANCER_ATTRIBUTE;
		}
		break;

	// Attributes of the <route>, <filter> & <query> tag
	case ROUTE_TAG:
	case FILTER_TAG:
	case QUERY_TAG:
		if (!charstring::compare(name,"user")) {
			currentattribute=ROUTER_USER_ATTRIBUTE;
		} else if (!charstring::compare(name,"password")) {
			currentattribute=ROUTER_PASSWORD_ATTRIBUTE;
		} else if (!charstring::compare(name,"host")) {
			currentattribute=ROUTER_HOST_ATTRIBUTE;
		} else if (!charstring::compare(name,"pattern")) {
			currentattribute=ROUTER_PATTERN_ATTRIBUTE;
		} else if (!charstring::compare(name,"port")) {
			currentattribute=ROUTER_PORT_ATTRIBUTE;
		} else if (!charstring::compare(name,"socket") ||
				!charstring::compare(name,"unixport")) {
			currentattribute=ROUTER_SOCKET_ATTRIBUTE;
		}
		break;

	case TRANSLATIONS_TAG:
		translations.append(" ")->append(name);
		currentattribute=TRANSLATIONS_ATTRIBUTE;
		break;

	case RESULTSETTRANSLATIONS_TAG:
		resultsettranslations.append(" ")->append(name);
		currentattribute=RESULTSETTRANSLATIONS_ATTRIBUTE;
		break;

	case TRIGGERS_TAG:
		triggers.append(" ")->append(name);
		currentattribute=TRIGGERS_ATTRIBUTE;
		break;

	case LOGGERS_TAG:
		loggers.append(" ")->append(name);
		currentattribute=LOGGERS_ATTRIBUTE;
		break;

	case QUERIES_TAG:
		queries.append(" ")->append(name);
		currentattribute=QUERIES_ATTRIBUTE;
		break;

	case PASSWORDENCRYPTIONS_TAG:
		passwordencryptions.append(" ")->append(name);
		currentattribute=PASSWORDENCRYPTIONS_ATTRIBUTE;
		break;

	// these tags have no attributes and there's nothing to do but the
	// compiler will complain if they aren't in the switch statement
	case SESSION_TAG:
	case START_TAG:
		break;
	case END_TAG:
		break;
	case RUNQUERY_TAG:
		break;
	}

	if (correctid && currentattribute==NO_ATTRIBUTE) {
		const char *tagname="instance";
		switch (currenttag) {
			case NO_TAG:
				tagname="instance";
				break;
			case AUTHENTICATIONS_TAG:
				tagname="authentications";
				break;
			case USERS_TAG:
				tagname="users";
				break;
			case USER_TAG:
				tagname="user";
				break;
			case CONNECTIONS_TAG:
				tagname="connections";
				break;
			case CONNECTION_TAG:
				tagname="connection";
				break;
			case ROUTER_TAG:
				tagname="router";
				break;
			case ROUTE_TAG:
				tagname="route";
				break;
			case FILTER_TAG:
				tagname="filter";
				break;
			case QUERY_TAG:
				tagname="query";
				break;
			case TRANSLATIONS_TAG:
				tagname="translations";
				break;
			case RESULTSETTRANSLATIONS_TAG:
				tagname="resultsettranslations";
				break;
			case TRIGGERS_TAG:
				tagname="triggers";
				break;
			case LOGGERS_TAG:
				tagname="loggers";
				break;
			case QUERIES_TAG:
				tagname="queries";
				break;
			case PASSWORDENCRYPTIONS_TAG:
				tagname="passwordencryptions";
				break;
			case SESSION_TAG:
				tagname="session";
				break;
			case START_TAG:
				tagname="start";
				break;
			case END_TAG:
				tagname="end";
				break;
			case RUNQUERY_TAG:
				tagname="runquery";
				break;
		}
		stderror.printf("WARNING: unrecognized attribute "
				"\"%s\" within <%s> tag or section\n",
				name,tagname);
	}

	// set the current attribute
	return true;
}

bool sqlrconfigfile::attributeValue(const char *value) {

	// don't do anything if we're already done
	if (done) {
		return true;
	}

	if (!correctid) {

		// if we haven't found the correct id yet, check for it
		if (currentattribute==ID_ATTRIBUTE) {
			if (value && !charstring::compare(value,id)) {
				correctid=true;
			}
		}
	
	} else {

		// if we have found the correct id, process the attribute...

		if (currenttag==AUTHENTICATIONS_TAG) {
			authentications.append("=\"");
			authentications.append(value)->append("\"");
		} else if (currenttag==TRANSLATIONS_TAG) {
			translations.append("=\"");
			translations.append(value)->append("\"");
		} else if (currenttag==RESULTSETTRANSLATIONS_TAG) {
			resultsettranslations.append("=\"");
			resultsettranslations.append(value)->append("\"");
		} else if (currenttag==TRIGGERS_TAG) {
			triggers.append("=\"");
			triggers.append(value)->append("\"");
		} else if (currenttag==LOGGERS_TAG) {
			loggers.append("=\"");
			loggers.append(value)->append("\"");
		} else if (currenttag==QUERIES_TAG) {
			queries.append("=\"");
			queries.append(value)->append("\"");
		} else if (currenttag==PASSWORDENCRYPTIONS_TAG) {
			passwordencryptions.append("=\"");
			passwordencryptions.append(value)->append("\"");
		} else if (currentattribute==ADDRESSES_ATTRIBUTE) {
			uint64_t	index;
			for (index=0; index<addresscount; index++) {
				delete[] addresses[index];
			}
			delete[] addresses;
			// if the attribute was left blank, assume 0.0.0.0
			if (!charstring::length(value)) {
				value=DEFAULT_ADDRESS;
			}
			charstring::split(value,",",true,
						&addresses,&addresscount);
			for (index=0; index<addresscount; index++) {
				charstring::bothTrim(addresses[index]);
			}
		} else if (currentattribute==PORT_ATTRIBUTE) {
			port=atouint32_t(value,"0",0);
		} else if (currentattribute==SOCKET_ATTRIBUTE) {
			delete[] unixport;
			unixport=charstring::duplicate(value);
		} else if (currentattribute==DBASE_ATTRIBUTE) {
			delete[] dbase;
			dbase=charstring::duplicate((value)?value:
							DEFAULT_DBASE);
		} else if (currentattribute==CONNECTIONS_ATTRIBUTE) {
			connections=atouint32_t(value,DEFAULT_CONNECTIONS,0);
			if (connections>MAXCONNECTIONS) {
				connections=MAXCONNECTIONS;
			}
			if (maxconnections<connections) {
				maxconnections=connections;
			}
		} else if (currentattribute==MAXCONNECTIONS_ATTRIBUTE) {
			maxconnections=atouint32_t(value,DEFAULT_CONNECTIONS,1);
			if (maxconnections>MAXCONNECTIONS) {
				maxconnections=MAXCONNECTIONS;
			}
			if (maxconnections<connections) {
				maxconnections=connections;
			}
		} else if (currentattribute==MAXQUEUELENGTH_ATTRIBUTE) {
			maxqueuelength=
				atouint32_t(value,DEFAULT_MAXQUEUELENGTH,0);
		} else if (currentattribute==GROWBY_ATTRIBUTE) {
			growby=atouint32_t(value,DEFAULT_GROWBY,1);
		} else if (currentattribute==TTL_ATTRIBUTE) {
			ttl=atouint32_t(value,DEFAULT_TTL,0);
		} else if (currentattribute==MAXSESSIONCOUNT_ATTRIBUTE) {
			maxsessioncount=
				atouint32_t(value,DEFAULT_MAXSESSIONCOUNT,0);
		} else if (currentattribute==ENDOFSESSION_ATTRIBUTE) {
			delete[] endofsession;
			endofsession=charstring::duplicate((value)?value:
							DEFAULT_ENDOFSESSION);
			endofsessioncommit=
				!charstring::compare(endofsession,"commit");
		} else if (currentattribute==SESSIONTIMEOUT_ATTRIBUTE) {
			sessiontimeout=
				atouint32_t(value,DEFAULT_SESSIONTIMEOUT,1);
		} else if (currentattribute==RUNASUSER_ATTRIBUTE) {
			delete[] runasuser;
			runasuser=charstring::duplicate((value)?value:
							DEFAULT_RUNASUSER);
		} else if (currentattribute==RUNASGROUP_ATTRIBUTE) {
			delete[] runasgroup;
			runasgroup=charstring::duplicate((value)?value:
							DEFAULT_RUNASGROUP);
		} else if (currentattribute==CURSORS_ATTRIBUTE) {
			cursors=atouint32_t(value,DEFAULT_CURSORS,0);
			if (maxcursors<cursors) {
				maxcursors=cursors;
			}
		} else if (currentattribute==MAXCURSORS_ATTRIBUTE) {
			maxcursors=atouint32_t(value,DEFAULT_CURSORS,1);
			if (maxcursors<cursors) {
				maxcursors=cursors;
			}
		} else if (currentattribute==CURSORS_GROWBY_ATTRIBUTE) {
			cursorsgrowby=atouint32_t(value,
						DEFAULT_CURSORS_GROWBY,1);
		} else if (currentattribute==AUTHTIER_ATTRIBUTE) {
			delete[] authtier;
			authtier=charstring::duplicate((value)?value:
							DEFAULT_AUTHTIER);
			authondatabase=
				!charstring::compare(authtier,"database");
			authonconnection=
				charstring::compare(authtier,"database");
		} else if (currentattribute==SESSION_HANDLER_ATTRIBUTE) {
			delete[] sessionhandler;
			sessionhandler=charstring::duplicate((value)?value:
						DEFAULT_SESSION_HANDLER);
		} else if (currentattribute==HANDOFF_ATTRIBUTE) {
			delete[] handoff;
			handoff=charstring::duplicate((value)?value:
							DEFAULT_HANDOFF);
		} else if (currentattribute==DENIEDIPS_ATTRIBUTE) {
			delete[] deniedips;
			deniedips=charstring::duplicate((value)?value:
							DEFAULT_DENIEDIPS);
		} else if (currentattribute==ALLOWEDIPS_ATTRIBUTE) {
			delete[] allowedips;
			allowedips=charstring::duplicate((value)?value:
							DEFAULT_DENIEDIPS);
		} else if (currentattribute==DEBUG_ATTRIBUTE) {
			delete[] debug;
			debug=charstring::duplicate((value)?value:
							DEFAULT_DEBUG);
			debugtranslations=charstring::contains(debug,
							"translations");
			debugtriggers=charstring::contains(debug,
							"triggers");
			debugbindtranslations=charstring::contains(debug,
							"bindtranslations");
		} else if (currentattribute==MAXCLIENTINFOLENGTH_ATTRIBUTE) {
			maxclientinfolength=
				charstring::toInteger((value)?value:
						DEFAULT_MAXCLIENTINFOLENGTH);
		} else if (currentattribute==MAXQUERYSIZE_ATTRIBUTE) {
			maxquerysize=charstring::toInteger((value)?value:
							DEFAULT_MAXQUERYSIZE);
		} else if (currentattribute==MAXBINDCOUNT_ATTRIBUTE) {
			maxbindcount=charstring::toInteger((value)?value:
							DEFAULT_MAXBINDCOUNT);
		} else if (currentattribute==MAXBINDNAMELENGTH_ATTRIBUTE) {
			maxbindnamelength=
				charstring::toInteger((value)?value:
						DEFAULT_MAXBINDNAMELENGTH);
		} else if (currentattribute==
				MAXSTRINGBINDVALUELENGTH_ATTRIBUTE) {
			maxstringbindvaluelength=
				charstring::toUnsignedInteger((value)?value:
					DEFAULT_MAXSTRINGBINDVALUELENGTH);
		} else if (currentattribute==MAXLOBBINDVALUELENGTH_ATTRIBUTE) {
			maxlobbindvaluelength=
				charstring::toUnsignedInteger((value)?value:
						DEFAULT_MAXLOBBINDVALUELENGTH);
		} else if (currentattribute==MAXERRORLENGTH_ATTRIBUTE) {
			maxerrorlength=
				charstring::toUnsignedInteger((value)?value:
							DEFAULT_MAXERRORLENGTH);
		} else if (currentattribute==IDLECLIENTTIMEOUT_ATTRIBUTE) {
			idleclienttimeout=charstring::toInteger((value)?value:
						DEFAULT_IDLECLIENTTIMEOUT);
		} else if (currentattribute==USER_ATTRIBUTE) {
			currentuser->setUser((value)?value:"");
		} else if (currentattribute==PASSWORD_ATTRIBUTE) {
			currentuser->setPassword((value)?value:"");
		} else if (currentattribute==PASSWORDENCRYPTION_ATTRIBUTE) {
			if (currenttag==USERS_TAG) {
				currentuser->setPasswordEncryption(
							(value)?value:NULL);
			} else if (currenttag==CONNECTIONS_TAG &&
							currentconnect) {
				currentconnect->setPasswordEncryption(
							(value)?value:NULL);
			}
		} else if (currentattribute==CONNECTIONID_ATTRIBUTE) {
			if (currentconnect) {
				if (charstring::length(value)>
						MAXCONNECTIONIDLEN) {
					stderror.printf("error: connectionid \"%s\" is too long, must be %d characters or fewer.\n",value,MAXCONNECTIONIDLEN);
					return false;
				}
				if (!value) {
					stringbuffer	connectionid;
					connectionid.append(id)->append("-");
					connectionid.append(connectioncount);
					currentconnect->setConnectionId(
						connectionid.getString());
				} else {
					currentconnect->setConnectionId(value);
				}
			}
		} else if (currentattribute==STRING_ATTRIBUTE) {
			if (currentconnect) {
				currentconnect->setString((value)?value:
							DEFAULT_CONNECTSTRING);
				currentconnect->parseConnectString();
			}
		} else if (currentattribute==METRIC_ATTRIBUTE) {
			if (currentconnect) {
				currentconnect->setMetric(
					atouint32_t(value,DEFAULT_METRIC,1));
			}
		} else if (currentattribute==BEHINDLOADBALANCER_ATTRIBUTE) {
			if (currentconnect) {
				currentconnect->setBehindLoadBalancer(
					!charstring::compareIgnoringCase(
								value,"yes"));
			}
		} else if (currentattribute==ROUTER_HOST_ATTRIBUTE) {
			currentroute->setHost((value)?value:
							DEFAULT_ROUTER_HOST);
		} else if (currentattribute==ROUTER_PORT_ATTRIBUTE) {
			currentroute->setPort(atouint32_t(value,
							DEFAULT_ROUTER_PORT,1));
		} else if (currentattribute==ROUTER_SOCKET_ATTRIBUTE) {
			currentroute->setSocket((value)?value:
							DEFAULT_ROUTER_SOCKET);
		} else if (currentattribute==ROUTER_USER_ATTRIBUTE) {
			currentroute->setUser((value)?value:
							DEFAULT_ROUTER_USER);
		} else if (currentattribute==ROUTER_PASSWORD_ATTRIBUTE) {
			currentroute->setPassword((value)?value:
						DEFAULT_ROUTER_PASSWORD);
		} else if (currentattribute==ROUTER_PATTERN_ATTRIBUTE) {
			regularexpression	*re=
				new regularexpression(
					(value)?value:DEFAULT_ROUTER_PATTERN);
			re->study();
			currentroute->getRegexList()->append(re);
		} else if (currentattribute==MAXLISTENERS_ATTRIBUTE) {
			maxlisteners=charstring::toInteger(
					(value)?value:DEFAULT_MAXLISTENERS);
			if (maxlisteners<-1) {
				maxlisteners=-1;
			}
		} else if (currentattribute==LISTENERTIMEOUT_ATTRIBUTE) {
			listenertimeout=
				charstring::toUnsignedInteger(
					(value)?value:DEFAULT_LISTENERTIMEOUT);
		} else if (currentattribute==RELOGINATSTART_ATTRIBUTE) {
			reloginatstart=
				!charstring::compareIgnoringCase(value,"yes");
		} else if (currentattribute==FAKEINPUTBINDVARIABLES_ATTRIBUTE) {
			fakeinputbindvariables=
				!charstring::compareIgnoringCase(value,"yes");
		} else if (currentattribute==TRANSLATEBINDVARIABLES_ATTRIBUTE) {
			translatebindvariables=
				!charstring::compareIgnoringCase(value,"yes");
		} else if (currentattribute==ISOLATIONLEVEL_ATTRIBUTE) {
			isolationlevel=charstring::duplicate(value);
		} else if (currentattribute==IGNORESELECTDB_ATTRIBUTE) {
			ignoreselectdb=
				!charstring::compareIgnoringCase(value,"yes");
		} else if (currentattribute==WAITFORDOWNDB_ATTRIBUTE) {
			waitfordowndb=
				!charstring::compareIgnoringCase(value,"yes");
		} else if (currentattribute==DATETIMEFORMAT_ATTRIBUTE) {
			delete[] datetimeformat;
			datetimeformat=charstring::duplicate(value);
			if (!dateformat) {
				dateformat=charstring::duplicate(value);
			}
			if (!timeformat) {
				timeformat=charstring::duplicate(value);
			}
		} else if (currentattribute==DATEFORMAT_ATTRIBUTE) {
			delete[] dateformat;
			dateformat=charstring::duplicate(value);
			if (!datetimeformat) {
				datetimeformat=charstring::duplicate(value);
			}
			if (!timeformat) {
				timeformat=charstring::duplicate(value);
			}
		} else if (currentattribute==TIMEFORMAT_ATTRIBUTE) {
			delete[] timeformat;
			timeformat=charstring::duplicate(value);
			if (!datetimeformat) {
				datetimeformat=charstring::duplicate(value);
			}
			if (!dateformat) {
				dateformat=charstring::duplicate(value);
			}
		} else if (currentattribute==DATEDDMM_ATTRIBUTE) {
			dateddmm=!charstring::compareIgnoringCase(value,"yes");
			if (!dateyyyyddmmset) {
				dateyyyyddmm=dateddmm;
			}
		} else if (currentattribute==DATEYYYYDDMM_ATTRIBUTE) {
			dateyyyyddmm=
				!charstring::compareIgnoringCase(value,"yes");
			dateyyyyddmmset=true;
		}
	}
	return true;
}

bool sqlrconfigfile::text(const char *string) {

	if (currenttag==RUNQUERY_TAG) {
		linkedlist< char * >	*ptr=NULL;
		if (instart) {
			ptr=&sessionstartqueries;
		} else if (inend) {
			ptr=&sessionendqueries;
		}
		if (ptr) {
			ptr->append(charstring::duplicate(string));
		}
	}

	return true;
}

uint32_t sqlrconfigfile::atouint32_t(const char *value,
				const char *defaultvalue, uint32_t minvalue) {
	uint32_t	retval=charstring::toUnsignedInteger(
						(value)?value:defaultvalue);
	if (retval<minvalue) {
		retval=charstring::toUnsignedInteger(defaultvalue);
	}
	return retval;
}

int32_t sqlrconfigfile::atoint32_t(const char *value,
				const char *defaultvalue, int32_t minvalue) {
	int32_t	retval=charstring::toInteger((value)?value:defaultvalue);
	if (retval<minvalue) {
		retval=charstring::toInteger(defaultvalue);
	}
	return retval;
}

routecontainer *sqlrconfigfile::routeAlreadyExists(routecontainer *cur) {

	for (routenode *rn=routelist.getFirst(); rn; rn=rn->getNext()) {

		routecontainer	*rc=rn->getValue();
		if (!charstring::compare(rc->getHost(),
					cur->getHost()) &&
			rc->getPort()==cur->getPort() &&
			!charstring::compare(rc->getSocket(),
						cur->getSocket()) &&
			!charstring::compare(rc->getUser(),
						cur->getUser()) &&
			!charstring::compare(rc->getPassword(),
						cur->getPassword())) {
			return rc;
		}
	}
	return NULL;
}

void sqlrconfigfile::moveRegexList(routecontainer *cur,
					routecontainer *existing) {

	for (linkedlistnode< regularexpression * > *re=
				cur->getRegexList()->getFirst();
						re; re=re->getNext()) {
		existing->getRegexList()->append(re->getValue());
	}
	cur->getRegexList()->clear();
}

bool sqlrconfigfile::parse(const char *config, const char *id) {

	// init some variables
	this->id=id;
	correctid=false;
	done=false;

	// attempt to parse the config file
	if (!config || !config[0]) {
		config=DEFAULT_CONFIG_FILE;
	}
	parseFile(config);

	// attempt to parse files in the config dir
	directory	d;
	stringbuffer	fullpath;
	bool		iswindows=!charstring::compareIgnoringCase(
						sys::getOperatingSystemName(),
						"Windows");
	if (!done && d.open(DEFAULT_CONFIG_DIR)) {
		for (;;) {
			char	*filename=d.read();
			if (!filename) {
				break;
			}
			if (charstring::compare(filename,".") &&
				charstring::compare(filename,"..")) {

				fullpath.clear();
				fullpath.append(DEFAULT_CONFIG_DIR);
				if (iswindows) {
					fullpath.append("\\");
				} else {
					fullpath.append("/");
				}
				fullpath.append(filename);
				delete[] filename;

				parseFile(fullpath.getString());
			}
		}
	}
	d.close();

	// attempt to parse the user's .sqlrelay.conf file
	const char	*homedir=environment::getValue("HOME");
	char		*filename;
	if (homedir && homedir[0]) {
		size_t	filenamelen=charstring::length(homedir)+15+1;
		filename=new char[filenamelen];
		charstring::copy(filename,homedir);
		charstring::append(filename,"/.sqlrelay.conf");
	} else {
		filename=charstring::duplicate("~/.sqlrelay.conf");
	}
	parseFile(filename);
	delete[] filename;

	// warn the user if the specified instance wasn't found
	if (!done) {
		stderror.printf("Couldn't find id %s.\n",id);
	}

	return done;
}

bool sqlrconfigfile::accessible() {
	// FIXME: implement this
	return true;
}

usercontainer::usercontainer() {
	user=NULL;
	password=NULL;
	pwdenc=NULL;
}

usercontainer::~usercontainer() {
	delete[] user;
	delete[] password;
	delete[] pwdenc;
}

void usercontainer::setUser(const char *user) {
	this->user=charstring::duplicate(user);
}

void usercontainer::setPassword(const char *password) {
	this->password=charstring::duplicate(password);
}

void usercontainer::setPasswordEncryption(const char *pwdenc) {
	this->pwdenc=charstring::duplicate(pwdenc);
}

const char *usercontainer::getUser() {
	return user;
}

const char *usercontainer::getPassword() {
	return password;
}

const char *usercontainer::getPasswordEncryption() {
	return pwdenc;
}

connectstringcontainer::connectstringcontainer() {
	connectionid=NULL;
	string=NULL;
	metric=charstring::toInteger(DEFAULT_METRIC);
	behindloadbalancer=!charstring::compareIgnoringCase(
					DEFAULT_BEHINDLOADBALANCER,"yes");
	pwdenc=NULL;
}

connectstringcontainer::~connectstringcontainer() {
	delete[] string;
	delete[] connectionid;
	delete[] pwdenc;
}

void connectstringcontainer::setConnectionId(const char *connectionid) {
	this->connectionid=charstring::duplicate(connectionid);
}

void connectstringcontainer::setString(const char *string) {
	this->string=charstring::duplicate(string);
}

void connectstringcontainer::setMetric(uint32_t metric) {
	this->metric=metric;
}

void connectstringcontainer::setBehindLoadBalancer(bool behindloadbalancer) {
	this->behindloadbalancer=behindloadbalancer;
}

void connectstringcontainer::setPasswordEncryption(const char *pwdenc) {
	this->pwdenc=charstring::duplicate(pwdenc);
}

const char *connectstringcontainer::getConnectionId() {
	return connectionid;
}

const char *connectstringcontainer::getString() {
	return string;
}

uint32_t connectstringcontainer::getMetric() {
	return metric;
}

bool connectstringcontainer::getBehindLoadBalancer() {
	return behindloadbalancer;
}

const char *connectstringcontainer::getPasswordEncryption() {
	return pwdenc;
}

void connectstringcontainer::parseConnectString() {
	connectstring.parse(string);
}

const char *connectstringcontainer::getConnectStringValue(
						const char *variable) {
	return connectstring.getValue(variable);
}


routecontainer::routecontainer() {
	isfilter=false;
	host=NULL;
	port=0;
	socket=NULL;
	user=NULL;
	password=NULL;
}

routecontainer::~routecontainer() {
	delete[] host;
	delete[] socket;
	delete[] user;
	delete[] password;
	for (linkedlistnode< regularexpression * > *re=
					regexlist.getFirst();
						re; re=re->getNext()) {
		delete re->getValue();
	}
}

void routecontainer::setIsFilter(bool isfilter) {
	this->isfilter=isfilter;
}

void routecontainer::setHost(const char *host) {
	this->host=charstring::duplicate(host);
}

void routecontainer::setPort(uint16_t port) {
	this->port=port;
}

void routecontainer::setSocket(const char *socket) {
	this->socket=charstring::duplicate(socket);
}

void routecontainer::setUser(const char *user) {
	this->user=charstring::duplicate(user);
}

void routecontainer::setPassword(const char *password) {
	this->password=charstring::duplicate(password);
}

bool routecontainer::getIsFilter() {
	return isfilter;
}

const char *routecontainer::getHost() {
	return host;
} 

uint16_t routecontainer::getPort() {
	return port;
} 

const char *routecontainer::getSocket() {
	return socket;
} 

const char *routecontainer::getUser() {
	return user;
} 

const char *routecontainer::getPassword() {
	return password;
} 

linkedlist< regularexpression * > *routecontainer::getRegexList() {
	return &regexlist;
}
