// Copyright (c) 2000-2001  David Muse
// See the file COPYING for more information

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <config.h>

#include <rudiments/xmlsax.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/linkedlist.h>
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

typedef linkedlistnode< usercontainer * >	usernode;

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

typedef linkedlistnode< connectstringcontainer * >	connectstringnode;

class sqlrconfigfile : public xmlsax {
	public:
			sqlrconfigfile();
			~sqlrconfigfile();
		int	parse(const char *config, char *id);
		int	parse(const char *config, char *id,
					int connectstringcount);
		int	getPort();
		char	*getUnixPort();
		bool	getListenOnInet();
		bool	getListenOnUnix();
		char	*getDbase();
		int	getConnections();
		int	getMaxConnections();
		int	getMaxQueueLength();
		int	getGrowBy();
		int	getTtl();
		bool	getDynamicScaling();
		char	*getEndOfSession();
		bool	getEndOfSessionCommit();
		int	getSessionTimeout();
		char	*getRunAsUser();
		char	*getRunAsGroup();
		int	getCursors();
		char	*getAuthTier();
		bool	getAuthOnListener();
		bool	getAuthOnConnection();
		bool	getAuthOnDatabase();
		char	*getHandOff();
		bool	getPassDescriptor();
		char	*getAllowedIps();
		char	*getDeniedIps();
		char	*getDebug();
		bool	getDebugListener();
		bool	getDebugConnection();

		linkedlist< usercontainer * >	*getUserList();
		linkedlist< connectstringcontainer * >
					*getConnectStringList();
		connectstringcontainer	*getConnectString(
						const char *connectionid);
		int			getConnectionCount();
		int			getMetricTotal();
	private:
		char		*id;
		bool		correctid;
		bool		done;
		attribute	currentattribute;

		int	atoi(const char *value,
					const char *defaultvalue,
					int minvalue);

		bool	tagStart(char *name);
		bool	attributeName(char *name);
		bool	attributeValue(char *value);
		bool	tagEnd(char *name);

		int	port;
		bool	listenoninet;
		char	*unixport;
		bool	listenonunix;
		char	*dbase;
		int	connections;
		int	maxconnections;
		int	maxqueuelength;
		int	growby;
		int	ttl;
		char	*endofsession;
		bool	endofsessioncommit;
		int	sessiontimeout;
		char	*runasuser;
		char	*runasgroup;
		int	cursors;
		char	*authtier;
		bool	authonlistener;
		bool	authonconnection;
		bool	authondatabase;
		char	*handoff;
		bool	passdescriptor;
		char	*allowedips;
		char	*deniedips;
		char	*debug;
		bool	debuglistener;
		bool	debugconnection;

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
