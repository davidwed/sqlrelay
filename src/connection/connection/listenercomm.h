#ifndef SQLRCONNECTION_LISTENERCOMM_H
#define SQLRCONNECTION_LISTENERCOMM_H

#include <connection/ipc.h>
#include <connection/connectioncmdline.h>

#ifdef SERVER_DEBUG
	#include <rudiments/logger.h>
#endif

#include <rudiments/unixclientsocket.h>

class listenercomm {
	public:
			listenercomm(ipc *ipcptr,
					connectioncmdline *cmdlineptr);
			~listenercomm();

		#ifdef SERVER_DEBUG
		void	setDebugLogger(logger *dl);
		#endif

		void	announceAvailability(char *tmpdir,
					bool passdescriptor,
					char *unixsocket,
					unsigned short inetport,
					char *connectionid);
		bool	receiveFileDescriptor(int *descriptor);
		void	deRegisterForHandoff(char *tmpdir);
	private:
		void	registerForHandoff(char *tmpdir);

		ipc			*ipcptr;
		connectioncmdline	*cmdlineptr;
		unixclientsocket	handoffsockun;
		bool			connected;

		#ifdef SERVER_DEBUG
		logger *dl;
		#endif
};

#endif
