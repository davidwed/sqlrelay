// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <config.h>

#include <rudiments/xmlsax.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/linkedlist.h>
#include <rudiments/parameterstring.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

typedef enum {
	ID_ATTRIBUTE=1,
	PORT_ATTRIBUTE,
	SOCKET_ATTRIBUTE,
	DBASE_ATTRIBUTE,
	CONNECTIONS_ATTRIBUTE,
	MAXCONNECTIONS_ATTRIBUTE,
	MAXQUEUELENGTH_ATTRIBUTE,
	GROWBY_ATTRIBUTE,
	TTL_ATTRIBUTE,
	ENDOFSESSION_ATTRIBUTE,
	SESSIONTIMEOUT_ATTRIBUTE,
	RUNASUSER_ATTRIBUTE,
	RUNASGROUP_ATTRIBUTE,
	CURSORS_ATTRIBUTE,
	AUTHTIER_ATTRIBUTE,
	HANDOFF_ATTRIBUTE,
	DENIEDIPS_ATTRIBUTE,
	ALLOWEDIPS_ATTRIBUTE,
	DEBUG_ATTRIBUTE,
	MAXQUERYSIZE_ATTRIBUTE,
	MAXSTRINGBINDVALUELENGTH_ATTRIBUTE,
	MAXLOBBINDVALUELENGTH_ATTRIBUTE,
	USER_ATTRIBUTE,
	PASSWORD_ATTRIBUTE,
	CONNECTIONID_ATTRIBUTE,
	STRING_ATTRIBUTE,
	METRIC_ATTRIBUTE
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
		const char	*getConnectionId();
		const char	*getString();
		uint32_t	getMetric();
		const char	*getConnectStringValue(const char *variable);
	private:

		const char	*connectionid;
		const char	*string;
		uint32_t	metric;

		// connect string parameters
		parameterstring	connectstring;
		uint16_t	connectstringcount;
};

typedef linkedlistnode< connectstringcontainer * >	connectstringnode;

class sqlrconfigfile : public xmlsax {
	public:
			sqlrconfigfile();
			~sqlrconfigfile();
		bool	parse(const char *config, const char *id);
		bool	parse(const char *config, const char *id,
						uint16_t connectstringcount);
		uint16_t	getPort();
		const char	*getUnixPort();
		bool		getListenOnInet();
		bool		getListenOnUnix();
		const char	*getDbase();
		uint32_t	getConnections();
		uint32_t	getMaxConnections();
		uint32_t	getMaxQueueLength();
		uint32_t	getGrowBy();
		uint32_t	getTtl();
		bool		getDynamicScaling();
		const char	*getEndOfSession();
		bool		getEndOfSessionCommit();
		uint32_t	getSessionTimeout();
		const char	*getRunAsUser();
		const char	*getRunAsGroup();
		uint16_t	getCursors();
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

		linkedlist< usercontainer * >	*getUserList();
		linkedlist< connectstringcontainer * >
					*getConnectStringList();
		connectstringcontainer	*getConnectString(
						const char *connectionid);
		uint32_t		getConnectionCount();
		uint32_t		getMetricTotal();
	private:
		const char	*id;
		bool		correctid;
		bool		done;
		attribute	currentattribute;

		uint32_t	atouint32_t(const char *value,
					const char *defaultvalue,
					uint32_t minvalue);
		uint16_t	atouint16_t(const char *value,
					const char *defaultvalue,
					uint16_t minvalue);

		bool	tagStart(const char *name);
		bool	attributeName(const char *name);
		bool	attributeValue(const char *value);
		bool	tagEnd(const char *name);

		uint16_t	port;
		bool		listenoninet;
		const char	*unixport;
		bool		listenonunix;
		const char	*dbase;
		uint32_t	connections;
		uint32_t	maxconnections;
		uint32_t	maxqueuelength;
		uint32_t	growby;
		uint32_t	ttl;
		const char	*endofsession;
		bool		endofsessioncommit;
		uint32_t	sessiontimeout;
		const char	*runasuser;
		const char	*runasgroup;
		uint16_t	cursors;
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

		usercontainer	*currentuser;

		connectstringcontainer	*firstconnect;
		connectstringcontainer	*currentconnect;
		uint32_t		connectioncount;
		uint32_t		metrictotal;

		uint16_t	connectstringcount;

		linkedlist< connectstringcontainer * >	connectstringlist;
		linkedlist< usercontainer * >		userlist;
};

#endif
