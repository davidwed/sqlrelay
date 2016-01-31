// Copyright (c) 2000-2015  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrutil.h>
#include <rudiments/xmldom.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/environment.h>
#include <rudiments/directory.h>
#include <rudiments/sys.h>
#include <rudiments/filedescriptor.h>
#include <rudiments/file.h>
#include <rudiments/url.h>
#include <rudiments/filesystem.h>
#include <rudiments/character.h>
#include <rudiments/stdio.h>

#include <defines.h>
#include <defaults.h>
//#define DEBUG_MESSAGES
#include <debugprint.h>

class SQLRUTIL_DLLSPEC sqlrconfig_xmldom : public sqlrconfig, public xmldom {
	public:
			sqlrconfig_xmldom();
			~sqlrconfig_xmldom();

		void	getEnabledIds(const char *urlname,
					linkedlist< char * > *idlist);
		bool	load(const char *urlname, const char *id);
		bool	accessible();

		const char * const	*getDefaultAddresses();
		uint64_t		getDefaultAddressCount();
		uint16_t		getDefaultPort();
		const char		*getDefaultSocket();
		bool			getDefaultKrb();
		const char		*getDefaultKrbService();

		bool		getListenOnInet();
		bool		getListenOnUnix();
		const char	*getDbase();
		uint32_t	getConnections();
		uint32_t	getMaxConnections();
		uint32_t	getMaxQueueLength();
		uint32_t	getGrowBy();
		int32_t		getTtl();
		int32_t		getSoftTtl();
		uint16_t	getMaxSessionCount();
		bool		getDynamicScaling();
		const char	*getEndOfSession();
		bool		getEndOfSessionCommit();
		uint32_t	getSessionTimeout();
		const char	*getRunAsUser();
		const char	*getRunAsGroup();
		uint16_t	getCursors();
		uint16_t	getMaxCursors();
		uint16_t	getCursorsGrowBy();
		const char	*getAuthTier();
		bool		getAuthOnConnection();
		bool		getAuthOnDatabase();
		const char	*getSessionHandler();
		const char	*getHandoff();
		const char	*getAllowedIps();
		const char	*getDeniedIps();
		const char	*getDebug();
		bool		getDebugParser();
		bool		getDebugTranslations();
		bool		getDebugFilters();
		bool		getDebugTriggers();
		bool		getDebugBindTranslations();
		bool		getDebugResultSetTranslations();
		uint64_t	getMaxClientInfoLength();
		uint32_t	getMaxQuerySize();
		uint16_t	getMaxBindCount();
		uint16_t	getMaxBindNameLength();
		uint32_t	getMaxStringBindValueLength();
		uint32_t	getMaxLobBindValueLength();
		uint32_t	getMaxErrorLength();
		int32_t		getIdleClientTimeout();
		int64_t		getMaxListeners();
		uint32_t	getListenerTimeout();
		bool		getReLoginAtStart();
		bool		getFakeInputBindVariables();
		bool		getTranslateBindVariables();
		const char	*getIsolationLevel();
		bool		getIgnoreSelectDatabase();
		bool		getWaitForDownDatabase();

		const char	*getDateTimeFormat();
		const char	*getDateFormat();
		const char	*getTimeFormat();
		bool		getDateDdMm();
		bool		getDateYyyyDdMm();
		const char	*getDateDelimiters();
		bool		getIgnoreNonDateTime();

		bool		getKrb();
		const char	*getKrbService();
		const char	*getKrbKeytab();

		linkedlist< char *>	*getSessionStartQueries();
		linkedlist< char *>	*getSessionEndQueries();

		const char	*getParser();
		const char	*getTranslations();
		const char	*getFilters();
		const char	*getResultSetTranslations();
		const char	*getTriggers();
		const char	*getLoggers();
		const char	*getQueries();
		const char	*getPasswordEncryptions();
		const char	*getAuthentications();

		linkedlist< listenercontainer * >	*getListenerList();

		linkedlist< usercontainer * >		*getUserList();

