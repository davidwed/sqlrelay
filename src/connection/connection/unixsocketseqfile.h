#ifndef SQLRCONNECTION_UNIXSOCKETSEQFILE_H
#define SQLRCONNECTION_UNIXSOCKETSEQFILE_H

#ifdef SERVER_DEBUG
	#include <rudiments/logger.h>
#endif

class unixsocketseqfile {
	public:

		#ifdef SERVER_DEBUG
		void	setDebugLogger(logger *dl);
		#endif

		bool	getUnixSocket(char *tmpdir, char *unixsocketptr);
	private:
		int	openSequenceFile(char *tmpdir, char *unixsocketptr);
		bool	lockSequenceFile();
		bool	getAndIncrementSequenceNumber(char *unixsocketptr);
		bool	unLockSequenceFile();

		int	sockseq;

		#ifdef SERVER_DEBUG
		logger	*dl;
		#endif
};

#endif
