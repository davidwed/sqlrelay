// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <config.h>

#include <rudiments/xmlsax.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/list.h>
#include <rudiments/parameterstring.h>

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

		void	setUser(const char *user);
		void	setPassword(const char *password);
		char	*getUser();
		char	*getPassword();
	private:
		char		*user;
		char		*password;
};

typedef listnode< usercontainer * >	usernode;

class connectstringcontainer {
	public:
				connectstringcontainer(int connectstringcount);
				~connectstringcontainer();
		void	parseConnectString();
		void	setConnectionId(const char *connectionid);
		void	setString(const char *string);
		void	setMetric(int metric);
		char	*getConnectionId();
		char	*getString();
		int	getMetric();
		char	*getConnectStringValue(const char *variable);
	private:

		char			*connectionid;
		char			*string;
		int			metric;

		// connect string parameters
		parameterstring	connectstring;
		int		connectstringcount;
};

typedef listnode< connectstringcontainer * >	connectstringnode;

class sqlrconfigfile : public xmlsax {
	public:
			sqlrconfigfile();
			~sqlrconfigfile();
		int	parse(const char *config, char *id);
		int	parse(const char *config, char *id,
					int connectstringcount);
		int	getPort();
		char	*getUnixPort();
		int	getListenOnInet();
		int	getListenOnUnix();
		char	*getDbase();
		int	getConnections();
		int	getMaxConnections();
		int	getMaxQueueLength();
		int	getGrowBy();
		int	getTtl();
		int	getDynamicScaling();
		char	*getEndOfSession();
		int	getEndOfSessionCommit();
		int	getSessionTimeout();
		char	*getRunAsUser();
		char	*getRunAsGroup();
		int	getCursors();
		char	*getAuthTier();
		int	getAuthOnListener();
		int	getAuthOnConnection();
		int	getAuthOnDatabase();
		char	*getHandOff();
		int	getPassDescriptor();
		char	*getAllowedIps();
		char	*getDeniedIps();
		char	*getDebug();
		int	getDebugListener();
		int	getDebugConnection();

		list< usercontainer * >	*getUserList();
		list< connectstringcontainer * >
					*getConnectStringList();
		connectstringcontainer	*getConnectString(
						const char *connectionid);
		int			getConnectionCount();
		int			getMetricTotal();
	private:
		char		*id;
		int		correctid;
		int		done;
		attribute	currentattribute;

		int	atoi(const char *value,
					const char *defaultvalue,
					int minvalue);

		bool	tagStart(char *name);
		bool	attributeName(char *name);
		bool	attributeValue(char *value);
		bool	tagEnd(char *name);

		int	port;
		int	listenoninet;
		char	*unixport;
		int	listenonunix;
		char	*dbase;
		int	connections;
		int	maxconnections;
		int	maxqueuelength;
		int	growby;
		int	ttl;
		char	*endofsession;
		int	endofsessioncommit;
		int	sessiontimeout;
		char	*runasuser;
		char	*runasgroup;
		int	cursors;
		char	*authtier;
		int	authonlistener;
		int	authonconnection;
		int	authondatabase;
		char	*handoff;
		int	passdescriptor;
		char	*allowedips;
		char	*deniedips;
		char	*debug;
		int	debuglistener;
		int	debugconnection;

		usercontainer	*currentuser;

		connectstringcontainer	*firstconnect;
		connectstringcontainer	*currentconnect;
		int			connectioncount;
		int			metrictotal;

		int	connectstringcount;

		list< connectstringcontainer * >	connectstringlist;
		list< usercontainer * >			userlist;
};

#endif