		linkedlist< connectstringcontainer * >	*getConnectStringList();
		connectstringcontainer	*getConnectString(
						const char *connectionid);
		uint32_t		getConnectionCount();
		uint32_t		getMetricTotal();

		linkedlist< routecontainer * >	*getRouteList();
	private:
		bool			getenabledids;
		char			*currentid;
		bool			enabled;
		linkedlist< char * >	*idlist;

		const char	*id;
		bool		foundspecifiedinstance;
		bool		done;

		void	init();
		void	clear();

		void	parseUrl(const char *urlname);
		void	cleanTree();
		void	parseDir(const char *dir);
		void	parseLinkFile(const char *urlname);

		bool	tagStart(const char *ns, const char *name);
		bool	tagEnd(const char *ns, const char *name);
		bool	attributeName(const char *name);
		bool	attributeValue(const char *value);
		bool	text(const char *value);

		char		**addresses;
		uint64_t	addresscount;

		bool		listenoninet;
		bool		listenonunix;

		bool		debugtranslations;
		bool		debugfilters;
		bool		debugtriggers;
		bool		debugbindtranslations;
		bool		debugresultsettranslations;

		bool		ininstancetag;
		bool		inidattribute;
		bool		inenabledattribute;
		bool		getattributes;
};

sqlrconfig_xmldom::sqlrconfig_xmldom() : sqlrconfig(), xmldom() {
	debugFunction();

	init();
}

sqlrconfig_xmldom::~sqlrconfig_xmldom() {
	debugFunction();

	clear();
}

void sqlrconfig_xmldom::init() {
	debugFunction();

	getenabledids=false;
	currentid=NULL;
	enabled=false;
	idlist=NULL;
	id=NULL;
	foundspecifiedinstance=false;
	done=false;

	addresses=NULL;
	addresscount=0;

	listenoninet=false;
	listenonunix=false;

	debugtranslations=false;
	debugfilters=false;
	debugtriggers=false;
	debugbindtranslations=false;
	debugresultsettranslations=false;

	ininstancetag=false;
	inidattribute=false;
	inenabledattribute=false;
	getattributes=true;
}

void sqlrconfig_xmldom::clear() {
	debugFunction();

	delete[] currentid;
	addresscount=0;
}

const char * const *sqlrconfig_xmldom::getDefaultAddresses() {
	//return defaultlistener->getAddresses();
	return NULL;
}

uint64_t sqlrconfig_xmldom::getDefaultAddressCount() {
	//return defaultlistener->getAddressCount();
	return 0;
}

uint16_t sqlrconfig_xmldom::getDefaultPort() {
	//return defaultlistener->getPort();
	return 0;
}

const char *sqlrconfig_xmldom::getDefaultSocket() {
	//return defaultlistener->getSocket();
	return NULL;
}

bool sqlrconfig_xmldom::getDefaultKrb() {
	//return defaultlistener->getKrb();
	return false;
}

const char *sqlrconfig_xmldom::getDefaultKrbService() {
	//return defaultlistener->getKrbService();
	return NULL;
}

bool sqlrconfig_xmldom::getListenOnInet() {
	//return listenoninet;
	return false;
}

bool sqlrconfig_xmldom::getListenOnUnix() {
	//return listenonunix;
	return false;
}

const char *sqlrconfig_xmldom::getDbase() {
	//return dbase;
	return NULL;
}

uint32_t sqlrconfig_xmldom::getConnections() {
	//return connections;
	return 0;
}

uint32_t sqlrconfig_xmldom::getMaxConnections() {
	//return maxconnections;
	return 0;
}

uint32_t sqlrconfig_xmldom::getMaxQueueLength() {
	//return maxqueuelength;
	return 0;
}

uint32_t sqlrconfig_xmldom::getGrowBy() {
	//return growby;
	return 0;
}

int32_t sqlrconfig_xmldom::getTtl() {
	//return ttl;
	return 0;
}

int32_t sqlrconfig_xmldom::getSoftTtl() {
	//return softttl;
	return 0;
}

uint16_t sqlrconfig_xmldom::getMaxSessionCount() {
	//return maxsessioncount;
	return 0;
}

