// Copyright (c) 2000-2005  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrconfigfile.h>
#include <rudiments/stringbuffer.h>

#include <stdlib.h>

#include <defines.h>
#include <defaults.h>

sqlrconfigfile::sqlrconfigfile() : xmlsax() {
	addresses=new char *[1];
	addresses[0]=charstring::duplicate("0.0.0.0");
	addresscount=1;
	port=0;
	listenoninet=false;
	unixport=charstring::duplicate("");
	listenonunix=false;
	dbase=charstring::duplicate(DEFAULT_DBASE);
	connections=charstring::toInteger(DEFAULT_CONNECTIONS);
	maxconnections=charstring::toInteger(DEFAULT_MAXCONNECTIONS);
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
	authtier=charstring::duplicate(DEFAULT_AUTHTIER);
	authonlistener=charstring::contains(authtier,"listener");
	authonconnection=charstring::contains(authtier,"connection");
	authondatabase=!charstring::compare(authtier,"database");
	handoff=charstring::duplicate(DEFAULT_HANDOFF);
	passdescriptor=!charstring::compare(handoff,"pass");
	allowedips=charstring::duplicate(DEFAULT_DENIEDIPS);
	deniedips=charstring::duplicate(DEFAULT_DENIEDIPS);
	debug=charstring::duplicate(DEFAULT_DEBUG);
	debuglistener=charstring::contains(debug,"listener");
	debugconnection=charstring::contains(debug,"connection");
	maxquerysize=charstring::toInteger(DEFAULT_MAXQUERYSIZE);
	maxstringbindvaluelength=charstring::toInteger(
					DEFAULT_MAXSTRINGBINDVALUELENGTH);
	maxlobbindvaluelength=charstring::toInteger(
					DEFAULT_MAXLOBBINDVALUELENGTH);
	idleclienttimeout=charstring::toInteger(DEFAULT_IDLECLIENTTIMEOUT);
	currentuser=NULL;
	firstconnect=NULL;
	currentconnect=NULL;
	connectstringcount=0;
	metrictotal=0;
	sidenabled=DEFAULT_SID_ENABLED;
	sidhost=charstring::duplicate(DEFAULT_SID_HOST);
	sidport=DEFAULT_SID_PORT;
	sidsocket=charstring::duplicate(DEFAULT_SID_SOCKET);
	siduser=charstring::duplicate(DEFAULT_SID_USER);
	sidpassword=charstring::duplicate(DEFAULT_SID_PASSWORD);
	maxlisteners=charstring::toInteger(DEFAULT_MAXLISTENERS);
	listenertimeout=charstring::toUnsignedInteger(DEFAULT_LISTENERTIMEOUT);
	reloginatstart=!charstring::compare(DEFAULT_RELOGINATSTART,"yes");
	timequeriessec=charstring::toInteger(DEFAULT_TIMEQUERIESSEC);
	timequeriesusec=charstring::toInteger(DEFAULT_TIMEQUERIESUSEC);
	currentroute=NULL;
	inrouter=false;
	ignoreconnections=false;
}

