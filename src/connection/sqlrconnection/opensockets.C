// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection::openSockets() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"listening on sockets...");
	#endif

	// get the next available unix socket and open it
	if (cfgfl->getListenOnUnix() && unixsocketptr && unixsocketptr[0]) {

		if (!serversockun) {
			serversockun=new unixserversocket();
			if (serversockun->listenOnSocket(unixsocket,0000,5)) {

				#ifdef SERVER_DEBUG
				char	*string=new char[26+
							strlen(unixsocket)+1];
				sprintf(string,"listening on unix socket: %s",
								unixsocket);
				debugPrint("connection",1,string);
				delete[] string;
				#endif

				addFileDescriptor(serversockun);

			} else {
				fprintf(stderr,"Could not listen on ");
				fprintf(stderr,"unix socket: ");
				fprintf(stderr,"%s\n",unixsocket);
				fprintf(stderr,"Make sure that the file and ");
				fprintf(stderr,"directory are readable ");
				fprintf(stderr,"and writable.\n\n");
				delete serversockun;
				return false;
			}
		}
	}

	// open the next available inet socket
	if (cfgfl->getListenOnInet()) {

		if (!serversockin) {
			serversockin=new inetserversocket();
			if (serversockin->listenOnSocket(NULL,inetport,5)) {

				if (!inetport) {
					inetport=serversockin->getPort();
				}

				#ifdef SERVER_DEBUG
				char	string[33];
				sprintf(string,"listening on inet socket: %d",
								inetport);
				debugPrint("connection",1,string);
				#endif

				addFileDescriptor(serversockin);

			} else {
				fprintf(stderr,"Could not listen on ");
				fprintf(stderr,"inet socket: ");
				fprintf(stderr,"%d\n\n",inetport);
				delete serversockin;
				return false;
			}
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done listening on sockets");
	#endif

	return true;
}
