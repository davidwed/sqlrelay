// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/private/sqlrserverdll.h>

#include <sqlrelay/sqlrutil.h>
#include <sqlrelay/sqlrloggers.h>

#include <rudiments/signalclasses.h>
#include <rudiments/listener.h>
#include <rudiments/unixsocketserver.h>
#include <rudiments/inetsocketserver.h>
#include <rudiments/filedescriptor.h>
#include <rudiments/semaphoreset.h>
#include <rudiments/sharedmemory.h>
#include <rudiments/regularexpression.h>
#include <rudiments/thread.h>

#include <sqlrelay/private/sqlrshmdata.h>

class SQLRSERVER_DLLSPEC handoffsocketnode {
	friend class sqlrlistener;
	private:
		uint32_t	pid;
		filedescriptor	*sock;
};
