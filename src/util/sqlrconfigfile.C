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

#include <defaults.h>

sqlrconfigfile::sqlrconfigfile() : xmlsax() {
	port=atoi(DEFAULT_PORT);
	listenoninet=(port)?1:0;
	unixport=DEFAULT_SOCKET;
	listenonunix=(unixport)?1:0;
	dbase=DEFAULT_DBASE;
	connections=atoi(DEFAULT_CONNECTIONS);
	maxconnections=atoi(DEFAULT_MAXCONNECTIONS);
	maxqueuelength=atoi(DEFAULT_MAXQUEUELENGTH);
	growby=atoi(DEFAULT_GROWBY);
	ttl=atoi(DEFAULT_TTL);
	endofsession=DEFAULT_ENDOFSESSION;
	endofsessioncommit=!strcmp(endofsession,"commit");
	sessiontimeout=atoi(DEFAULT_SESSIONTIMEOUT);
	runasuser=DEFAULT_RUNASUSER;
	runasgroup=DEFAULT_RUNASGROUP;
	cursors=atoi(DEFAULT_CURSORS);
	authtier=DEFAULT_AUTHTIER;
	authonlistener=(strstr(authtier,"listener"))?1:0;
	authonconnection=(strstr(authtier,"connection"))?1:0;
	authondatabase=(!strcmp(authtier,"database"))?1:0;
	handoff=DEFAULT_HANDOFF;
	passdescriptor=(strcmp(handoff,"pass")==0);
	allowedips=DEFAULT_DENIEDIPS;
	deniedips=DEFAULT_DENIEDIPS;
	debug=DEFAULT_DEBUG;
	debuglistener=(strstr(debug,"listener"))?1:0;
	debugconnection=(strstr(debug,"connection"))?1:0;
	firstuser=NULL;
	currentuser=NULL;
	usercount=0;
	firstconnect=NULL;
	currentconnect=NULL;
	connectstringcount=0;
	connectioncount=0;
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

	currentuser=firstuser;
	while (currentuser) {
		firstuser=currentuser->next;
		delete currentuser;
		currentuser=firstuser;
	}

	currentconnect=firstconnect;
	while (currentconnect) {
		firstconnect=currentconnect->next;
		delete currentconnect;
		currentconnect=firstconnect;
	}
}

int	sqlrconfigfile::getPort() {
	return port;
}

char	*sqlrconfigfile::getUnixPort() {
	return unixport;
}

int	sqlrconfigfile::getListenOnInet() {
	return listenoninet;
}

int	sqlrconfigfile::getListenOnUnix() {
	return listenonunix;
}

char	*sqlrconfigfile::getDbase() {
	return dbase;
}

int	sqlrconfigfile::getConnections() {
	return connections;
}

int	sqlrconfigfile::getMaxConnections() {
	return maxconnections;
}

int	sqlrconfigfile::getMaxQueueLength() {
	return maxqueuelength;
}

int	sqlrconfigfile::getGrowBy() {
	return growby;
}

int	sqlrconfigfile::getTtl() {
	return ttl;
}

int	sqlrconfigfile::getDynamicScaling() {
	return (maxqueuelength>=0 && maxconnections>connections &&
			growby>0 && ttl>0);
}

char	*sqlrconfigfile::getEndOfSession() {
	return endofsession;
}

int	sqlrconfigfile::getEndOfSessionCommit() {
	return endofsessioncommit;
}

int	sqlrconfigfile::getSessionTimeout() {
	return sessiontimeout;
}

char	*sqlrconfigfile::getRunAsUser() {
	return runasuser;
}

char	*sqlrconfigfile::getRunAsGroup() {
	return runasgroup;
}

int	sqlrconfigfile::getCursors() {
	return cursors;
}

char	*sqlrconfigfile::getAuthTier() {
	return authtier;
}

int	sqlrconfigfile::getAuthOnListener() {
	return authonlistener;
}

int	sqlrconfigfile::getAuthOnConnection() {
	return authonconnection;
}

int	sqlrconfigfile::getAuthOnDatabase() {
	return authondatabase;
}

char	*sqlrconfigfile::getHandOff() {
	return handoff;
}

