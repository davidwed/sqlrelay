// Copyright (c) 2000  David Muse
// See the file COPYING for more information

#include <configfile.h>

#include <defaults.h>

#include <rudiments/permissions.h>
#include <stdio.h>

configfile::configfile() {
	doc=NULL;
	firstinstance=NULL;
	lastinstance=NULL;
	currentfile=NULL;
}

configfile::~configfile() {
	close();
}

void configfile::blank() {

	currentfile=NULL;

	if (doc) {
		delete doc;
	}

	doc=new xmldom();
	doc->createRootNode();
	root=doc->getRootNode();
	root->cascadeOnDelete();

	// create the xml version node
	xmldomnode	*xmlversion=new xmldomnode(doc,root->getNullNode(),
							TAG_XMLDOMNODETYPE,
							"?xml","?xml");
	xmlversion->insertAttribute("version","1.0",
					xmlversion->getAttributeCount());

	// create the doctype node

	// create the instances node
	instances=new xmldomnode(doc,root->getNullNode(),TAG_XMLDOMNODETYPE,
							"instances",
							"instances");

	// add the xml version, doctype and instances
	// tags with \n's after each
	root->insertChild(xmlversion,root->getChildCount());
	root->insertText("\n",root->getChildCount());
	root->insertChild(instances,root->getChildCount());
	root->insertText("\n",root->getChildCount());
}

bool configfile::parse(const char *filename) {

	currentfile=(char *)filename;

	if (doc) {
		delete doc;
	}

	// create a new xml document
	doc=new xmldom();

	// parse the file
	if (doc->parseFile(filename)) {

		// get the root element
		root=doc->getRootNode();

		// get the instances element
		instances=root->getChild("instances");

		// build the instance list
		firstinstance=NULL;
		lastinstance=NULL;

		// get the first instance element
		currentnode=instances->getChild("instance");

		// loop through the children of the instance tag
		// and create more instances
		for (int32_t i=0; i<instances->getChildCount(); i++) {

			// get the next child node, make sure it's
			// an "instance" tag
			currentnode=instances->getChild(i);
			if (currentnode->isNullNode() || 
				currentnode->getType()!=TAG_XMLDOMNODETYPE ||
				charstring::compare(currentnode->getName(),
								"instance")) {
				continue;
			}

			// create a new instance
			if (!firstinstance) {
				firstinstance=new instance(doc,currentnode);
				currentinstance=firstinstance;
			} else {
				currentinstance->next=new instance(doc,
								currentnode);
				currentinstance->next->previous=currentinstance;
				currentinstance=currentinstance->next;
			}
			lastinstance=currentinstance;
		}
		return true;
	}
	delete doc;
	return false;
}

bool configfile::write() {
	return doc->writeFile(currentfile,permissions::ownerReadWrite());
}

bool configfile::write(const char *filename) {
	return doc->writeFile(filename,permissions::ownerReadWrite());
}

void configfile::close() {

	// delete the instance list
	if (firstinstance) {
		currentinstance=firstinstance;
		while (currentinstance) {
			firstinstance=currentinstance;
			currentinstance=currentinstance->next;
			delete firstinstance;
		}
	}
	firstinstance=NULL;
	lastinstance=NULL;

	// free the doc
	delete doc;
	doc=NULL;

	// set currentfile NULL
	currentfile=NULL;
}

char *configfile::currentFile() {
	return currentfile;
}

