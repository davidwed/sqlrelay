// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrutil.h>

#include <rudiments/signalclasses.h>
#include <rudiments/listener.h>
#include <rudiments/unixsocketserver.h>
#include <rudiments/inetsocketserver.h>
#include <rudiments/unixsocketclient.h>
#include <rudiments/filedescriptor.h>
#include <rudiments/semaphoreset.h>
#include <rudiments/sharedmemory.h>
#include <rudiments/regularexpression.h>
#include <rudiments/thread.h>
#include <rudiments/memorypool.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/datetime.h>
#include <rudiments/singlylinkedlist.h>
#include <rudiments/dictionary.h>
#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>
#include <rudiments/dynamiclib.h>
#include <rudiments/gss.h>
#include <rudiments/tls.h>

#ifndef SQLRSERVER_DLLSPEC
	#ifdef _WIN32
		#ifdef SQLRSERVER_EXPORTS
			#define SQLRSERVER_DLLSPEC __declspec(dllexport)
		#else
			#define SQLRSERVER_DLLSPEC __declspec(dllimport)
		#endif
	#else
		#define SQLRSERVER_DLLSPEC
	#endif
#endif

#include <sqlrelay/private/sqlrshm.h>

class SQLRSERVER_DLLSPEC handoffsocketnode {
	friend class sqlrlistener;
	private:
		uint32_t	pid;
		filedescriptor	*sock;
};