sqlrconfigfile::~sqlrconfigfile() {

	for (uint64_t index=0; index<addresscount; index++) {
		delete[] addresses[index];
	}
	delete[] addresses;

	delete[] dbase;
	delete[] unixport;
	delete[] endofsession;
	delete[] runasuser;
	delete[] runasgroup;
	delete[] authtier;
	delete[] handoff;
	delete[] allowedips;
	delete[] deniedips;
	delete[] debug;

	for (usernode *un=userlist.getFirstNode();
				un; un=un->getNext()) {
		delete un->getData();
	}

	for (connectstringnode *csn=connectstringlist.getFirstNode();
						csn; csn=csn->getNext()) {
		delete csn->getData();
	}

	delete[] sidhost;
	delete[] sidsocket;
	delete[] siduser;
	delete[] sidpassword;

	for (routenode *rn=routelist.getFirstNode(); rn; rn=rn->getNext()) {
		delete rn->getData();
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

uint32_t sqlrconfigfile::getTtl() {
	return ttl;
}

uint16_t sqlrconfigfile::getMaxSessionCount() {
	return maxsessioncount;
}

bool sqlrconfigfile::getDynamicScaling() {
	return (maxconnections>connections && growby>0 && ttl>0 &&
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

const char *sqlrconfigfile::getAuthTier() {
	return authtier;
}

bool sqlrconfigfile::getAuthOnListener() {
	return authonlistener;
}

bool sqlrconfigfile::getAuthOnConnection() {
	return authonconnection;
}

bool sqlrconfigfile::getAuthOnDatabase() {
	return authondatabase;
}

const char *sqlrconfigfile::getHandOff() {
	return handoff;
}

bool sqlrconfigfile::getPassDescriptor() {
	return passdescriptor;
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

bool sqlrconfigfile::getDebugListener() {
	return debuglistener;
}

bool sqlrconfigfile::getDebugConnection() {
	return debugconnection;
}

uint32_t sqlrconfigfile::getMaxQuerySize() {
	return maxquerysize;
}

uint32_t sqlrconfigfile::getMaxStringBindValueLength() {
	return maxstringbindvaluelength;
}

uint32_t sqlrconfigfile::getMaxLobBindValueLength() {
	return maxlobbindvaluelength;
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

int64_t sqlrconfigfile::getTimeQueriesSeconds() {
	return timequeriessec;
}

int64_t sqlrconfigfile::getTimeQueriesMicroSeconds() {
	return timequeriesusec;
}

bool sqlrconfigfile::getSidEnabled() {
	return sidenabled;
}

const char *sqlrconfigfile::getSidHost() {
	return sidhost;
}

uint16_t sqlrconfigfile::getSidPort() {
	return sidport;
}

const char *sqlrconfigfile::getSidUnixPort() {
	return sidsocket;
}

const char *sqlrconfigfile::getSidUser() {
	return siduser;
}

const char *sqlrconfigfile::getSidPassword() {
	return sidpassword;
}

linkedlist< usercontainer * > *sqlrconfigfile::getUserList() {
	// if there are no users in the list, add a default user/password
	if (!userlist.getLength()) {
		currentuser=new usercontainer();
		currentuser->setUser(DEFAULT_USER);
		currentuser->setPassword(DEFAULT_PASSWORD);
		userlist.append(currentuser);
	}
	return &userlist;
}

linkedlist< connectstringcontainer * > *sqlrconfigfile::getConnectStringList() {
	return &connectstringlist;
}

connectstringcontainer *sqlrconfigfile::getConnectString(
						const char *connectionid) {
	for (connectstringnode *csn=connectstringlist.getFirstNode();
						csn; csn=csn->getNext()) {
		if (!charstring::compare(connectionid,
					csn->getData()->getConnectionId())) {
			return csn->getData();
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
		for (connectstringnode *csn=
				connectstringlist.getFirstNode();
						csn; csn=csn->getNext()) {
			metrictotal=metrictotal+csn->getData()->getMetric();
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

	// set the current tag
	if (!charstring::compare(name,"user")) {
		currentuser=new usercontainer();
		userlist.append(currentuser);
	} else if (!charstring::compare(name,"connection")) {
		if (!ignoreconnections) {
			currentconnect=
				new connectstringcontainer(connectstringcount);
			connectstringlist.append(currentconnect);
		}
	} else if (!charstring::compare(name,"router")) {
		inrouter=true;
		currentconnect=new connectstringcontainer(connectstringcount);
		connectstringlist.append(currentconnect);
		currentconnect->setConnectionId(DEFAULT_CONNECTIONID);
		ignoreconnections=true;
	} else if (!charstring::compare(name,"route")) {
		currentroute=new routecontainer();
	} else if (!charstring::compare(name,"filter")) {
		currentroute=new routecontainer();
		currentroute->setIsFilter(true);
	}

	return true;
}

bool sqlrconfigfile::attributeName(const char *name) {

	// don't do anything if we're already done
	if (done) {
		return true;
	}

	// set the current attribute
	if (!charstring::compare(name,"id")) {
		currentattribute=ID_ATTRIBUTE;
	} else if (!charstring::compare(name,"addresses")) {
		currentattribute=ADDRESSES_ATTRIBUTE;
	} else if (!charstring::compare(name,"port")) {
		if (!inrouter) {
			currentattribute=PORT_ATTRIBUTE;
		} else {
			currentattribute=ROUTER_PORT_ATTRIBUTE;
		}
	} else if (!charstring::compare(name,"socket") ||
			!charstring::compare(name,"unixport")) {
		if (!inrouter) {
			currentattribute=SOCKET_ATTRIBUTE;
		} else {
			currentattribute=ROUTER_SOCKET_ATTRIBUTE;
		}
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
	} else if (!charstring::compare(name,"authtier") ||
			!charstring::compare(name,"authentication")) {
		currentattribute=AUTHTIER_ATTRIBUTE;
	} else if (!charstring::compare(name,"handoff")) {
		currentattribute=HANDOFF_ATTRIBUTE;
	} else if (!charstring::compare(name,"deniedips")) {
		currentattribute=DENIEDIPS_ATTRIBUTE;
	} else if (!charstring::compare(name,"allowedips")) {
		currentattribute=ALLOWEDIPS_ATTRIBUTE;
	} else if (!charstring::compare(name,"debug")) {
		currentattribute=DEBUG_ATTRIBUTE;
	} else if (!charstring::compare(name,"maxquerysize")) {
		currentattribute=MAXQUERYSIZE_ATTRIBUTE;
	} else if (!charstring::compare(name,"maxstringbindvaluelength")) {
		currentattribute=MAXSTRINGBINDVALUELENGTH_ATTRIBUTE;
	} else if (!charstring::compare(name,"maxlobbindvaluelength")) {
		currentattribute=MAXLOBBINDVALUELENGTH_ATTRIBUTE;
	} else if (!charstring::compare(name,"idleclienttimeout")) {
		currentattribute=IDLECLIENTTIMEOUT_ATTRIBUTE;
	} else if (!charstring::compare(name,"sidenabled")) {
		currentattribute=SID_ENABLED_ATTRIBUTE;
	} else if (!charstring::compare(name,"sidhost")) {
		currentattribute=SID_HOST_ATTRIBUTE;
	} else if (!charstring::compare(name,"sidport")) {
		currentattribute=SID_PORT_ATTRIBUTE;
	} else if (!charstring::compare(name,"sidsocket")) {
		currentattribute=SID_SOCKET_ATTRIBUTE;
	} else if (!charstring::compare(name,"siduser")) {
		currentattribute=SID_USER_ATTRIBUTE;
	} else if (!charstring::compare(name,"sidpassword")) {
		currentattribute=SID_PASSWORD_ATTRIBUTE;
	} else if (!charstring::compare(name,"user")) {
		if (!inrouter) {
			currentattribute=USER_ATTRIBUTE;
		} else {
			currentattribute=ROUTER_USER_ATTRIBUTE;
		}
	} else if (!charstring::compare(name,"password")) {
		if (!inrouter) {
			currentattribute=PASSWORD_ATTRIBUTE;
		} else {
			currentattribute=ROUTER_PASSWORD_ATTRIBUTE;
		}
	} else if (!charstring::compare(name,"connectionid")) {
		currentattribute=CONNECTIONID_ATTRIBUTE;
	} else if (!charstring::compare(name,"string")) {
		currentattribute=STRING_ATTRIBUTE;
	} else if (!charstring::compare(name,"metric")) {
		currentattribute=METRIC_ATTRIBUTE;
	} else if (!charstring::compare(name,"behindloadbalancer")) {
		currentattribute=BEHINDLOADBALANCER_ATTRIBUTE;
	} else if (!charstring::compare(name,"host")) {
		currentattribute=ROUTER_HOST_ATTRIBUTE;
	} else if (!charstring::compare(name,"pattern")) {
		currentattribute=ROUTER_PATTERN_ATTRIBUTE;
	} else if (!charstring::compare(name,"maxlisteners")) {
		currentattribute=MAXLISTENERS_ATTRIBUTE;
	} else if (!charstring::compare(name,"listenertimeout")) {
		currentattribute=LISTENERTIMEOUT_ATTRIBUTE;
	} else if (!charstring::compare(name,"reloginatstart")) {
		currentattribute=RELOGINATSTART_ATTRIBUTE;
	} else if (!charstring::compare(name,"timequeries")) {
		currentattribute=TIMEQUERIES_ATTRIBUTE;
	} else {
		currentattribute=(attribute)0;
	}
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

		// if we have found the correct id, process the attribute
		if (currentattribute==ADDRESSES_ATTRIBUTE) {
			for (uint64_t index=0; index<addresscount; index++) {
				delete[] addresses[index];
			}
			delete[] addresses;
			charstring::split(
				(value &&
				!charstring::contains(value,DEFAULT_ADDRESSES))?
				value:DEFAULT_ADDRESSES,
				",",true,&addresses,&addresscount);
			for (uint64_t index=0; index<addresscount; index++) {
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
		} else if (currentattribute==MAXCONNECTIONS_ATTRIBUTE) {
			maxconnections=
				atouint32_t(value,DEFAULT_MAXCONNECTIONS,1);
		} else if (currentattribute==MAXQUEUELENGTH_ATTRIBUTE) {
			maxqueuelength=
				atouint32_t(value,DEFAULT_MAXQUEUELENGTH,0);
		} else if (currentattribute==GROWBY_ATTRIBUTE) {
			growby=atouint32_t(value,DEFAULT_GROWBY,1);
		} else if (currentattribute==TTL_ATTRIBUTE) {
			ttl=atouint32_t(value,DEFAULT_TTL,1);
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
			cursors=atouint32_t(value,DEFAULT_CURSORS,1);
		} else if (currentattribute==AUTHTIER_ATTRIBUTE) {
			delete[] authtier;
			authtier=charstring::duplicate((value)?value:
							DEFAULT_AUTHTIER);
			authonlistener=charstring::contains(authtier,
								"listener");
			authonconnection=charstring::contains(authtier,
								"connection");
			authondatabase=
				!charstring::compare(authtier,"database");
		} else if (currentattribute==HANDOFF_ATTRIBUTE) {
			delete[] handoff;
			handoff=charstring::duplicate((value)?value:
							DEFAULT_HANDOFF);
			passdescriptor=!charstring::compare(handoff,"pass");
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
			debuglistener=charstring::contains(debug,
							"listener");
			debugconnection=charstring::contains(debug,
							"connection");
		} else if (currentattribute==MAXQUERYSIZE_ATTRIBUTE) {
			maxquerysize=charstring::toInteger((value)?value:
							DEFAULT_MAXQUERYSIZE);
		} else if (currentattribute==
				MAXSTRINGBINDVALUELENGTH_ATTRIBUTE) {
			maxstringbindvaluelength=
				charstring::toUnsignedInteger((value)?value:
					DEFAULT_MAXSTRINGBINDVALUELENGTH);
		} else if (currentattribute==MAXLOBBINDVALUELENGTH_ATTRIBUTE) {
			maxlobbindvaluelength=
				charstring::toUnsignedInteger((value)?value:
						DEFAULT_MAXLOBBINDVALUELENGTH);
		} else if (currentattribute==IDLECLIENTTIMEOUT_ATTRIBUTE) {
			idleclienttimeout=charstring::toInteger((value)?value:
						DEFAULT_IDLECLIENTTIMEOUT);
		} else if (currentattribute==SID_ENABLED_ATTRIBUTE) {
			sidenabled=
				!charstring::compareIgnoringCase(value,"yes");
		} else if (currentattribute==SID_HOST_ATTRIBUTE) {
			sidhost=charstring::duplicate((value)?value:
							DEFAULT_SID_HOST);
		} else if (currentattribute==SID_PORT_ATTRIBUTE) {
			sidport=(value)?charstring::toInteger(value):
							DEFAULT_SID_PORT;
		} else if (currentattribute==SID_SOCKET_ATTRIBUTE) {
			sidsocket=charstring::duplicate((value)?value:
							DEFAULT_SID_SOCKET);
		} else if (currentattribute==SID_USER_ATTRIBUTE) {
			siduser=charstring::duplicate((value)?value:
							DEFAULT_SID_USER);
		} else if (currentattribute==SID_PASSWORD_ATTRIBUTE) {
			sidpassword=charstring::duplicate((value)?value:
							DEFAULT_SID_PASSWORD);
		} else if (currentattribute==USER_ATTRIBUTE) {
			currentuser->setUser((value)?value:DEFAULT_USER);
		} else if (currentattribute==PASSWORD_ATTRIBUTE) {
			currentuser->setPassword((value)?value:
							DEFAULT_PASSWORD);
		} else if (currentattribute==CONNECTIONID_ATTRIBUTE) {
			if (currentconnect) {
				if (charstring::length(value)>
						MAXCONNECTIONIDLEN) {
					fprintf(stderr,"error: connectionid \"%s\" is too long, must be %d characters or fewer.\n",value,MAXCONNECTIONIDLEN);
					return false;
				}
				currentconnect->setConnectionId((value)?value:
							DEFAULT_CONNECTIONID);
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
		} else if (currentattribute==TIMEQUERIES_ATTRIBUTE) {
			if (charstring::toFloat(value)>0) {
				char		**list;
				uint64_t	listlength;
				charstring::split(value,".",true,
							&list,&listlength);
				timequeriessec=
					charstring::toInteger(list[0]);
				if (listlength>1) {
					char	buffer[7];
					size_t	list1len=
						charstring::length(list[1]);
					for (size_t i=0; i<6; i++) {
						buffer[i]=list[1][i];
						if (i>=list1len) {
							buffer[i]='0';
						}
					}
					buffer[6]='\0';
					timequeriesusec=
						charstring::toInteger(buffer);
				} else {
					timequeriesusec=0;
				}
				for (uint64_t i=0; i<listlength; i++) {
					delete[] list[i];
				}
				delete[] list;
			} else {
				timequeriessec=-1;
				timequeriesusec=-1;
			}
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

bool sqlrconfigfile::tagEnd(const char *name) {

	// if neither port nor socket were specified, use the default port
	if (!charstring::compare(name,"instance")) {
		if (!port && !unixport[0]) {
			port=charstring::toInteger(DEFAULT_PORT);
			addresscount=1;
		}
		listenoninet=(port)?true:false;
		listenonunix=(unixport[0])?true:false;
	}

	// don't do anything if we're already done
	// or have not found the correct id
	if (done || !correctid) {
		return true;
	}

	if (!charstring::compare(name,"router")) {
		inrouter=false;
	} else if (!charstring::compare(name,"route") ||
			!charstring::compare(name,"filter")) {
		routecontainer	*existingroute=routeAlreadyExists(currentroute);
		if (existingroute) {
			moveRegexList(currentroute,existingroute);
			delete currentroute;
		} else {
			routelist.append(currentroute);
		}
	}

	// don't do anything if we're already done
	// or have not found the correct id
	if (done || !correctid) {
		return true;
	}

	// we're done if we've found the right instance at this point
	if (correctid && !charstring::compare((char *)name,"instance")) {
		done=true;
	}
	return true;
}

routecontainer *sqlrconfigfile::routeAlreadyExists(routecontainer *cur) {

	for (routenode *rn=routelist.getFirstNode(); rn; rn=rn->getNext()) {

		routecontainer	*rc=rn->getData();
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
				cur->getRegexList()->getFirstNode();
						re; re=re->getNext()) {
		existing->getRegexList()->append(re->getData());
	}
	cur->getRegexList()->clear();
}

bool sqlrconfigfile::parse(const char *config, const char *id) {
	return parse(config,id,0);
}

bool sqlrconfigfile::parse(const char *config, const char *id,
					uint16_t connectstringcount) {

	// init some variables
	this->connectstringcount=connectstringcount;
	this->id=id;
	correctid=false;
	done=false;

	// parse the file
	bool	retval=true;
	if (!parseFile(config)) {
		fprintf(stderr,"Couldn't parse config file %s.\n",config);
		retval=false;
	}

	// parse the user's .sqlrelay.conf file
	const char	*homedir=getenv("HOME");
	char		*filename;
	if (homedir && homedir[0]) {
		size_t	filenamelen=charstring::length(homedir)+15+1;
		filename=new char[filenamelen];
		snprintf(filename,filenamelen,"%s/.sqlrelay.conf",homedir);
	} else {
		filename=charstring::duplicate("~/.sqlrelay.conf");
	}

	// see if the file exists before trying to parse it, don't worry about
	// an error message here
	parseFile(filename);
	delete[] filename;

	// if the specified instance wasn't found, warn the user
	if (!done) {
		fprintf(stderr,"Couldn't find id %s.\n",id);
		retval=false;
	}

	return retval;
}

usercontainer::usercontainer() {
	user=NULL;
	password=NULL;
}

usercontainer::~usercontainer() {
	delete[] user;
	delete[] password;
}

void usercontainer::setUser(const char *user) {
	this->user=charstring::duplicate(user);
}

void usercontainer::setPassword(const char *password) {
	this->password=charstring::duplicate(password);
}

const char *usercontainer::getUser() {
	return user;
}

const char *usercontainer::getPassword() {
	return password;
}

connectstringcontainer::connectstringcontainer(uint16_t connectstringcount) {
	this->connectstringcount=connectstringcount;
	connectionid=NULL;
	string=NULL;
	metric=charstring::toInteger(DEFAULT_METRIC);
	behindloadbalancer=!charstring::compareIgnoringCase(
					DEFAULT_BEHINDLOADBALANCER,"yes");
}

connectstringcontainer::~connectstringcontainer() {
	delete[] string;
	delete[] connectionid;
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

void connectstringcontainer::parseConnectString() {
	if (!connectstringcount) {
		return;
	}
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
					regexlist.getFirstNode();
						re; re=re->getNext()) {
		delete re->getData();
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