instance *configfile::addInstance(const char *id,
				const char *port, 
				const char *unixport,
				const char *dbase,
				const char *connections,
				const char *maxconnections,
				const char *maxqueuelength,
				const char *growby,
				const char *ttl,
				const char *endofsession,
				const char *sessiontimeout,
				const char *runasuser,
				const char *runasgroup,
				const char *cursors,
				const char *authtier,
				const char *handoff,
				const char *deniedips,
				const char *allowedips,
				const char *debug,
				const char *maxquerysize,
				const char *maxstringbindvaluelength,
				const char *maxlobbindvaluelength,
				const char *idleclienttimeout,
				const char *maxlisteners,
				const char *listenertimeout) {

	xmldomnode	*newchild=new xmldomnode(doc,root->getNullNode(),
							TAG_XMLDOMNODETYPE,
							"instance",
							"instance");

	newchild->insertAttribute("id",id,
					newchild->getAttributeCount());
	newchild->insertAttribute("port",port,
					newchild->getAttributeCount());
	newchild->insertAttribute("unixport",unixport,
					newchild->getAttributeCount());
	newchild->insertAttribute("dbase",dbase,
					newchild->getAttributeCount());
	newchild->insertAttribute("connections",connections,
					newchild->getAttributeCount());
	newchild->insertAttribute("maxconnections",maxconnections,
					newchild->getAttributeCount());
	newchild->insertAttribute("maxqueuelength",maxqueuelength,
					newchild->getAttributeCount());
	newchild->insertAttribute("growby",growby,
					newchild->getAttributeCount());
	newchild->insertAttribute("ttl",ttl,
					newchild->getAttributeCount());
	newchild->insertAttribute("endofsession",endofsession,
					newchild->getAttributeCount());
	newchild->insertAttribute("sessiontimeout",sessiontimeout,
					newchild->getAttributeCount());
	newchild->insertAttribute("runasuser",runasuser,
					newchild->getAttributeCount());
	newchild->insertAttribute("runasgroup",runasgroup,
					newchild->getAttributeCount());
	newchild->insertAttribute("cursors",cursors,
					newchild->getAttributeCount());
	newchild->insertAttribute("authtier",authtier,
					newchild->getAttributeCount());
	newchild->insertAttribute("handoff",handoff,
					newchild->getAttributeCount());
	newchild->insertAttribute("deniedips",deniedips,
					newchild->getAttributeCount());
	newchild->insertAttribute("allowedips",allowedips,
					newchild->getAttributeCount());
	newchild->insertAttribute("debug",debug,
					newchild->getAttributeCount());
	newchild->insertAttribute("maxquerysize",maxquerysize,
					newchild->getAttributeCount());
	newchild->insertAttribute("maxstringbindvaluelength",
					maxstringbindvaluelength,
					newchild->getAttributeCount());
	newchild->insertAttribute("maxlobbindvaluelength",
					maxlobbindvaluelength,
					newchild->getAttributeCount());
	newchild->insertAttribute("idleclienttimeout",
					idleclienttimeout,
					newchild->getAttributeCount());
	newchild->insertAttribute("maxlisteners",
					maxlisteners,
					newchild->getAttributeCount());
	newchild->insertAttribute("listenertimeout",
					listenertimeout,
					newchild->getAttributeCount());

	xmldomnode	*newusers=new xmldomnode(doc,root->getNullNode(),
							TAG_XMLDOMNODETYPE,
							"users",
							"users");
	xmldomnode	*newconnections=new xmldomnode(doc,root->getNullNode(),
							TAG_XMLDOMNODETYPE,
							"connections",
							"connections");

	instances->insertText("\n	",instances->getChildCount());
	instances->insertChild(newchild,instances->getChildCount());
	instances->insertText("\n",instances->getChildCount());
	newchild->insertText("\n		",newchild->getChildCount());
	newchild->insertChild(newusers,newchild->getChildCount());
	newchild->insertText("\n		",newchild->getChildCount());
	newchild->insertChild(newconnections,newchild->getChildCount());
	newchild->insertText("\n	",newchild->getChildCount());

	if (lastinstance) {
		lastinstance->next=new instance(doc,newchild);
		lastinstance->next->previous=lastinstance;
		lastinstance=lastinstance->next;
	} else {
		lastinstance=new instance(doc,newchild);
		firstinstance=lastinstance;
	}
	return lastinstance;
}

void configfile::deleteInstance(instance *instanceptr) {

	if (instanceptr==firstinstance) {
		firstinstance=instanceptr->next;
	}
	if (instanceptr==lastinstance) {
		lastinstance=instanceptr->previous;
	}
	if (instanceptr->next) {
		instanceptr->next->previous=instanceptr->previous;
	}
	if (instanceptr->previous) {
		instanceptr->previous->next=instanceptr->next;
	}
	delete instanceptr;
}

instance *configfile::findInstance(const char *id) {

	const char	*currentid;

	currentinstance=firstinstance;

	while (currentinstance) {
		currentid=currentinstance->getId();
		if (currentid && !charstring::compare(id,currentid)) {
			delete currentid;
			return currentinstance;
		} else {
			if (currentinstance->next) {
				currentinstance=currentinstance->next;
			} else {
				break;
			}
		}
	}

	return NULL;
}

instance *configfile::firstInstance() {
	return firstinstance;
}

