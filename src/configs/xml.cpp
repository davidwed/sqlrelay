// Copyright (c) 2000-2015  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrutil.h>
#include <rudiments/xmlsax.h>
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

class SQLRUTIL_DLLSPEC sqlrconfig_xml : public sqlrconfig, public xmlsax {
	public:
			sqlrconfig_xml();
			~sqlrconfig_xml();

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
		bool		foundcorrectid;
		bool		done;

		void	init();
		void	clear();

		void	parseUrl(const char *urlname);
		void	parseDir(const char *dir);
		void	parseLinkFile(const char *urlname);

		uint32_t	atouint32_t(const char *value,
					const char *defaultvalue,
					uint32_t minvalue);
		int32_t		atoint32_t(const char *value,
					const char *defaultvalue,
					int32_t minvalue);

		bool	tagStart(const char *ns, const char *name);
		bool	attributeName(const char *name);
		bool	attributeValue(const char *value);
		bool	text(const char *string);
		bool	tagEnd(const char *ns, const char *name);

		routecontainer	*routeAlreadyExists(routecontainer *cur);
		void		moveRegexList(routecontainer *cur,
						routecontainer *existing);

		char		**addresses;
		uint64_t	addresscount;
		uint16_t	port;
		char		*unixport;
		bool		listenoninet;
		bool		listenonunix;
		char		*dbase;
		uint32_t	connections;
		uint32_t	maxconnections;
		uint32_t	maxqueuelength;
		uint32_t	growby;
		int32_t		ttl;
		int32_t		softttl;
		uint16_t	maxsessioncount;
		char		*endofsession;
		bool		endofsessioncommit;
		uint32_t	sessiontimeout;
		char		*runasuser;
		char		*runasgroup;
		uint16_t	cursors;
		uint16_t	maxcursors;
		uint16_t	cursorsgrowby;
		char		*authtier;
		char		*sessionhandler;
		char		*handoff;
		bool		authonconnection;
		bool		authondatabase;
		char		*allowedips;
		char		*deniedips;
		char		*debug;
		bool		debugparser;
		bool		debugtranslations;
		bool		debugfilters;
		bool		debugtriggers;
		bool		debugbindtranslations;
		bool		debugresultsettranslations;
		uint64_t	maxclientinfolength;
		uint32_t	maxquerysize;
		uint16_t	maxbindcount;
		uint16_t	maxbindnamelength;
		uint32_t	maxstringbindvaluelength;
		uint32_t	maxlobbindvaluelength;
		uint32_t	maxerrorlength;
		int32_t		idleclienttimeout;
		int64_t		maxlisteners;
		uint32_t	listenertimeout;
		bool		reloginatstart;
		bool		fakeinputbindvariables;
		bool		translatebindvariables;
		char		*isolationlevel;
		bool		ignoreselectdb;
		bool		waitfordowndb;
		char		*datetimeformat;
		char		*dateformat;
		char		*timeformat;
		bool		dateddmm;
		bool		dateyyyyddmm;
		bool		dateyyyyddmmset;
		char		*datedelimiters;
		bool		ignorenondatetime;
		bool		krb;
		char		*krbservice;
		char		*krbkeytab;

		bool		instart;
		bool		inend;
		linkedlist< char *>	sessionstartqueries;
		linkedlist< char *>	sessionendqueries;

		stringbuffer	parser;
		uint16_t	parserdepth;

		stringbuffer	authentications;
		uint16_t	authenticationsdepth;

		stringbuffer	translations;
		uint16_t	translationsdepth;

		stringbuffer	filters;
		uint16_t	filtersdepth;

		stringbuffer	resultsettranslations;
		uint16_t	resultsettranslationsdepth;

		stringbuffer	triggers;
		uint16_t	triggersdepth;

		stringbuffer	loggers;
		uint16_t	loggersdepth;

		stringbuffer	queries;
		uint16_t	queriesdepth;

		stringbuffer	passwordencryptions;
		uint16_t	passwordencryptionsdepth;

		listenercontainer	*currentlistener;
		listenercontainer	*defaultlistener;

		usercontainer		*currentuser;

		connectstringcontainer	*currentconnect;
		uint32_t		connectioncount;
		uint32_t		metrictotal;

		routecontainer		*currentroute;

		linkedlist< listenercontainer * >	listenerlist;
		linkedlist< usercontainer * >		userlist;
		linkedlist< routecontainer *>		routelist;
		linkedlist< connectstringcontainer * >	connectstringlist;
		
		typedef enum {
			INSTANCE_TAG,
			LISTENERS_TAG,
			LISTENER_TAG,
			PARSER_TAG,
			AUTHENTICATIONS_TAG,
			USERS_TAG,
			USER_TAG,
			SESSION_TAG,
			START_TAG,
			END_TAG,
			RUNQUERY_TAG,
			CONNECTIONS_TAG,
			CONNECTION_TAG,
			ROUTER_TAG,
			ROUTE_TAG,
			FILTER_TAG,
			QUERY_TAG,
	   		TRANSLATIONS_TAG,
	   		FILTERS_TAG,
	   		RESULTSETTRANSLATIONS_TAG,
	   		TRIGGERS_TAG,
	   		LOGGERS_TAG,
	   		QUERIES_TAG,
	   		PASSWORDENCRYPTIONS_TAG
		} tag;
		
		tag currenttag;

