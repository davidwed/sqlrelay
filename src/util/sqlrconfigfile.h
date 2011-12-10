// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <config.h>

#include <rudiments/xmlsax.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/linkedlist.h>
#include <rudiments/parameterstring.h>
#include <rudiments/regularexpression.h>

#define MAX_ADDRESSES	32

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

typedef enum {
	NO_ATTRIBUTE,
	ID_ATTRIBUTE,
	ADDRESSES_ATTRIBUTE,
	PORT_ATTRIBUTE,
	SOCKET_ATTRIBUTE,
	MYSQLADDRESSES_ATTRIBUTE,
	MYSQLPORT_ATTRIBUTE,
	MYSQLSOCKET_ATTRIBUTE,
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
	HANDOFF_ATTRIBUTE,
	DENIEDIPS_ATTRIBUTE,
	ALLOWEDIPS_ATTRIBUTE,
	DEBUG_ATTRIBUTE,
	MAXQUERYSIZE_ATTRIBUTE,
	MAXSTRINGBINDVALUELENGTH_ATTRIBUTE,
	MAXLOBBINDVALUELENGTH_ATTRIBUTE,
	IDLECLIENTTIMEOUT_ATTRIBUTE,
	USER_ATTRIBUTE,
	PASSWORD_ATTRIBUTE,
	CONNECTIONID_ATTRIBUTE,
	STRING_ATTRIBUTE,
	METRIC_ATTRIBUTE,
	BEHINDLOADBALANCER_ATTRIBUTE,
	SID_ENABLED_ATTRIBUTE,
	SID_HOST_ATTRIBUTE,
	SID_PORT_ATTRIBUTE,
	SID_SOCKET_ATTRIBUTE,
	SID_USER_ATTRIBUTE,
	SID_PASSWORD_ATTRIBUTE,
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
	SQLTRANSLATIONRULES_ATTRIBUTE
} attribute;

class usercontainer {
	friend class sqlrconfigfile;
	public:
				usercontainer();
				~usercontainer();

		void		setUser(const char *user);
		void		setPassword(const char *password);
		const char	*getUser();
		const char	*getPassword();
	private:
		const char	*user;
		const char	*password;
};

typedef linkedlistnode< usercontainer * >	usernode;

class connectstringcontainer {
	public:
				connectstringcontainer(
						uint16_t connectstringcount);
				~connectstringcontainer();
		void		parseConnectString();
		void		setConnectionId(const char *connectionid);
		void		setString(const char *string);
		void		setMetric(uint32_t metric);
		void		setBehindLoadBalancer(bool behindloadbalancer);
		const char	*getConnectionId();
		const char	*getString();
		uint32_t	getMetric();
		bool		getBehindLoadBalancer();
		const char	*getConnectStringValue(const char *variable);
	private:

		const char	*connectionid;
		const char	*string;
		uint32_t	metric;
		bool		behindloadbalancer;

		// connect string parameters
		parameterstring	connectstring;
		uint16_t	connectstringcount;
};

typedef linkedlistnode< connectstringcontainer * >	connectstringnode;

class routecontainer {
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
		bool			isfilter;
		char			*host;
		uint16_t		port;
		char			*socket;
		char			*user;
		char			*password;
		linkedlist< regularexpression * >	regexlist;
};

typedef linkedlistnode< routecontainer * >	routenode;

class sqlrconfigfile : public xmlsax {
	public:
			sqlrconfigfile();
			~sqlrconfigfile();
		bool	parse(const char *config, const char *id);
		bool	parse(const char *config, const char *id,
						uint16_t connectstringcount);
		const char * const *getAddresses();
		uint64_t	getAddressCount();
		uint16_t	getPort();
		const char	*getUnixPort();
		bool		getListenOnInet();
		bool		getListenOnUnix();
		const char * const *getMySQLAddresses();
		uint64_t	getMySQLAddressCount();
		uint16_t	getMySQLPort();
		const char	*getMySQLUnixPort();
		bool		getMySQLListenOnInet();
		bool		getMySQLListenOnUnix();
		const char	*getDbase();
		uint32_t	getConnections();
		uint32_t	getMaxConnections();
		uint32_t	getMaxQueueLength();
		uint32_t	getGrowBy();
		uint32_t	getTtl();
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
		bool		getAuthOnListener();
		bool		getAuthOnConnection();
		bool		getAuthOnDatabase();
		const char	*getHandOff();
		bool		getPassDescriptor();
		const char	*getAllowedIps();
		const char	*getDeniedIps();
		const char	*getDebug();
		bool		getDebugListener();
		bool		getDebugConnection();
		uint32_t	getMaxQuerySize();
		uint32_t	getMaxStringBindValueLength();
		uint32_t	getMaxLobBindValueLength();
		int32_t		getIdleClientTimeout();
		int64_t		getMaxListeners();
		uint32_t	getListenerTimeout();
		bool		getReLoginAtStart();
		int64_t		getTimeQueriesSeconds();
		int64_t		getTimeQueriesMicroSeconds();

