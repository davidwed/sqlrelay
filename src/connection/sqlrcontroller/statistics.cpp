// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>
#include <rudiments/process.h>
#include <rudiments/charstring.h>
#include <rudiments/rawbuffer.h>

// for gettimeofday()
#include <sys/time.h>

void sqlrcontroller_svr::initConnStats() {
	if (handoffindex<STATMAXCONNECTIONS) {
		connstats=&shm->connstats[handoffindex];
		clearConnStats();
		connstats->processid=process::getProcessId();
		setState(INIT);
		connstats->index=handoffindex;
		connstats->logged_in_tv.tv_sec=loggedinsec;
		connstats->logged_in_tv.tv_usec=loggedinusec;
	}
}

void sqlrcontroller_svr::clearConnStats() {
	if (!connstats) {
		return;
	}
	rawbuffer::zero(connstats,sizeof(struct sqlrconnstatistics));
}

void sqlrcontroller_svr::setState(enum sqlrconnectionstate state) {
	if (!connstats) {
		return;
	}
	connstats->state=state;
	gettimeofday(&connstats->state_start_tv,NULL);
}

void sqlrcontroller_svr::setCurrentQuery(sqlrcursor_svr *cursor) {
	if (!connstats) {
		return;
	}
	uint32_t	len=cursor->querylength;
	if (len>STATSQLTEXTLEN) {
		len=STATSQLTEXTLEN;
	}
	charstring::copy(connstats->sqltext,cursor->querybuffer,len);
	connstats->sqltext[len]='\0';
}

void sqlrcontroller_svr::setClientInfo() {
	if (!connstats) {
		return;
	}
	uint64_t	len=clientinfolen;
	if (len>STATCLIENTINFOLEN) {
		len=STATCLIENTINFOLEN;
	}
	charstring::copy(connstats->clientinfo,clientinfo,len);
	connstats->clientinfo[len]='\0';
}

void sqlrcontroller_svr::setClientAddr() {
	if (!connstats) {
		return;
	}
	if (clientsock) {
		char	*clientaddrbuf=clientsock->getPeerAddress();
		if (clientaddrbuf) {
			charstring::copy(connstats->clientaddr,clientaddrbuf);
			delete[] clientaddrbuf;
		} else {
			charstring::copy(connstats->clientaddr,"UNIX");
		}
	} else {
		charstring::copy(connstats->clientaddr,"internal");
	}
}