int	sqlrconfigfile::getPassDescriptor() {
	// descriptor passing doesn't work if we're using dynamic scaling,
	// so override it here...
	return passdescriptor && !getDynamicScaling();
}

char	*sqlrconfigfile::getAllowedIps() {
	return allowedips;
}

char	*sqlrconfigfile::getDeniedIps() {
	return deniedips;
}

char	*sqlrconfigfile::getDebug() {
	return debug;
}

int	sqlrconfigfile::getDebugListener() {
	return debuglistener;
}

int	sqlrconfigfile::getDebugConnection() {
	return debugconnection;
}

usernode	*sqlrconfigfile::getUsers() {
	return firstuser;
}

int	sqlrconfigfile::getUserCount() {
	return usercount;
}

connectstringnode	*sqlrconfigfile::getConnectStrings() {
	return firstconnect;
}

connectstringnode	*sqlrconfigfile::getConnectString(
						const char *connectionid) {
	currentconnect=firstconnect;
	while (currentconnect) {
		if (!strcmp(connectionid,currentconnect->getConnectionId())) {
			return currentconnect;
		}
		currentconnect=currentconnect->next;
	}
	return NULL;
}

int	sqlrconfigfile::getConnectionCount() {
	return connectioncount;
}

int	sqlrconfigfile::getMetricTotal() {
	return metrictotal;
}

int	sqlrconfigfile::tagStart(char *name) {

	// don't do anything if we're already done
	// or have not found the correct id
	if (done || !correctid) {
		return 1;
	}

	// set the current tag
	if (!strcmp(name,"user")) {
		if (!firstuser) {
			firstuser=new usernode();
			currentuser=firstuser;
			usercount=1;
		} else {
			currentuser->next=new usernode();
			currentuser=currentuser->next;
			usercount++;
		}
	} else if (!strcmp(name,"connection")) {
		if (!firstconnect) {
			firstconnect=
				new connectstringnode(connectstringcount);
			currentconnect=firstconnect;
			connectioncount=1;
			metrictotal=0;
		} else {
			currentconnect->next=
				new connectstringnode(connectstringcount);
			currentconnect=currentconnect->next;
			connectioncount++;
		}
	}

	return 1;
}

