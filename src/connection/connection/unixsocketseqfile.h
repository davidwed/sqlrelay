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

		int	getUnixSocket(char *tmpdir, char *unixsocketptr);
	private:
		int	openSequenceFile(char *tmpdir, char *unixsocketptr);
		int	lockSequenceFile();
		int	getAndIncrementSequenceNumber(char *unixsocketptr);
		int	unLockSequenceFile();

		int	sockseq;

		#ifdef SERVER_DEBUG
		logger	*dl;
		#endif
};

#endif
