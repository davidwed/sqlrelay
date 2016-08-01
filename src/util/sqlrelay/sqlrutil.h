// Copyright (c) 1999-2014 David Muse
// See the file COPYING for more information

#ifndef SQLRUTIL_H
#define SQLRUTIL_H

#include <sqlrelay/private/sqlrutilincludes.h>

class SQLRUTIL_DLLSPEC sqlrcmdline : public commandline {
	friend class sqlrpaths;
	public:
			sqlrcmdline(int argc, const char **argv);

		const char	*getId() const;
	private:
		const char	*id;
};

class SQLRUTIL_DLLSPEC sqlrpaths {
	public:
				sqlrpaths(sqlrcmdline *cmdl);
				~sqlrpaths();
		const char	*getBinDir();
		const char	*getLocalStateDir();
		const char	*getTmpDir();
		const char	*getSockSeqFile();
		const char	*getSocketsDir();
		const char	*getIpcDir();
		const char	*getPidDir();
		const char	*getLogDir();
		const char	*getDebugDir();
		const char	*getCacheDir();
		const char	*getDefaultConfigFile();
		const char	*getDefaultConfigDir();
		const char	*getDefaultConfigUrl();
		const char	*getConfigUrl();
		const char	*getLibExecDir();
	protected:
		char		*bindir;
		char		*localstatedir;
		char		*tmpdir;
		char		*sockseqfile;
		char		*socketsdir;
		char		*ipcdir;
		char		*piddir;
		uint32_t	tmpdirlen;
		char		*logdir;
		char		*debugdir;
		char		*cachedir;
		char		*defaultconfigfile;
		char		*defaultconfigdir;
		char		*defaultconfigurl;
		const char	*configurl;
		char		*libexecdir;
};

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

class SQLRUTIL_DLLSPEC sqlrconfig {
	public:
			sqlrconfig();
		virtual ~sqlrconfig();

		virtual void	getEnabledIds(const char *url,
					linkedlist< char * > *idlist)=0;
		virtual bool	load(const char *url, const char *id)=0;
		virtual bool	accessible()=0;

		virtual const char 	*getDefaultAddresses()=0;
		virtual uint16_t	getDefaultPort()=0;
		virtual const char	*getDefaultSocket()=0;

		virtual bool		getDefaultKrb()=0;
		virtual const char	*getDefaultKrbService()=0;
		virtual const char	*getDefaultKrbKeytab()=0;
		virtual const char	*getDefaultKrbMech()=0;
		virtual const char	*getDefaultKrbFlags()=0;

		virtual bool		getDefaultTls()=0;
		virtual const char	*getDefaultTlsCiphers()=0;

		virtual const char	*getDefaultUser()=0;
		virtual const char	*getDefaultPassword()=0;

		virtual bool		getListenOnInet()=0;
		virtual bool		getListenOnUnix()=0;

		virtual const char	*getDbase()=0;

		virtual uint32_t	getConnections()=0;
		virtual uint32_t	getMaxConnections()=0;
		virtual uint32_t	getMaxQueueLength()=0;
		virtual uint32_t	getGrowBy()=0;
		virtual int32_t		getTtl()=0;
		virtual int32_t		getSoftTtl()=0;
		virtual uint16_t	getMaxSessionCount()=0;
		virtual bool		getDynamicScaling()=0;

		virtual const char	*getEndOfSession()=0;
		virtual bool		getEndOfSessionCommit()=0;
		virtual uint32_t	getSessionTimeout()=0;

		virtual const char	*getRunAsUser()=0;
		virtual const char	*getRunAsGroup()=0;

		virtual uint16_t	getCursors()=0;
		virtual uint16_t	getMaxCursors()=0;
		virtual uint16_t	getCursorsGrowBy()=0;

		virtual const char	*getAuthTier()=0;
		virtual bool		getAuthOnConnection()=0;
		virtual bool		getAuthOnDatabase()=0;

		virtual const char	*getSessionHandler()=0;

		virtual const char	*getHandoff()=0;

		virtual const char	*getAllowedIps()=0;
		virtual const char	*getDeniedIps()=0;

		virtual const char	*getDebug()=0;
		virtual bool		getDebugParser()=0;
		virtual bool		getDebugTranslations()=0;
		virtual bool		getDebugFilters()=0;
		virtual bool		getDebugTriggers()=0;
		virtual bool		getDebugBindTranslations()=0;
		virtual bool		getDebugResultSetTranslations()=0;
		virtual bool		getDebugProtocols()=0;
		virtual bool		getDebugAuths()=0;

		virtual uint64_t	getMaxClientInfoLength()=0;
		virtual uint32_t	getMaxQuerySize()=0;
		virtual uint16_t	getMaxBindCount()=0;
		virtual uint16_t	getMaxBindNameLength()=0;
		virtual uint32_t	getMaxStringBindValueLength()=0;
		virtual uint32_t	getMaxLobBindValueLength()=0;
		virtual uint32_t	getMaxErrorLength()=0;

		virtual int32_t		getIdleClientTimeout()=0;

		virtual int64_t		getMaxListeners()=0;
		virtual uint32_t	getListenerTimeout()=0;

		virtual bool		getReLoginAtStart()=0;

		virtual bool		getFakeInputBindVariables()=0;
		virtual const char	*getFakeInputBindVariablesDateFormat()=0;
		virtual bool		getTranslateBindVariables()=0;

		virtual const char	*getIsolationLevel()=0;

		virtual bool		getIgnoreSelectDatabase()=0;

		virtual bool		getWaitForDownDatabase()=0;

		virtual linkedlist< char *>	*getSessionStartQueries()=0;
		virtual linkedlist< char *>	*getSessionEndQueries()=0;

		virtual xmldomnode	*getListeners()=0;
		virtual xmldomnode	*getParser()=0;
		virtual xmldomnode	*getTranslations()=0;
		virtual xmldomnode	*getFilters()=0;
		virtual xmldomnode	*getResultSetTranslations()=0;
		virtual xmldomnode	*getTriggers()=0;
		virtual xmldomnode	*getLoggers()=0;
		virtual xmldomnode	*getNotifications()=0;
		virtual xmldomnode	*getSchedules()=0;
		virtual xmldomnode	*getRouters()=0;
		virtual xmldomnode	*getQueries()=0;
		virtual xmldomnode	*getPasswordEncryptions()=0;
		virtual xmldomnode	*getAuths()=0;

		virtual linkedlist< connectstringcontainer * >
						*getConnectStringList()=0;
		virtual connectstringcontainer	*getConnectString(
						const char *connectionid)=0;
		virtual uint32_t		getConnectionCount()=0;
		virtual uint32_t		getMetricTotal()=0;

		virtual linkedlist< routecontainer * >	*getRouteList()=0;
};

class SQLRUTIL_DLLSPEC sqlrconfigs {
	public:
			sqlrconfigs(sqlrpaths *sqlrpth);
			~sqlrconfigs();
		void		getEnabledIds(const char *urls,
						linkedlist< char * > *idlist);
		sqlrconfig	*load(const char *urls, const char *id);
	private:
		void		loadConfig(const char *module);

		const char	*libexecdir;
		sqlrconfig	*cfg;
		dynamiclib	*dl;
};

#endif
