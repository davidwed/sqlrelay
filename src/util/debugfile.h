#ifndef SQLRUTIL_DEBUGFILE_H
#define SQLRUTIL_DEBUGFILE_H

#include <rudiments/logger.h>

class debugfile {
	public:
			debugfile();
			~debugfile();
		void	init(const char *name, const char *localstatedir);
		bool	openDebugFile();
		void	closeDebugFile();
		void	enable();
		void	disable();
		bool	debugEnabled();
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
	private:
		rudiments::filedestination	*dbgfile;
		rudiments::logger		*debuglogger;

		char	*dbgfilename;
		bool	enabled;
};

#endif
