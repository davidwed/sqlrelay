// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrutil.h>

#include <rudiments/filedescriptor.h>
#include <rudiments/semaphoreset.h>
#include <rudiments/thread.h>
#include <rudiments/memorypool.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/datetime.h>
#include <rudiments/singlylinkedlist.h>
#include <rudiments/dictionary.h>
#include <rudiments/xmldom.h>
#include <rudiments/domnode.h>
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

class sqlrservermodule;
class sqlrserverbaseprivate;
class sqlrlistener;
class sqlrlistenerprivate;
class sqlrservercontroller;
class sqlrservercontrollerprivate;
class sqlrserverconnection;
class sqlrserverconnectionprivate;
class sqlrservercursor;
class sqlrservercursorprivate;
class sqlrservermoduleprivate;
class sqlrprotocol;
class sqlrprotocolprivate;
class sqlrprotocols;
class sqlrprotocolsprivate;
class sqlrcredentials;
class sqlruserpasswordcredentialsprivate;
class sqlrgsscredentialsprivate;
class sqlrtlscredentialsprivate;
class sqlrauth;
class sqlrauthprivate;
class sqlrauthsprivate;
class sqlrlogger;
class sqlrloggerprivate;
class sqlrloggersprivate;
class sqlrnotification;
class sqlrnotificationprivate;
class sqlrnotificationsprivate;
class sqlrscheduleperiod;
class sqlrscheduledaypart;
class sqlrschedulerule;
class sqlrscheduleruleprivate;
class sqlrschedule;
class sqlrscheduleprivate;
class sqlrschedulesprivate;
class sqlrconnection;
class sqlrrouterprivate;
class sqlrroutersprivate;
class sqlrparser;
class sqlrparserprivate;
class sqlrdirective;
class sqlrdirectiveprivate;
class sqlrdirectivesprivate;
class sqlrquerytranslation;
class sqlrquerytranslationprivate;
class sqlrdatabaseobject;
class sqlrquerytranslationsprivate;
class sqlrfilter;
class sqlrfilterprivate;
class sqlrfilterplugin;
class sqlrfiltersprivate;
class sqlrbindvariabletranslation;
class sqlrbindvariabletranslationprivate;
class sqlrbindvariabletranslationsprivate;
class sqlrresultsettranslation;
class sqlrresultsettranslationprivate;
class sqlrresultsettranslationsprivate;
class sqlrresultsetrowtranslation;
class sqlrresultsetrowtranslationprivate;
class sqlrresultsetrowtranslationsprivate;
class sqlrresultsetrowblocktranslation;
class sqlrresultsetrowblocktranslationprivate;
class sqlrresultsetrowblocktranslationsprivate;
class sqlrresultsetheadertranslation;
class sqlrresultsetheadertranslationprivate;
class sqlrresultsetheadertranslationsprivate;
class sqlrerrortranslation;
class sqlrerrortranslationprivate;
class sqlrerrortranslationsprivate;
class sqlrtrigger;
class sqlrtriggerprivate;
class sqlrtriggerplugin;
class sqlrtriggersprivate;
class sqlrquery;
class sqlrqueryprivate;
class sqlrquerycursor;
class sqlrquerycursorprivate;
class sqlrqueriesprivate;
class sqlrmoduledata;
class sqlrmoduledataprivate;
class sqlrmoduledatasprivate;

typedef sqlrquerytranslation sqlrtranslation;
