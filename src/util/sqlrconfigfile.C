// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrconfigfile.h>
#include <rudiments/stringbuffer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
	#include <strings.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <defines.h>
#include <defaults.h>

sqlrconfigfile::sqlrconfigfile() : xmlsax() {
	port=::atoi(DEFAULT_PORT);
	listenoninet=(port)?1:0;
	unixport=strdup(DEFAULT_SOCKET);
	listenonunix=(unixport[0])?1:0;
	dbase=strdup(DEFAULT_DBASE);
	connections=::atoi(DEFAULT_CONNECTIONS);
	maxconnections=::atoi(DEFAULT_MAXCONNECTIONS);
	maxqueuelength=::atoi(DEFAULT_MAXQUEUELENGTH);
	growby=::atoi(DEFAULT_GROWBY);
	ttl=::atoi(DEFAULT_TTL);
	endofsession=strdup(DEFAULT_ENDOFSESSION);
	endofsessioncommit=!strcmp(endofsession,"commit");
	sessiontimeout=::atoi(DEFAULT_SESSIONTIMEOUT);
	runasuser=strdup(DEFAULT_RUNASUSER);
	runasgroup=strdup(DEFAULT_RUNASGROUP);
	cursors=::atoi(DEFAULT_CURSORS);
	authtier=strdup(DEFAULT_AUTHTIER);
	authonlistener=(strstr(authtier,"listener"))?1:0;
	authonconnection=(strstr(authtier,"connection"))?1:0;
	authondatabase=(!strcmp(authtier,"database"))?1:0;
	handoff=strdup(DEFAULT_HANDOFF);
	passdescriptor=(strcmp(handoff,"pass")==0);
	allowedips=strdup(DEFAULT_DENIEDIPS);
	deniedips=strdup(DEFAULT_DENIEDIPS);
	debug=strdup(DEFAULT_DEBUG);
	debuglistener=(strstr(debug,"listener"))?1:0;
	debugconnection=(strstr(debug,"connection"))?1:0;
	currentuser=NULL;
	firstconnect=NULL;
	currentconnect=NULL;
	connectstringcount=0;
	metrictotal=0;
}