bool sqlrconfig_xmldom::getDynamicScaling() {
	//return (maxconnections>connections && growby>0 && ttl>-1 &&
		//(maxlisteners==-1 || maxqueuelength<=maxlisteners));
	return false;
}

const char *sqlrconfig_xmldom::getEndOfSession() {
	//return endofsession;
	return NULL;
}

bool sqlrconfig_xmldom::getEndOfSessionCommit() {
	//return endofsessioncommit;
	return false;
}

uint32_t sqlrconfig_xmldom::getSessionTimeout() {
	//return sessiontimeout;
	return 0;
}

const char *sqlrconfig_xmldom::getRunAsUser() {
	//return runasuser;
	return NULL;
}

const char *sqlrconfig_xmldom::getRunAsGroup() {
	//return runasgroup;
	return NULL;
}

uint16_t sqlrconfig_xmldom::getCursors() {
	//return cursors;
	return 0;
}

uint16_t sqlrconfig_xmldom::getMaxCursors() {
	//return maxcursors;
	return 0;
}

uint16_t sqlrconfig_xmldom::getCursorsGrowBy() {
	//return cursorsgrowby;
	return 0;
}

const char *sqlrconfig_xmldom::getAuthTier() {
	//return authtier;
	return NULL;
}

const char *sqlrconfig_xmldom::getSessionHandler() {
	//return sessionhandler;
	return NULL;
}

const char *sqlrconfig_xmldom::getHandoff() {
	//return handoff;
	return NULL;
}

bool sqlrconfig_xmldom::getAuthOnConnection() {
	//return authonconnection;
	return false;
}

bool sqlrconfig_xmldom::getAuthOnDatabase() {
	//return authondatabase;
	return false;
}

const char *sqlrconfig_xmldom::getAllowedIps() {
	//return allowedips;
	return NULL;
}

const char *sqlrconfig_xmldom::getDeniedIps() {
	//return deniedips;
	return NULL;
}

const char *sqlrconfig_xmldom::getDebug() {
	//return debug;
	return NULL;
}

bool sqlrconfig_xmldom::getDebugParser() {
	//return debugparser;
	return false;
}

bool sqlrconfig_xmldom::getDebugTranslations() {
	//return debugtranslations;
	return false;
}

bool sqlrconfig_xmldom::getDebugFilters() {
	//return debugfilters;
	return false;
}

bool sqlrconfig_xmldom::getDebugTriggers() {
	//return debugtriggers;
	return false;
}

bool sqlrconfig_xmldom::getDebugBindTranslations() {
	//return debugbindtranslations;
	return false;
}

bool sqlrconfig_xmldom::getDebugResultSetTranslations() {
	//return debugresultsettranslations;
	return false;
}

uint64_t sqlrconfig_xmldom::getMaxClientInfoLength() {
	//return maxclientinfolength;
	return 0;
}

uint32_t sqlrconfig_xmldom::getMaxQuerySize() {
	//return maxquerysize;
	return 0;
}

uint16_t sqlrconfig_xmldom::getMaxBindCount() {
	//return maxbindcount;
	return 0;
}

uint16_t sqlrconfig_xmldom::getMaxBindNameLength() {
	//return maxbindnamelength;
	return 0;
}

uint32_t sqlrconfig_xmldom::getMaxStringBindValueLength() {
	//return maxstringbindvaluelength;
	return 0;
}

uint32_t sqlrconfig_xmldom::getMaxLobBindValueLength() {
	//return maxlobbindvaluelength;
	return 0;
}

uint32_t sqlrconfig_xmldom::getMaxErrorLength() {
	//return maxerrorlength;
	return 0;
}

int32_t sqlrconfig_xmldom::getIdleClientTimeout() {
	//return idleclienttimeout;
	return 0;
}

int64_t sqlrconfig_xmldom::getMaxListeners() {
	//return maxlisteners;
	return 0;
}

uint32_t sqlrconfig_xmldom::getListenerTimeout() {
	//return listenertimeout;
	return 0;
}

bool sqlrconfig_xmldom::getReLoginAtStart() {
	//return reloginatstart;
	return false;
}