		typedef enum {
			NO_ATTRIBUTE,
			ID_ATTRIBUTE,
			PARSER_ATTRIBUTE,
			AUTHENTICATIONS_ATTRIBUTE,
			ADDRESSES_ATTRIBUTE,
			PORT_ATTRIBUTE,
			SOCKET_ATTRIBUTE,
			PROTOCOL_ATTRIBUTE,
			DBASE_ATTRIBUTE,
			CONNECTIONS_ATTRIBUTE,
			MAXCONNECTIONS_ATTRIBUTE,
			MAXQUEUELENGTH_ATTRIBUTE,
			GROWBY_ATTRIBUTE,
			TTL_ATTRIBUTE,
			SOFTTTL_ATTRIBUTE,
			MAXSESSIONCOUNT_ATTRIBUTE,
			ENDOFSESSION_ATTRIBUTE,
			SESSIONTIMEOUT_ATTRIBUTE,
			RUNASUSER_ATTRIBUTE,
			RUNASGROUP_ATTRIBUTE,
			CURSORS_ATTRIBUTE,
			MAXCURSORS_ATTRIBUTE,
			CURSORS_GROWBY_ATTRIBUTE,
			AUTHTIER_ATTRIBUTE,
			SESSION_HANDLER_ATTRIBUTE,
			HANDOFF_ATTRIBUTE,
			DENIEDIPS_ATTRIBUTE,
			ALLOWEDIPS_ATTRIBUTE,
			DEBUG_ATTRIBUTE,
			MAXCLIENTINFOLENGTH_ATTRIBUTE,
			MAXQUERYSIZE_ATTRIBUTE,
			MAXBINDCOUNT_ATTRIBUTE,
			MAXBINDNAMELENGTH_ATTRIBUTE,
			MAXSTRINGBINDVALUELENGTH_ATTRIBUTE,
			MAXLOBBINDVALUELENGTH_ATTRIBUTE,
			MAXERRORLENGTH_ATTRIBUTE,
			IDLECLIENTTIMEOUT_ATTRIBUTE,
			USER_ATTRIBUTE,
			PASSWORD_ATTRIBUTE,
			PASSWORDENCRYPTIONID_ATTRIBUTE,
			CONNECTIONID_ATTRIBUTE,
			STRING_ATTRIBUTE,
			METRIC_ATTRIBUTE,
			BEHINDLOADBALANCER_ATTRIBUTE,
			ROUTER_HOST_ATTRIBUTE,
			ROUTER_PORT_ATTRIBUTE,
			ROUTER_SOCKET_ATTRIBUTE,
			ROUTER_USER_ATTRIBUTE,
			ROUTER_PASSWORD_ATTRIBUTE,
			ROUTER_PATTERN_ATTRIBUTE,
			MAXLISTENERS_ATTRIBUTE,
			LISTENERTIMEOUT_ATTRIBUTE,
			RELOGINATSTART_ATTRIBUTE,
			TIMEQUERIES_ATTRIBUTE,
			TIMEQUERIESSEC_ATTRIBUTE,
			TIMEQUERIESUSEC_ATTRIBUTE,
			FAKEINPUTBINDVARIABLES_ATTRIBUTE,
			TRANSLATEBINDVARIABLES_ATTRIBUTE,
			TRANSLATIONS_ATTRIBUTE,
			FILTERS_ATTRIBUTE,
			RESULTSETTRANSLATIONS_ATTRIBUTE,
			TRIGGERS_ATTRIBUTE,
			LOGGERS_ATTRIBUTE,
			QUERIES_ATTRIBUTE,
			PASSWORDENCRYPTIONS_ATTRIBUTE,
			ISOLATIONLEVEL_ATTRIBUTE,
			IGNORESELECTDB_ATTRIBUTE,
			WAITFORDOWNDB_ATTRIBUTE,
			DATETIMEFORMAT_ATTRIBUTE,
			DATEFORMAT_ATTRIBUTE,
			TIMEFORMAT_ATTRIBUTE,
			DATEDDMM_ATTRIBUTE,
			DATEYYYYDDMM_ATTRIBUTE,
			DATEDELIMITERS_ATTRIBUTE,
			IGNORENONDATETIME_ATTRIBUTE,
			KRB_ATTRIBUTE,
			KRBSERVICE_ATTRIBUTE,
			KRBKEYTAB_ATTRIBUTE,
			ENABLED_ATTRIBUTE,
		} attribute;

		attribute	currentattribute;
};

sqlrconfig_xml::sqlrconfig_xml() : sqlrconfig(), xmlsax() {
	init();
}

sqlrconfig_xml::~sqlrconfig_xml() {
	clear();
}

void sqlrconfig_xml::init() {
	getenabledids=false;
	currentid=NULL;
	enabled=false;
	idlist=NULL;
	id=NULL;
	foundcorrectid=false;
	done=false;
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
	softttl=charstring::toInteger(DEFAULT_SOFTTTL);
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
	debugparser=charstring::contains(debug,"parser");
	debugtranslations=charstring::contains(debug,"translations");
	debugfilters=charstring::contains(debug,"filters");
	debugtriggers=charstring::contains(debug,"triggers");
	debugbindtranslations=charstring::contains(debug,"bindtranslations");
	debugresultsettranslations=
			charstring::contains(debug,"resultsettranslations");
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
	currentlistener=NULL;
	defaultlistener=NULL;
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
	currenttag=INSTANCE_TAG;
	parserdepth=0;
	authenticationsdepth=0;
	translationsdepth=0;
	filtersdepth=0;
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
	datedelimiters=charstring::duplicate(DEFAULT_DATEDELIMITERS);
	ignorenondatetime=false;
	krb=false;
	krbservice=charstring::duplicate(DEFAULT_KRBSERVICE);
	krbkeytab=NULL;
	instart=false;
	inend=false;
}