sqlrconfigfile::~sqlrconfigfile() {

	if (dbase[0]) {
		delete[] dbase;
	}

	if (unixport[0]) {
		delete[] unixport;
	}

	if (endofsession[0]) {
		delete[] endofsession;
	}
	if (runasuser[0]) {
		delete[] runasuser;
	}
	if (runasgroup[0]) {
		delete[] runasgroup;
	}
	if (authtier[0]) {
		delete[] authtier;
	}
	if (handoff[0]) {
		delete[] handoff;
	}
	if (allowedips[0]) {
		delete[] allowedips;
	}
	if (deniedips[0]) {
		delete[] deniedips;
	}
	if (debug[0]) {
		delete[] debug;
	}

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

int sqlrconfigfile::getPort() {
	return port;
}

char *sqlrconfigfile::getUnixPort() {
	return unixport;
}

int sqlrconfigfile::getListenOnInet() {
	return listenoninet;
}

int sqlrconfigfile::getListenOnUnix() {
	return listenonunix;
}

char *sqlrconfigfile::getDbase() {
	return dbase;
}

int sqlrconfigfile::getConnections() {
	return connections;
}

int sqlrconfigfile::getMaxConnections() {
	return maxconnections;
}

int sqlrconfigfile::getMaxQueueLength() {
	return maxqueuelength;
}

int sqlrconfigfile::getGrowBy() {
	return growby;
}

int sqlrconfigfile::getTtl() {
	return ttl;
}

int sqlrconfigfile::getDynamicScaling() {
	return (maxqueuelength>=0 && maxconnections>connections &&
			growby>0 && ttl>0);
}

char *sqlrconfigfile::getEndOfSession() {
	return endofsession;
}

int sqlrconfigfile::getEndOfSessionCommit() {
	return endofsessioncommit;
}

int sqlrconfigfile::getSessionTimeout() {
	return sessiontimeout;
}

char *sqlrconfigfile::getRunAsUser() {
	return runasuser;
}

char *sqlrconfigfile::getRunAsGroup() {
	return runasgroup;
}

int sqlrconfigfile::getCursors() {
	return cursors;
}

char *sqlrconfigfile::getAuthTier() {
	return authtier;
}

int sqlrconfigfile::getAuthOnListener() {
	return authonlistener;
}

int sqlrconfigfile::getAuthOnConnection() {
	return authonconnection;
}

int sqlrconfigfile::getAuthOnDatabase() {
	return authondatabase;
}

char *sqlrconfigfile::getHandOff() {
	return handoff;
}

int sqlrconfigfile::getPassDescriptor() {
	// descriptor passing doesn't work if we're using dynamic scaling,
	// so override it here...
	return passdescriptor && !getDynamicScaling();
}

char *sqlrconfigfile::getAllowedIps() {
	return allowedips;
}

char *sqlrconfigfile::getDeniedIps() {
	return deniedips;
}

char *sqlrconfigfile::getDebug() {
	return debug;
}

int sqlrconfigfile::getDebugListener() {
	return debuglistener;
}

int sqlrconfigfile::getDebugConnection() {
	return debugconnection;
}

list< usercontainer * > *sqlrconfigfile::getUserList() {
	// if there are no users in the list, add a default user/password
	if (!userlist.getLength()) {
		currentuser=new usercontainer();
		currentuser->setUser(DEFAULT_USER);
		currentuser->setPassword(DEFAULT_PASSWORD);
		userlist.append(currentuser);
	}
	return &userlist;
}

list< connectstringcontainer * > *sqlrconfigfile::getConnectStringList() {
	return &connectstringlist;
}

connectstringcontainer *sqlrconfigfile::getConnectString(
						const char *connectionid) {
	connectstringnode	*csn=connectstringlist.getNodeByIndex(0);
	while (csn) {
		if (!strcmp(connectionid,csn->getData()->getConnectionId())) {
			return csn->getData();
		}
		csn=csn->getNext();
	}
	return NULL;
}

int sqlrconfigfile::getConnectionCount() {
	return connectstringlist.getLength();
}

int sqlrconfigfile::getMetricTotal() {
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

bool sqlrconfigfile::tagStart(char *name) {

	// don't do anything if we're already done
	// or have not found the correct id
	if (done || !correctid) {
		return true;
	}

	// set the current tag
	if (!strcmp(name,"user")) {
		currentuser=new usercontainer();
		userlist.append(currentuser);
	} else if (!strcmp(name,"connection")) {
		currentconnect=new connectstringcontainer(connectstringcount);
		connectstringlist.append(currentconnect);
	}

	return true;
}

bool sqlrconfigfile::attributeName(char *name) {

	// don't do anything if we're already done
	if (done) {
		return true;
	}

	// set the current attribute
	if (!strcmp(name,"id")) {
		currentattribute=ID_ATTRIBUTE;
	} else if (!strcmp(name,"port")) {
		currentattribute=PORT_ATTRIBUTE;
	} else if (!strcmp(name,"socket") || !strcmp(name,"unixport")) {
		currentattribute=SOCKET_ATTRIBUTE;
	} else if (!strcmp(name,"dbase")) {
		currentattribute=DBASE_ATTRIBUTE;
	} else if (!strcmp(name,"connections")) {
		currentattribute=CONNECTIONS_ATTRIBUTE;
	} else if (!strcmp(name,"maxconnections")) {
		currentattribute=MAXCONNECTIONS_ATTRIBUTE;
	} else if (!strcmp(name,"maxqueuelength")) {
		currentattribute=MAXQUEUELENGTH_ATTRIBUTE;
	} else if (!strcmp(name,"growby")) {
		currentattribute=GROWBY_ATTRIBUTE;
	} else if (!strcmp(name,"ttl")) {
		currentattribute=TTL_ATTRIBUTE;
	} else if (!strcmp(name,"endofsession")) {
		currentattribute=ENDOFSESSION_ATTRIBUTE;
	} else if (!strcmp(name,"sessiontimeout")) {
		currentattribute=SESSIONTIMEOUT_ATTRIBUTE;
	} else if (!strcmp(name,"runasuser")) {
		currentattribute=RUNASUSER_ATTRIBUTE;
	} else if (!strcmp(name,"runasgroup")) {
		currentattribute=RUNASGROUP_ATTRIBUTE;
	} else if (!strcmp(name,"cursors")) {
		currentattribute=CURSORS_ATTRIBUTE;
	} else if (!strcmp(name,"authtier") || !strcmp(name,"authentication")) {
		currentattribute=AUTHTIER_ATTRIBUTE;
	} else if (!strcmp(name,"handoff")) {
		currentattribute=HANDOFF_ATTRIBUTE;
	} else if (!strcmp(name,"deniedips")) {
		currentattribute=DENIEDIPS_ATTRIBUTE;
	} else if (!strcmp(name,"allowedips")) {
		currentattribute=ALLOWEDIPS_ATTRIBUTE;
	} else if (!strcmp(name,"debug")) {
		currentattribute=DEBUG_ATTRIBUTE;
	} else if (!strcmp(name,"user")) {
		currentattribute=USER_ATTRIBUTE;
	} else if (!strcmp(name,"password")) {
		currentattribute=PASSWORD_ATTRIBUTE;
	} else if (!strcmp(name,"connectionid")) {
		currentattribute=CONNECTIONID_ATTRIBUTE;
	} else if (!strcmp(name,"string")) {
		currentattribute=STRING_ATTRIBUTE;
	} else if (!strcmp(name,"metric")) {
		currentattribute=METRIC_ATTRIBUTE;
	}
	return true;
}

bool sqlrconfigfile::attributeValue(char *value) {

	// don't do anything if we're already done
	if (done) {
		return true;
	}

	if (!correctid) {

		// if we haven't found the correct id yet, check for it
		if (currentattribute==ID_ATTRIBUTE) {
			if (value && !strcmp(value,id)) {
				correctid=1;
			}
		}
	
	} else {

		// if we have found the correct id, process the attribute
		if (currentattribute==PORT_ATTRIBUTE) {
			port=atoi(value,DEFAULT_PORT,1);
			listenoninet=1;
		} else if (currentattribute==SOCKET_ATTRIBUTE) {
			delete[] unixport;
			unixport=strdup((value)?value:DEFAULT_SOCKET);
			listenonunix=(unixport[0]!=(char)NULL);
		} else if (currentattribute==DBASE_ATTRIBUTE) {
			delete[] dbase;
			dbase=strdup((value)?value:DEFAULT_DBASE);
		} else if (currentattribute==CONNECTIONS_ATTRIBUTE) {
			connections=atoi(value,DEFAULT_CONNECTIONS,1);
		} else if (currentattribute==MAXCONNECTIONS_ATTRIBUTE) {
			maxconnections=atoi(value,DEFAULT_MAXCONNECTIONS,1);
		} else if (currentattribute==MAXQUEUELENGTH_ATTRIBUTE) {
			maxqueuelength=atoi(value,DEFAULT_MAXQUEUELENGTH,1);
		} else if (currentattribute==GROWBY_ATTRIBUTE) {
			growby=atoi(value,DEFAULT_GROWBY,1);
		} else if (currentattribute==TTL_ATTRIBUTE) {
			ttl=atoi(value,DEFAULT_TTL,1);
		} else if (currentattribute==ENDOFSESSION_ATTRIBUTE) {
			delete[] endofsession;
			endofsession=strdup((value)?value:DEFAULT_ENDOFSESSION);
			endofsessioncommit=!strcmp(endofsession,"commit");
		} else if (currentattribute==SESSIONTIMEOUT_ATTRIBUTE) {
			sessiontimeout=atoi(value,DEFAULT_SESSIONTIMEOUT,1);
		} else if (currentattribute==RUNASUSER_ATTRIBUTE) {
			delete[] runasuser;
			runasuser=strdup((value)?value:DEFAULT_RUNASUSER);
		} else if (currentattribute==RUNASGROUP_ATTRIBUTE) {
			delete[] runasgroup;
			runasgroup=strdup((value)?value:DEFAULT_RUNASGROUP);
		} else if (currentattribute==CURSORS_ATTRIBUTE) {
			cursors=atoi(value,DEFAULT_CURSORS,1);
		} else if (currentattribute==AUTHTIER_ATTRIBUTE) {
			delete[] authtier;
			authtier=strdup((value)?value:DEFAULT_AUTHTIER);
			authonlistener=(strstr(authtier,"listener"))?1:0;
			authonconnection=(strstr(authtier,"connection"))?1:0;
			authondatabase=(!strcmp(authtier,"database"))?1:0;
		} else if (currentattribute==HANDOFF_ATTRIBUTE) {
			delete[] handoff;
			handoff=strdup((value)?value:DEFAULT_HANDOFF);
			passdescriptor=(strcmp(handoff,"pass")==0);
		} else if (currentattribute==DENIEDIPS_ATTRIBUTE) {
			delete[] deniedips;
			deniedips=strdup((value)?value:DEFAULT_DENIEDIPS);
		} else if (currentattribute==ALLOWEDIPS_ATTRIBUTE) {
			delete[] allowedips;
			allowedips=strdup((value)?value:DEFAULT_DENIEDIPS);
		} else if (currentattribute==DEBUG_ATTRIBUTE) {
			delete[] debug;
			debug=strdup((value)?value:DEFAULT_DEBUG);
			debuglistener=(strstr(debug,"listener"))?1:0;
			debugconnection=(strstr(debug,"connection"))?1:0;
		} else if (currentattribute==USER_ATTRIBUTE) {
			currentuser->setUser((value)?value:DEFAULT_USER);
		} else if (currentattribute==PASSWORD_ATTRIBUTE) {
			currentuser->setPassword((value)?value:
							DEFAULT_PASSWORD);
		} else if (currentattribute==CONNECTIONID_ATTRIBUTE) {
			if (strlen(value)>MAXCONNECTIONIDLEN) {
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
			currentconnect->setMetric(atoi(value,DEFAULT_METRIC,1));
		}
	}
	return true;
}

int sqlrconfigfile::atoi(const char *value,
				const char *defaultvalue, int minvalue) {
	int	retval=::atoi((value)?value:defaultvalue);
	if (retval<minvalue) {
		retval=::atoi(defaultvalue);
	}
	return retval;
}

bool sqlrconfigfile::tagEnd(char *name) {

	// don't do anything if we're already done
	// or have not found the correct id
	if (done || !correctid) {
		return true;
	}

	// we're done if we've found the right instance at this point
	if (correctid && !strcmp((char *)name,"instance")) {
		done=1;
	}
	return true;
}

int sqlrconfigfile::parse(const char *config, char *id) {
	return parse(config,id,0);
}

int sqlrconfigfile::parse(const char *config, char *id,
					int connectstringcount) {

	// init some variables
	this->connectstringcount=connectstringcount;
	this->id=id;
	correctid=0;
	done=0;

	// parse the file
	int	retval=1;
	if (!parseFile(config)) {
		fprintf(stderr,"Couldn't parse config file %s.\n",config);
		retval=0;
	}

	// parse the user's .sqlrelay.conf file
	char *filename;
	char *homedir=getenv("HOME");
	if (homedir && homedir[0]) {
		filename=new char[strlen(homedir)+15+1];
		sprintf(filename,"%s/.sqlrelay.conf",homedir);
	} else {
		filename=strdup("~/.sqlrelay.conf");
	}

	// see if the file exists before trying to parse it, don't worry about
	// an error message here
	parseFile(filename);
	delete[] filename;

	// if the specified instance wasn't found, warn the user
	if (!done) {
		fprintf(stderr,"Couldn't find id %s.\n",id);
		retval=0;
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
		this->user=strdup(user);
	}
}

void usercontainer::setPassword(const char *password) {
	if (password) {
		this->password=strdup(password);
	}
}

char *usercontainer::getUser() {
	return user;
}

char *usercontainer::getPassword() {
	return password;
}

connectstringcontainer::connectstringcontainer(int connectstringcount) {
	this->connectstringcount=connectstringcount;
	connectionid=NULL;
	string=NULL;
	metric=::atoi(DEFAULT_METRIC);
}

connectstringcontainer::~connectstringcontainer() {
	delete[] string;
	delete[] connectionid;
}

void connectstringcontainer::setConnectionId(const char *connectionid) {
	if (connectionid) {
		this->connectionid=strdup(connectionid);
	}
}

void connectstringcontainer::setString(const char *string) {
	if (string) {
		this->string=strdup(string);
	}
}

void connectstringcontainer::setMetric(int metric) {
	this->metric=metric;
}

char *connectstringcontainer::getConnectionId() {
	return connectionid;
}

char *connectstringcontainer::getString() {
	return string;
}

int connectstringcontainer::getMetric() {
	return metric;
}

void connectstringcontainer::parseConnectString() {
	if (!connectstringcount) {
		return;
	}
	connectstring.parse(string);
}

char *connectstringcontainer::getConnectStringValue(const char *variable) {
	return connectstring.getValue(variable);
}