bool sqlrconfig_xmldom::getFakeInputBindVariables() {
	//return fakeinputbindvariables;
	return false;
}

bool sqlrconfig_xmldom::getTranslateBindVariables() {
	//return translatebindvariables;
	return false;
}

const char *sqlrconfig_xmldom::getIsolationLevel() {
	//return isolationlevel;
	return NULL;
}

bool sqlrconfig_xmldom::getIgnoreSelectDatabase() {
	//return ignoreselectdb;
	return false;
}

bool sqlrconfig_xmldom::getWaitForDownDatabase() {
	//return waitfordowndb;
	return false;
}

const char *sqlrconfig_xmldom::getDateTimeFormat() {
	//return datetimeformat;
	return NULL;
}

const char *sqlrconfig_xmldom::getDateFormat() {
	//return dateformat;
	return NULL;
}

const char *sqlrconfig_xmldom::getTimeFormat() {
	//return timeformat;
	return NULL;
}

bool sqlrconfig_xmldom::getDateDdMm() {
	//return dateddmm;
	return false;
}

bool sqlrconfig_xmldom::getDateYyyyDdMm() {
	//return dateyyyyddmm;
	return false;
}

const char *sqlrconfig_xmldom::getDateDelimiters() {
	//return datedelimiters;
	return NULL;
}

bool sqlrconfig_xmldom::getIgnoreNonDateTime() {
	//return ignorenondatetime;
	return false;
}

bool sqlrconfig_xmldom::getKrb() {
	//return krb;
	return false;
}

const char *sqlrconfig_xmldom::getKrbService() {
	//return krbservice;
	return NULL;
}

const char *sqlrconfig_xmldom::getKrbKeytab() {
	//return krbkeytab;
	return NULL;
}

linkedlist< char * > *sqlrconfig_xmldom::getSessionStartQueries() {
	//return &sessionstartqueries;
	return NULL;
}

linkedlist< char * > *sqlrconfig_xmldom::getSessionEndQueries() {
	//return &sessionendqueries;
	return NULL;
}

const char *sqlrconfig_xmldom::getParser() {
	//return parser.getString();
	return NULL;
}

const char *sqlrconfig_xmldom::getTranslations() {
	//return translations.getString();
	return NULL;
}

const char *sqlrconfig_xmldom::getFilters() {
	//return filters.getString();
	return NULL;
}

const char *sqlrconfig_xmldom::getResultSetTranslations() {
	//return resultsettranslations.getString();
	return NULL;
}

const char *sqlrconfig_xmldom::getTriggers() {
	//return triggers.getString();
	return NULL;
}

const char *sqlrconfig_xmldom::getLoggers() {
	//return loggers.getString();
	return NULL;
}

const char *sqlrconfig_xmldom::getQueries() {
	//return queries.getString();
	return NULL;
}

const char *sqlrconfig_xmldom::getPasswordEncryptions() {
	//return passwordencryptions.getString();
	return NULL;
}

const char *sqlrconfig_xmldom::getAuthentications() {
	//return authentications.getString();
	return NULL;
}

linkedlist< listenercontainer * > *sqlrconfig_xmldom::getListenerList() {
	//return &listenerlist;
	return NULL;
}

linkedlist< usercontainer * > *sqlrconfig_xmldom::getUserList() {
	//return &userlist;
	return NULL;
}

linkedlist< connectstringcontainer * > *sqlrconfig_xmldom::getConnectStringList() {
	//return &connectstringlist;
	return NULL;
}

connectstringcontainer *sqlrconfig_xmldom::getConnectString(
						const char *connectionid) {
	/*for (connectstringnode *csn=connectstringlist.getFirst();
						csn; csn=csn->getNext()) {
		if (!charstring::compare(connectionid,
					csn->getValue()->getConnectionId())) {
			return csn->getValue();
		}
	}*/
	return NULL;
}

uint32_t sqlrconfig_xmldom::getConnectionCount() {
	//return connectstringlist.getLength();
	return 0;
}