void sqlrconfig_xml::clear() {
	delete[] currentid;
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
	delete[] datedelimiters;

	for (listenernode *ln=listenerlist.getFirst(); ln; ln=ln->getNext()) {
		delete ln->getValue();
	}
	listenerlist.clear();

	for (usernode *un=userlist.getFirst(); un; un=un->getNext()) {
		delete un->getValue();
	}
	userlist.clear();

	for (connectstringnode *csn=connectstringlist.getFirst();
						csn; csn=csn->getNext()) {
		delete csn->getValue();
	}
	connectstringlist.clear();

	for (routenode *rn=routelist.getFirst(); rn; rn=rn->getNext()) {
		delete rn->getValue();
	}
	routelist.clear();

	for (linkedlistnode< char * > *ssln=sessionstartqueries.getFirst();
						ssln; ssln=ssln->getNext()) {
		delete[] ssln->getValue();
	}
	sessionstartqueries.clear();

	for (linkedlistnode< char * > *seln=sessionendqueries.getFirst();
						seln; seln=seln->getNext()) {
		delete[] seln->getValue();
	}
	sessionendqueries.clear();

	addresscount=0;
}

const char * const *sqlrconfig_xml::getDefaultAddresses() {
	return defaultlistener->getAddresses();
}

uint64_t sqlrconfig_xml::getDefaultAddressCount() {
	return defaultlistener->getAddressCount();
}

uint16_t sqlrconfig_xml::getDefaultPort() {
	return defaultlistener->getPort();
}

const char *sqlrconfig_xml::getDefaultSocket() {
	return defaultlistener->getSocket();
}

bool sqlrconfig_xml::getDefaultKrb() {
	return defaultlistener->getKrb();
}

const char *sqlrconfig_xml::getDefaultKrbService() {
	return defaultlistener->getKrbService();
}

bool sqlrconfig_xml::getListenOnInet() {
	return listenoninet;
}

bool sqlrconfig_xml::getListenOnUnix() {
	return listenonunix;
}

const char *sqlrconfig_xml::getDbase() {
	return dbase;
}

uint32_t sqlrconfig_xml::getConnections() {
	return connections;
}

uint32_t sqlrconfig_xml::getMaxConnections() {
	return maxconnections;
}

uint32_t sqlrconfig_xml::getMaxQueueLength() {
	return maxqueuelength;
}

uint32_t sqlrconfig_xml::getGrowBy() {
	return growby;
}

int32_t sqlrconfig_xml::getTtl() {
	return ttl;
}

int32_t sqlrconfig_xml::getSoftTtl() {
	return softttl;
}

uint16_t sqlrconfig_xml::getMaxSessionCount() {
	return maxsessioncount;
}

bool sqlrconfig_xml::getDynamicScaling() {
	return (maxconnections>connections && growby>0 && ttl>-1 &&
		(maxlisteners==-1 || maxqueuelength<=maxlisteners));
}

const char *sqlrconfig_xml::getEndOfSession() {
	return endofsession;
}

bool sqlrconfig_xml::getEndOfSessionCommit() {
	return endofsessioncommit;
}

uint32_t sqlrconfig_xml::getSessionTimeout() {
	return sessiontimeout;
}

const char *sqlrconfig_xml::getRunAsUser() {
	return runasuser;
}

const char *sqlrconfig_xml::getRunAsGroup() {
	return runasgroup;
}

uint16_t sqlrconfig_xml::getCursors() {
	return cursors;
}

uint16_t sqlrconfig_xml::getMaxCursors() {
	return maxcursors;
}

uint16_t sqlrconfig_xml::getCursorsGrowBy() {
	return cursorsgrowby;
}

const char *sqlrconfig_xml::getAuthTier() {
	return authtier;
}

const char *sqlrconfig_xml::getSessionHandler() {
	return sessionhandler;
}

const char *sqlrconfig_xml::getHandoff() {
	return handoff;
}

bool sqlrconfig_xml::getAuthOnConnection() {
	return authonconnection;
}

bool sqlrconfig_xml::getAuthOnDatabase() {
	return authondatabase;
}

const char *sqlrconfig_xml::getAllowedIps() {
	return allowedips;
}

const char *sqlrconfig_xml::getDeniedIps() {
	return deniedips;
}

const char *sqlrconfig_xml::getDebug() {
	return debug;
}

bool sqlrconfig_xml::getDebugParser() {
	return debugparser;
}

bool sqlrconfig_xml::getDebugTranslations() {
	return debugtranslations;
}

bool sqlrconfig_xml::getDebugFilters() {
	return debugfilters;
}

bool sqlrconfig_xml::getDebugTriggers() {
	return debugtriggers;
}

bool sqlrconfig_xml::getDebugBindTranslations() {
	return debugbindtranslations;
}

bool sqlrconfig_xml::getDebugResultSetTranslations() {
	return debugresultsettranslations;
}

uint64_t sqlrconfig_xml::getMaxClientInfoLength() {
	return maxclientinfolength;
}