instance::instance(xmldom *doc, xmldomnode *instancenode) {

	this->doc=doc;
	this->instancenode=instancenode;

	next=NULL;
	previous=NULL;

	firstuser=NULL;
	firstconnection=NULL;

	// create the list of users
	users=instancenode->getChild("users");
	for (int32_t i=0; i<users->getChildCount(); i++) {

		// get the next child node, make sure it's
		// an "user" tag
		xmldomnode	*currentnode=users->getChild(i);
		if (currentnode->isNullNode() || 
			currentnode->getType()!=TAG_XMLDOMNODETYPE ||
			charstring::compare(currentnode->getName(),"user")) {
			continue;
		}

		// create a new user
		if (!firstuser) {
			firstuser=new user(currentnode);
			currentuser=firstuser;
		} else {
			currentuser->next=new user(currentnode);
			currentuser->next->previous=currentuser;
			currentuser=currentuser->next;
		}
		lastuser=currentuser;
	}


	// create the list of connections
	connections=instancenode->getChild("connections");
	for (int32_t i=0; i<connections->getChildCount(); i++) {

		// get the next child node, make sure it's
		// an "connection" tag
		xmldomnode	*currentnode=connections->getChild(i);
		if (currentnode->isNullNode() || 
			currentnode->getType()!=TAG_XMLDOMNODETYPE ||
			charstring::compare(currentnode->getName(),
							"connection")) {
			continue;
		}

		// create a new connection
		if (!firstconnection) {
			firstconnection=new connection(currentnode);
			currentconnection=firstconnection;
		} else {
			currentconnection->next=new connection(currentnode);
			currentconnection->next->previous=currentconnection;
			currentconnection=currentconnection->next;
		}
		lastconnection=currentconnection;
	}
}

instance::~instance() {

	// delete the user list
	if (firstuser) {
		currentuser=firstuser;
		while (currentuser) {
			firstuser=currentuser;
			currentuser=currentuser->next;
			delete firstuser;
		}
	}

	// delete the connection list
	if (firstconnection) {
		currentconnection=firstconnection;
		while (currentconnection) {
			firstconnection=currentconnection;
			currentconnection=currentconnection->next;
			delete firstconnection;
		}
	}

	if (next) {
		next->previous=previous;
	}
	if (previous) {
		previous->next=next;
	}
}

const char	*instance::getId() {
	const char	*retval=instancenode->getAttributeValue("id");
	if (retval) {
		return retval;
	}
	return DEFAULT_ID;
}

const char	*instance::getPort() {
	const char	*retval=instancenode->getAttributeValue("port");
	if (retval) {
		return retval;
	}
	return DEFAULT_PORT;
}

const char	*instance::getUnixPort() {
	const char	*retval=instancenode->getAttributeValue("socket");
	if (retval) {
		return retval;
	} else {
		retval=instancenode->getAttributeValue("unixport");
		if (retval) {
			return retval;
		}
	}
	return DEFAULT_SOCKET;
}

const char	*instance::getDbase() {
	const char	*retval=instancenode->getAttributeValue("dbase");
	if (retval) {
		return retval;
	}
	return DEFAULT_DBASE;
}

const char	*instance::getConnections() {
	const char	*retval=instancenode->getAttributeValue("connections");
	if (retval) {
		return retval;
	}
	return DEFAULT_CONNECTIONS;
}

const char	*instance::getMaxConnections() {
	const char	*retval=instancenode->
				getAttributeValue("maxconnections");
	if (retval) {
		return retval;
	}
	return DEFAULT_MAXCONNECTIONS;
}

const char	*instance::getMaxQueueLength() {
	const char	*retval=instancenode->
				getAttributeValue("maxqueuelength");
	if (retval) {
		return retval;
	}
	return DEFAULT_MAXQUEUELENGTH;
}

const char	*instance::getGrowby() {
	const char	*retval=instancenode->getAttributeValue("growby");
	if (retval) {
		return retval;
	}
	return DEFAULT_GROWBY;
}

const char	*instance::getTtl() {
	const char	*retval=instancenode->getAttributeValue("ttl");
	if (retval) {
		return retval;
	}
	return DEFAULT_TTL;
}

const char	*instance::getEndOfSession() {
	const char	*retval=instancenode->getAttributeValue("endofsession");
	if (retval) {
		return retval;
	}
	return DEFAULT_ENDOFSESSION;
}

const char	*instance::getSessionTimeout() {
	const char	*retval=instancenode->
				getAttributeValue("sessiontimeout");
	if (retval) {
		return retval;
	}
	return DEFAULT_SESSIONTIMEOUT;
}