uint32_t sqlrconfig_xmldom::getMetricTotal() {
	// This is tallied here instead of whenever the parser runs into a
	// metric attribute because people often forget to include metric
	// attributes.  In that case, though each connection has a metric,
	// metrictotal=0, causing no connections to start.
	/*if (!metrictotal) {
		for (connectstringnode *csn=connectstringlist.getFirst();
						csn; csn=csn->getNext()) {
			metrictotal=metrictotal+csn->getValue()->getMetric();
		}
	}
	return metrictotal;*/
	return 0;
}

linkedlist< routecontainer * >	*sqlrconfig_xmldom::getRouteList() {
	//return &routelist;
	return NULL;
}

bool sqlrconfig_xmldom::tagStart(const char *ns, const char *name) {
	debugFunction();

	debugPrintf("<%s>\n",name);

	// bail if we're already done
	if (done) {
		debugPrintf("  already done\n");
		return true;
	}

	// is this an instance tag?
	ininstancetag=!charstring::compare(name,"instance");

	if (ininstancetag) {

		debugPrintf("  in instance tag\n");

		// re-init enabled flag
		enabled=false;

		// re-init get attributes flag
		getattributes=true;

	} else {

		debugPrintf("  not in instance tag\n");

		// bail if we haven't found the specified instance yet
		if (!foundspecifiedinstance) {
			debugPrintf("  haven't found specified instance yet\n");
			return true;
		}
	}

	return (getenabledids)?true:xmldom::tagStart(ns,name);
}

bool sqlrconfig_xmldom::tagEnd(const char *ns, const char *name) {
	debugFunction();

	debugPrintf("<%s>\n",name);

	// bail if we're already done
	if (done) {
		debugPrintf("  already done\n");
		return true;
	}

	// bail if we haven't found the specified instance yet
	// unless we're closing an instance tag
	if (!foundspecifiedinstance && charstring::compare(name,"instance")) {
		debugPrintf("  haven't found specified instance yet\n");
		return true;
	}

	// if we've found the specified instance at this point then we're done
	if (!getenabledids && foundspecifiedinstance &&
				!charstring::compare(name,"instance")) {
		debugPrintf("  found specified instance\n");
		done=true;
	}

	return (getenabledids)?true:xmldom::tagEnd(ns,name);
}

bool sqlrconfig_xmldom::attributeName(const char *name) {
	debugFunction();

	debugPrintf("attribute name=%s\n",name);

	// bail if we're not supposed to get attributes
	if (!getattributes) {
		debugPrintf("  not getting attributes\n");
		return true;
	}

	// bail if we're already done
	if (done) {
		debugPrintf("  already done\n");
		return true;
	}

	// bail if we're not in an instance tag and
	// we haven't found the specified instance yet
	if (!ininstancetag && !foundspecifiedinstance) {
		debugPrintf("  not in instance tag,"
				"haven't found specified instance yet\n");
		return true;
	}

	// check for id and enabled attributes of the instance tag
	inidattribute=(ininstancetag &&
				!charstring::compare(name,"id"));
	inenabledattribute=(ininstancetag &&
				!charstring::compare(name,"enabled"));
	debugPrintf("  inidattribute=%d  inenabledattribute=%d\n",
					inidattribute,inenabledattribute);

	return (getenabledids)?true:xmldom::attributeName(name);
}

bool sqlrconfig_xmldom::attributeValue(const char *value) {
	debugFunction();

	debugPrintf("attribute value=\"%s\"\n",value);

	// bail if we're not supposed to get attributes
	if (!getattributes) {
		debugPrintf("  not getting attributes\n");
		return true;
	}

	// bail if we're already done
	if (done) {
		debugPrintf("  already done\n");
		return true;
	}

	// bail if we're not in an instance tag and
	// we haven't found the specified instance yet
	if (!ininstancetag && !foundspecifiedinstance) {
		debugPrintf("  not in instance tag,"
				"haven't found specified instance yet\n");
		return true;
	}

	if (getenabledids) {

		// set the current id
		if (inidattribute) {
			delete[] currentid;
			currentid=charstring::duplicate(value);
			debugPrintf("  setting current id to \"%s\"\n",
								currentid);
		}

		// if this instance is enabled, then add its id to the id list
		if (inenabledattribute &&
				!charstring::compareIgnoringCase(value,"yes")) {
			idlist->append(charstring::duplicate(currentid));
		}

	} else {

		// check for the specified instance, if we haven't found it yet
		if (!foundspecifiedinstance) {
			foundspecifiedinstance=
				(inidattribute && value &&
					!charstring::compare(value,id));
		}

		// bail if we haven't found the specified instance,
		// unless we're in an instance tag
		if (!foundspecifiedinstance && !ininstancetag) {
			return true;
		}

		// if this is an id attribute but it wasn't the one we were
		// looking for, then disable getting the rest of the
		// attributes for this tag
		if (!foundspecifiedinstance && inidattribute) {
			getattributes=false;
		}
	}

	return (getenabledids)?true:xmldom::attributeValue(value);
}

