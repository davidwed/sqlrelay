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
				connectstringcontainer(int connectstringcount);
				~connectstringcontainer();
		void		parseConnectString();
		void		setConnectionId(const char *connectionid);
		void		setString(const char *string);
		void		setMetric(int metric);
		const char	*getConnectionId();
		const char	*getString();
		int		getMetric();
		const char	*getConnectStringValue(const char *variable);
	private:

		const char	*connectionid;
		const char	*string;
		int		metric;

		// connect string parameters
		parameterstring	connectstring;
		int		connectstringcount;
};

typedef linkedlistnode< connectstringcontainer * >	connectstringnode;

class sqlrconfigfile : public xmlsax {
	public:
			sqlrconfigfile();
			~sqlrconfigfile();
		int	parse(const char *config, const char *id);
		int	parse(const char *config, const char *id,
						int connectstringcount);
		int		getPort();
		const char	*getUnixPort();
		bool		getListenOnInet();
		bool		getListenOnUnix();
		const char	*getDbase();
		int		getConnections();
		int		getMaxConnections();
		int		getMaxQueueLength();
		int		getGrowBy();
		int		getTtl();
		bool		getDynamicScaling();
		const char	*getEndOfSession();
		bool		getEndOfSessionCommit();
		long		getSessionTimeout();
		const char	*getRunAsUser();
		const char	*getRunAsGroup();
		int		getCursors();
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

		linkedlist< usercontainer * >	*getUserList();
		linkedlist< connectstringcontainer * >
					*getConnectStringList();
		connectstringcontainer	*getConnectString(
						const char *connectionid);
		int			getConnectionCount();
		int			getMetricTotal();
	private:
		const char	*id;
		bool		correctid;
		bool		done;
		attribute	currentattribute;

		int	atoi(const char *value,
					const char *defaultvalue,
					int minvalue);
		long	atol(const char *value,
					const char *defaultvalue,
					long minvalue);

		bool	tagStart(const char *name);
		bool	attributeName(const char *name);
		bool	attributeValue(const char *value);
		bool	tagEnd(const char *name);

		int		port;
		bool		listenoninet;
		const char	*unixport;
		bool		listenonunix;
		const char	*dbase;
		int		connections;
		int		maxconnections;
		int		maxqueuelength;
		int		growby;
		int		ttl;
		const char	*endofsession;
		bool		endofsessioncommit;
		long		sessiontimeout;
		const char	*runasuser;
		const char	*runasgroup;
		int		cursors;
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

		usercontainer	*currentuser;

		connectstringcontainer	*firstconnect;
		connectstringcontainer	*currentconnect;
		int			connectioncount;
		int			metrictotal;

		int	connectstringcount;

		linkedlist< connectstringcontainer * >	connectstringlist;
		linkedlist< usercontainer * >		userlist;
};

#endif
