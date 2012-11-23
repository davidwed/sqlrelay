// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

bool sqlrcontroller_svr::openSockets() {

	dbgfile.debugPrint("connection",0,"listening on sockets...");

	// get the next available unix socket and open it
	if (cfgfl->getListenOnUnix() && unixsocketptr && unixsocketptr[0]) {

		if (!serversockun) {
			serversockun=new unixserversocket();
			if (serversockun->listen(unixsocket,0000,5)) {

				size_t	stringlen=26+
					charstring::length(unixsocket)+1;
				char	*string=new char[stringlen];
				snprintf(string,stringlen,
					"listening on unix socket: %s",
								unixsocket);
				dbgfile.debugPrint("connection",1,string);
				delete[] string;

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
			const char * const *addresses=cfgfl->getAddresses();
			serversockincount=cfgfl->getAddressCount();
			serversockin=new inetserversocket *[serversockincount];
			bool	failed=false;
			for (uint64_t index=0;
					index<serversockincount;
					index++) {
				serversockin[index]=NULL;
				if (failed) {
					continue;
				}
				serversockin[index]=new inetserversocket();
				if (serversockin[index]->
					listen(addresses[index],inetport,5)) {

					if (!inetport) {
						inetport=serversockin[index]->
								getPort();
					}

					char	string[33];
					snprintf(string,33,
						"listening on inet socket: %d",
						inetport);
					dbgfile.debugPrint("connection",1,string);
	
					addFileDescriptor(serversockin[index]);

				} else {
					fprintf(stderr,"Could not listen on ");
					fprintf(stderr,"inet socket: ");
					fprintf(stderr,"%d\n\n",inetport);
					failed=true;
				}
			}
			if (failed) {
				for (uint64_t index=0;
						index<serversockincount;
						index++) {
					delete serversockin[index];
				}
				delete[] serversockin;
				serversockincount=0;
				return false;
			}
		}
	}

	dbgfile.debugPrint("connection",0,"done listening on sockets");

	return true;
}