bool sqlrconfig_xmldom::text(const char *value) {
	debugFunction();
	debugPrintf("text: %s\n",value);

	// bail if we're already done
	if (done) {
		debugPrintf("  already done\n");
		return true;
	}

	// bail if we haven't found the specified instance yet
	if (!foundspecifiedinstance) {
		debugPrintf("  haven't found specified instance yet\n");
		return true;
	}

	return xmldom::text(value);
}

bool sqlrconfig_xmldom::load(const char *urlname, const char *id) {
	debugFunction();

	debugPrintf("urlname=\"%s\"  id=\"%s\"\n",urlname,id);

	// sanity check
	if (charstring::isNullOrEmpty(urlname) ||
			charstring::isNullOrEmpty(id)) {
		return false;
	}

	// re-init
	clear();
	init();

	// set some stateful variables
	getenabledids=false;
	this->id=id;
	foundspecifiedinstance=false;
	done=false;

	// parse the url
	parseUrl(urlname);

	// clean up the tree
	cleanTree();
getRootNode()->print(&stdoutput);

	return foundspecifiedinstance;
}

void sqlrconfig_xmldom::cleanTree() {

	// prune instances for non-specified id's
	xmldomnode *instance=getRootNode()->getFirstTagChild("instance");
	while (!instance->isNullNode()) {
		xmldomnode	*next=instance->getNextTagSibling("instance");
		if (charstring::compare(instance->getAttributeValue("id"),id)) {
			instance->getParent()->deleteChild(instance);
		}
		instance=next;
	}

	// refactor...

	// get the instance
	instance=getRootNode()->getFirstTagChild("instance");

	// addresses="" -> addresses="0.0.0.0"
	xmldomnode	*attr=instance->getAttribute("addresses");
	if (!attr->isNullNode()) {
		if (charstring::isNullOrEmpty(attr->getValue())) {
			attr->setValue("0.0.0.0");
		}
	}

	// unixport -> socket
	attr=instance->getAttribute("unixport");
	if (!attr->isNullNode()) {
		attr->setName("socket");
	}

	// authentication -> authtier
	attr=instance->getAttribute("authentication");
	if (!attr->isNullNode()) {
		attr->setName("authtier");
	}

	// oracle8 -> oracle, sybase -> sap
	attr=instance->getAttribute("dbase");
	if (!attr->isNullNode()) {
		if (!charstring::compare(attr->getValue(),"oracle8")) {
			attr->setValue("oracle");
		} else if (!charstring::compare(attr->getValue(),"sybase")) {
			attr->setValue("sap");
		}
	}

	// missing listeners tag
	if (instance->getFirstTagChild("listeners")->isNullNode()) {
		instance->insertTag("listeners",0);
	}

	// instance.addresses/port/socket/etc. -> listener

	// empty listeners tag

	// no port or socket -> default port

	// convert users/user into auth_userlist

	// passwordencryption -> passwordencryptionid
}


