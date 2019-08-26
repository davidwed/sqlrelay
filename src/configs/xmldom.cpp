// Copyright (c) 1999-2018 David Muse
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
#include <rudiments/process.h>
//#define DEBUG_MESSAGES
#include <rudiments/debugprint.h>

#include <defines.h>
#include <defaults.h>

class SQLRUTIL_DLLSPEC sqlrconfig_xmldom : public sqlrconfig, public xmldom {
	public:
			sqlrconfig_xmldom();
			~sqlrconfig_xmldom();

		void	getEnabledIds(const char *urlname,
					linkedlist< char * > *idlist);
		bool	load(const char *urlname, const char *id);
		bool	accessible();

		const char	*getDefaultAddresses();
		uint16_t	getDefaultPort();
		const char	*getDefaultSocket();

		bool		getDefaultKrb();
		const char	*getDefaultKrbService();
		const char	*getDefaultKrbKeytab();
		const char	*getDefaultKrbMech();
		const char	*getDefaultKrbFlags();

		bool		getDefaultTls();
		const char	*getDefaultTlsCiphers();

		const char	*getDefaultUser();
		const char	*getDefaultPassword();

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
		bool		getDebugSql();
		bool		getDebugBulkLoad();
		bool		getDebugParser();
		bool		getDebugDirectives();
		bool		getDebugTranslations();
		bool		getDebugFilters();
		bool		getDebugTriggers();
		bool		getDebugBindTranslations();
		bool		getDebugBindVariableTranslations();
		bool		getDebugResultSetTranslations();
		bool		getDebugResultSetRowTranslations();
		bool		getDebugResultSetRowBlockTranslations();
		bool		getDebugResultSetHeaderTranslations();
		bool		getDebugProtocols();
		bool		getDebugAuths();
		bool		getDebugPasswordEncryptions();
		bool		getDebugLoggers();
		bool		getDebugNotifications();
		bool		getDebugSchedules();
		bool		getDebugRouters();
		bool		getDebugQueries();
		bool		getDebugModuleDatas();
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
		const char	*getFakeInputBindVariablesDateFormat();
		bool		getFakeInputBindVariablesUnicodeStrings();
		bool		getBindVariableDelimiterQuestionMarkSupported();
		bool		getBindVariableDelimiterColonSupported();
		bool		getBindVariableDelimiterAtSignSupported();
		bool		getBindVariableDelimiterDollarSignSupported();
		bool		getTranslateBindVariables();
		const char	*getIsolationLevel();
		bool		getIgnoreSelectDatabase();
		bool		getWaitForDownDatabase();

		linkedlist< char *>	*getSessionStartQueries();
		linkedlist< char *>	*getSessionEndQueries();

		domnode	*getListeners();
		domnode	*getParser();
		domnode	*getDirectives();
		domnode	*getTranslations();
		domnode	*getFilters();
		domnode	*getBindVariableTranslations();
		domnode	*getResultSetTranslations();
		domnode	*getResultSetRowTranslations();
		domnode	*getResultSetRowBlockTranslations();
		domnode	*getResultSetHeaderTranslations();
		domnode	*getTriggers();
		domnode	*getLoggers();
		domnode	*getNotifications();
		domnode	*getSchedules();
		domnode	*getRouters();
		domnode	*getQueries();
		domnode	*getPasswordEncryptions();
		domnode	*getAuths();
		domnode	*getModuleDatas();

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
		void	normalizeTree();
		void	getTreeValues();
		routecontainer	*routeAlreadyExists(routecontainer *cur);
		void		moveRegexList(routecontainer *cur,
						routecontainer *existing);
		uint32_t	atouint32_t(const char *value,
						const char *defaultvalue,
						uint32_t minvalue);
		int32_t		atoint32_t(const char *value,
						const char *defaultvalue,
						int32_t minvalue);
		void	parseDir(const char *dir);
		void	parseLinkFile(const char *urlname);

		bool	tagStart(const char *ns, const char *name);
		bool	tagEnd(const char *ns, const char *name);
		bool	attributeName(const char *name);
		bool	attributeValue(const char *value);
		bool	text(const char *value);
		bool	comment(const char *value);

		bool	hasDebug(const char *value, const char *debug);

		bool		listenoninet;
		bool		listenonunix;
		const char	*dbase;
		uint32_t	connections;
		uint32_t	maxconnections;
		uint32_t	maxqueuelength;
		uint32_t	growby;
		int32_t		ttl;
		int32_t		softttl;
		uint16_t	maxsessioncount;
		const char	*endofsession;
		bool		endofsessioncommit;
		uint32_t	sessiontimeout;
		const char	*runasuser;
		const char	*runasgroup;
		uint16_t	cursors;
		uint16_t	maxcursors;
		uint16_t	cursorsgrowby;
		const char	*authtier;
		const char	*sessionhandler;
		const char	*handoff;
		bool		authonconnection;
		bool		authondatabase;
		const char	*allowedips;
		const char	*deniedips;
		const char	*debug;
		bool		debugsql;
		bool		debugbulkload;
		bool		debugparser;
		bool		debugdirectives;
		bool		debugtranslations;
		bool		debugfilters;
		bool		debugtriggers;
		bool		debugbindtranslations;
		bool		debugbindvariabletranslations;
		bool		debugresultsettranslations;
		bool		debugresultsetrowtranslations;
		bool		debugresultsetrowblocktranslations;
		bool		debugresultsetheadertranslations;
		bool		debugprotocols;
		bool		debugauths;
		bool		debugpwdencs;
		bool		debugloggers;
		bool		debugnotifications;
		bool		debugschedules;
		bool		debugrouters;
		bool		debugqueries;
		bool		debugmoduledatas;
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
		const char	*fakeinputbindvariablesdateformat;
		bool		fakeinputbindvariablesunicodestrings;
		const char	*bindvariabledelimiters;
		bool		questionmarksupported;
		bool		colonsupported;
		bool		atsignsupported;
		bool		dollarsignsupported;
		bool		translatebindvariables;
		const char	*isolationlevel;
		bool		ignoreselectdb;
		bool		waitfordowndb;

		linkedlist< char *>	sessionstartqueries;
		linkedlist< char *>	sessionendqueries;

		domnode	*listenersxml;
		domnode	*parserxml;
		domnode	*directivesxml;
		domnode	*translationsxml;
		domnode	*filtersxml;
		domnode	*bindvariabletranslationsxml;
		domnode	*resultsettranslationsxml;
		domnode	*resultsetrowtranslationsxml;
		domnode	*resultsetrowblocktranslationsxml;
		domnode	*resultsetheadertranslationsxml;
		domnode	*triggersxml;
		domnode	*loggersxml;
		domnode	*notificationsxml;
		domnode	*schedulesxml;
		domnode	*routersxml;
		domnode	*queriesxml;
		domnode	*pwdencsxml;
		domnode	*authsxml;
		domnode	*moduledatasxml;

		uint32_t	metrictotal;

		linkedlist< routecontainer *>		routelist;
		linkedlist< connectstringcontainer * >	connectstringlist;

		domnode	*defaultlistener;
		const char	*defaultaddresses;
		uint16_t	defaultport;
		const char	*defaultsocket;
		bool		defaultkrb;
		const char	*defaultkrbkeytab;
		const char	*defaultkrbservice;
		const char	*defaultkrbmech;
		const char	*defaultkrbflags;
		bool		defaulttls;
		const char	*defaulttlsciphers;
		const char	*defaultuser;
		const char	*defaultpassword;

