#ifndef SQLRCONNECTION_CMDLINE_H
#define SQLRCONNECTION_CMDLINE_H

#ifdef SERVER_DEBUG
	#include <rudiments/logger.h>
#endif

#include <rudiments/commandline.h>
#include <cmdline.h>

class connectioncmdline : public cmdline {
	public:
			connectioncmdline(int argc, const char **argv);

		char	*getConnectionId() const;
		int	getTtl() const;
	private:
		void	setConnectionId();
		void	setTtl();

		// command line parameters
		char	*connectionid;
		int	ttl;
};

inline char	*connectioncmdline::getConnectionId() const {
	return connectionid;
}

inline int	connectioncmdline::getTtl() const {
	return ttl;
}

#endif