void sqlrconfig_xmldom::parseUrl(const char *urlname) {
	debugFunction();

	// skip leading whitespace
	while (*urlname && character::isWhitespace(*urlname)) {
		urlname++;
	}

	// bump past xmldom protocol identifiers
	if (!charstring::compare(urlname,"xmldom://",9)) {
		urlname+=9;
	} else if (!charstring::compare(urlname,"xmldom:",7)) {
		urlname+=7;
	}

	debugPrintf("urlname=\"%s\"\n",urlname);

	// parse the url as a config directory, config file or link file
	if (!charstring::compare(urlname,"dir:",4)) {
		parseDir(urlname);
	} else {
		debugPrintf("parseFile()...\n");
		if (!parseFile(urlname)) {
			debugPrintf("failed...\n");
			parseLinkFile(urlname);
		}
	}
}

void sqlrconfig_xmldom::parseDir(const char *urlname) {
	debugFunction();

	debugPrintf("urlname=\"%s\"\n",urlname);

	// skip the protocol
	const char	*dir=
		(!charstring::compare(urlname,"dir://",6))?
					(urlname+6):(urlname+4);

	// attempt to parse files in the config dir
	directory	d;
	stringbuffer	fullpath;
	const char	*slash=(!charstring::compareIgnoringCase(
					sys::getOperatingSystemName(),
					"Windows"))?"\\":"/";
	if (!done && d.open(dir)) {
		for (;;) {
			char	*filename=d.read();
			if (!filename) {
				break;
			}
			if (charstring::compare(filename,".") &&
				charstring::compare(filename,"..")) {

				fullpath.clear();
				fullpath.append(dir);
				fullpath.append(slash);
				fullpath.append(filename);
				delete[] filename;

				parseFile(fullpath.getString());
			}
		}
	}
	d.close();
}

void sqlrconfig_xmldom::parseLinkFile(const char *urlname) {
	debugFunction();

	// process the file "urlname" as a list of urls...
	filedescriptor	*fd=NULL;
	file	fl;
	url	u;

	// bump past file protocol identifiers
	if (!charstring::compare(urlname,"file://",7)) {
		urlname+=7;
	} else if (!charstring::compare(urlname,"file:",5)) {
		urlname+=5;
	}

	// bump past xmldom protocol identifiers
	if (!charstring::compare(urlname,"xmldom://",9)) {
		urlname+=9;
	} else if (!charstring::compare(urlname,"xmldom:",7)) {
		urlname+=7;
	}

	debugPrintf("urlname=\"%s\"\n",urlname);

	// parse file or url...
	if (charstring::contains(urlname,"://")) {

		// open the url
		if (!u.open(urlname,O_RDONLY)) {
			return;
		}

		// set fd
		fd=&u;

	} else {

		// open the file
		if (!fl.open(urlname,O_RDONLY)) {
			return;
		}

		// optimize
		filesystem	fs;
		if (fs.initialize(urlname)) {
			fl.setReadBufferSize(fs.getOptimumTransferBlockSize());
		}
		fl.sequentialAccess(0,fl.getSize());
		fl.onlyOnce(0,fl.getSize());

		// set fd
		fd=&fl;
	}

	// read lines from the file
	char	*line=NULL;
	while (fd->read(&line,"\n")>0) {
		
		// trim whitespace
		charstring::bothTrim(line);

		// parse the line (skipping blank lines and comments)
		if (line[0] && line[0]!='#') {
			parseUrl(line);
		}

		// clean up
		delete[] line;

		// break if we found the id we were looking for
		if (foundspecifiedinstance) {
			break;
		}
	}
}

void sqlrconfig_xmldom::getEnabledIds(const char *urlname,
					linkedlist< char * > *idlist) {
	debugFunction();

	debugPrintf("urlname=\"%s\"\n",urlname);

	// sanity check
	if (charstring::isNullOrEmpty(urlname)) {
		return;
	}

	// re-init
	clear();
	init();

	// set some variables
	getenabledids=true;
	this->idlist=idlist;
	foundspecifiedinstance=false;
	done=false;

	// parse the url
	parseUrl(urlname);
}

bool sqlrconfig_xmldom::accessible() {
	debugFunction();
	// FIXME: implement this
	return true;
}


extern "C" {
	SQLRUTIL_DLLSPEC sqlrconfig *new_sqlrconfig_xmldom() {
		return new sqlrconfig_xmldom();
	}
}