		bool		ininstancetag;
		bool		inidattribute;
		bool		inenabledattribute;
		bool		getattributes;
};

sqlrconfig_xmldom::sqlrconfig_xmldom() : sqlrconfig(), xmldom(false) {
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
	ininstancetag=false;
	inidattribute=false;
	inenabledattribute=false;
	getattributes=true;
	done=false;

	listenoninet=false;
	listenonunix=false;
	dbase=DEFAULT_DBASE;
	connections=charstring::toInteger(DEFAULT_CONNECTIONS);
	maxconnections=0;
	maxqueuelength=charstring::toInteger(DEFAULT_MAXQUEUELENGTH);
	growby=charstring::toInteger(DEFAULT_GROWBY);
	ttl=charstring::toInteger(DEFAULT_TTL);
	softttl=charstring::toInteger(DEFAULT_SOFTTTL);
	maxsessioncount=charstring::toInteger(DEFAULT_MAXSESSIONCOUNT);
	endofsession=DEFAULT_ENDOFSESSION;
	endofsessioncommit=!charstring::compare(endofsession,"commit");
	sessiontimeout=charstring::toUnsignedInteger(DEFAULT_SESSIONTIMEOUT);
	runasuser=DEFAULT_RUNASUSER;
	runasgroup=DEFAULT_RUNASGROUP;
	cursors=charstring::toInteger(DEFAULT_CURSORS);
	maxcursors=charstring::toInteger(DEFAULT_MAXCURSORS);
	cursorsgrowby=charstring::toInteger(DEFAULT_CURSORS_GROWBY);
	authtier=DEFAULT_AUTHTIER;
	authonconnection=true;
	authondatabase=false;
	sessionhandler=DEFAULT_SESSION_HANDLER;
	handoff=DEFAULT_HANDOFF;
	allowedips=DEFAULT_DENIEDIPS;
	deniedips=DEFAULT_DENIEDIPS;
	debug=DEFAULT_DEBUG;
	debugsql=hasDebug(debug,"sql");
	debugbulkload=hasDebug(debug,"bulkload");
	debugparser=hasDebug(debug,"parser");
	debugdirectives=hasDebug(debug,"directives");
	debugtranslations=hasDebug(debug,"translations");
	debugfilters=hasDebug(debug,"filters");
	debugtriggers=hasDebug(debug,"triggers");
	debugbindtranslations=
		hasDebug(debug,"bindtranslations");
	debugbindvariabletranslations=
		hasDebug(debug,"bindvariabletranslations");
	debugresultsettranslations=
		hasDebug(debug,"resultsettranslations");
	debugresultsetrowtranslations=
		hasDebug(debug,"resultsetrowtranslations");
	debugresultsetrowblocktranslations=
		hasDebug(debug,"resultsetrowblocktranslations");
	debugresultsetheadertranslations=
		hasDebug(debug,"resultsetheadertranslations");
	debugprotocols=hasDebug(debug,"protocols");
	debugauths=hasDebug(debug,"auths");
	debugpwdencs=hasDebug(debug,"passwordencrypytions");
	debugloggers=hasDebug(debug,"loggers");
	debugnotifications=hasDebug(debug,"notifications");
	debugschedules=hasDebug(debug,"schedules");
	debugrouters=hasDebug(debug,"routers");
	debugqueries=hasDebug(debug,"queries");
	debugmoduledatas=hasDebug(debug,"moduledatas");
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
	metrictotal=0;
	maxlisteners=charstring::toInteger(DEFAULT_MAXLISTENERS);
	listenertimeout=charstring::toUnsignedInteger(DEFAULT_LISTENERTIMEOUT);
	reloginatstart=charstring::isYes(DEFAULT_RELOGINATSTART);
	fakeinputbindvariables=charstring::isYes(
					DEFAULT_FAKEINPUTBINDVARIABLES);
	fakeinputbindvariablesdateformat=NULL;
	fakeinputbindvariablesunicodestrings=charstring::isYes(
				DEFAULT_FAKEINPUTBINDVARIABLESUNICODESTRINGS);
	bindvariabledelimiters=DEFAULT_BINDVARIABLEDELIMITERS;
	questionmarksupported=charstring::contains(bindvariabledelimiters,'?');
	colonsupported=charstring::contains(bindvariabledelimiters,':');
	atsignsupported=charstring::contains(bindvariabledelimiters,'@');
	dollarsignsupported=charstring::contains(bindvariabledelimiters,'$');
	translatebindvariables=charstring::isYes(
					DEFAULT_TRANSLATEBINDVARIABLES);
	isolationlevel=NULL;
	ignoreselectdb=false;
	waitfordowndb=true;

	defaultlistener=NULL;
	defaultaddresses=NULL;
	defaultport=0;
	defaultsocket=NULL;
	defaultkrb=false;
	defaultkrbkeytab=NULL;
	defaultkrbservice=NULL;
	defaultkrbmech=NULL;
	defaultkrbflags=NULL;
	defaulttls=false;
	defaulttlsciphers=NULL;
	defaultuser=NULL;
	defaultpassword=NULL;
}

void sqlrconfig_xmldom::clear() {
	debugFunction();

	connectstringlist.clearAndDelete();
	routelist.clearAndDelete();
	sessionstartqueries.clearAndArrayDelete();
	sessionendqueries.clearAndArrayDelete();
}

const char *sqlrconfig_xmldom::getDefaultAddresses() {
	return defaultaddresses;
}

uint16_t sqlrconfig_xmldom::getDefaultPort() {
	return defaultport;
}

const char *sqlrconfig_xmldom::getDefaultSocket() {
	return defaultsocket;
}

bool sqlrconfig_xmldom::getDefaultKrb() {
	return defaultkrb;
}

const char *sqlrconfig_xmldom::getDefaultKrbService() {
	return defaultkrbservice;
}

const char *sqlrconfig_xmldom::getDefaultKrbKeytab() {
	return defaultkrbkeytab;
}

const char *sqlrconfig_xmldom::getDefaultKrbMech() {
	return defaultkrbmech;
}

const char *sqlrconfig_xmldom::getDefaultKrbFlags() {
	return defaultkrbflags;
}

bool sqlrconfig_xmldom::getDefaultTls() {
	return defaulttls;
}

const char *sqlrconfig_xmldom::getDefaultTlsCiphers() {
	return defaulttlsciphers;
}

const char *sqlrconfig_xmldom::getDefaultUser() {
	return defaultuser;
}

const char *sqlrconfig_xmldom::getDefaultPassword() {
	return defaultpassword;
}

bool sqlrconfig_xmldom::getListenOnInet() {
	return listenoninet;
}

bool sqlrconfig_xmldom::getListenOnUnix() {
	return listenonunix;
}

const char *sqlrconfig_xmldom::getDbase() {
	return dbase;
}

uint32_t sqlrconfig_xmldom::getConnections() {
	return connections;
}

uint32_t sqlrconfig_xmldom::getMaxConnections() {
	return maxconnections;
}

uint32_t sqlrconfig_xmldom::getMaxQueueLength() {
	return maxqueuelength;
}

uint32_t sqlrconfig_xmldom::getGrowBy() {
	return growby;
}

int32_t sqlrconfig_xmldom::getTtl() {
	return ttl;
}

int32_t sqlrconfig_xmldom::getSoftTtl() {
	return softttl;
}

uint16_t sqlrconfig_xmldom::getMaxSessionCount() {
	return maxsessioncount;
}

bool sqlrconfig_xmldom::getDynamicScaling() {
	return (maxconnections>connections && growby>0 && ttl>-1 &&
		(maxlisteners==-1 || maxqueuelength<=maxlisteners));
}