int	sqlrconfigfile::attributeName(char *name) {

	// don't do anything if we're already done
	if (done) {
		return 1;
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
	return 1;
}

int	sqlrconfigfile::attributeValue(char *value) {

	// don't do anything if we're already done
	if (done) {
		return 1;
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
			if (value) {
				port=atoi(value);
			} else {
				port=atoi(DEFAULT_PORT);
			}
			listenoninet=1;
		} else if (currentattribute==SOCKET_ATTRIBUTE) {
			if (value) {
				unixport=strdup(value);
			} else {
				unixport=DEFAULT_SOCKET;
			}
			listenonunix=(unixport[0]!=(char)NULL);
		} else if (currentattribute==DBASE_ATTRIBUTE) {
			if (value) {
				dbase=strdup(value);
			} else {
				dbase=DEFAULT_DBASE;
			}
		} else if (currentattribute==CONNECTIONS_ATTRIBUTE) {
			if (value) {
				connections=atoi(value);
			} else {
				connections=atoi(DEFAULT_CONNECTIONS);
			}
		} else if (currentattribute==MAXCONNECTIONS_ATTRIBUTE) {
			if (value) {
				maxconnections=atoi(value);
			} else {
				maxconnections=atoi(DEFAULT_MAXCONNECTIONS);
			}
		} else if (currentattribute==MAXQUEUELENGTH_ATTRIBUTE) {
			if (value) {
				maxqueuelength=atoi(value);
			} else {
				maxqueuelength=atoi(DEFAULT_MAXQUEUELENGTH);
			}
		} else if (currentattribute==GROWBY_ATTRIBUTE) {
			if (value) {
				growby=atoi(value);
			} else {
				growby=atoi(DEFAULT_GROWBY);
			}
		} else if (currentattribute==TTL_ATTRIBUTE) {
			if (value) {
				ttl=atoi(value);
			} else {
				ttl=atoi(DEFAULT_TTL);
			}
		} else if (currentattribute==ENDOFSESSION_ATTRIBUTE) {
			if (value) {
				endofsession=strdup(value);
			} else {
				endofsession=DEFAULT_ENDOFSESSION;
			}
			endofsessioncommit=!strcmp(endofsession,"commit");
		} else if (currentattribute==SESSIONTIMEOUT_ATTRIBUTE) {
			if (value) {
				sessiontimeout=atoi(value);
			} else {
				sessiontimeout=atoi(DEFAULT_SESSIONTIMEOUT);
			}
		} else if (currentattribute==RUNASUSER_ATTRIBUTE) {
			if (value) {
				runasuser=strdup(value);
			} else {
				runasuser=DEFAULT_RUNASUSER;
			}
		} else if (currentattribute==RUNASGROUP_ATTRIBUTE) {
			if (value) {
				runasgroup=strdup(value);
			} else {
				runasgroup=DEFAULT_RUNASGROUP;
			}
		} else if (currentattribute==CURSORS_ATTRIBUTE) {
			if (value) {
				cursors=atoi(value);
			} else {
				cursors=atoi(DEFAULT_CURSORS);
			}
		} else if (currentattribute==AUTHTIER_ATTRIBUTE) {
			if (value) {
				authtier=strdup(value);
			} else {
				authtier=DEFAULT_AUTHTIER;
			}
			authonlistener=(strstr(authtier,"listener"))?1:0;
			authonconnection=(strstr(authtier,"connection"))?1:0;
			authondatabase=(!strcmp(authtier,"database"))?1:0;
		} else if (currentattribute==HANDOFF_ATTRIBUTE) {
			if (value) {
				handoff=strdup(value);
			} else {
				handoff=DEFAULT_HANDOFF;
			}
			passdescriptor=(strcmp(handoff,"pass")==0);
		} else if (currentattribute==DENIEDIPS_ATTRIBUTE) {
			if (value) {
				deniedips=strdup(value);
			} else {
				deniedips=DEFAULT_DENIEDIPS;
			}
		} else if (currentattribute==ALLOWEDIPS_ATTRIBUTE) {
			if (value) {
				allowedips=strdup(value);
			} else {
				allowedips=DEFAULT_DENIEDIPS;
			}
		} else if (currentattribute==DEBUG_ATTRIBUTE) {
			if (value) {
				debug=strdup(value);
			} else {
				debug=DEFAULT_DEBUG;
			}
			debuglistener=(strstr(debug,"listener"))?1:0;
			debugconnection=(strstr(debug,"connection"))?1:0;
		} else if (currentattribute==USER_ATTRIBUTE) {
			if (value) {
				currentuser->setUser(value);
			} else {
				currentuser->setUser(DEFAULT_USER);
			}
		} else if (currentattribute==PASSWORD_ATTRIBUTE) {
			if (value) {
				currentuser->setPassword(value);
			} else {
				currentuser->setPassword(DEFAULT_PASSWORD);
			}
		} else if (currentattribute==CONNECTIONID_ATTRIBUTE) {
			if (value) {
				currentconnect->setConnectionId(value);
			} else {
				currentconnect->setConnectionId(
							DEFAULT_CONNECTIONID);
			}
		} else if (currentattribute==STRING_ATTRIBUTE) {
			if (value) {
				currentconnect->setString(value);
			} else {
				currentconnect->setString(
							DEFAULT_CONNECTSTRING);
			}
			currentconnect->parseConnectString();
		} else if (currentattribute==METRIC_ATTRIBUTE) {
			int	metric;
			if (value) {
				metric=atoi(value);
			} else {
				metric=atoi(DEFAULT_METRIC);
			}
			currentconnect->setMetric(metric);
			metrictotal=metrictotal+metric;
		}
	}
	return 1;
}

int	sqlrconfigfile::tagEnd(char *name) {

	// don't do anything if we're already done
	// or have not found the correct id
	if (done || !correctid) {
		return 1;
	}

	// we're done if we've found the right instance at this point
	if (correctid && !strcmp((char *)name,"instance")) {
		done=1;
	}
	return 1;
}

