#ifndef SQLRCONNECTION_SCALERCOMM_H
#define SQLRCONNECTION_SCALERCOMM_H

#ifdef SERVER_DEBUG
	#include <rudiments/logger.h>
#endif

#include <connection/ipc.h>

class scalercomm {
	public:
			scalercomm(ipc *ipcptr);

		#ifdef SERVER_DEBUG
		void	setDebugLogger(logger *dl);
		#endif

		void	incrementConnectionCount();
		void	decrementConnectionCount();
		void	decrementSessionCount();
	private:
		int	connectioncount;
		ipc	*ipcptr;
		#ifdef SERVER_DEBUG
		logger	*dl;
		#endif
};

#endif