		bool		getSidEnabled();
		const char	*getSidHost();
		uint16_t	getSidPort();
		const char	*getSidUnixPort();
		const char	*getSidUser();
		const char	*getSidPassword();

		const char	*getSqlTranslationRules();

		linkedlist< usercontainer * >	*getUserList();
		linkedlist< connectstringcontainer * >
					*getConnectStringList();
		connectstringcontainer	*getConnectString(
						const char *connectionid);
		uint32_t		getConnectionCount();
		uint32_t		getMetricTotal();

		linkedlist< routecontainer * >	*getRouteList();
	private:
		const char	*id;
		bool		correctid;
		bool		done;
		attribute	currentattribute;

		uint32_t	atouint32_t(const char *value,
					const char *defaultvalue,
					uint32_t minvalue);

		bool	tagStart(const char *name);
		bool	attributeName(const char *name);
		bool	attributeValue(const char *value);
		bool	tagEnd(const char *name);

		routecontainer	*routeAlreadyExists(routecontainer *cur);
		void		moveRegexList(routecontainer *cur,
						routecontainer *existing);

		char		**addresses;
		uint64_t	addresscount;
		uint16_t	port;
		const char	*unixport;
		bool		listenoninet;
		bool		listenonunix;
		char		**mysqladdresses;
		uint64_t	mysqladdresscount;
		uint16_t	mysqlport;
		const char	*mysqlunixport;
		bool		mysqllistenoninet;
		bool		mysqllistenonunix;
		const char	*dbase;
		uint32_t	connections;
		uint32_t	maxconnections;
		uint32_t	maxqueuelength;
		uint32_t	growby;
		uint32_t	ttl;
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
		bool		authonlistener;
		bool		authonconnection;
		bool		authondatabase;
		const char	*handoff;
		bool		passdescriptor;
		const char	*allowedips;
		const char	*deniedips;
		const char	*debug;
		bool		debuglistener;
		bool		debugconnection;
		uint32_t	maxquerysize;
		uint32_t	maxstringbindvaluelength;
		uint32_t	maxlobbindvaluelength;
		int32_t		idleclienttimeout;
		int64_t		maxlisteners;
		uint32_t	listenertimeout;
		bool		reloginatstart;
		int64_t		timequeriessec;
		int64_t		timequeriesusec;

		bool		sidenabled;
		const char	*sidhost;
		uint16_t	sidport;
		const char	*sidsocket;
		const char	*siduser;
		const char	*sidpassword;

		stringbuffer	sqltranslationrules;
		uint16_t	sqltranslationrulesdepth;

		usercontainer	*currentuser;

		connectstringcontainer	*firstconnect;
		connectstringcontainer	*currentconnect;
		uint32_t		connectioncount;
		uint32_t		metrictotal;

		routecontainer		*currentroute;

		uint16_t	connectstringcount;

		linkedlist< connectstringcontainer * >	connectstringlist;
		linkedlist< usercontainer * >		userlist;
		linkedlist< routecontainer *>		routelist;
		
		typedef enum {
			NO_TAG,
			USERS_TAG,
			USER_TAG,
			CONNECTIONS_TAG,
			CONNECTION_TAG,
			ROUTER_TAG,
			ROUTE_TAG,
			FILTER_TAG,
			QUERY_TAG,
	   		SQLTRANSLATIONRULES_TAG
		} tag;
		
		tag currenttag;
};

#endif