const char *sqlrconfig_xmldom::getEndOfSession() {
	return endofsession;
}

bool sqlrconfig_xmldom::getEndOfSessionCommit() {
	return endofsessioncommit;
}

uint32_t sqlrconfig_xmldom::getSessionTimeout() {
	return sessiontimeout;
}

const char *sqlrconfig_xmldom::getRunAsUser() {
	return runasuser;
}

const char *sqlrconfig_xmldom::getRunAsGroup() {
	return runasgroup;
}

uint16_t sqlrconfig_xmldom::getCursors() {
	return cursors;
}

uint16_t sqlrconfig_xmldom::getMaxCursors() {
	return maxcursors;
}

uint16_t sqlrconfig_xmldom::getCursorsGrowBy() {
	return cursorsgrowby;
}

const char *sqlrconfig_xmldom::getAuthTier() {
	return authtier;
}

const char *sqlrconfig_xmldom::getSessionHandler() {
	return sessionhandler;
}

const char *sqlrconfig_xmldom::getHandoff() {
	return handoff;
}

bool sqlrconfig_xmldom::getAuthOnConnection() {
	return authonconnection;
}

bool sqlrconfig_xmldom::getAuthOnDatabase() {
	return authondatabase;
}

const char *sqlrconfig_xmldom::getAllowedIps() {
	return allowedips;
}

const char *sqlrconfig_xmldom::getDeniedIps() {
	return deniedips;
}

const char *sqlrconfig_xmldom::getDebug() {
	return debug;
}

bool sqlrconfig_xmldom::getDebugSql() {
	return debugsql;
}

bool sqlrconfig_xmldom::getDebugBulkLoad() {
	return debugbulkload;
}

bool sqlrconfig_xmldom::getDebugParser() {
	return debugparser;
}

bool sqlrconfig_xmldom::getDebugDirectives() {
	return debugdirectives;
}

bool sqlrconfig_xmldom::getDebugTranslations() {
	return debugtranslations;
}

bool sqlrconfig_xmldom::getDebugFilters() {
	return debugfilters;
}

bool sqlrconfig_xmldom::getDebugTriggers() {
	return debugtriggers;
}

bool sqlrconfig_xmldom::getDebugBindTranslations() {
	return debugbindtranslations;
}

bool sqlrconfig_xmldom::getDebugBindVariableTranslations() {
	return debugbindvariabletranslations;
}

bool sqlrconfig_xmldom::getDebugResultSetTranslations() {
	return debugresultsettranslations;
}

bool sqlrconfig_xmldom::getDebugResultSetRowTranslations() {
	return debugresultsetrowtranslations;
}

bool sqlrconfig_xmldom::getDebugResultSetRowBlockTranslations() {
	return debugresultsetrowblocktranslations;
}

bool sqlrconfig_xmldom::getDebugResultSetHeaderTranslations() {
	return debugresultsetheadertranslations;
}

bool sqlrconfig_xmldom::getDebugProtocols() {
	return debugprotocols;
}

bool sqlrconfig_xmldom::getDebugAuths() {
	return debugauths;
}

bool sqlrconfig_xmldom::getDebugPasswordEncryptions() {
	return debugpwdencs;
}

bool sqlrconfig_xmldom::getDebugLoggers() {
	return debugloggers;
}

bool sqlrconfig_xmldom::getDebugNotifications() {
	return debugnotifications;
}

bool sqlrconfig_xmldom::getDebugSchedules() {
	return debugschedules;
}

bool sqlrconfig_xmldom::getDebugRouters() {
	return debugrouters;
}

bool sqlrconfig_xmldom::getDebugQueries() {
	return debugqueries;
}

bool sqlrconfig_xmldom::getDebugModuleDatas() {
	return debugmoduledatas;
}

uint64_t sqlrconfig_xmldom::getMaxClientInfoLength() {
	return maxclientinfolength;
}

uint32_t sqlrconfig_xmldom::getMaxQuerySize() {
	return maxquerysize;
}

uint16_t sqlrconfig_xmldom::getMaxBindCount() {
	return maxbindcount;
}

uint16_t sqlrconfig_xmldom::getMaxBindNameLength() {
	return maxbindnamelength;
}

uint32_t sqlrconfig_xmldom::getMaxStringBindValueLength() {
	return maxstringbindvaluelength;
}

uint32_t sqlrconfig_xmldom::getMaxLobBindValueLength() {
	return maxlobbindvaluelength;
}

uint32_t sqlrconfig_xmldom::getMaxErrorLength() {
	return maxerrorlength;
}

int32_t sqlrconfig_xmldom::getIdleClientTimeout() {
	return idleclienttimeout;
}

int64_t sqlrconfig_xmldom::getMaxListeners() {
	return maxlisteners;
}

uint32_t sqlrconfig_xmldom::getListenerTimeout() {
	return listenertimeout;
}

bool sqlrconfig_xmldom::getReLoginAtStart() {
	return reloginatstart;
}

bool sqlrconfig_xmldom::getFakeInputBindVariables() {
	return fakeinputbindvariables;
}

const char *sqlrconfig_xmldom::getFakeInputBindVariablesDateFormat() {
	return fakeinputbindvariablesdateformat;
}

bool sqlrconfig_xmldom::getFakeInputBindVariablesUnicodeStrings() {
	return fakeinputbindvariablesunicodestrings;
}

bool sqlrconfig_xmldom::getBindVariableDelimiterQuestionMarkSupported() {
	return questionmarksupported;
}

bool sqlrconfig_xmldom::getBindVariableDelimiterColonSupported() {
	return colonsupported;
}

bool sqlrconfig_xmldom::getBindVariableDelimiterAtSignSupported() {
	return atsignsupported;
}

bool sqlrconfig_xmldom::getBindVariableDelimiterDollarSignSupported() {
	return dollarsignsupported;
}

bool sqlrconfig_xmldom::getTranslateBindVariables() {
	return translatebindvariables;
}

const char *sqlrconfig_xmldom::getIsolationLevel() {
	return isolationlevel;
}

bool sqlrconfig_xmldom::getIgnoreSelectDatabase() {
	return ignoreselectdb;
}

bool sqlrconfig_xmldom::getWaitForDownDatabase() {
	return waitfordowndb;
}

linkedlist< char * > *sqlrconfig_xmldom::getSessionStartQueries() {
	return &sessionstartqueries;
}

linkedlist< char * > *sqlrconfig_xmldom::getSessionEndQueries() {
	return &sessionendqueries;
}

domnode *sqlrconfig_xmldom::getListeners() {
	return listenersxml;
}

domnode *sqlrconfig_xmldom::getParser() {
	return parserxml;
}

domnode *sqlrconfig_xmldom::getDirectives() {
	return directivesxml;
}

domnode *sqlrconfig_xmldom::getTranslations() {
	return translationsxml;
}

domnode *sqlrconfig_xmldom::getFilters() {
	return filtersxml;
}

domnode *sqlrconfig_xmldom::getBindVariableTranslations() {
	return bindvariabletranslationsxml;
}

domnode *sqlrconfig_xmldom::getResultSetTranslations() {
	return resultsettranslationsxml;
}

domnode *sqlrconfig_xmldom::getResultSetRowTranslations() {
	return resultsetrowtranslationsxml;
}

domnode *sqlrconfig_xmldom::getResultSetRowBlockTranslations() {
	return resultsetrowblocktranslationsxml;
}

domnode *sqlrconfig_xmldom::getResultSetHeaderTranslations() {
	return resultsetheadertranslationsxml;
}

