#ifndef SQLRUTIL_CMDLINE_H
#define SQLRUTIL_CMDLINE_H

#ifdef SERVER_DEBUG
	#include <rudiments/logger.h>
#endif

#include <rudiments/commandline.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class cmdline : public commandline {
	public:
			cmdline(int argc, const char **argv);

		const char	*getConfig() const;
		const char	*getId() const;
		const char	*getLocalStateDir() const;
	private:
		void	setConfig();
		void	setId();
		void	setLocalStateDir();

		// command line parameters
		const char	*id;
		const char	*config;
		const char	*localstatedir;
};

inline const char *cmdline::getConfig() const {
	return config;
}

inline const char *cmdline::getId() const {
	return id;
}

inline const char *cmdline::getLocalStateDir() const {
	return localstatedir;
}

#endif