int	sqlrconfigfile::parse(const char *config, char *id) {
	return parse(config,id,0);
}

int	sqlrconfigfile::parse(const char *config, char *id,
					int connectstringcount) {

	// init some variables
	this->connectstringcount=connectstringcount;
	this->id=id;
	correctid=0;
	done=0;

	// parse the file
	int	retval=1;
	if (!parseFile(config)<0) {
		fprintf(stderr,"Couldn't parse config file %s.\n",config);
		retval=0;
	}

	// parse the user's .sqlrelay.conf file
	char	*filename;
	char	*homedir=getenv("HOME");
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

usernode::usernode() {
	user=NULL;
	password=NULL;
	next=NULL;
}

usernode::~usernode() {
	if (user) {
		delete[] user;
	}
	if (password) {
		delete[] password;
	}
}

void	usernode::setUser(const char *user) {
	if (user) {
		this->user=strdup(user);
	}
}

void	usernode::setPassword(const char *password) {
	if (password) {
		this->password=strdup(password);
	}
}

char	*usernode::getUser() {
	return user;
}

char	*usernode::getPassword() {
	return password;
}

usernode	*usernode::getNext() {
	return next;
}

connectstringnode::connectstringnode(int connectstringcount) {
	this->connectstringcount=connectstringcount;
	connectionid=NULL;
	string=NULL;
	metric=atoi(DEFAULT_METRIC);
	next=NULL;
	connectstringvar=NULL;
	connectstringval=NULL;
}

connectstringnode::~connectstringnode() {
	delete[] string;
	delete[] connectionid;

	// clean up connect strings
	for (int i=0; i<connectstringcount; i++) {
		delete connectstringvar[i];
		delete connectstringval[i];
	}
	delete[] connectstringvar;
	delete[] connectstringval;
}

void	connectstringnode::setConnectionId(const char *connectionid) {
	if (connectionid) {
		this->connectionid=strdup(connectionid);
	}
}

void	connectstringnode::setString(const char *string) {
	if (string) {
		this->string=strdup(string);
	}
}

void	connectstringnode::setMetric(int metric) {
	this->metric=metric;
}

char	*connectstringnode::getConnectionId() {
	return connectionid;
}

char	*connectstringnode::getString() {
	return string;
}

int	connectstringnode::getMetric() {
	return metric;
}

connectstringnode	*connectstringnode::getNext() {
	return next;
}

void	connectstringnode::parseConnectString() {

	if (!connectstringcount) {
		return;
	}

	// initialize containers for the connect string vars and vals
	connectstringvar=new stringbuffer *[connectstringcount];
	connectstringval=new stringbuffer *[connectstringcount];
	for (int i=0; i<connectstringcount; i++) {
		connectstringvar[i]=NULL;
		connectstringval[i]=NULL;
	}

	// parse connect string of form:
	//	 name1=value1;name2=value2;name3=value3;
	char	*ptr1=string;
	char	*ptr2=string;
	int	current=0;
	while (*ptr2) {
		while (*ptr2 && *ptr2!='=') {
			ptr2++;
		}
		if (!*ptr2) {
			break;
		}
		connectstringvar[current]=new stringbuffer();
		while (ptr1!=ptr2) {
			connectstringvar[current]->append((char)*ptr1);
			ptr1++;
		}
		ptr1++;
		ptr2++;
		while (*ptr2 && *ptr2!=';') {
			ptr2++;
		}
		connectstringval[current]=new stringbuffer();
		while (*ptr1 && ptr1!=ptr2) {
			connectstringval[current]->append((char)*ptr1);
			ptr1++;
		}

		if (!*ptr2) {
			break;
		}

		current++;
		ptr1++;
		ptr2++;
	}
}

char	*connectstringnode::getConnectStringValue(const char *variable) {

	// search for the given variable, return the given value,
	// search is case insensitive
	for (int i=0; i<connectstringcount; i++) {
		if (!connectstringvar[i]) {
			break;
		}
		if (!strcasecmp(variable,connectstringvar[i]->getString())) {
			return connectstringval[i]->getString();
		}
	}
	return NULL;
}