const char	*instance::getRunAsUser() {
	const char	*retval=instancenode->getAttributeValue("runasuser");
	if (retval) {
		return retval;
	}
	return DEFAULT_RUNASUSER;
}

const char	*instance::getRunAsGroup() {
	const char	*retval=instancenode->getAttributeValue("runasgroup");
	if (retval) {
		return retval;
	}
	return DEFAULT_RUNASGROUP;
}

const char	*instance::getCursors() {
	const char	*retval=instancenode->getAttributeValue("cursors");
	if (retval) {
		return retval;
	}
	return DEFAULT_CURSORS;
}

const char	*instance::getAuthTier() {
	const char	*retval=instancenode->getAttributeValue("authtier");
	if (retval) {
		return retval;
	}
	return DEFAULT_AUTHTIER;
}

const char	*instance::getHandoff() {
	const char	*retval=instancenode->getAttributeValue("handoff");
	if (retval) {
		return retval;
	}
	return DEFAULT_HANDOFF;
}

const char	*instance::getDeniedIps() {
	const char	*retval=instancenode->getAttributeValue("deniedips");
	if (retval) {
		return retval;
	}
	return DEFAULT_DENIEDIPS;
}

const char	*instance::getAllowedIps() {
	const char	*retval=instancenode->getAttributeValue("allowedips");
	if (retval) {
		return retval;
	}
	return DEFAULT_ALLOWEDIPS;
}

const char	*instance::getDebug() {
	const char	*retval=instancenode->getAttributeValue("debug");
	if (retval) {
		return retval;
	}
	return DEFAULT_DEBUG;
}

const char	*instance::getMaxQuerySize() {
	const char	*retval=instancenode->getAttributeValue("maxquerysize");
	if (retval) {
		return retval;
	}
	return DEFAULT_MAXQUERYSIZE;
}

const char	*instance::getMaxStringBindValueLength() {
	const char	*retval=instancenode->
				getAttributeValue("maxstringbindvaluelength");
	if (retval) {
		return retval;
	}
	return DEFAULT_MAXSTRINGBINDVALUELENGTH;
}

const char	*instance::getMaxLobBindValueLength() {
	const char	*retval=instancenode->
				getAttributeValue("maxlobbindvaluelength");
	if (retval) {
		return retval;
	}
	return DEFAULT_MAXLOBBINDVALUELENGTH;
}

const char	*instance::getIdleClientTimeout() {
	const char	*retval=instancenode->
				getAttributeValue("idleclienttimeout");
	if (retval) {
		return retval;
	}
	return DEFAULT_IDLECLIENTTIMEOUT;
}

const char	*instance::getMaxListeners() {
	const char	*retval=instancenode->
				getAttributeValue("maxlisteners");
	if (retval) {
		return retval;
	}
	return DEFAULT_MAXLISTENERS;
}

const char	*instance::getListenerTimeout() {
	const char	*retval=instancenode->
				getAttributeValue("listenertimeout");
	if (retval) {
		return retval;
	}
	return DEFAULT_LISTENERTIMEOUT;
}

void	instance::setId(const char *id) {
	instancenode->getAttribute("id")->setValue(id);
}

void	instance::setPort(const char *port) {
	instancenode->getAttribute("port")->setValue(port);
}

void	instance::setUnixPort(const char *unixport) {
	instancenode->getAttribute("socket")->setValue(unixport);
}


void	instance::setDbase(const char *dbase) {
	instancenode->getAttribute("dbase")->setValue(dbase);
}

void	instance::setConnections(const char *connections) {
	instancenode->getAttribute("connections")->setValue(connections);
}

void	instance::setMaxConnections(const char *maxconnections) {
	instancenode->getAttribute("maxconnections")->setValue(maxconnections);
}

void	instance::setMaxQueueLength(const char *maxqueuelength) {
	instancenode->getAttribute("maxqueuelength")->setValue(maxqueuelength);
}

void	instance::setGrowby(const char *growby) {
	instancenode->getAttribute("growby")->setValue(growby);
}

void	instance::setTtl(const char *ttl) {
	instancenode->getAttribute("ttl")->setValue(ttl);
}

void	instance::setEndOfSession(const char *endofsession) {
	instancenode->getAttribute("endofsession")->setValue(endofsession);
}

