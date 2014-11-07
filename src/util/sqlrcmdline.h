#ifndef SQLRUTIL_SQLRCMDLINE_H
#define SQLRUTIL_SQLRCMDLINE_H

#include <sqlrutildll.h>

#include <rudiments/commandline.h>

class SQLRUTIL_DLLSPEC sqlrcmdline : public commandline {
	public:
			sqlrcmdline(int argc, const char **argv);

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

#endif
