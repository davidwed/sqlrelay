#ifndef SQLRUTIL_DEBUGFILE_H
#define SQLRUTIL_DEBUGFILE_H

#include <rudiments/logger.h>

class debugfile {
	public:
				debugfile();
				~debugfile();
		void	openDebugFile(const char *name,
					const char *localstatedir);
		void	closeDebugFile();
		void	debugPrint(const char *name, int tabs,
						const char *string);
		void	debugPrint(const char *name, int tabs, long number);
		void	debugPrintBlob(const char *blob, unsigned long length);
		void	debugPrintClob(const char *clob, unsigned long length);
		logger	*getDebugLogger();
	private:
		filedestination	*dbgfile;
		logger		*debuglogger;
};

#endif
