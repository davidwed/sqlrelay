#ifndef SQLRUTIL_CMDLINE_H
#define SQLRUTIL_CMDLINE_H

#ifdef SERVER_DEBUG
	#include <rudiments/logger.h>
#endif

#include <rudiments/commandline.h>

class cmdline : public commandline {
	public:
			cmdline(int argc, const char **argv);

		char	*getConfig() const;
		char	*getId() const;
		char	*getLocalStateDir() const;
	private:
		void	setConfig();
		void	setId();
		void	setLocalStateDir();

		// command line parameters
		char	*id;
		char	*config;
		char	*localstatedir;
};

inline char	*cmdline::getConfig() const {
	return config;
}

inline char	*cmdline::getId() const {
	return id;
}

inline char	*cmdline::getLocalStateDir() const {
	return localstatedir;
}

#endif
