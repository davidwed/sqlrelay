// Copyright (c) 2000  David Muse
// See the file COPYING for more information.

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <config.h>

#include <rudiments/xmldom.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class user {
	friend class instance;
	public:
				user(xmldomnode *usernode);
				~user();
		const char	*getUser();
		const char	*getPassword();
		void		setUser(const char *user);
		void		setPassword(const char *password);
		user		*nextUser();
	private:
		xmldomnode	*usernode;
		user		*next;
		user		*previous;
};

class connection {
	friend class instance;
	public:
				connection(xmldomnode *connectionnode);
				~connection();
		const char	*getConnectionId();
		const char	*getString();
		const char	*getMetric();
		void		setConnectionId(const char *connectionid);
		void		setString(const char *string);
		void		setMetric(const char *metric);
		connection	*nextConnection();
	private:
		xmldomnode	*connectionnode;
		connection	*next;
		connection	*previous;
};


class instance {
	friend class configfile;
	public:
			instance(xmldomnode *instancenode);
			~instance();
		const char	*getId();
		const char	*getPort();
		const char	*getUnixPort();
		const char	*getDbase();
		const char	*getConnections();
		const char	*getMaxConnections();
		const char	*getMaxQueueLength();
		const char	*getGrowby();
		const char	*getTtl();
		const char	*getEndOfSession();
		const char	*getSessionTimeout();
		const char	*getRunAsUser();
		const char	*getRunAsGroup();
		const char	*getCursors();
		const char	*getAuthTier();
		const char	*getHandoff();
		const char	*getDeniedIps();
		const char	*getAllowedIps();
		const char	*getDebug();
		void	setId(const char *id);
		void	setPort(const char *port);
		void	setUnixPort(const char *unixport);
		void	setDbase(const char *dbase);
		void	setConnections(const char *connections);
		void	setMaxConnections(const char *maxconnections);
		void	setMaxQueueLength(const char *maxqueuelength);
		void	setGrowby(const char *growby);
		void	setTtl(const char *ttl);
		void	setEndOfSession(const char *endofsession);
		void	setSessionTimeout(const char *sessiontimeout);
		void	setRunAsUser(const char *runasuser);
		void	setRunAsGroup(const char *runasgroup);
		void	setCursors(const char *cursors);
		void	setAuthTier(const char *authtier);
		void	setHandoff(const char *handoff);
		void	setDeniedIps(const char *deniedips);
		void	setAllowedIps(const char *allowedips);
		void	setDebug(const char *debug);
		user	*addUser(const char *usr, const char *password);
		void	deleteUser(user *usr);
		user	*findUser(const char *userid);
		user	*firstUser();
		connection	*addConnection(const char *connectionid, 
					const char *string, 
					const char *metric);
		void	deleteConnection(connection *conn);
		connection	*findConnection(const char *conn);
		connection	*firstConnection();
		instance	*nextInstance();
	private:
		xmldomnode	*instancenode;
		xmldomnode	*users;
		xmldomnode	*connections;
		user		*firstuser;
		user		*currentuser;
		user		*lastuser;
		connection	*firstconnection;
		connection	*currentconnection;
		connection	*lastconnection;
		instance	*next;
		instance	*previous;
};

class configfile {
	public:
				configfile();
				~configfile();
		void		blank();
		int		parse(const char *filename);
		int		write();
		int		write(const char *filename);
		void		close();
		char		*currentFile();
		instance	*addInstance(const char *id,
					const char *port,
					const char *unixport,
					const char *dbase,
					const char *connections, 
					const char *maxconnections, 
					const char *maxqueuelength,
					const char *growby, 
					const char *ttl,
					const char *endofsession,
					const char *sessiontimeout,
					const char *runasuser,
					const char *runasgroup,
					const char *cursors,
					const char *authtier, 
					const char *handoff,
					const char *deniedips,
					const char *allowedips,
					const char *debug);
		void		deleteInstance(instance *inst);
		instance	*findInstance(const char *id);
		instance	*firstInstance();
	private:
		xmldom		*doc;
		xmldomnode	*root;
		xmldomnode	*instances;

		xmldomnode	*currentnode;

		instance	*firstinstance;
		instance	*currentinstance;
		instance	*lastinstance;

		char		*currentfile;
};

#endif
