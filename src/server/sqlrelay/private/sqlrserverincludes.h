// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrutil.h>

#include <rudiments/filedescriptor.h>
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

class sqlrlistenerprivate;
class sqlrservercontrollerprivate;
class sqlrserverconnection;
class sqlrserverconnectionprivate;
class sqlrservercursor;
class sqlrservercursorprivate;
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
class sqlrauths;
class sqlrauthsprivate;
class sqlrpwdenc;
class sqlrpwdencprivate;
class sqlrpwdencs;
class sqlrpwdencsprivate;
class sqlrlogger;
class sqlrloggerprivate;
class sqlrloggers;
class sqlrloggersprivate;
class sqlrnotification;
class sqlrnotificationprivate;
class sqlrnotifications;
class sqlrnotificationsprivate;
class sqlrscheduleperiod;
class sqlrscheduledaypart;
class sqlrschedulerule;
class sqlrscheduleruleprivate;
class sqlrschedule;
class sqlrscheduleprivate;
class sqlrschedules;
class sqlrschedulesprivate;
class sqlrconnection;
class sqlrrouterprivate;
class sqlrrouters;
class sqlrroutersprivate;
class sqlrparser;
class sqlrparserprivate;
class sqlrdirective;
class sqlrdirectiveprivate;
class sqlrdirectives;
class sqlrdirectivesprivate;
class sqlrtranslation;
class sqlrtranslationprivate;
class sqlrdatabaseobject;
class sqlrtranslations;
class sqlrtranslationsprivate;
class sqlrfilter;
class sqlrfilterprivate;
class sqlrfilterplugin;
class sqlrfilters;
class sqlrfiltersprivate;
class sqlrbindvariabletranslation;
class sqlrbindvariabletranslationprivate;
class sqlrbindvariabletranslations;
class sqlrbindvariabletranslationsprivate;
class sqlrresultsettranslation;
class sqlrresultsettranslationprivate;
class sqlrresultsettranslations;
class sqlrresultsettranslationsprivate;
class sqlrresultsetrowtranslation;
class sqlrresultsetrowtranslationprivate;
class sqlrresultsetrowtranslations;
class sqlrresultsetrowtranslationsprivate;
class sqlrresultsetrowblocktranslation;
class sqlrresultsetrowblocktranslationprivate;
class sqlrresultsetrowblocktranslations;
class sqlrresultsetrowblocktranslationsprivate;
class sqlrresultsetheadertranslation;
class sqlrresultsetheadertranslationprivate;
class sqlrresultsetheadertranslations;
class sqlrresultsetheadertranslationsprivate;
class sqlrtrigger;
class sqlrtriggerprivate;
class sqlrtriggerplugin;
class sqlrtriggers;
class sqlrtriggersprivate;
class sqlrquery;
class sqlrqueryprivate;
class sqlrquerycursor;
class sqlrquerycursorprivate;
class sqlrqueries;
class sqlrqueriesprivate;
class sqlrmoduledata;
class sqlrmoduledataprivate;
class sqlrmoduledatas;
class sqlrmoduledatasprivate;