void	instance::setSessionTimeout(const char *sessiontimeout) {
	instancenode->getAttribute("sessiontimeout")->setValue(sessiontimeout);
}

void	instance::setRunAsUser(const char *runasuser) {
	instancenode->getAttribute("runasuser")->setValue(runasuser);
}

void	instance::setRunAsGroup(const char *runasgroup) {
	instancenode->getAttribute("runasgroup")->setValue(runasgroup);
}

void	instance::setCursors(const char *cursors) {
	instancenode->getAttribute("cursors")->setValue(cursors);
}

void	instance::setAuthTier(const char *authtier) {
	instancenode->getAttribute("authtier")->setValue(authtier);
}

void	instance::setHandoff(const char *handoff) {
	instancenode->getAttribute("handoff")->setValue(handoff);
}

void	instance::setDeniedIps(const char *deniedips) {
	instancenode->getAttribute("deniedips")->setValue(deniedips);
}

void	instance::setAllowedIps(const char *allowedips) {
	instancenode->getAttribute("allowedips")->setValue(allowedips);
}

void	instance::setDebug(const char *debug) {
	instancenode->getAttribute("debug")->setValue(debug);
}

void	instance::setMaxQuerySize(const char *maxquerysize) {
	instancenode->getAttribute("maxquerysize")->setValue(maxquerysize);
}

void	instance::setMaxStringBindValueLength(
				const char *maxstringbindvaluelength) {
	instancenode->getAttribute("maxstringbindvaluelength")->
					setValue(maxstringbindvaluelength);
}

void	instance::setMaxLobBindValueLength(
				const char *maxlobbindvaluelength) {
	instancenode->getAttribute("maxlobbindvaluelength")->
					setValue(maxlobbindvaluelength);
}

void	instance::setIdleClientTimeout(const char *idleclienttimeout) {
	instancenode->getAttribute("idleclienttimeout")->
					setValue(idleclienttimeout);
}

void	instance::setMaxListeners(const char *maxlisteners) {
	instancenode->getAttribute("maxlisteners")->
					setValue(maxlisteners);
}

void	instance::setListenerTimeout(const char *listenertimeout) {
	instancenode->getAttribute("listenertimeout")->
					setValue(listenertimeout);
}

user	*instance::addUser(const char *usr, const char *password) {

	xmldomnode	*newchild=new xmldomnode(doc,
					users->getNullNode(),
					TAG_XMLDOMNODETYPE,
					"user","user");

	newchild->insertAttribute("user",usr,
					newchild->getAttributeCount());
	newchild->insertAttribute("password",password,
					newchild->getAttributeCount());

	if (!users->getChildCount()) {
		users->insertText("\n			",
					users->getChildCount());
	} else {
		users->getChild(users->getChildCount()-1)->
				setValue("\n			");
	}
	users->insertChild(newchild,users->getChildCount());
	users->insertText("\n		",users->getChildCount());

	if (lastuser) {
		lastuser->next=new user(newchild);
		lastuser->next->previous=lastuser;
		lastuser=lastuser->next;
	} else {
		lastuser=new user(newchild);
		firstuser=lastuser;
	}
	return lastuser;
}

void	instance::deleteUser(user *userptr) {

	if (userptr==firstuser) {
		firstuser=userptr->next;
	}
	if (userptr==lastuser) {
		lastuser=userptr->previous;
	}
	if (userptr->next) {
		userptr->next->previous=userptr->previous;
	}
	if (userptr->previous) {
		userptr->previous->next=userptr->next;
	}
	delete userptr;
}

user	*instance::findUser(const char *userid) {

	const char	*currentuserid;

	currentuser=firstuser;

	while (currentuser) {
		currentuserid=currentuser->getUser();
		if (currentuserid &&
			!charstring::compare(userid,currentuserid)) {
			delete currentuserid;
			return currentuser;
		} else {
			if (currentuser->next) {
				currentuser=currentuser->next;
			} else {
				break;
			}
		}
	}

	return NULL;
}

user	*instance::firstUser() {
	return firstuser;
}

user::user(xmldomnode *usernode) {
	this->usernode=usernode;
	next=NULL;
	previous=NULL;
}

user::~user() {
	if (next) {
		next->previous=previous;
	}
	if (previous) {
		previous->next=next;
	}
}

const char	*user::getUser() {
	const char	*retval=usernode->getAttributeValue("user");
	if (retval) {
		return retval;
	}
	return DEFAULT_USER;
}