domnode *sqlrconfig_xmldom::getTriggers() {
	return triggersxml;
}

domnode *sqlrconfig_xmldom::getLoggers() {
	return loggersxml;
}

domnode *sqlrconfig_xmldom::getNotifications() {
	return notificationsxml;
}

domnode *sqlrconfig_xmldom::getSchedules() {
	return schedulesxml;
}

domnode *sqlrconfig_xmldom::getRouters() {
	return routersxml;
}

domnode *sqlrconfig_xmldom::getQueries() {
	return queriesxml;
}

domnode *sqlrconfig_xmldom::getPasswordEncryptions() {
	return pwdencsxml;
}

domnode *sqlrconfig_xmldom::getAuths() {
	return authsxml;
}

domnode *sqlrconfig_xmldom::getModuleDatas() {
	return moduledatasxml;
}

linkedlist< connectstringcontainer * > *sqlrconfig_xmldom::
						getConnectStringList() {
	return &connectstringlist;
}

connectstringcontainer *sqlrconfig_xmldom::getConnectString(
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

uint32_t sqlrconfig_xmldom::getConnectionCount() {
	return connectstringlist.getLength();
}

uint32_t sqlrconfig_xmldom::getMetricTotal() {
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

linkedlist< routecontainer * >	*sqlrconfig_xmldom::getRouteList() {
	return &routelist;
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

	debugPrintf("</%s>\n",name);

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
		debugPrintf("  not in instance tag, "
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
		debugPrintf("  not in instance tag, "
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
		if (inenabledattribute && charstring::isYes(value)) {
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

	debugPrintf("  text: %s\n",value);

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

	// bail if the text is entirely whitespace
	for (const char *c=value; *c; c++) {
		if (!character::isWhitespace(*c)) {
			return xmldom::text(value);
		}
	}
	debugPrintf("  entirely whitespce, removing\n");
	return true;
}

bool sqlrconfig_xmldom::comment(const char *value) {
	debugFunction();
	debugPrintf("  comment: %s\n",value);
	return true;
}

bool sqlrconfig_xmldom::hasDebug(const char *str, const char *debugstr) {

	const char	*end=str+charstring::length(str);
	size_t		debugstrlen=charstring::length(debugstr);

	while(str<end) {

		// see if the debug string is in the string at all
		const char	*ptr=charstring::findFirst(str,debugstr);
		if (!ptr) {
			return false;
		}

		// if we found something, then verify that
		// * it is at the beginning of the string
		//   or
		// * it is preceeded by a space
		// and
		// * it ends at the end of the string
		//   or
		// * it is followed by a space
		if ((ptr==str || *(ptr-1)==' ') &&
			(ptr+debugstrlen==end || *(ptr+debugstrlen)==' ')) {
			return true;
		}

		// if not, move on and look for it again
		str+=debugstrlen;
	}
	return false;
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

	// bail if we didn't find the instance we were looking for
	if (!foundspecifiedinstance) {
		return false;
	}

	#ifdef DEBUG_MESSAGES
		debugPrintf("\noriginal tree:\n");
		getRootNode()->write(&stdoutput);
		debugPrintf("\n");
	#endif

	// normalize the tree
	normalizeTree();

	#ifdef DEBUG_MESSAGES
		debugPrintf("normalized tree:\n");
		getRootNode()->write(&stdoutput);
		debugPrintf("\n");
	#endif
/*stdoutput.printf("normalized tree:\n");
getRootNode()->write(&stdoutput);
stdoutput.printf("\n");*/

	// get values from the tree
	getTreeValues();

	return true;
}

void sqlrconfig_xmldom::normalizeTree() {

	// prune instances for non-specified id's
	domnode *instance=getRootNode()->getFirstTagChild("instance");
	while (!instance->isNullNode()) {
		domnode	*next=instance->getNextTagSibling("instance");
		if (charstring::compare(instance->getAttributeValue("id"),id)) {
			instance->getParent()->deleteChild(instance);
		}
		instance=next;
	}

	// get the instance
	instance=getRootNode()->getFirstTagChild("instance");

	// addresses="" -> addresses="0.0.0.0"
	domnode	*attr=instance->getAttribute("addresses");
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

	// oracle8 -> oracle, sybase -> sap, mariadb -> mysql
	attr=instance->getAttribute("dbase");
	if (!attr->isNullNode()) {
		if (!charstring::compare(attr->getValue(),"oracle8")) {
			attr->setValue("oracle");
		} else if (!charstring::compare(attr->getValue(),"sybase")) {
			attr->setValue("sap");
		} else if (!charstring::compare(attr->getValue(),"mariadb")) {
			attr->setValue("mysql");
		}
	}

	// add missing listeners tag
	domnode	*listeners=instance->getFirstTagChild("listeners");
	if (listeners->isNullNode()) {
		listeners=instance->insertTag("listeners",0);
	}

	// addresses/port/socket/etc. in instance -> listener
	domnode	*addresses=instance->getAttribute("addresses");
	domnode	*port=instance->getAttribute("port");
	domnode	*socket=instance->getAttribute("socket");
	domnode	*krb=instance->getAttribute("krb");
	domnode	*krbservice=instance->getAttribute("krbservice");
	domnode	*krbkeytab=instance->getAttribute("krbkeytab");
	domnode	*krbmech=instance->getAttribute("krbmech");
	domnode	*krbflags=instance->getAttribute("krbflags");
	domnode	*tls=instance->getAttribute("tls");
	domnode	*tlsversion=instance->getAttribute("tlsversion");
	domnode	*tlscert=instance->getAttribute("tlscert");
	domnode	*tlskey=instance->getAttribute("tlskey");
	domnode	*tlspassword=instance->getAttribute("tlspassword");
	domnode	*tlsvalidate=instance->getAttribute("tlsvalidate");
	domnode	*tlsca=instance->getAttribute("tlsca");
	domnode	*tlsciphers=instance->getAttribute("tlsciphers");
	domnode	*tlsdepth=instance->getAttribute("tlsdepth");
	if (!addresses->isNullNode() ||
			!port->isNullNode() ||
			!socket->isNullNode() ||
			!krb->isNullNode() ||
			!krbservice->isNullNode() ||
			!krbkeytab->isNullNode() ||
			!krbmech->isNullNode() ||
			!krbflags->isNullNode() ||
			!tls->isNullNode() ||
			!tlsversion->isNullNode() ||
			!tlscert->isNullNode() ||
			!tlskey->isNullNode() ||
			!tlspassword->isNullNode() ||
			!tlsvalidate->isNullNode() ||
			!tlsca->isNullNode() ||
			!tlsciphers->isNullNode() ||
			!tlsdepth->isNullNode()) {

		domnode	*listener=listeners->insertTag("listener",0);
		listener->setAttributeValue("protocol",DEFAULT_PROTOCOL);

		if (!addresses->isNullNode()) {
			listener->setAttributeValue("addresses",
							addresses->getValue());
			instance->deleteAttribute(addresses);
		}
		if (!port->isNullNode()) {
			listener->setAttributeValue("port",
							port->getValue());
			instance->deleteAttribute(port);
		}
		if (!socket->isNullNode()) {
			listener->setAttributeValue("socket",
							socket->getValue());
			instance->deleteAttribute(socket);
		}
		if (!krb->isNullNode()) {
			listener->setAttributeValue("krb",
							krb->getValue());
			instance->deleteAttribute(krb);
		}
		if (!krbservice->isNullNode()) {
			listener->setAttributeValue("krbservice",
							krbservice->getValue());
			instance->deleteAttribute(krbservice);
		}
		if (!krbkeytab->isNullNode()) {
			listener->setAttributeValue("krbkeytab",
							krbkeytab->getValue());
			instance->deleteAttribute(krbkeytab);
		}
		if (!krbmech->isNullNode()) {
			listener->setAttributeValue("krbmech",
							krbmech->getValue());
			instance->deleteAttribute(krbmech);
		}
		if (!krbflags->isNullNode()) {
			listener->setAttributeValue("krbflags",
							krbflags->getValue());
			instance->deleteAttribute(krbflags);
		}
		if (!tls->isNullNode()) {
			listener->setAttributeValue("tls",
							tls->getValue());
			instance->deleteAttribute(tls);
		}
		if (!tlsversion->isNullNode()) {
			listener->setAttributeValue("tlsversion",
							tlsversion->getValue());
			instance->deleteAttribute(tlsversion);
		}
		if (!tlscert->isNullNode()) {
			listener->setAttributeValue("tlscert",
							tlscert->getValue());
			instance->deleteAttribute(tlscert);
		}
		if (!tlskey->isNullNode()) {
			listener->setAttributeValue("tlskey",
							tlskey->getValue());
			instance->deleteAttribute(tlskey);
		}
		if (!tlspassword->isNullNode()) {
			listener->setAttributeValue("tlspassword",
						tlspassword->getValue());
			instance->deleteAttribute(tlspassword);
		}
		if (!tlsvalidate->isNullNode()) {
			listener->setAttributeValue("tlsvalidate",
						tlsvalidate->getValue());
			instance->deleteAttribute(tlsvalidate);
		}
		if (!tlsca->isNullNode()) {
			listener->setAttributeValue("tlsca",
						tlsca->getValue());
			instance->deleteAttribute(tlsca);
		}
		if (!tlsciphers->isNullNode()) {
			listener->setAttributeValue("tlsciphers",
						tlsciphers->getValue());
			instance->deleteAttribute(tlsciphers);
		}
		if (!tlsdepth->isNullNode()) {
			listener->setAttributeValue("tlsdepth",
						tlsdepth->getValue());
			instance->deleteAttribute(tlsdepth);
		}
	}

	// empty listeners tag
	if (listeners->getFirstTagChild("listener")->isNullNode()) {
		domnode	*listener=listeners->appendTag("listener");
		listener->setAttributeValue("protocol",DEFAULT_PROTOCOL);
	}
	
	// normalize listeners
	for (domnode *listener=listeners->getFirstTagChild("listener");
			!listener->isNullNode();
			listener=listener->getNextTagSibling("listener")) {

		bool	hasprotocol=!listener->
				getAttribute("protocol")->isNullNode();
		bool	hasaddresses=!listener->
				getAttribute("addresses")->isNullNode();
		bool	hasport=!listener->
				getAttribute("port")->isNullNode();
		bool	hassocket=!listener->
				getAttribute("socket")->isNullNode();

		// no protocol -> default protocol
		if (!hasprotocol) {
			listener->setAttributeValue(
					"protocol",DEFAULT_PROTOCOL);
		}

		// nothing specified -> default address, default port, no socket
		if (!hasaddresses && !hasport && !hassocket) {
			listener->setAttributeValue(
					"addresses",DEFAULT_ADDRESS);
			listener->setAttributeValue(
					"port",DEFAULT_PORT);
		} else

		// port but no address -> default address
		if (!hasaddresses && hasport) {
			listener->setAttributeValue(
					"addresses",DEFAULT_ADDRESS);
		} else

		// address but no port -> default port
		if (!hasaddresses && hasport) {
			listener->setAttributeValue(
					"port",DEFAULT_PORT);
		}

		// krb but no service -> default service
		domnode	*krbservice=
				instance->getAttribute("krbservice");
		if (krbservice->isNullNode() &&
			charstring::isYes(listener->getAttributeValue("krb"))) {
			listener->setAttributeValue("krbservice",
						DEFAULT_KRBSERVICE);
		}
	}

	// authentications -> auths
	domnode	*auths=instance->getFirstTagChild("authentications");
	if (!auths->isNullNode()) {
		auths->setName("auths");
	}

	// add missing auths tag
	auths=instance->getFirstTagChild("auths");
	if (auths->isNullNode()) {
		auths=instance->insertTag("auths",1);
	}

	// authentication -> auth
	for (domnode *auth=auths->getFirstTagChild("authentication");
			!auth->isNullNode();
			auth=auth->getNextTagSibling("authentication")) {
		auth->setName("auth");
	}

	// users -> auth_userlist
	bool		addeduserlist=false;
	domnode	*users=instance->getFirstTagChild("users");
	if (!users->isNullNode()) {

		domnode	*auth=auths->insertTag("auth",0);
		auth->setAttributeValue("module","userlist");

		for (domnode *user=users->getFirstTagChild("user");
				!user->isNullNode();
				user=user->getNextTagSibling("user")) {

			domnode	*authuser=auth->appendTag("user");

			domnode	*userattr=
					user->getAttribute("user");
			if (!userattr->isNullNode()) {
				authuser->setAttributeValue(
						"user",
						userattr->getValue());
			}

			domnode	*passwordattr=
					user->getAttribute("password");
			if (!passwordattr->isNullNode()) {
				authuser->setAttributeValue(
						"password",
						passwordattr->getValue());
			}

			// passwordencryption -> passwordencryptionid
			domnode	*pwdencidattr=
					user->getAttribute(
						"passwordencryptionid");
			if (pwdencidattr->isNullNode()) {
				pwdencidattr=
					user->getAttribute(
						"passwordencryption");
			}
			if (!pwdencidattr->isNullNode()) {
				authuser->setAttributeValue(
						"passwordencryptionid",
						pwdencidattr->getValue());
			}
		}

		users->getParent()->deleteChild(users);

		addeduserlist=true;
	}

	// authtier="database" -> auth_database
	// authtier="proxied" -> auth_proxied
	attr=instance->getAttribute("authtier");
	if (!attr->isNullNode()) {
		if (!charstring::compare(attr->getValue(),"database") ||
			!charstring::compare(attr->getValue(),"proxied")) {

			domnode	*auth=(addeduserlist)?
					auths->insertTag("auth",1):
					auths->insertTag("auth",0);

			auth->setAttributeValue("module",attr->getValue());

			instance->deleteAttribute(attr);
		}
	}

	// krb_userlist/tls_userlist -> userlist
	for (domnode *auth=instance->getFirstTagChild("auths")->
						getFirstTagChild("auth");
				!auth->isNullNode();
				auth=auth->getNextTagSibling("auth")) {
		if (!charstring::compare(
				auth->getAttributeValue("module"),
				"krb_userlist") ||
			!charstring::compare(
				auth->getAttributeValue("module"),
				"tls_userlist")) {
			auth->setAttributeValue("module","userlist");
		}
	}

	// normalize connections
	uint32_t	connectioncount=0;
	domnode	*conns=instance->getFirstTagChild("connections");
	for (domnode *conn=conns->getFirstTagChild("connection");
			!conn->isNullNode();
			conn=conn->getNextTagSibling("connection")) {

		// add missing connection id
		const char	*connid=conn->getAttributeValue("connectionid");
		if (charstring::isNullOrEmpty(connid)) {
			stringbuffer	connectionid;
			connectionid.append(id)->append('-');
			connectionid.append(connectioncount);
			conn->setAttributeValue("connectionid",
						connectionid.getString());
			connectioncount++;
		}

		// passwordencryption -> passwordencryptionid
		domnode	*pwdencattr=
				conn->getAttribute("passwordencryption");
		if (!pwdencattr->isNullNode()) {
			pwdencattr->setName("passwordencryptionid");
		}
	}

	// datetimeformat -> resultsettranslation_reformatdatetime
	domnode	*dtformat=instance->getAttribute("datetimeformat");
	domnode	*dateformat=instance->getAttribute("dateformat");
	domnode	*timeformat=instance->getAttribute("timeformat");
	domnode	*dateddmm=instance->getAttribute("dateddmm");
	domnode	*dateyyyyddmm=instance->getAttribute("dateyyyyddmm");
	domnode	*datedelims=instance->getAttribute("datedelimiters");
	if (!dtformat->isNullNode() ||
		!dateformat->isNullNode() ||
		!timeformat->isNullNode() ||
		!dateddmm->isNullNode() ||
		!dateyyyyddmm->isNullNode() ||
		!datedelims->isNullNode()) {

		// get/add resultsettranslations tag
		domnode	*rstrans=
			instance->getFirstTagChild("resultsettranslations");
		if (rstrans->isNullNode()) {
			rstrans=instance->appendTag("resultsettranslations");
		}

		// add resultsettranslation tag
		domnode	*rst=
			rstrans->insertTag("resultsettranslation",0);
		rst->setAttributeValue("module","reformatdatetime");

		if (!dtformat->isNullNode()) {
			rst->setAttributeValue("datetimeformat",
						dtformat->getValue());
			instance->deleteAttribute(dtformat);
		}
		if (!dateformat->isNullNode()) {
			rst->setAttributeValue("dateformat",
						dateformat->getValue());
			instance->deleteAttribute(dateformat);
		}
		if (!timeformat->isNullNode()) {
			rst->setAttributeValue("timeformat",
						timeformat->getValue());
			instance->deleteAttribute(timeformat);
		}
		if (!dateddmm->isNullNode()) {
			rst->setAttributeValue("dateddmm",
						dateddmm->getValue());
			instance->deleteAttribute(dateddmm);
		}
		if (!dateyyyyddmm->isNullNode()) {
			rst->setAttributeValue("dateyyyyddmm",
						dateyyyyddmm->getValue());
			instance->deleteAttribute(dateyyyyddmm);
		}
		if (!datedelims->isNullNode()) {
			rst->setAttributeValue("datedelims",
						datedelims->getValue());
			instance->deleteAttribute(datedelims);
		}
	}

	// old router format to new router format
	domnode	*router=instance->getFirstTagChild("router");
	if (!router->isNullNode()) {

		// add a routers node if none exists
		domnode	*rtrs=instance->getFirstTagChild("routers");
		if (rtrs->isNullNode()) {
			rtrs=instance->appendTag("routers");
		}

		// add a connections node if none exists
		domnode	*cons=instance->getFirstTagChild("connections");
		if (cons->isNullNode()) {
			cons=instance->appendTag("connections");
		}

		// for each router.route node...
		uint16_t	index=0;
		stringbuffer	conid;
		stringbuffer	str;
		for (domnode *route=router->getFirstTagChild("route");
				!route->isNullNode();
				route=route->getNextTagSibling("route")) {

			// generate a connection id
			conid.append("autogenerated-route-")->append(index);
			index++;

			// build the connect string
			const char	*host=
					route->getAttributeValue("host");
			if (!charstring::isNullOrEmpty(host)) {
				str.append("host=");
				str.append(host)->append(';');
			}
			const char	*port=
					route->getAttributeValue("port");
			if (!charstring::isNullOrEmpty(port)) {
				str.append("port=");
				str.append(port)->append(';');
			}
			const char	*socket=
					route->getAttributeValue("socket");
			if (!charstring::isNullOrEmpty(socket)) {
				str.append("socket=");
				str.append(socket)->append(';');
			}
			const char	*user=
					route->getAttributeValue("user");
			if (!charstring::isNullOrEmpty(user)) {
				str.append("user=");
				str.append(user)->append(';');
			}
			const char	*password=
					route->getAttributeValue("password");
			if (!charstring::isNullOrEmpty(password)) {
				str.append("password=");
				str.append(password)->append(';');
			}

			// add a connections.connection node
			domnode	*con=cons->appendTag("connection");
			con->setAttributeValue("connectionid",
						conid.getString());
			con->setAttributeValue("string",
						str.getString());

			// add a routers.router node
			domnode	*rtr=rtrs->appendTag("router");
			rtr->setAttributeValue("module","regex");
			rtr->setAttributeValue("connectionid",
						conid.getString());

			// for each query...
			for (domnode *query=route->getFirstTagChild("query");
				!query->isNullNode();
				query=query->getNextTagSibling("query")) {

				// create a routers.router.pattern tag
				rtr->appendTag("pattern")->
					setAttributeValue("pattern",
						query->getAttributeValue(
								"pattern"));
			}

			// clear buffers
			conid.clear();
			str.clear();
		}

		// delete the old router tag
		router->getParent()->deleteChild(router);
	}
}

void sqlrconfig_xmldom::getTreeValues() {

	// instance tag...
	domnode	*instance=getRootNode()->getFirstTagChild("instance");
	domnode	*attr=instance->getAttribute("dbase");
	if (!attr->isNullNode()) {
		dbase=attr->getValue();
	}
	attr=instance->getAttribute("connections");
	if (!attr->isNullNode()) {
		connections=atouint32_t(attr->getValue(),
						DEFAULT_CONNECTIONS,0);
		if (connections>MAXCONNECTIONS) {
			connections=MAXCONNECTIONS;
		}
		if (maxconnections<connections) {
			maxconnections=connections;
		}
	}
	attr=instance->getAttribute("maxconnections");
	if (!attr->isNullNode()) {
		maxconnections=atouint32_t(attr->getValue(),
						DEFAULT_CONNECTIONS,1);
		if (maxconnections>MAXCONNECTIONS) {
			maxconnections=MAXCONNECTIONS;
		}
		if (maxconnections<connections) {
			maxconnections=connections;
		}
	}
	attr=instance->getAttribute("maxqueuelength");
	if (!attr->isNullNode()) {
		maxqueuelength=atouint32_t(attr->getValue(),
						DEFAULT_MAXQUEUELENGTH,0);
	}
	attr=instance->getAttribute("growby");
	if (!attr->isNullNode()) {
		growby=atouint32_t(attr->getValue(),DEFAULT_GROWBY,1);
	}
	attr=instance->getAttribute("ttl");
	if (!attr->isNullNode()) {
		ttl=atoint32_t(attr->getValue(),DEFAULT_TTL,0);
	}
	attr=instance->getAttribute("softttl");
	if (!attr->isNullNode()) {
		softttl=atoint32_t(attr->getValue(),DEFAULT_SOFTTTL,0);
	}
	attr=instance->getAttribute("maxsessioncount");
	if (!attr->isNullNode()) {
		maxsessioncount=atouint32_t(attr->getValue(),
						DEFAULT_MAXSESSIONCOUNT,0);
	}
	attr=instance->getAttribute("endofsession");
	if (!attr->isNullNode()) {
		endofsession=attr->getValue();
		endofsessioncommit=!charstring::compare(endofsession,"commit");
	}
	attr=instance->getAttribute("sessiontimeout");
	if (!attr->isNullNode()) {
		sessiontimeout=atouint32_t(attr->getValue(),
						DEFAULT_SESSIONTIMEOUT,1);
	}
	attr=instance->getAttribute("runasuser");
	if (!attr->isNullNode()) {
		runasuser=attr->getValue();
	}
	attr=instance->getAttribute("runasgroup");
	if (!attr->isNullNode()) {
		runasgroup=attr->getValue();
	}
	attr=instance->getAttribute("cursors");
	if (!attr->isNullNode()) {
		cursors=atouint32_t(attr->getValue(),DEFAULT_CURSORS,0);
		if (maxcursors<cursors) {
			maxcursors=cursors;
		}
	}
	attr=instance->getAttribute("maxcursors");
	if (!attr->isNullNode()) {
		maxcursors=atouint32_t(attr->getValue(),DEFAULT_CURSORS,0);
		if (maxcursors<cursors) {
			maxcursors=cursors;
		}
	}
	attr=instance->getAttribute("cursors_growby");
	if (!attr->isNullNode()) {
		cursorsgrowby=atouint32_t(attr->getValue(),
						DEFAULT_CURSORS_GROWBY,1);
	}
	attr=instance->getAttribute("authtier");
	if (!attr->isNullNode()) {
		authtier=attr->getValue();
	}
	attr=instance->getAttribute("sessionhandler");
	if (!attr->isNullNode()) {
		sessionhandler=attr->getValue();
	}
	attr=instance->getAttribute("handoff");
	if (!attr->isNullNode()) {
		handoff=attr->getValue();
	}
	attr=instance->getAttribute("allowedips");
	if (!attr->isNullNode()) {
		allowedips=attr->getValue();
	}
	attr=instance->getAttribute("deniedips");
	if (!attr->isNullNode()) {
		deniedips=attr->getValue();
	}
	attr=instance->getAttribute("debug");
	if (!attr->isNullNode()) {
		debug=attr->getValue();
		debugsql=hasDebug(debug,"sql");
		debugbulkload=hasDebug(debug,"bulkload");
		debugparser=hasDebug(debug,"parser");
		debugdirectives=hasDebug(debug,"directives");
		debugtranslations=hasDebug(debug,"translations");
		debugfilters=hasDebug(debug,"filters");
		debugtriggers=hasDebug(debug,"triggers");
		debugbindtranslations=hasDebug(debug,"bindtranslations");
		debugbindvariabletranslations=
			hasDebug(debug,"bindvariabletranslations");
		debugresultsettranslations=
			hasDebug(debug,"resultsettranslations");
		debugresultsetrowtranslations=
			hasDebug(debug,"resultsetrowtranslations");
		debugresultsetrowblocktranslations=
			hasDebug(debug,"resultsetrowblocktranslations");
		debugresultsetheadertranslations=
			hasDebug(debug,"resultsetheadertranslations");
		debugprotocols=hasDebug(debug,"protocols");
		debugauths=hasDebug(debug,"auths");
		debugpwdencs=hasDebug(debug,"passwordencryptions");
		debugloggers=hasDebug(debug,"loggers");
		debugnotifications=hasDebug(debug,"notifications");
		debugschedules=hasDebug(debug,"schedules");
		debugrouters=hasDebug(debug,"routers");
		debugqueries=hasDebug(debug,"queries");
		debugmoduledatas=hasDebug(debug,"moduledatas");
	}
	attr=instance->getAttribute("maxclientinfolength");
	if (!attr->isNullNode()) {
		maxclientinfolength=charstring::toInteger(attr->getValue());
	}
	attr=instance->getAttribute("maxquerysize");
	if (!attr->isNullNode()) {
		maxquerysize=charstring::toInteger(attr->getValue());
	}
	attr=instance->getAttribute("maxbindcount");
	if (!attr->isNullNode()) {
		maxbindcount=charstring::toInteger(attr->getValue());
	}
	attr=instance->getAttribute("maxbindnamelength");
	if (!attr->isNullNode()) {
		maxbindnamelength=charstring::toInteger(attr->getValue());
	}
	attr=instance->getAttribute("maxstringbindvaluelength");
	if (!attr->isNullNode()) {
		maxstringbindvaluelength=
				charstring::toInteger(attr->getValue());
	}
	attr=instance->getAttribute("maxlobbindvaluelength");
	if (!attr->isNullNode()) {
		maxlobbindvaluelength=charstring::toInteger(attr->getValue());
	}
	attr=instance->getAttribute("maxerrorlength");
	if (!attr->isNullNode()) {
		maxerrorlength=charstring::toInteger(attr->getValue());
	}
	attr=instance->getAttribute("idleclienttimeout");
	if (!attr->isNullNode()) {
		idleclienttimeout=charstring::toInteger(attr->getValue());
	}
	attr=instance->getAttribute("maxlisteners");
	if (!attr->isNullNode()) {
		maxlisteners=charstring::toInteger(attr->getValue());
	}
	attr=instance->getAttribute("listenertimeout");
	if (!attr->isNullNode()) {
		listenertimeout=charstring::toUnsignedInteger(attr->getValue());
	}
	attr=instance->getAttribute("reloginatstart");
	if (!attr->isNullNode()) {
		reloginatstart=charstring::isYes(attr->getValue());
	}
	attr=instance->getAttribute("fakeinputbindvariables");
	if (!attr->isNullNode()) {
		fakeinputbindvariables=charstring::isYes(attr->getValue());
	}
	attr=instance->getAttribute("fakeinputbindvariablesdateformat");
	if (!attr->isNullNode()) {
		fakeinputbindvariablesdateformat=attr->getValue();
	}
	attr=instance->getAttribute("fakeinputbindvariablesunicodestrings");
	if (!attr->isNullNode()) {
		fakeinputbindvariablesunicodestrings=
					charstring::isYes(attr->getValue());
	}
	attr=instance->getAttribute("bindvariabledelimiters");
	if (!attr->isNullNode()) {
		bindvariabledelimiters=attr->getValue();
		questionmarksupported=charstring::contains(
						bindvariabledelimiters,'?');
		colonsupported=charstring::contains(
						bindvariabledelimiters,':');
		atsignsupported=charstring::contains(
						bindvariabledelimiters,'@');
		dollarsignsupported=charstring::contains(
						bindvariabledelimiters,'$');
	}
	attr=instance->getAttribute("translatebindvariables");
	if (!attr->isNullNode()) {
		translatebindvariables=charstring::isYes(attr->getValue());
	}
	attr=instance->getAttribute("isolationlevel");
	if (!attr->isNullNode()) {
		isolationlevel=attr->getValue();
	}
	attr=instance->getAttribute("ignoreselectdatabase");
	if (!attr->isNullNode()) {
		ignoreselectdb=charstring::isYes(attr->getValue());
	}
	attr=instance->getAttribute("waitfordowndatabase");
	if (!attr->isNullNode()) {
		waitfordowndb=charstring::isYes(attr->getValue());
	}


	// xmls...
	listenersxml=instance->getFirstTagChild("listeners");
	parserxml=instance->getFirstTagChild("parser");
	directivesxml=instance->getFirstTagChild("directives");
	translationsxml=instance->getFirstTagChild("translations");
	filtersxml=instance->getFirstTagChild("filters");
	bindvariabletranslationsxml=instance->getFirstTagChild(
					"bindvariabletranslations");
	resultsettranslationsxml=instance->getFirstTagChild(
					"resultsettranslations");
	resultsetrowtranslationsxml=instance->getFirstTagChild(
					"resultsetrowtranslations");
	resultsetrowblocktranslationsxml=instance->getFirstTagChild(
					"resultsetrowblocktranslations");
	resultsetheadertranslationsxml=instance->getFirstTagChild(
					"resultsetheadertranslations");
	triggersxml=instance->getFirstTagChild("triggers");
	loggersxml=instance->getFirstTagChild("loggers");
	notificationsxml=instance->getFirstTagChild("notifications");
	schedulesxml=instance->getFirstTagChild("schedules");
	routersxml=instance->getFirstTagChild("routers");
	queriesxml=instance->getFirstTagChild("queries");
	pwdencsxml=instance->getFirstTagChild("passwordencryptions");
	authsxml=instance->getFirstTagChild("auths");
	moduledatasxml=instance->getFirstTagChild("moduledatas");


	// listeners tag...
	defaultlistener=NULL;
	for (domnode *node=listenersxml->getFirstTagChild("listener");
				!node->isNullNode();
				node=node->getNextTagSibling("listener")) {

		// get the default listener...
		// use the first listener for the default protocol,
		if (!defaultlistener &&
			!charstring::compare(
					node->getAttributeValue("protocol"),
					DEFAULT_PROTOCOL)) {
			defaultlistener=node;
		}

		// listen on inet/unix...
		if (!node->getAttribute("port")->isNullNode()) {
			listenoninet=true;
		}
		if (!node->getAttribute("socket")->isNullNode()) {
			listenonunix=true;
		}
	}

	// just use the first listener if no default listener was found yet
	if (!defaultlistener) {
		defaultlistener=listenersxml->getFirstTagChild("listener");
	}

	// default listener parameters
	defaultaddresses=defaultlistener->getAttributeValue("addresses");
	defaultport=charstring::toUnsignedInteger(
			defaultlistener->getAttributeValue("port"));
	defaultsocket=defaultlistener->getAttributeValue("socket");
	defaultkrb=charstring::isYes(defaultlistener->getAttributeValue("krb"));
	defaultkrbkeytab=defaultlistener->getAttributeValue("krbkeytab");
	defaultkrbservice=defaultlistener->getAttributeValue("krbservice");
	defaultkrbmech=defaultlistener->getAttributeValue("krbmech");
	defaultkrbflags=defaultlistener->getAttributeValue("krbflags");
	defaulttls=charstring::isYes(defaultlistener->getAttributeValue("tls"));
	defaulttlsciphers=defaultlistener->getAttributeValue("tlsciphers");


	// session queries
	domnode	*session=instance->getFirstTagChild("session");
	domnode	*start=session->getFirstTagChild("start");
	for (domnode *runquery=start->getFirstTagChild("runquery");
			!runquery->isNullNode();
			runquery=runquery->getNextTagSibling("runquery")) {
		sessionstartqueries.append(charstring::duplicate(
				runquery->getFirstChild("text")->getValue()));
	}
	domnode	*end=session->getFirstTagChild("end");
	for (domnode *runquery=end->getFirstTagChild("runquery");
			!runquery->isNullNode();
			runquery=runquery->getNextTagSibling("runquery")) {
		sessionendqueries.append(charstring::duplicate(
				runquery->getFirstChild("text")->getValue()));
	}


	// connect string list
	for (domnode *connection=instance->
					getFirstTagChild("connections")->
					getFirstTagChild("connection");
			!connection->isNullNode();
			connection=connection->
					getNextTagSibling("connection")) {
		
		// add an item to the connect string list
		connectstringcontainer	*c=new connectstringcontainer();
		const char	*connectionid=connection->
				getAttributeValue("connectionid");
		const char	*str=connection->
				getAttributeValue("string");
		const char	*metric=connection->
				getAttributeValue("metric");
		const char	*blb=connection->
				getAttributeValue("behindloadbalancer");
		const char	*pwdencid=connection->
				getAttributeValue("passwordencryptionid");
		c->setConnectionId(connectionid);
		c->setString((str)?str:DEFAULT_CONNECTSTRING);
		c->parseConnectString();
		c->setMetric(atouint32_t(metric,DEFAULT_METRIC,1));
		c->setBehindLoadBalancer(charstring::isYes(blb));
		c->setPasswordEncryption(pwdencid);
		connectstringlist.append(c);
	}


	// route list
	uint32_t	routecount=0;
	for (domnode *route=instance->
				getFirstTagChild("router")->
				getFirstTagChild("route");
			!route->isNullNode();
			route=route->getNextTagSibling("route")) {
		
		// add an item to the route list
		routecontainer	*r=new routecontainer();
		r->setIsFilter(false);
		r->setHost(route->getAttributeValue("host"));
		r->setPort(atouint32_t(
				route->getAttributeValue("port"),"0",0));
		r->setSocket(route->getAttributeValue("socket"));
		r->setUser(route->getAttributeValue("user"));
		r->setPassword(route->getAttributeValue("password"));

		for (domnode *query=route->getFirstTagChild("query");
				!query->isNullNode();
				query=query->getNextTagSibling("query")) {
			const char	*pattern=
					query->getAttributeValue("pattern");
			regularexpression	*re=
				new regularexpression(
					(pattern)?pattern:
						DEFAULT_ROUTER_PATTERN);
			re->study();
			r->getRegexList()->append(re);
		}

		routecontainer	*er=routeAlreadyExists(r);
		if (er) {
			moveRegexList(r,er);
			delete r;
		} else {
			routelist.append(r);
		}

		// add an item to the connect string list
		connectstringcontainer	*c=new connectstringcontainer();
		stringbuffer	connectionid;
		connectionid.append(id)->append('-');
		connectionid.append(routecount++);
		c->setConnectionId(connectionid.getString());
		connectstringlist.append(c);
	}

	// default user/password
	domnode	*defaultusertag=instance->getFirstTagChild("auths")->							getFirstTagChild(
						"auth","module","userlist")->
						getFirstTagChild("user");
	defaultuser=defaultusertag->getAttributeValue("user");
	defaultpassword=defaultusertag->getAttributeValue("password");
}

routecontainer *sqlrconfig_xmldom::routeAlreadyExists(routecontainer *cur) {

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

void sqlrconfig_xmldom::moveRegexList(routecontainer *cur,
					routecontainer *existing) {

	for (linkedlistnode< regularexpression * > *re=
				cur->getRegexList()->getFirst();
						re; re=re->getNext()) {
		existing->getRegexList()->append(re->getValue());
	}
	cur->getRegexList()->clear();
}

uint32_t sqlrconfig_xmldom::atouint32_t(const char *value,
				const char *defaultvalue, uint32_t minvalue) {
	uint32_t	retval=charstring::toUnsignedInteger(
						(value)?value:defaultvalue);
	if (retval<minvalue) {
		retval=charstring::toUnsignedInteger(defaultvalue);
	}
	return retval;
}

int32_t sqlrconfig_xmldom::atoint32_t(const char *value,
				const char *defaultvalue, int32_t minvalue) {
	int32_t	retval=charstring::toInteger((value)?value:defaultvalue);
	if (retval<minvalue) {
		retval=charstring::toInteger(defaultvalue);
	}
	return retval;
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
	char		*osname=sys::getOperatingSystemName();
	const char	*slash=(!charstring::compareIgnoringCase(
					osname,"Windows"))?"\\":"/";
	delete[] osname;

	if (!done && d.open(dir)) {
		while (!done) {
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

				parseFile(fullpath.getString());
			}
			delete[] filename;
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
		if (fs.open(urlname)) {
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

	#ifdef DEBUG_MESSAGES
		debugPrintf("enabled ids:\n");
		for (linkedlistnode< char * > *n=idlist->getFirst();
						n; n=n->getNext()) {
			debugPrintf("  %s\n",n->getValue());
		}
	#endif
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