uint32_t sqlrconfig_xml::getMaxQuerySize() {
	return maxquerysize;
}

uint16_t sqlrconfig_xml::getMaxBindCount() {
	return maxbindcount;
}

uint16_t sqlrconfig_xml::getMaxBindNameLength() {
	return maxbindnamelength;
}

uint32_t sqlrconfig_xml::getMaxStringBindValueLength() {
	return maxstringbindvaluelength;
}

uint32_t sqlrconfig_xml::getMaxLobBindValueLength() {
	return maxlobbindvaluelength;
}

uint32_t sqlrconfig_xml::getMaxErrorLength() {
	return maxerrorlength;
}

int32_t sqlrconfig_xml::getIdleClientTimeout() {
	return idleclienttimeout;
}

int64_t sqlrconfig_xml::getMaxListeners() {
	return maxlisteners;
}

uint32_t sqlrconfig_xml::getListenerTimeout() {
	return listenertimeout;
}

bool sqlrconfig_xml::getReLoginAtStart() {
	return reloginatstart;
}

bool sqlrconfig_xml::getFakeInputBindVariables() {
	return fakeinputbindvariables;
}

bool sqlrconfig_xml::getTranslateBindVariables() {
	return translatebindvariables;
}

const char *sqlrconfig_xml::getIsolationLevel() {
	return isolationlevel;
}

bool sqlrconfig_xml::getIgnoreSelectDatabase() {
	return ignoreselectdb;
}

bool sqlrconfig_xml::getWaitForDownDatabase() {
	return waitfordowndb;
}

const char *sqlrconfig_xml::getDateTimeFormat() {
	return datetimeformat;
}

const char *sqlrconfig_xml::getDateFormat() {
	return dateformat;
}

const char *sqlrconfig_xml::getTimeFormat() {
	return timeformat;
}

bool sqlrconfig_xml::getDateDdMm() {
	return dateddmm;
}

bool sqlrconfig_xml::getDateYyyyDdMm() {
	return dateyyyyddmm;
}

const char *sqlrconfig_xml::getDateDelimiters() {
	return datedelimiters;
}

bool sqlrconfig_xml::getIgnoreNonDateTime() {
	return ignorenondatetime;
}

bool sqlrconfig_xml::getKrb() {
	return krb;
}

const char *sqlrconfig_xml::getKrbService() {
	return krbservice;
}

const char *sqlrconfig_xml::getKrbKeytab() {
	return krbkeytab;
}

linkedlist< char * > *sqlrconfig_xml::getSessionStartQueries() {
	return &sessionstartqueries;
}

linkedlist< char * > *sqlrconfig_xml::getSessionEndQueries() {
	return &sessionendqueries;
}

const char *sqlrconfig_xml::getParser() {
	return parser.getString();
}

const char *sqlrconfig_xml::getTranslations() {
	return translations.getString();
}

const char *sqlrconfig_xml::getFilters() {
	return filters.getString();
}

const char *sqlrconfig_xml::getResultSetTranslations() {
	return resultsettranslations.getString();
}

const char *sqlrconfig_xml::getTriggers() {
	return triggers.getString();
}

const char *sqlrconfig_xml::getLoggers() {
	return loggers.getString();
}

const char *sqlrconfig_xml::getQueries() {
	return queries.getString();
}

const char *sqlrconfig_xml::getPasswordEncryptions() {
	return passwordencryptions.getString();
}

const char *sqlrconfig_xml::getAuthentications() {
	return authentications.getString();
}

linkedlist< listenercontainer * > *sqlrconfig_xml::getListenerList() {
	return &listenerlist;
}

linkedlist< usercontainer * > *sqlrconfig_xml::getUserList() {
	return &userlist;
}

linkedlist< connectstringcontainer * > *sqlrconfig_xml::getConnectStringList() {
	return &connectstringlist;
}

