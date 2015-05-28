// Copyright (c) 1999-2014 David Muse
// See the file COPYING for more information

#ifndef SQLRUTIL_H
#define SQLRUTIL_H

#include <rudiments/xmlsax.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/parameterstring.h>
#include <rudiments/regularexpression.h>
#include <rudiments/commandline.h>
#include <rudiments/linkedlist.h>

#include <sqlrelay/private/sqlrutildll.h>

class SQLRUTIL_DLLSPEC sqlrcmdline : public commandline {
	friend class sqlrpaths;
	public:
			sqlrcmdline(int argc, const char **argv);

		const char	*getId() const;
	private:
		const char	*getConfig() const;
		const char	*getLocalStateDir() const;

		const char	*id;
		const char	*config;
		const char	*localstatedir;
};

class SQLRUTIL_DLLSPEC sqlrpaths {
	public:
				sqlrpaths(sqlrcmdline *cmdl);
				~sqlrpaths();
		const char	*getLocalStateDir();
		const char	*getTmpDir();
		uint32_t	getTmpDirLength();
		const char	*getLogDir();
		const char	*getDebugDir();
		const char	*getCacheDir();
		const char	*getDefaultConfigFile();
		const char	*getDefaultConfigDir();
		const char	*getConfigFile();
	protected:
		const char	*localstatedir;
		char		*tmpdir;
		uint32_t	tmpdirlen;
		char		*logdir;
		char		*debugdir;
		char		*cachedir;
		char		*defaultconfigfile;
		char		*defaultconfigdir;
		const char	*configfile;
};

class SQLRUTIL_DLLSPEC listenercontainer {
	friend class sqlrconfigfile;
	public:
				listenercontainer();
				~listenercontainer();

		void		setAddresses(char **addresses,
						uint64_t addresscount);
		void		setPort(uint16_t port);
		void		setSocket(const char *socket);
		void		setProtocol(const char *protocol);
		const char * const *getAddresses();
		uint64_t	getAddressCount();
		uint16_t	getPort();
		const char	*getSocket();
		const char	*getProtocol();
	private:
		char		**addresses;
		uint64_t	addresscount;
		uint16_t	port;
		char		*socket;
		char		*protocol;
};

typedef linkedlistnode< listenercontainer * >	listenernode;

class SQLRUTIL_DLLSPEC usercontainer {
	friend class sqlrconfigfile;
	public:
				usercontainer();
				~usercontainer();

		void		setUser(const char *user);
		void		setPassword(const char *password);
		void		setPasswordEncryption(const char *pwdenc);
		const char	*getUser();
		const char	*getPassword();
		const char	*getPasswordEncryption();
	private:
		char	*user;
		char	*password;
		char	*pwdenc;
};

typedef linkedlistnode< usercontainer * >	usernode;

class SQLRUTIL_DLLSPEC connectstringcontainer {
	public:
				connectstringcontainer();
				~connectstringcontainer();
		void		parseConnectString();
		void		setConnectionId(const char *connectionid);
		void		setString(const char *string);
		void		setMetric(uint32_t metric);
		void		setBehindLoadBalancer(bool behindloadbalancer);
		void		setPasswordEncryption(const char *pwdenc);
		const char	*getConnectionId();
		const char	*getString();
		uint32_t	getMetric();
		bool		getBehindLoadBalancer();
		const char	*getConnectStringValue(const char *variable);
		const char	*getPasswordEncryption();
	private:

		char		*connectionid;
		char		*string;
		uint32_t	metric;
		bool		behindloadbalancer;
		char		*pwdenc;

		parameterstring	connectstring;
};

typedef linkedlistnode< connectstringcontainer * >	connectstringnode;

class SQLRUTIL_DLLSPEC routecontainer {
	friend class sqlrconfigfile;
	public:
			routecontainer();
			~routecontainer();

		void	setIsFilter(bool isfilter);
		void	setHost(const char *host);
		void	setPort(uint16_t port);
		void	setSocket(const char *socket);
		void	setUser(const char *user);
		void	setPassword(const char *password);

		bool		getIsFilter();
		const char	*getHost();
		uint16_t	getPort();
		const char	*getSocket();
		const char	*getUser();
		const char	*getPassword();
		linkedlist< regularexpression * >	*getRegexList();
	private:
		bool		isfilter;
		char		*host;
		uint16_t	port;
		char		*socket;
		char		*user;
		char		*password;
		linkedlist< regularexpression * >	regexlist;
};

typedef linkedlistnode< routecontainer * >	routenode;

class SQLRUTIL_DLLSPEC sqlrconfigfile : public xmlsax {
	public:
			sqlrconfigfile(sqlrpaths *sqlrpth);
			~sqlrconfigfile();
		void	getEnabledIds(const char *config,
					linkedlist< char * > *idlist);
		bool	parse(const char *config, const char *id);
		bool	accessible();
		const char * const	*getDefaultAddresses();
		uint64_t		getDefaultAddressCount();
		uint16_t		getDefaultPort();
		const char		*getDefaultSocket();
		bool		getListenOnInet();
		bool		getListenOnUnix();
		const char	*getDbase();
		uint32_t	getConnections();
		uint32_t	getMaxConnections();
		uint32_t	getMaxQueueLength();
		uint32_t	getGrowBy();
		int32_t		getTtl();
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
		bool		getDebugTranslations();
		bool		getDebugTriggers();
		bool		getDebugBindTranslations();
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

		linkedlist< char *>	*getSessionStartQueries();
		linkedlist< char *>	*getSessionEndQueries();

		const char	*getTranslations();

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
		sqlrpaths		*sqlrpth;

		bool			getenabledids;
		char			*currentid;
		bool			enabled;
		linkedlist< char * >	*idlist;

		const char	*id;
		bool		correctid;
		bool		done;

		void	init();
		void	clear();

		uint32_t	atouint32_t(const char *value,
					const char *defaultvalue,
					uint32_t minvalue);
		int32_t		atoint32_t(const char *value,
					const char *defaultvalue,
					int32_t minvalue);

		bool	tagStart(const char *name);
		bool	attributeName(const char *name);
		bool	attributeValue(const char *value);
		bool	text(const char *string);
		bool	tagEnd(const char *name);

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
		bool		debugtranslations;
		bool		debugtriggers;
		bool		debugbindtranslations;
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

		bool		instart;
		bool		inend;
		linkedlist< char *>	sessionstartqueries;
		linkedlist< char *>	sessionendqueries;

		stringbuffer	authentications;
		uint16_t	authenticationsdepth;

		stringbuffer	translations;
		uint16_t	translationsdepth;

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
			PASSWORDENCRYPTION_ATTRIBUTE,
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
			ENABLED_ATTRIBUTE,
		} attribute;

		attribute	currentattribute;
};

#endif
