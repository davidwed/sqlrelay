#ifndef SQLRUTIL_DEBUGFILE_H
#define SQLRUTIL_DEBUGFILE_H

#include <rudiments/logger.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class debugfile {
	public:
				debugfile();
				~debugfile();
		void	openDebugFile(const char *name,
					const char *localstatedir);
		void	closeDebugFile();
		void	debugPrint(const char *name, int32_t tabs,
							const char *string);
		void	debugPrint(const char *name, int32_t tabs,
							int32_t number);
		void	debugPrint(const char *name, int32_t tabs,
							uint32_t number);
		void	debugPrint(const char *name, int32_t tabs,
							double number);
		void	debugPrintBlob(const char *blob, uint32_t length);
		void	debugPrintClob(const char *clob, uint32_t length);
		logger	*getDebugLogger();
	private:
		filedestination	*dbgfile;
		logger		*debuglogger;
};

#endif