connectstringcontainer *sqlrconfig_xml::getConnectString(
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

uint32_t sqlrconfig_xml::getConnectionCount() {
	return connectstringlist.getLength();
}

uint32_t sqlrconfig_xml::getMetricTotal() {
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

linkedlist< routecontainer * >	*sqlrconfig_xml::getRouteList() {
	return &routelist;
}

bool sqlrconfig_xml::tagStart(const char *ns, const char *name) {

	// don't do anything if we're already done
	// or have not found the correct id
	// (unless we're getting enabled ids)
	if (done || (!foundcorrectid && !getenabledids)) {
		return true;
	}

	bool		ok=true;
	const char	*currentname="instance";
	tag		thistag=currenttag;

	// re-init enabled flag and address count
	if (!charstring::compare(name,"instance")) {
		enabled=false;
		addresses=NULL;
		addresscount=0;
	}

	// set the current tag, validate structure in the process
	switch(currenttag) {
	
		case INSTANCE_TAG:
			currentname="instance";
			if (!charstring::compare(name,"listeners")) {
				thistag=LISTENERS_TAG;
			} else if (!charstring::compare(name,"parser")) {
				thistag=PARSER_TAG;
				parser.clear();
			} else if (!charstring::compare(name,
						"authentications")) {
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
			} else if (!charstring::compare(name,"filters")) {
				thistag=FILTERS_TAG;
				filters.clear();
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

		case LISTENERS_TAG:
			currentname="listeners";
			if (!charstring::compare(name,"listener")) {
				thistag=LISTENER_TAG;
			} else {
				ok=false;
			}
			break;

		case LISTENER_TAG:
			currentname="listener";
			ok=false;
			break;
		
		case USERS_TAG:
			currentname="users";
			if (!charstring::compare(name,"user")) {
				thistag=USER_TAG;
			} else {
				ok=false;
			}
			break;

		case USER_TAG:
			currentname="user";
			ok=false;
			break;
		
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
		
		case START_TAG:
			currentname="start";
			if (!charstring::compare(name,"runquery")) {
				thistag=RUNQUERY_TAG;
			} else {
				ok=false;
			}
			break;

		case END_TAG:
			currentname="end";
			if (!charstring::compare(name,"runquery")) {
				thistag=RUNQUERY_TAG;
			} else {
				ok=false;
			}
			break;

		case RUNQUERY_TAG:
			currentname="runquery";
			ok=false;
			break;
		
		case CONNECTIONS_TAG:
			currentname="connections";
			if (!charstring::compare(name,"connection")) {
				thistag=CONNECTION_TAG;
			} else {
				ok=false;
			}
			break;

		case CONNECTION_TAG:
			currentname="connection";
			ok=false;
			break;
		
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
		
		case FILTER_TAG:
			currentname="filter";
			if (!charstring::compare(name,"query")) {
				thistag=QUERY_TAG;
			} else {
				ok=false;
			}
			break;

		case QUERY_TAG:
			currentname="query";
			ok=false;
			break;
		
		case ROUTE_TAG:
			currentname="route";
			if (!charstring::compare(name,"query")) {
				thistag=QUERY_TAG;
			} else {
				ok=false;
			}
			break;

		default:
			// Nothing to do
			break;
	}
	
	if (!getenabledids && !ok) {
		stderror.printf("unexpected tag <%s> within <%s>\n",
							name,currentname);
		return false;
	}

	// initialize tag data
	switch (thistag) {
		case LISTENERS_TAG:
			currenttag=thistag;
			break;
		case LISTENER_TAG:
			currenttag=thistag;
			currentlistener=new listenercontainer();
			listenerlist.append(currentlistener);
			break;
		case PARSER_TAG:
			if (!charstring::compare(name,"parser")) {
				parserdepth=0;
			} else {
				parserdepth++;
			}
			if (parserdepth) {
				parser.append(">");
			}
			parser.append("<");
			parser.append(name);
			currenttag=thistag;
			break;
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
		case USERS_TAG:
			currenttag=thistag;
			break;
		case USER_TAG:
			currentuser=new usercontainer();
			userlist.append(currentuser);
			break;
		case CONNECTIONS_TAG:
			currenttag=thistag;
			break;
		case CONNECTION_TAG:
			if (id) {
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
		case ROUTER_TAG:
			if (id) {
				currentconnect=new connectstringcontainer();
				connectstringlist.append(currentconnect);
				stringbuffer	connectionid;
				connectionid.append(id)->append("-");
				connectionid.append(connectioncount);
				currentconnect->setConnectionId(
						connectionid.getString());
				connectioncount++;
			}
			currenttag=thistag;
			break;
		case ROUTE_TAG:
		case FILTER_TAG:
			currentroute=new routecontainer();
			currentroute->setIsFilter(thistag==FILTER_TAG);
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
		case FILTERS_TAG:
			if (!charstring::compare(name,"filters")) {
				filtersdepth=0;
			} else {
				filtersdepth++;
			}
			if (filtersdepth) {
				filters.append(">");
			}
			filters.append("<");
			filters.append(name);
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
			currenttag=thistag;
			break;
		case START_TAG:
			currenttag=thistag;
			break;
		case END_TAG:
			currenttag=thistag;
			break;
		case RUNQUERY_TAG:
			currenttag=thistag;
			break;
		default:
			// Nothing to do
		break;
	}
	return true;
}

bool sqlrconfig_xml::tagEnd(const char *ns, const char *name) {

	// don't do anything if we're already done
	// or have not found the correct id
	// (unless we're getting enabled ids)
	if (done || (!foundcorrectid && !getenabledids)) {
		return true;
	}

	if (!charstring::compare(name,"instance")) {

		// if a port or socket was specified in the instance tag then
		// add a listener node for whatever was in the instance tag
		if (port || !charstring::isNullOrEmpty(unixport)) {
			defaultlistener=new listenercontainer();
			defaultlistener->setAddresses(addresses,addresscount);
			defaultlistener->setPort(port);
			defaultlistener->setSocket(unixport);
			listenerlist.append(defaultlistener);
		} else

		// if no port or socket was specified in the instance tag and
		// no listener was specified, then add a listener node with the
		// default port
		if (!listenerlist.getLength()) {
			defaultlistener=new listenercontainer();
			defaultlistener->setPort(
				charstring::toInteger(DEFAULT_PORT));
			listenerlist.append(defaultlistener);
		}

		// if a default listener was defined, then give it the kerberos
		// settings that were specified in the instance tag too
		if (defaultlistener) {
			defaultlistener->setKrb(krb);
			defaultlistener->setKrbService(krbservice);
			defaultlistener->setKrbKeytab(krbkeytab);
		}

		// reset flags
		listenoninet=false;
		listenonunix=false;

		for (listenernode *node=listenerlist.getFirst();
					node; node=node->getNext()) {

			listenercontainer	*l=node->getValue();

			// normalize various address, port, socket combos...

			const char * const	*setaddresses=l->getAddresses();
			uint16_t		setport=l->getPort();
			const char		*setsocket=l->getSocket();

			// if nothing was specified, use the
			// default address and port but no socket
			if (!setaddresses && !setport && !setsocket) {
				char	**addr=new char *[1];
				addr[0]=charstring::duplicate(DEFAULT_ADDRESS);
				l->setAddresses(addr,1);
				l->setPort(charstring::toInteger(DEFAULT_PORT));
			} else

			// if a port was specified but no address,
			// use the default address
			if (setport && !setaddresses) {
				char	**addr=new char *[1];
				addr[0]=charstring::duplicate(DEFAULT_ADDRESS);
				l->setAddresses(addr,1);
			} else

			// if an address was specified by no port
			// use the default port
			if (!setport && setaddresses) {
				l->setPort(charstring::toInteger(DEFAULT_PORT));
			}

			// update flags
			// (FIXME: maybe only set these if the listener uses
			// the default protocol)
			if (l->getPort()) {
				listenoninet=true;
			}
			if (l->getSocket()) {
				listenonunix=true;
			}
		}

		// get the "default" listener if it hasn't already been defined
		if (!defaultlistener) {

			// use the first listener for the default protocol...
			for (listenernode *node=listenerlist.getFirst();
						node; node=node->getNext()) {

				listenercontainer	*l=node->getValue();
				if (!charstring::compare(l->getProtocol(),
							DEFAULT_PROTOCOL)) {
					defaultlistener=l;
					break;
				}
			}

			// if that doesn't exist then just the first listener
			if (!defaultlistener) {
				defaultlistener=
					listenerlist.getFirst()->getValue();
			}
		}

		// if we're looking for enabled ids then add this one here
		if (getenabledids && enabled) {
			idlist->append(charstring::duplicate(currentid));
		}
	}

	// Close up the current tag
	switch (currenttag) {
		case PARSER_TAG:
			if (!charstring::compare(name,"parser")) {
				currenttag=INSTANCE_TAG;
			}
			parser.append("></");
			parser.append(name);
			if (!parserdepth) {
				parser.append(">");
			}
			parserdepth--;
			break;
		case AUTHENTICATIONS_TAG:
			if (!charstring::compare(name,"authentications")) {
				currenttag=INSTANCE_TAG;
			}
			authentications.append("></");
			authentications.append(name);
			if (!authenticationsdepth) {
				authentications.append(">");
			}
			authenticationsdepth--;
			break;
		case LISTENERS_TAG:
			if (!charstring::compare(name,"listeners")) {
				currenttag=INSTANCE_TAG;
			}
			break;
		case LISTENER_TAG:
			currenttag=LISTENERS_TAG;
			break;
		case USERS_TAG:
			if (!charstring::compare(name,"users")) {
				currenttag=INSTANCE_TAG;
			}
			break;
		case CONNECTIONS_TAG:
			if (!charstring::compare(name,"connections")) {
				currenttag=INSTANCE_TAG;
			}
			break;
		case ROUTER_TAG:
			if (!charstring::compare(name,"router")) {
				currenttag=INSTANCE_TAG;
			}
			break;
		case ROUTE_TAG:
		case FILTER_TAG:
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
				currenttag=INSTANCE_TAG;
			}
			translations.append("></");
			translations.append(name);
			if (!translationsdepth) {
				translations.append(">");
			}
			translationsdepth--;
			break;
		case FILTERS_TAG:
			if (!charstring::compare(name,"filters")) {
				currenttag=INSTANCE_TAG;
			}
			filters.append("></");
			filters.append(name);
			if (!filtersdepth) {
				filters.append(">");
			}
			filtersdepth--;
			break;
		case RESULTSETTRANSLATIONS_TAG:
			if (!charstring::compare(name,
						"resultsettranslations")) {
				currenttag=INSTANCE_TAG;
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
				currenttag=INSTANCE_TAG;
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
				currenttag=INSTANCE_TAG;
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
				currenttag=INSTANCE_TAG;
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
				currenttag=INSTANCE_TAG;
			}
			passwordencryptions.append("></");
			passwordencryptions.append(name);
			if (!passwordencryptionsdepth) {
				passwordencryptions.append(">");
			}
			passwordencryptionsdepth--;
			break;
		case SESSION_TAG:
			currenttag=INSTANCE_TAG;
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
	if (!getenabledids && foundcorrectid &&
			!charstring::compare((char *)name,"instance")) {
		done=true;
	}
	return true;
}

bool sqlrconfig_xml::attributeName(const char *name) {

	// don't do anything if we're already done
	if (done) {
		return true;
	}

	currentattribute=NO_ATTRIBUTE;

	switch (currenttag) {
	
	// Attributes of the <instance> tag
	case INSTANCE_TAG:

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
		} else if (!charstring::compare(name,"softttl")) {
			currentattribute=SOFTTTL_ATTRIBUTE;
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
		} else if (!charstring::compare(name,"datedelimiters")) {
			currentattribute=DATEDELIMITERS_ATTRIBUTE;
		} else if (!charstring::compare(name,"ignorenondatetime")) {
			currentattribute=IGNORENONDATETIME_ATTRIBUTE;
		} else if (!charstring::compare(name,"krb")) {
			currentattribute=KRB_ATTRIBUTE;
		} else if (!charstring::compare(name,"krbservice")) {
			currentattribute=KRBSERVICE_ATTRIBUTE;
		} else if (!charstring::compare(name,"krbkeytab")) {
			currentattribute=KRBKEYTAB_ATTRIBUTE;
		} else if (!charstring::compare(name,"enabled")) {
			currentattribute=ENABLED_ATTRIBUTE;
		}
		break;

	// Attributes of the <listeners> and <listener> tags
	case LISTENERS_TAG:
	case LISTENER_TAG:
		if (!charstring::compare(name,"addresses")) {
			currentattribute=ADDRESSES_ATTRIBUTE;
		} else if (!charstring::compare(name,"port")) {
			currentattribute=PORT_ATTRIBUTE;
		} else if (!charstring::compare(name,"socket")) {
			currentattribute=SOCKET_ATTRIBUTE;
		} else if (!charstring::compare(name,"protocol")) {
			currentattribute=PROTOCOL_ATTRIBUTE;
		} else if (!charstring::compare(name,"krb")) {
			currentattribute=KRB_ATTRIBUTE;
		} else if (!charstring::compare(name,"krbservice")) {
			currentattribute=KRBSERVICE_ATTRIBUTE;
		} else if (!charstring::compare(name,"krbkeytab")) {
			currentattribute=KRBKEYTAB_ATTRIBUTE;
		}
		break;

	case PARSER_TAG:
		parser.append(" ")->append(name);
		currentattribute=PARSER_ATTRIBUTE;
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
		} else if (!charstring::compare(name,"passwordencryptionid") ||
			// also support older version of the attribute
			!charstring::compare(name,"passwordencryption")) {
			currentattribute=PASSWORDENCRYPTIONID_ATTRIBUTE;
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
		} else if (!charstring::compare(name,"passwordencryptionid") ||
			// also support older version of the attribute
			!charstring::compare(name,"passwordencryption")) {
			currentattribute=PASSWORDENCRYPTIONID_ATTRIBUTE;
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

	case FILTERS_TAG:
		filters.append(" ")->append(name);
		currentattribute=FILTERS_ATTRIBUTE;
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

	if ((getenabledids || foundcorrectid) &&
			currentattribute==NO_ATTRIBUTE) {
		const char *tagname="instance";
		switch (currenttag) {
			case INSTANCE_TAG:
				tagname="instance";
				break;
			case LISTENERS_TAG:
				tagname="listeners";
				break;
			case LISTENER_TAG:
				tagname="listener";
				break;
			case PARSER_TAG:
				tagname="parser";
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
			case FILTERS_TAG:
				tagname="filters";
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
		if (!getenabledids) {
			stderror.printf("WARNING: unrecognized attribute "
					"\"%s\" within <%s> tag or section\n",
					name,tagname);
		}
	}

	// set the current attribute
	return true;
}

bool sqlrconfig_xml::attributeValue(const char *value) {

	// don't do anything if we're already done
	if (done) {
		return true;
	}

	// set the current id
	if (getenabledids && currentattribute==ID_ATTRIBUTE) {
		delete[] currentid;
		currentid=charstring::duplicate(value);
	}

	if (!foundcorrectid && !getenabledids) {

		// if we haven't found the correct id yet, check for it
		if (currentattribute==ID_ATTRIBUTE &&
			value && !charstring::compare(value,id)) {
			foundcorrectid=true;
		}
	
	} else {

		// if we have found the correct id, process the attribute...

		if (currenttag==PARSER_TAG) {
			parser.append("=\"");
			parser.append(value)->append("\"");
		} else if (currenttag==AUTHENTICATIONS_TAG) {
			authentications.append("=\"");
			authentications.append(value)->append("\"");
		} else if (currenttag==TRANSLATIONS_TAG) {
			translations.append("=\"");
			translations.append(value)->append("\"");
		} else if (currenttag==FILTERS_TAG) {
			filters.append("=\"");
			filters.append(value)->append("\"");
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

			// if the attribute was left blank,
			// assume 0.0.0.0
			if (!charstring::length(value)) {
				value=DEFAULT_ADDRESS;
			}
			char		**addr=NULL;
			uint64_t	addrcount=0;
			charstring::split(value,",",true,
					&addr,&addrcount);
			uint64_t	index;
			for (index=0; index<addrcount; index++) {
				charstring::bothTrim(addr[index]);
			}

			if (currenttag==INSTANCE_TAG) {
				for (index=0; index<addresscount; index++) {
					delete[] addresses[index];
				}
				delete[] addresses;
				addresses=addr;
				addresscount=addrcount;
			} else if (currenttag==LISTENER_TAG) {
				currentlistener->setAddresses(addr,addrcount);
			}
		} else if (currentattribute==PORT_ATTRIBUTE) {
			if (currenttag==INSTANCE_TAG) {
				port=atouint32_t(value,"0",0);
			} else if (currenttag==LISTENER_TAG) {
				currentlistener->setPort(
						atouint32_t(value,"0",0));
			}
		} else if (currentattribute==PROTOCOL_ATTRIBUTE) {
			currentlistener->setProtocol(value);
		} else if (currentattribute==SOCKET_ATTRIBUTE) {
			if (currenttag==INSTANCE_TAG) {
				delete[] unixport;
				unixport=charstring::duplicate(value);
			} else if (currenttag==LISTENER_TAG) {
				currentlistener->setSocket(value);
			}
		} else if (currentattribute==DBASE_ATTRIBUTE) {
			delete[] dbase;
			// support dbase="oracle8" and dbase="sybase" by
			// converting them to oracle8 and sap
			if (!charstring::compare(value,"oracle8")) {
				value="oracle";
			} else if (!charstring::compare(value,"sybase")) {
				value="sap";
			}
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
			ttl=atoint32_t(value,DEFAULT_TTL,0);
		} else if (currentattribute==SOFTTTL_ATTRIBUTE) {
			softttl=atoint32_t(value,DEFAULT_SOFTTTL,0);
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
			debugparser=charstring::contains(debug,
							"parser");
			debugtranslations=charstring::contains(debug,
							"translations");
			debugfilters=charstring::contains(debug,
							"filters");
			debugtriggers=charstring::contains(debug,
							"triggers");
			debugbindtranslations=
					charstring::contains(debug,
							"bindtranslations");
			debugresultsettranslations=
					charstring::contains(debug,
						"resultsettranslations");
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
		} else if (currentattribute==PASSWORDENCRYPTIONID_ATTRIBUTE) {
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
		} else if (currentattribute==DATEDELIMITERS_ATTRIBUTE) {
			delete[] datedelimiters;
			datedelimiters=charstring::duplicate(value);
		} else if (currentattribute==IGNORENONDATETIME_ATTRIBUTE) {
			ignorenondatetime=
				!charstring::compareIgnoringCase(value,"yes");
		} else if (currentattribute==KRB_ATTRIBUTE) {
			if (currenttag==INSTANCE_TAG) {
				krb=!charstring::compareIgnoringCase(
								value,"yes");
			} else if (currenttag==LISTENER_TAG) {
				currentlistener->setKrb(
					!charstring::compareIgnoringCase(
								value,"yes"));
			}
		} else if (currentattribute==KRBSERVICE_ATTRIBUTE) {
			if (currenttag==INSTANCE_TAG) {
				delete[] krbservice;
				krbservice=charstring::duplicate(value);
			} else if (currenttag==LISTENER_TAG) {
				currentlistener->setKrbService(value);
			}
		} else if (currentattribute==KRBKEYTAB_ATTRIBUTE) {
			if (currenttag==INSTANCE_TAG) {
				delete[] krbkeytab;
				krbkeytab=charstring::duplicate(value);
			} else if (currenttag==LISTENER_TAG) {
				currentlistener->setKrbKeytab(value);
			}
		} else if (currentattribute==ENABLED_ATTRIBUTE) {
			enabled=!charstring::compareIgnoringCase(value,"yes");
		}
	}
	return true;
}

bool sqlrconfig_xml::text(const char *string) {

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

uint32_t sqlrconfig_xml::atouint32_t(const char *value,
				const char *defaultvalue, uint32_t minvalue) {
	uint32_t	retval=charstring::toUnsignedInteger(
						(value)?value:defaultvalue);
	if (retval<minvalue) {
		retval=charstring::toUnsignedInteger(defaultvalue);
	}
	return retval;
}

int32_t sqlrconfig_xml::atoint32_t(const char *value,
				const char *defaultvalue, int32_t minvalue) {
	int32_t	retval=charstring::toInteger((value)?value:defaultvalue);
	if (retval<minvalue) {
		retval=charstring::toInteger(defaultvalue);
	}
	return retval;
}

routecontainer *sqlrconfig_xml::routeAlreadyExists(routecontainer *cur) {

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

void sqlrconfig_xml::moveRegexList(routecontainer *cur,
					routecontainer *existing) {

	for (linkedlistnode< regularexpression * > *re=
				cur->getRegexList()->getFirst();
						re; re=re->getNext()) {
		existing->getRegexList()->append(re->getValue());
	}
	cur->getRegexList()->clear();
}

bool sqlrconfig_xml::load(const char *urlname, const char *id) {

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
	foundcorrectid=false;
	done=false;

	// parse the url
	parseUrl(urlname);

	return foundcorrectid;
}


void sqlrconfig_xml::parseUrl(const char *urlname) {

	// skip leading whitespace
	while (*urlname && character::isWhitespace(*urlname)) {
		urlname++;
	}

	// bump past xml protocol identifiers
	if (!charstring::compare(urlname,"xml://",6)) {
		urlname+=6;
	} else if (!charstring::compare(urlname,"xml:",4)) {
		urlname+=4;
	}

	// parse the url as a config directory, config file or link file
	if (!charstring::compare(urlname,"dir:",4)) {
		parseDir(urlname);
	} else {
		if (!parseFile(urlname)) {
			parseLinkFile(urlname);
		}
	}
}

void sqlrconfig_xml::parseDir(const char *urlname) {

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

void sqlrconfig_xml::parseLinkFile(const char *urlname) {

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

	// bump past xml protocol identifiers
	if (!charstring::compare(urlname,"xml://",6)) {
		urlname+=6;
	} else if (!charstring::compare(urlname,"xml:",4)) {
		urlname+=4;
	}


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
		if (foundcorrectid) {
			break;
		}
	}
}

void sqlrconfig_xml::getEnabledIds(const char *urlname,
					linkedlist< char * > *idlist) {

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
	foundcorrectid=false;
	done=false;

	// parse the url
	parseUrl(urlname);
}

bool sqlrconfig_xml::accessible() {
	// FIXME: implement this
	return true;
}


extern "C" {
	SQLRUTIL_DLLSPEC sqlrconfig *new_sqlrconfig_xml() {
		return new sqlrconfig_xml();
	}
}