const char	*user::getPassword() {
	const char	*retval=usernode->getAttributeValue("password");
	if (retval) {
		return retval;
	}
	return DEFAULT_PASSWORD;
}

void	user::setUser(const char *user) {
	usernode->getAttribute("user")->setValue(user);
}

void	user::setPassword(const char *password) {
	usernode->getAttribute("password")->setValue(password);
}

user	*user::nextUser() {
	return next;
}

connection	*instance::addConnection(const char *connectionid,
				const char *string, 
				const char *metric,
				const char *behindloadbalancer) {

	xmldomnode	*newchild=new xmldomnode(doc,
					connections->getNullNode(),
					TAG_XMLDOMNODETYPE,
					"connection","connection");

	newchild->insertAttribute("connectionid",connectionid,
				newchild->getAttributeCount());
	newchild->insertAttribute("string",string,
				newchild->getAttributeCount());
	newchild->insertAttribute("metric",metric,
				newchild->getAttributeCount());
	newchild->insertAttribute("behindloadbalancer",behindloadbalancer,
				newchild->getAttributeCount());

	if (!connections->getChildCount()) {
		connections->insertText("\n			",
					connections->getChildCount());
	} else {
		connections->getChild(connections->getChildCount()-1)->
				setValue("\n			");
	}
	connections->insertChild(newchild,connections->getChildCount());
	connections->insertText("\n		",connections->getChildCount());

	if (lastconnection) {
		lastconnection->next=new connection(newchild);
		lastconnection->next->previous=lastconnection;
		lastconnection=lastconnection->next;
	} else {
		lastconnection=new connection(newchild);
		firstconnection=lastconnection;
	}
	return lastconnection;
}

void	instance::deleteConnection(connection *connectionptr) {

	if (connectionptr==firstconnection) {
		firstconnection=connectionptr->next;
	}
	if (connectionptr==lastconnection) {
		lastconnection=connectionptr->previous;
	}
	if (connectionptr->next) {
		connectionptr->next->previous=connectionptr->previous;
	}
	if (connectionptr->previous) {
		connectionptr->previous->next=connectionptr->next;
	}
	delete connectionptr;
}

connection	*instance::findConnection(const char *connectionnode) {

	const char	*currentconnid;

	currentconnection=firstconnection;

	while (currentconnection) {
		currentconnid=currentconnection->getConnectionId();
		if (currentconnid &&
			!charstring::compare(connectionnode,currentconnid)) {
			delete currentconnid;
			return currentconnection;
		} else {
			if (currentconnection->next) {
				currentconnection=currentconnection->next;
			} else {
				break;
			}
		}
	}

	return NULL;
}


connection	*instance::firstConnection() {
	return firstconnection;
}

instance	*instance::nextInstance() {
	return next;
}

connection::connection(xmldomnode *connectionnode) {
	this->connectionnode=connectionnode;
	next=NULL;
	previous=NULL;
}

connection::~connection() {
	if (next) {
		next->previous=previous;
	}
	if (previous) {
		previous->next=next;
	}
}

const char	*connection::getConnectionId() {
	const char	*retval=connectionnode->
					getAttributeValue("connectionid");
	if (retval) {
		return retval;
	}
	return DEFAULT_CONNECTIONID;
}

const char	*connection::getString() {
	const char	*retval=connectionnode->getAttributeValue("string");
	if (retval) {
		return retval;
	}
	return DEFAULT_CONNECTSTRING;
}

const char	*connection::getMetric() {
	const char	*retval=connectionnode->getAttributeValue("metric");
	if (retval) {
		return retval;
	}
	return DEFAULT_METRIC;
}

const char	*connection::getBehindLoadBalancer() {
	const char	*retval=connectionnode->
				getAttributeValue("behindloadbalancer");
	if (retval) {
		return retval;
	}
	return DEFAULT_BEHINDLOADBALANCER;
}

void	connection::setConnectionId(const char *connectionid) {
	connectionnode->getAttribute("connectionid")->setValue(connectionid);
}

void	connection::setString(const char *string) {
	connectionnode->getAttribute("string")->setValue(string);
}

void	connection::setMetric(const char *metric) {
	connectionnode->getAttribute("metric")->setValue(metric);
}

void	connection::setBehindLoadBalancer(const char *behindloadbalancer) {
	connectionnode->getAttribute("behindloadbalancer")->
					setValue(behindloadbalancer);
}

connection	*connection::nextConnection() {
	return next;
}
