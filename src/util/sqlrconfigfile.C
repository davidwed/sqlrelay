// Copyright (c) 2000-2005  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrconfigfile.h>
#include <rudiments/stringbuffer.h>

#include <stdlib.h>

#include <defines.h>
#include <defaults.h>

sqlrconfigfile::sqlrconfigfile() : xmlsax() {
	port=charstring::toInteger(DEFAULT_PORT);
	listenoninet=(port)?true:false;
	unixport=charstring::duplicate(DEFAULT_SOCKET);
	listenonunix=(unixport[0])?true:false;
	dbase=charstring::duplicate(DEFAULT_DBASE);
	connections=charstring::toInteger(DEFAULT_CONNECTIONS);
	maxconnections=charstring::toInteger(DEFAULT_MAXCONNECTIONS);
	maxqueuelength=charstring::toInteger(DEFAULT_MAXQUEUELENGTH);
	growby=charstring::toInteger(DEFAULT_GROWBY);
	ttl=charstring::toInteger(DEFAULT_TTL);
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
	currentuser=NULL;
	firstconnect=NULL;
	currentconnect=NULL;
	connectstringcount=0;
	metrictotal=0;
}

sqlrconfigfile::~sqlrconfigfile() {

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

	usernode	*un=userlist.getNodeByIndex(0);
	while (un) {
		delete un->getData();
		un=un->getNext();
	}

	connectstringnode	*csn=connectstringlist.getNodeByIndex(0);
	while (csn) {
		delete csn->getData();
		csn=csn->getNext();
	}
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

bool sqlrconfigfile::getDynamicScaling() {
	return (maxqueuelength>=0 && maxconnections>connections &&
			growby>0 && ttl>0);
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
	connectstringnode	*csn=connectstringlist.getNodeByIndex(0);
	while (csn) {
		if (!charstring::compare(connectionid,
					csn->getData()->getConnectionId())) {
			return csn->getData();
		}
		csn=csn->getNext();
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
		connectstringnode	*csn=
					connectstringlist.getNodeByIndex(0);
		while (csn) {
			metrictotal=metrictotal+csn->getData()->getMetric();
			csn=csn->getNext();
		}
	}
	return metrictotal;
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
		currentconnect=new connectstringcontainer(connectstringcount);
		connectstringlist.append(currentconnect);
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
	} else if (!charstring::compare(name,"user")) {
		currentattribute=USER_ATTRIBUTE;
	} else if (!charstring::compare(name,"password")) {
		currentattribute=PASSWORD_ATTRIBUTE;
	} else if (!charstring::compare(name,"connectionid")) {
		currentattribute=CONNECTIONID_ATTRIBUTE;
	} else if (!charstring::compare(name,"string")) {
		currentattribute=STRING_ATTRIBUTE;
	} else if (!charstring::compare(name,"metric")) {
		currentattribute=METRIC_ATTRIBUTE;
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
		if (currentattribute==PORT_ATTRIBUTE) {
			port=atouint32_t(value,DEFAULT_PORT,1);
			listenoninet=true;
		} else if (currentattribute==SOCKET_ATTRIBUTE) {
			delete[] unixport;
			unixport=charstring::duplicate((value)?value:
							DEFAULT_SOCKET);
			listenonunix=(unixport[0]!=(char)NULL);
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
		} else if (currentattribute==USER_ATTRIBUTE) {
			currentuser->setUser((value)?value:DEFAULT_USER);
		} else if (currentattribute==PASSWORD_ATTRIBUTE) {
			currentuser->setPassword((value)?value:
							DEFAULT_PASSWORD);
		} else if (currentattribute==CONNECTIONID_ATTRIBUTE) {
			if (charstring::length(value)>MAXCONNECTIONIDLEN) {
				fprintf(stderr,"error: connectionid \"%s\" is too long, must be %d characters or fewer.\n",value,MAXCONNECTIONIDLEN);
				return false;
			}
			currentconnect->setConnectionId((value)?value:
							DEFAULT_CONNECTIONID);
		} else if (currentattribute==STRING_ATTRIBUTE) {
			currentconnect->setString((value)?value:
							DEFAULT_CONNECTSTRING);
			currentconnect->parseConnectString();
		} else if (currentattribute==METRIC_ATTRIBUTE) {
			currentconnect->setMetric(
					atouint32_t(value,DEFAULT_METRIC,1));
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
	if (user) {
		this->user=charstring::duplicate(user);
	}
}

void usercontainer::setPassword(const char *password) {
	if (password) {
		this->password=charstring::duplicate(password);
	}
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
}

connectstringcontainer::~connectstringcontainer() {
	delete[] string;
	delete[] connectionid;
}

void connectstringcontainer::setConnectionId(const char *connectionid) {
	if (connectionid) {
		this->connectionid=charstring::duplicate(connectionid);
	}
}

void connectstringcontainer::setString(const char *string) {
	if (string) {
		this->string=charstring::duplicate(string);
	}
}

void connectstringcontainer::setMetric(uint32_t metric) {
	this->metric=metric;
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
