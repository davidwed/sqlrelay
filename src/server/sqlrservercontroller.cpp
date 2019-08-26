// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <config.h>

#include <sqlrelay/sqlrserver.h>
#include <rudiments/file.h>
#include <rudiments/socketclient.h>
#include <rudiments/bytestring.h>
#include <rudiments/userentry.h>
#include <rudiments/groupentry.h>
#include <rudiments/process.h>
#include <rudiments/permissions.h>
#include <rudiments/snooze.h>
#include <rudiments/error.h>
#include <rudiments/character.h>
#include <rudiments/charstring.h>
#include <rudiments/randomnumber.h>
#include <rudiments/sys.h>
#include <rudiments/environment.h>
#include <rudiments/stdio.h>
#include <rudiments/semaphoreset.h>
#include <rudiments/sharedmemory.h>
#include <rudiments/unixsocketserver.h>
#include <rudiments/unixsocketclient.h>
#include <rudiments/inetsocketserver.h>
#include <rudiments/listener.h>
#include <rudiments/md5.h>

#include <defines.h>
#include <defaults.h>
#define NEED_DATATYPESTRING 1
#define NEED_IS_BIT_TYPE_CHAR 1
#define NEED_IS_BIT_TYPE_INT 1
#define NEED_IS_BOOL_TYPE_CHAR 1
#define NEED_IS_BOOL_TYPE_INT 1
#define NEED_IS_FLOAT_TYPE_CHAR 1
#define NEED_IS_FLOAT_TYPE_INT 1
#define NEED_IS_NUMBER_TYPE_CHAR 1
#define NEED_IS_NUMBER_TYPE_INT 1
#define NEED_IS_BLOB_TYPE_CHAR 1
#define NEED_IS_BLOB_TYPE_INT 1
#define NEED_IS_UNSIGNED_TYPE_CHAR 1
#define NEED_IS_UNSIGNED_TYPE_INT 1
#define NEED_IS_BINARY_TYPE_CHAR 1
#define NEED_IS_BINARY_TYPE_INT 1
#define NEED_IS_DATETIME_TYPE_CHAR 1
#define NEED_IS_DATETIME_TYPE_INT 1
#include <datatypes.h>
#define NEED_CONVERT_DATE_TIME 1
#include <parsedatetime.h>
#define NEED_BEFORE_BIND_VARIABLE 1
#define NEED_IS_BIND_DELIMITER 1
#define NEED_AFTER_BIND_VARIABLE 1
#define NEED_COUNT_BIND_VARIABLES 1
#include <bindvariables.h>

#ifndef SQLRELAY_ENABLE_SHARED
	extern "C" {
		#include "sqlrserverconnectiondeclarations.cpp"
		#include "sqlrparserdeclarations.cpp"
	}
#endif

class sqlrservercontrollerprivate {
	friend class sqlrservercontroller;

	// listener
	listener		_lsnr;

	// connection
	sqlrserverconnection	*_conn;

	// configuration
	sqlrconfigs		*_sqlrcfgs;
	sqlrconfig		*_cfg;
	sqlrpaths		*_pth;

	// statistics
	sqlrshm			*_shm;
	sqlrconnstatistics	*_connstats;

	sqlrcmdline	*_cmdl;

	semaphoreset	*_semset;
	sharedmemory	*_shmem;

	sqlrprotocols				*_sqlrpr;
	sqlrparser				*_sqlrp;
	sqlrdirectives				*_sqlrd;
	sqlrtranslations			*_sqlrt;
	sqlrfilters				*_sqlrf;
	sqlrbindvariabletranslations		*_sqlrbvt;
	sqlrresultsettranslations		*_sqlrrst;
	sqlrresultsetrowtranslations		*_sqlrrsrt;
	sqlrresultsetrowblocktranslations	*_sqlrrsrbt;
	sqlrresultsetheadertranslations		*_sqlrrsht;
	sqlrtriggers				*_sqlrtr;
	sqlrloggers				*_sqlrlg;
	sqlrnotifications			*_sqlrn;
	sqlrschedules				*_sqlrs;
	sqlrqueries				*_sqlrq;
	sqlrpwdencs				*_sqlrpe;
	sqlrauths				*_sqlra;
	sqlrmoduledatas				*_sqlrmd;

	filedescriptor	*_clientsock;

	uint16_t	_protocolindex;
	sqlrprotocol	*_currentprotocol;

	const char	*_user;
	const char	*_password;

	bool		_dbchanged;
	char		*_originaldb;

	connectstringcontainer	*_constr;

	char		*_updown;

	uint16_t	_inetport;
	stringbuffer	_unixsocket;

	bool		_autocommitforthissession;

	bool		_faketransactionblocks;
	bool		_faketransactionblocksautocommiton;
	bool		_infaketransactionblock;
	bool		_intransaction;

	bool		_needscommitorrollback;

	bool		_fakeautocommit;
	bool		_initialautocommit;

	bool		_fakeinputbinds;
	bool		_translatebinds;
	bool		_questionmarksupported;
	bool		_colonsupported;
	bool		_atsignsupported;
	bool		_dollarsignsupported;

	const char	*_isolationlevel;

	uint16_t	_sendcolumninfo;

	int32_t		_accepttimeout;

	bool		_suspendedsession;

	inetsocketserver	**_serversockin;
	uint64_t		_serversockincount;
	unixsocketserver	*_serversockun;

	memorypool	_txpool;
	memorypool	_sessionpool;

	bool		_debugsql;
	bool		_debugbulkload;
	bool		_debugsqlrparser;
	bool		_debugsqlrdirectives;
	bool		_debugsqlrtranslations;
	bool		_debugsqlrfilters;
	bool		_debugbindtranslation;
	bool		_debugsqlrbindvariabletranslation;
	bool		_debugsqlrresultsettranslation;
	bool		_debugsqlrresultsetrowtranslation;
	bool		_debugsqlrresultsetrowblocktranslation;
	bool		_debugsqlrresultsetheadertranslation;
	bool		_debugsqlrmoduledata;

	dynamiclib	_conndl;
	dynamiclib	_sqlrpdl;

	domnode	*_sqlrpnode;

	uint16_t	_cursorcount;
	uint16_t	_mincursorcount;
	uint16_t	_maxcursorcount;
	sqlrservercursor	**_cur;

	char		*_decrypteddbpassword;

	unixsocketclient	_handoffsockun;
	bool			_proxymode;
	uint32_t		_proxypid;

	bool		_connected;
	bool		_inclientsession;
	bool		_loggedin;
	uint32_t	_reloginseed;
	time_t		_relogintime;

	bool		_scalerspawned;
	const char	*_connectionid;
	int32_t		_ttl;

	char		*_pidfile;

	int32_t		_idleclienttimeout;

	bool		_decrementonclose;
	bool		_silent;

	stringbuffer	_debugstr;

	uint32_t	_maxquerysize;
	uint16_t	_maxbindcount;
	uint32_t	_maxerrorlength;

	uint32_t	_fetchatonce;
	uint32_t	_maxcolumncount;
	uint32_t	_maxfieldlength;

	uint64_t	_connecttimeout;
	uint64_t	_querytimeout;
	bool		_executedirect;

	int64_t		_loggedinsec;
	int64_t		_loggedinusec;

	const char	*_dbhostname;
	const char	*_dbipaddress;

	char		*_reformattedfield;
	uint32_t	_reformattedfieldlength;

	singlylinkedlist< char * >	_globaltemptables;
	bool				_allglobaltemptables;
	singlylinkedlist< char * >	_sessiontemptablesfordrop;
	singlylinkedlist< char * >	_sessiontemptablesfortrunc;
	singlylinkedlist< char * >	_transtemptablesfordrop;
	singlylinkedlist< char * >	_transtemptablesfortrunc;

	dictionary< uint32_t, uint32_t >	*_columnmap;
	dictionary< uint32_t, uint32_t >	_mysqldatabasescolumnmap;
	dictionary< uint32_t, uint32_t >	_mysqltablescolumnmap;
	dictionary< uint32_t, uint32_t >	_mysqlcolumnscolumnmap;
	dictionary< uint32_t, uint32_t >	_odbcdatabasescolumnmap;
	dictionary< uint32_t, uint32_t >	_odbctablescolumnmap;
	dictionary< uint32_t, uint32_t >	_odbccolumnscolumnmap;

	const char	**_columnnames;
	uint16_t	*_columnnamelengths;
	uint16_t	*_columntypes;
	const char	**_columntypenames;
	uint16_t	*_columntypenamelengths;
	uint32_t	*_columnlengths;
	uint32_t	*_columnprecisions;
	uint32_t	*_columnscales;
	uint16_t	*_columnisnullables;
	uint16_t	*_columnisprimarykeys;
	uint16_t	*_columnisuniques;
	uint16_t	*_columnispartofkeys;
	uint16_t	*_columnisunsigneds;
	uint16_t	*_columniszerofilleds;
	uint16_t	*_columnisbinarys;
	uint16_t	*_columnisautoincrements;
	const char	**_columntables;
	uint16_t	*_columntablelengths;

	const char	**_fieldnames;
	const char	**_fields;
	uint64_t	*_fieldlengths;
	bool		*_blobs;
	bool		*_nulls;

	char			*_bulkserveridfilename;
	sharedmemory		*_bulkservershmem;
	unsigned char 		*_bulkservershm;
	unsigned char 		*_bulkservershmquery;
	unsigned char 		*_bulkservershmdataformat;
	sharedmemory		*_bulkclientshmem;
	unsigned char		*_bulkclientshm;
	sqlrservercursor	*_bulkcursor;
	const char		*_bulkerrorfieldtable;
	const char		*_bulkerrorrowtable;
	uint64_t		_bulkmaxerrorcount;
	uint64_t		_bulkdroperrortables;
	uint64_t		_bulkquerylen;
	const char		*_bulkquery;
	const unsigned char	*_bulkdataformat;
	singlylinkedlist<const unsigned char *>	_bulkdata;
	singlylinkedlist<uint64_t>		_bulkdatalen;
};

static signalhandler		alarmhandler;
static volatile sig_atomic_t	alarmrang=0;

sqlrservercontroller::sqlrservercontroller() {

	pvt=new sqlrservercontrollerprivate;

	pvt->_conn=NULL;
	pvt->_sqlrcfgs=NULL;
	pvt->_cfg=NULL;
	pvt->_pth=NULL;
	pvt->_connstats=NULL;

	pvt->_cmdl=NULL;
	pvt->_semset=NULL;
	pvt->_shmem=NULL;

	pvt->_updown=NULL;

	pvt->_clientsock=NULL;

	pvt->_protocolindex=0;
	pvt->_currentprotocol=NULL;

	pvt->_user=NULL;
	pvt->_password=NULL;

	pvt->_dbchanged=false;
	pvt->_originaldb=NULL;

	pvt->_serversockun=NULL;
	pvt->_serversockin=NULL;
	pvt->_serversockincount=0;

	pvt->_inetport=0;

	pvt->_needscommitorrollback=false;

	pvt->_fakeautocommit=false;
	pvt->_initialautocommit=false;

	pvt->_autocommitforthissession=false;

	pvt->_faketransactionblocks=false;
	pvt->_faketransactionblocksautocommiton=false;
	pvt->_infaketransactionblock=false;
	pvt->_intransaction=false;

	pvt->_fakeinputbinds=false;
	pvt->_translatebinds=false;
	pvt->_questionmarksupported=true;
	pvt->_colonsupported=true;
	pvt->_atsignsupported=true;
	pvt->_dollarsignsupported=true;

	pvt->_isolationlevel=NULL;

	pvt->_sendcolumninfo=SEND_COLUMN_INFO;

	pvt->_maxquerysize=0;
	pvt->_maxbindcount=0;
	pvt->_maxerrorlength=0;
	pvt->_idleclienttimeout=-1;

	pvt->_fetchatonce=1;
	pvt->_maxcolumncount=0;
	pvt->_maxfieldlength=0;

	pvt->_connecttimeout=0;
	pvt->_querytimeout=0;
	pvt->_executedirect=false;

	pvt->_connected=false;
	pvt->_inclientsession=false;
	pvt->_loggedin=false;
	pvt->_reloginseed=0;
	pvt->_relogintime=0;

	pvt->_sqlrpr=NULL;
	pvt->_sqlrp=NULL;
	pvt->_sqlrd=NULL;
	pvt->_sqlrt=NULL;
	pvt->_sqlrf=NULL;
	pvt->_sqlrbvt=NULL;
	pvt->_sqlrrst=NULL;
	pvt->_sqlrrsrt=NULL;
	pvt->_sqlrrsrbt=NULL;
	pvt->_sqlrrsht=NULL;
	pvt->_sqlrtr=NULL;
	pvt->_sqlrlg=NULL;
	pvt->_sqlrn=NULL;
	pvt->_sqlrs=NULL;
	pvt->_sqlrq=NULL;
	pvt->_sqlrpe=NULL;
	pvt->_sqlra=NULL;
	pvt->_sqlrmd=NULL;

	pvt->_decrypteddbpassword=NULL;

	pvt->_debugsql=false;
	pvt->_debugbulkload=false;
	pvt->_debugsqlrparser=false;
	pvt->_debugsqlrdirectives=false;
	pvt->_debugsqlrtranslations=false;
	pvt->_debugsqlrfilters=false;
	pvt->_debugbindtranslation=false;
	pvt->_debugsqlrbindvariabletranslation=false;
	pvt->_debugsqlrresultsettranslation=false;
	pvt->_debugsqlrresultsetrowtranslation=false;
	pvt->_debugsqlrresultsetrowblocktranslation=false;
	pvt->_debugsqlrresultsetheadertranslation=false;
	pvt->_debugsqlrmoduledata=false;

	pvt->_cur=NULL;

	pvt->_pidfile=NULL;

	pvt->_decrementonclose=false;
	pvt->_silent=false;

	pvt->_loggedinsec=0;
	pvt->_loggedinusec=0;

	pvt->_dbhostname=NULL;
	pvt->_dbipaddress=NULL;

	pvt->_reformattedfield=NULL;
	pvt->_reformattedfieldlength=0;

	pvt->_allglobaltemptables=false;

	pvt->_proxymode=false;
	pvt->_proxypid=0;

	pvt->_columnmap=NULL;

	pvt->_bulkserveridfilename=NULL;
	pvt->_bulkservershmem=NULL;
	pvt->_bulkservershm=NULL;
	pvt->_bulkservershmquery=NULL;
	pvt->_bulkservershmdataformat=NULL;
	pvt->_bulkclientshmem=NULL;
	pvt->_bulkclientshm=NULL;
	pvt->_bulkcursor=NULL;
	pvt->_bulkerrorfieldtable=NULL;
	pvt->_bulkerrorrowtable=NULL;
	pvt->_bulkmaxerrorcount=0;
	pvt->_bulkquerylen=0;
	pvt->_bulkquery=NULL;
	pvt->_bulkdataformat=NULL;
}

sqlrservercontroller::~sqlrservercontroller() {

	shutDown();

	if (pvt->_connstats) {
		bytestring::zero(pvt->_connstats,sizeof(sqlrconnstatistics));
	}

	delete pvt->_cmdl;
	delete pvt->_sqlrcfgs;

	delete[] pvt->_updown;

	delete[] pvt->_originaldb;

	delete pvt->_pth;

	delete pvt->_shmem;

	delete pvt->_semset;

	if (pvt->_unixsocket.getStringLength()) {
		file::remove(pvt->_unixsocket.getString());
	}

	delete pvt->_sqlrpr;
	delete pvt->_sqlrp;
	delete pvt->_sqlrd;
	delete pvt->_sqlrt;
	delete pvt->_sqlrf;
	delete pvt->_sqlrbvt;
	delete pvt->_sqlrrst;
	delete pvt->_sqlrrsrt;
	delete pvt->_sqlrrsrbt;
	delete pvt->_sqlrrsht;
	delete pvt->_sqlrtr;
	delete pvt->_sqlrlg;
	delete pvt->_sqlrn;
	delete pvt->_sqlrs;
	delete pvt->_sqlrq;
	delete pvt->_sqlrpe;
	delete pvt->_sqlra;
	delete pvt->_sqlrmd;

	delete[] pvt->_decrypteddbpassword;

	if (pvt->_pidfile) {
		file::remove(pvt->_pidfile);
		delete[] pvt->_pidfile;
	}

	delete[] pvt->_reformattedfield;

	for (singlylinkedlistnode< char * >
			*sln=pvt->_globaltemptables.getFirst();
						sln; sln=sln->getNext()) {
		delete[] sln->getValue();
	}

	delete pvt->_conn;

	if (pvt->_bulkserveridfilename) {
		file::remove(pvt->_bulkserveridfilename);
		delete[] pvt->_bulkserveridfilename;
	}
	delete pvt->_bulkservershmem;
	delete pvt->_bulkclientshmem;
	delete pvt->_bulkcursor;

	delete pvt;
}

bool sqlrservercontroller::init(int argc, const char **argv) {

	// process command line
	pvt->_cmdl=new sqlrcmdline(argc,argv);

	// initialize the paths
	pvt->_pth=new sqlrpaths(pvt->_cmdl);

	// default id warning
	if (!charstring::compare(pvt->_cmdl->getId(),DEFAULT_ID)) {
		stderror.printf("Warning: using default id.\n");
	}

	// get whether this connection was spawned by the scaler
	pvt->_scalerspawned=pvt->_cmdl->found("-scaler");

	// get the connection id from the command line
	pvt->_connectionid=pvt->_cmdl->getValue("-connectionid");
	if (charstring::isNullOrEmpty(pvt->_connectionid)) {
		pvt->_connectionid=DEFAULT_CONNECTIONID;
		stderror.printf("Warning: using default connectionid.\n");
	}

	// get the time to live from the command line
	const char	*ttlstr=pvt->_cmdl->getValue("-ttl");
	pvt->_ttl=(!charstring::isNullOrEmpty(ttlstr))?
				charstring::toInteger(ttlstr):-1;

	// should we run quietly?
	pvt->_silent=pvt->_cmdl->found("-silent");

	// load the configuration
	pvt->_sqlrcfgs=new sqlrconfigs(pvt->_pth);
	pvt->_cfg=pvt->_sqlrcfgs->load(pvt->_pth->getConfigUrl(),
						pvt->_cmdl->getId());
	if (!pvt->_cfg) {
		return false;
	}

	buildColumnMaps();
	setUserAndGroup();

	// update various configurable parameters
	pvt->_maxquerysize=pvt->_cfg->getMaxQuerySize();
	pvt->_maxbindcount=pvt->_cfg->getMaxBindCount();
	pvt->_maxerrorlength=pvt->_cfg->getMaxErrorLength();
	pvt->_idleclienttimeout=pvt->_cfg->getIdleClientTimeout();
	pvt->_debugsql=pvt->_cfg->getDebugSql();
	pvt->_debugbulkload=pvt->_cfg->getDebugBulkLoad();

	// get password encryptions
	domnode	*pwdencs=pvt->_cfg->getPasswordEncryptions();
	if (!pwdencs->isNullNode()) {
		pvt->_sqlrpe=new sqlrpwdencs(
			pvt->_pth,pvt->_cfg->getDebugPasswordEncryptions());
		pvt->_sqlrpe->load(pwdencs);
	}	

	// initialize auth
	domnode	*auths=pvt->_cfg->getAuths();
	if (!auths->isNullNode()) {
		pvt->_sqlra=new sqlrauths(this);
		pvt->_sqlra->load(auths,pvt->_sqlrpe);
	}

	// load database plugin
	pvt->_conn=initConnection(pvt->_cfg->getDbase());
	if (!pvt->_conn) {
		return false;
	}

	// get loggers
	domnode	*loggers=pvt->_cfg->getLoggers();
	if (!loggers->isNullNode()) {
		pvt->_sqlrlg=new sqlrloggers(pvt->_pth);
		pvt->_sqlrlg->load(loggers);
		pvt->_sqlrlg->init(NULL,pvt->_conn);
	}

	// get notifications
	domnode	*notifications=pvt->_cfg->getNotifications();
	if (!notifications->isNullNode()) {
		pvt->_sqlrn=new sqlrnotifications(pvt->_pth);
		pvt->_sqlrn->load(notifications);
	}

	// get schedules
	domnode	*schedules=pvt->_cfg->getSchedules();
	if (!schedules->isNullNode()) {
		pvt->_sqlrs=new sqlrschedules(this);
		pvt->_sqlrs->load(schedules);
	}

	// handle the pid file
	if (!handlePidFile()) {
		return false;
	}

	// handle the connect string
	pvt->_constr=pvt->_cfg->getConnectString(pvt->_connectionid);
	if (!pvt->_constr) {
		stderror.printf("Error: invalid connectionid \"%s\".\n",
							pvt->_connectionid);
		return false;
	}
	pvt->_conn->handleConnectString();

	initDatabaseAvailableFileName();

	// set unix socket filename (for suspended/resumed sessions)
	pvt->_unixsocket.append(pvt->_pth->getSocketsDir())->
				append((uint32_t)process::getProcessId())->
				append(".sock");

	if (!createSharedMemoryAndSemaphores(pvt->_cmdl->getId())) {
		return false;
	}

	// if there's no way to interrupt a semaphore wait,
	// then force the ttl to zero
	if (pvt->_ttl>0 &&
			!pvt->_semset->supportsTimedSemaphoreOperations() &&
			!sys::signalsInterruptSystemCalls()) {
		pvt->_ttl=0;
	}

	// log in and detach
	if (pvt->_conn->mustDetachBeforeLogIn() &&
			!pvt->_cmdl->found("-nodetach")) {
		process::detach();
	}
	bool	reloginatstart=pvt->_cfg->getReLoginAtStart();
	if (!reloginatstart) {
		if (!attemptLogIn(!pvt->_silent)) {
			return false;
		}
	}
	if (!pvt->_conn->mustDetachBeforeLogIn() &&
			!pvt->_cmdl->found("-nodetach")) {
		process::detach();
	}
	if (reloginatstart) {
		while (!attemptLogIn(false)) {
			snooze::macrosnooze(5);
		}
	}
	initConnStats();

	// get the module datas
	pvt->_debugsqlrmoduledata=pvt->_cfg->getDebugModuleDatas();
	domnode	*moduledatas=pvt->_cfg->getModuleDatas();
	if (!moduledatas->isNullNode()) {
		pvt->_sqlrmd=new sqlrmoduledatas(this);
		pvt->_sqlrmd->load(moduledatas);
	}

	// get the query directives
	pvt->_debugsqlrdirectives=pvt->_cfg->getDebugDirectives();
	domnode	*directives=pvt->_cfg->getDirectives();
	if (!directives->isNullNode()) {
		pvt->_sqlrd=new sqlrdirectives(this);
		pvt->_sqlrd->load(directives);
	}

	// get the query translations
	pvt->_debugsqlrtranslations=pvt->_cfg->getDebugTranslations();
	domnode	*translations=pvt->_cfg->getTranslations();
	if (!translations->isNullNode()) {
		pvt->_sqlrp=newParser();
		pvt->_sqlrt=new sqlrtranslations(this);
		pvt->_sqlrt->load(translations);
	}

	// get the query filters
	pvt->_debugsqlrfilters=pvt->_cfg->getDebugFilters();
	domnode	*filters=pvt->_cfg->getFilters();
	if (!filters->isNullNode()) {
		if (!pvt->_sqlrp) {
			pvt->_sqlrp=newParser();
		}
		pvt->_sqlrf=new sqlrfilters(this);
		pvt->_sqlrf->load(filters);
	}

	// get the bind variable translations
	pvt->_debugsqlrbindvariabletranslation=
				pvt->_cfg->getDebugBindVariableTranslations();
	domnode	*bindvariabletranslations=
				pvt->_cfg->getBindVariableTranslations();
	if (!bindvariabletranslations->isNullNode()) {
		pvt->_sqlrbvt=new sqlrbindvariabletranslations(this);
		pvt->_sqlrbvt->load(bindvariabletranslations);
	}

	// get the result set translations
	pvt->_debugsqlrresultsettranslation=
				pvt->_cfg->getDebugResultSetTranslations();
	domnode	*resultsettranslations=
				pvt->_cfg->getResultSetTranslations();
	if (!resultsettranslations->isNullNode()) {
		pvt->_sqlrrst=new sqlrresultsettranslations(this);
		pvt->_sqlrrst->load(resultsettranslations);
	}

	// get the result set row translations
	pvt->_debugsqlrresultsetrowtranslation=
				pvt->_cfg->getDebugResultSetRowTranslations();
	domnode	*resultsetrowtranslations=
				pvt->_cfg->getResultSetRowTranslations();
	if (!resultsetrowtranslations->isNullNode()) {
		pvt->_sqlrrsrt=new sqlrresultsetrowtranslations(this);
		pvt->_sqlrrsrt->load(resultsetrowtranslations);
	}

	// get the result set row block translations
	pvt->_debugsqlrresultsetrowblocktranslation=
			pvt->_cfg->getDebugResultSetRowBlockTranslations();
	domnode	*resultsetrowblocktranslations=
				pvt->_cfg->getResultSetRowBlockTranslations();
	if (!resultsetrowblocktranslations->isNullNode()) {
		pvt->_sqlrrsrbt=new sqlrresultsetrowblocktranslations(this);
		pvt->_sqlrrsrbt->load(resultsetrowblocktranslations);
	}

	// get the result set header translations
	pvt->_debugsqlrresultsetheadertranslation=
			pvt->_cfg->getDebugResultSetHeaderTranslations();
	domnode	*resultsetheadertranslations=
			pvt->_cfg->getResultSetHeaderTranslations();
	if (!resultsetheadertranslations->isNullNode()) {
		pvt->_sqlrrsht=new sqlrresultsetheadertranslations(this);
		pvt->_sqlrrsht->load(resultsetheadertranslations);
	}

	// get the triggers
	domnode	*triggers=pvt->_cfg->getTriggers();
	if (!triggers->isNullNode()) {
		// for triggers, we'll need an sqlrparser as well
		if (!pvt->_sqlrp) {
			pvt->_sqlrp=newParser();
		}
		pvt->_sqlrtr=new sqlrtriggers(this);
		pvt->_sqlrtr->load(triggers);
	}

	// get fake input bind variable behavior
	// (this may have already been set true by the connect string)
	pvt->_fakeinputbinds=(pvt->_fakeinputbinds ||
				pvt->_cfg->getFakeInputBindVariables());

	// get translate bind variable behavior
	pvt->_translatebinds=pvt->_cfg->getTranslateBindVariables();
	pvt->_questionmarksupported=
		pvt->_cfg->getBindVariableDelimiterQuestionMarkSupported();
	pvt->_colonsupported=
		pvt->_cfg->getBindVariableDelimiterColonSupported();
	pvt->_atsignsupported=
		pvt->_cfg->getBindVariableDelimiterAtSignSupported();
	pvt->_dollarsignsupported=
		pvt->_cfg->getBindVariableDelimiterDollarSignSupported();
	pvt->_debugbindtranslation=pvt->_cfg->getDebugBindTranslations();

	// initialize cursors
	pvt->_mincursorcount=pvt->_cfg->getCursors();
	pvt->_maxcursorcount=pvt->_cfg->getMaxCursors();
	if (!initCursors(pvt->_mincursorcount)) {
		closeCursors(false);
		logOut();
		return false;
	}

	// set autocommit behavior
	setAutoCommit(pvt->_initialautocommit);

	// create connection pid file
	pid_t	pid=process::getProcessId();
	charstring::printf(&pvt->_pidfile,
				"%ssqlr-connection-%s.%ld.pid",
				pvt->_pth->getPidDir(),
				pvt->_cmdl->getId(),
				(long)pid);
	process::createPidFile(pvt->_pidfile,permissions::ownerReadWrite());

	// increment connection counter
	if (pvt->_cfg->getDynamicScaling()) {
		incrementConnectionCount();
	}

	// set the transaction isolation level
	pvt->_isolationlevel=pvt->_cfg->getIsolationLevel();
	pvt->_conn->setIsolationLevel(pvt->_isolationlevel);

	// get the database/schema we're using so
	// we can switch back to it at end of session
	pvt->_originaldb=pvt->_conn->getCurrentDatabase();

	markDatabaseAvailable();

	// get the custom query handlers
	domnode	*queries=pvt->_cfg->getQueries();
	if (!queries->isNullNode()) {
		pvt->_sqlrq=new sqlrqueries(this);
		pvt->_sqlrq->load(queries);
	}

	// init client protocols
	pvt->_sqlrpr=new sqlrprotocols(this);
	pvt->_sqlrpr->load(pvt->_cfg->getListeners());

	// set a handler for SIGALARMs, if necessary
	#ifdef SIGALRM
	if (pvt->_ttl>0 &&
			!pvt->_semset->supportsTimedSemaphoreOperations() &&
			sys::signalsInterruptSystemCalls()) {
		alarmhandler.setHandler(alarmHandler);
		alarmhandler.handleSignal(SIGALRM);
	}
	#endif

	return true;
}

void sqlrservercontroller::setUserAndGroup() {

	// get the user that we're currently running as
	char	*currentuser=
		userentry::getName(process::getEffectiveUserId());

	// get the group that we're currently running as
	char	*currentgroup=
		groupentry::getName(process::getEffectiveGroupId());

	stringbuffer	errorstr;

	// switch groups, but only if we're not currently running as the
	// group that we should switch to
	if (charstring::compare(currentgroup,pvt->_cfg->getRunAsGroup()) &&
			!process::setGroup(pvt->_cfg->getRunAsGroup())) {
		errorstr.append("Warning: could not change group to ")->
			append(pvt->_cfg->getRunAsGroup())->append('\n');
	}

	// switch users, but only if we're not currently running as the
	// user that we should switch to
	if (charstring::compare(currentuser,pvt->_cfg->getRunAsUser()) &&
			!process::setUser(pvt->_cfg->getRunAsUser())) {
		errorstr.append("Warning: could not change user to ")->
			append(pvt->_cfg->getRunAsUser())->append('\n');
	}

	// write the error, if there was one
	stderror.write(errorstr.getString(),errorstr.getStringLength());

	// clean up
	delete[] currentuser;
	delete[] currentgroup;
}

sqlrserverconnection *sqlrservercontroller::initConnection(const char *dbase) {

#ifdef SQLRELAY_ENABLE_SHARED
	// load the connection module
	stringbuffer	modulename;
	modulename.append(pvt->_pth->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("connection_");
	modulename.append(dbase)->append(".")->append(SQLRELAY_MODULESUFFIX);
	if (!pvt->_conndl.open(modulename.getString(),true,true)) {
		char	*error=pvt->_conndl.getError();
		stderror.printf("failed to load connection module: %s\n%s\n",
				modulename.getString(),(error)?error:"");
		delete[] error;
		return NULL;
	}

	// load the connection itself
	stringbuffer	functionname;
	functionname.append("new_")->append(dbase)->append("connection");
	sqlrserverconnection	*(*newConn)(sqlrservercontroller *)=
			(sqlrserverconnection *(*)(sqlrservercontroller *))
			pvt->_conndl.getSymbol(functionname.getString());
	if (!newConn) {
		char	*error=pvt->_conndl.getError();
		stderror.printf("failed to load connection: %s\n%s\n",
				dbase,(error)?error:"");
		delete[] error;
		return NULL;
	}

	sqlrserverconnection	*conn=(*newConn)(this);

#else
	sqlrserverconnection	*conn;
	#include "sqlrserverconnectionassignments.cpp"
	{
		conn=NULL;
	}
#endif
	return conn;
}

bool sqlrservercontroller::handlePidFile() {

	// check for listener's pid file
	// (Look a few times.  It might not be there right away.  The listener
	// writes it out after forking and it's possible that the connection
	// might start up after the sqlr-listener has forked, but before it
	// writes out the pid file)
	char	*listenerpidfile=NULL;
	charstring::printf(&listenerpidfile,
				"%ssqlr-listener-%s.pid",
				pvt->_pth->getPidDir(),
				pvt->_cmdl->getId());

	// On most platforms, 1 second is plenty of time to wait for the
	// listener to come up, but on windows, it can take a while longer.
	// On 64-bit windows, when running 32-bit apps, listening on an inet
	// socket can take an especially long time.
	uint16_t	listenertimeout=10;
	char		*osname=sys::getOperatingSystemName();
	if (!charstring::compareIgnoringCase(osname,"Windows")) {
		listenertimeout=100;
		if ((!charstring::compareIgnoringCase(
				sys::getOperatingSystemArchitecture(),
				"x86_64") ||
			!charstring::compareIgnoringCase(
				sys::getOperatingSystemArchitecture(),
				"amd64")) &&
			sizeof(void *)==4) {
			listenertimeout=200;
		}
	}
	delete[] osname;

	bool	retval=true;
	bool	found=false;
	for (uint16_t i=0; !found && i<listenertimeout; i++) {
		if (i) {
			snooze::microsnooze(0,100000);
		}
		found=(process::checkForPidFile(listenerpidfile)!=-1);
	}
	if (!found) {
		stderror.printf("\n%s-connection error:\n"
				"	The pid file %s"
				" was not found.\n"
				"	This usually means "
					"that the %s-listener \n"
				"is not running.\n"
				"	The %s-listener must be running "
				"for the %s-connection to start.\n\n",
				SQLR,listenerpidfile,SQLR,SQLR,SQLR);
		retval=false;
	}

	delete[] listenerpidfile;

	return retval;
}

void sqlrservercontroller::initDatabaseAvailableFileName() {

	// initialize the database up/down filename
	charstring::printf(&pvt->_updown,
				"%s%s-%s.up",
				pvt->_pth->getIpcDir(),
				pvt->_cmdl->getId(),
				pvt->_connectionid);
}

bool sqlrservercontroller::attemptLogIn(bool printerrors) {

	raiseDebugMessageEvent("logging in...");

	// log in
	if (!logIn(printerrors)) {
		return false;
	}

	// get stats
	datetime	dt;
	dt.getSystemDateAndTime();
	pvt->_loggedinsec=dt.getSeconds();
	pvt->_loggedinusec=dt.getMicroseconds();

	raiseDebugMessageEvent("done logging in");
	return true;
}

bool sqlrservercontroller::logIn(bool printerrors) {

	// don't do anything if we're already logged in
	if (pvt->_loggedin) {
		return true;
	}

	// attempt to log in
	const char	*err=NULL;
	const char	*warning=NULL;
	if (!pvt->_conn->logIn(&err,&warning)) {
		if (printerrors) {
			stringbuffer	loginerror;
			loginerror.append("Couldn't log into database.\n");
			if (err) {
				loginerror.append(err)->append('\n');
			}
			stderror.write(loginerror.getString());
		}
		if (pvt->_sqlrlg) {
			pvt->_debugstr.clear();
			pvt->_debugstr.append("database login failed");
			if (err) {
				pvt->_debugstr.append(": ")->append(err);
			}
			raiseInternalErrorEvent(NULL,
					pvt->_debugstr.getString());
		}
		return false;
	}
	if (warning) {
		if (printerrors) {
			stderror.printf("Warning logging into database.\n%s\n",
					(warning)?warning:"");
		}
		if (pvt->_sqlrlg) {
			pvt->_debugstr.clear();
			pvt->_debugstr.append("database login warning");
			if (warning) {
				pvt->_debugstr.append(": ")->append(warning);
			}
			raiseInternalWarningEvent(NULL,
					pvt->_debugstr.getString());
		}
	}

	// success... update stats
	incrementOpenDatabaseConnections();

	// update db host name and ip address
	// (Only do this if logging is enabled.  The loggers use them, and if
	// someone forgot to put the database host name in DNS then it can
	// cause the connection to delay until a DNS timeout occurs.)
	if (pvt->_sqlrlg) {
		// this will cause the db host name and ip address
		// to be fetched from the db and stored locally
		dbHostName();
	}

	pvt->_loggedin=true;

	// If the db is behind a load balancer, we need to re-login
	// periodically to redistribute connections over newly added nodes.
	// Determine when to re-login next.
	if (pvt->_constr->getBehindLoadBalancer()) {
		datetime	dt;
		if (dt.getSystemDateAndTime()) {
			if (!pvt->_reloginseed) {
				// Ideally we'd use randomnumber:getSeed for
				// this, but on some platforms that's generated
				// from the epoch and could end up being the
				// same for all sqlr-connections.  The process
				// id is guaranteed unique.
				pvt->_reloginseed=process::getProcessId();
			}
			pvt->_reloginseed=randomnumber::generateNumber(
							pvt->_reloginseed);
			int32_t	seconds=randomnumber::scaleNumber(
							pvt->_reloginseed,
							600,900);
			pvt->_relogintime=dt.getEpoch()+seconds;
		}
	}

	raiseDbLogInEvent();

	return true;
}

void sqlrservercontroller::logOut() {

	// don't do anything if we're already logged out
	if (!pvt->_loggedin) {
		return;
	}

	raiseDebugMessageEvent("logging out...");

	// log out
	pvt->_conn->logOut();

	// update stats
	decrementOpenDatabaseConnections();

	raiseDbLogOutEvent();

	pvt->_loggedin=false;

	raiseDebugMessageEvent("done logging out");
}

void sqlrservercontroller::setAutoCommit(bool ac) {
	raiseDebugMessageEvent("setting autocommit...");
	if (ac) {
		if (!autoCommitOn()) {
			raiseDebugMessageEvent("setting autocommit on failed");
			stderror.printf("Couldn't set autocommit on.\n");
			return;
		}
	} else {
		if (!autoCommitOff()) {
			raiseDebugMessageEvent("setting autocommit off failed");
			stderror.printf("Couldn't set autocommit off.\n");
			return;
		}
	}
	raiseDebugMessageEvent("done setting autocommit");
}

bool sqlrservercontroller::initCursors(uint16_t count) {

	raiseDebugMessageEvent("initializing cursors...");

	pvt->_cursorcount=count;
	if (!pvt->_cur) {
		pvt->_cur=new sqlrservercursor *[pvt->_maxcursorcount];
		bytestring::zero(pvt->_cur,
				pvt->_maxcursorcount*
				sizeof(sqlrservercursor *));
	}

	for (uint16_t i=0; i<pvt->_cursorcount; i++) {

		if (!pvt->_cur[i]) {
			pvt->_cur[i]=newCursor(i);
		}
		if (!open(pvt->_cur[i])) {
			pvt->_debugstr.clear();
			pvt->_debugstr.append("cursor init failed: ");
			pvt->_debugstr.append(i);
			raiseInternalErrorEvent(NULL,
					pvt->_debugstr.getString());
			return false;
		}
	}

	raiseDebugMessageEvent("done initializing cursors");

	return true;
}

sqlrservercursor *sqlrservercontroller::newCursor(uint16_t id) {
	sqlrservercursor	*cursor=pvt->_conn->newCursor(id);
	if (cursor) {
		incrementOpenDatabaseCursors();
	}
	return cursor;
}

sqlrservercursor *sqlrservercontroller::newCursor() {
	// return a cursor with an ID that can't already exist
	return newCursor(pvt->_cursorcount+1);
}

void sqlrservercontroller::incrementConnectionCount() {

	raiseDebugMessageEvent("incrementing connection count...");

	if (pvt->_scalerspawned) {

		raiseDebugMessageEvent("scaler will do the job");
		signalScalerToRead();

	} else {

		acquireConnectionCountMutex();

		// increment the counter
		pvt->_shm->totalconnections++;
		pvt->_decrementonclose=true;

		releaseConnectionCountMutex();
	}

	raiseDebugMessageEvent("done incrementing connection count");
}

void sqlrservercontroller::decrementConnectionCount() {

	raiseDebugMessageEvent("decrementing connection count...");

	if (pvt->_scalerspawned) {

		raiseDebugMessageEvent("scaler will do the job");

	} else {

		acquireConnectionCountMutex();

		if (pvt->_shm->totalconnections) {
			pvt->_shm->totalconnections--;
		}
		pvt->_decrementonclose=false;

		releaseConnectionCountMutex();
	}

	raiseDebugMessageEvent("done decrementing connection count");
}

void sqlrservercontroller::markDatabaseAvailable() {

	pvt->_debugstr.clear();
	pvt->_debugstr.append("creating ")->append(pvt->_updown);
	raiseDebugMessageEvent(pvt->_debugstr.getString());

	// the database is up if the file is there, 
	// opening and closing it will create it
	file	fd;
	fd.create(pvt->_updown,permissions::ownerReadWrite());
}

void sqlrservercontroller::markDatabaseUnavailable() {

	// if the database is behind a load balancer, don't mark it unavailable
	if (pvt->_constr->getBehindLoadBalancer()) {
		return;
	}

	pvt->_debugstr.clear();
	pvt->_debugstr.append("unlinking ")->append(pvt->_updown);
	raiseDebugMessageEvent(pvt->_debugstr.getString());

	// the database is down if the file isn't there
	file::remove(pvt->_updown);
}

bool sqlrservercontroller::openSockets() {

	raiseDebugMessageEvent("listening on sockets...");

	// open the unix socket
	if (pvt->_cfg->getListenOnUnix() && !pvt->_serversockun) {

		pvt->_serversockun=new unixsocketserver();
		if (pvt->_serversockun->listen(
				pvt->_unixsocket.getString(),0000,128)) {
			pvt->_debugstr.clear();
			pvt->_debugstr.append("listening on unix socket: ");
			pvt->_debugstr.append(pvt->_unixsocket.getString());
			raiseDebugMessageEvent(pvt->_debugstr.getString());

			pvt->_lsnr.addReadFileDescriptor(pvt->_serversockun);
		} else {
			pvt->_debugstr.clear();
			pvt->_debugstr.append("failed to listen on socket: ");
			pvt->_debugstr.append(pvt->_unixsocket.getString());
			raiseInternalErrorEvent(NULL,
						pvt->_debugstr.getString());

			char	*currentuser=
					userentry::getName(
						process::getEffectiveUserId());
			char	*currentgroup=
					groupentry::getName(
						process::getEffectiveGroupId());
			stderror.printf("Could not listen on unix socket: %s\n"
					"Make sure that the directory is "
					"writable by %s:%s.\n\n",
					pvt->_unixsocket.getString(),
					currentuser,currentgroup);
			delete[] currentuser;
			delete[] currentgroup;
			delete pvt->_serversockun;
			pvt->_serversockun=NULL;
			return false;
		}
	}

	bool	retval=true;

	// open the next available inet socket
	if (pvt->_cfg->getListenOnInet() && !pvt->_serversockin) {

		const char	*addresses=pvt->_cfg->getDefaultAddresses();

		char		**addr=NULL;
		uint64_t	addrcount=0;
		charstring::split(addresses,",",true,&addr,&addrcount);

		pvt->_serversockincount=addrcount;
		pvt->_serversockin=new inetsocketserver *[addrcount];

		uint64_t	index=0;
		for (index=0; index<pvt->_serversockincount; index++) {
			pvt->_serversockin[index]=NULL;
			if (!retval) {
				continue;
			}
			pvt->_serversockin[index]=new inetsocketserver();
			if (pvt->_serversockin[index]->
				listen(addr[index],pvt->_inetport,128)) {

				if (!pvt->_inetport) {
					pvt->_inetport=
					pvt->_serversockin[index]->getPort();
				}

				char	string[33];
				charstring::printf(string,sizeof(string),
						"listening on inet socket: %d",
						pvt->_inetport);
				raiseDebugMessageEvent(string);

				pvt->_lsnr.addReadFileDescriptor(
						pvt->_serversockin[index]);

			} else {
				pvt->_debugstr.clear();
				pvt->_debugstr.append(
						"failed to listen on port: ");
				pvt->_debugstr.append(pvt->_inetport);
				raiseInternalErrorEvent(NULL,
						pvt->_debugstr.getString());

				stderror.printf("Could not listen on "
						"inet socket: ",
						"%d\n\n",pvt->_inetport);
				retval=false;
			}
		}

		if (!retval) {
			// clean up
			for (index=0; index<pvt->_serversockincount; index++) {
				delete pvt->_serversockin[index];
			}
			delete[] pvt->_serversockin;
			pvt->_serversockincount=0;
		}

		// clean up addresses
		for (index=0; index<addrcount; index++) {
			delete[] addr[index];
		}
		delete[] addr;
	}

	raiseDebugMessageEvent("done listening on sockets");

	return retval;
}

bool sqlrservercontroller::listen() {

	uint16_t	sessioncount=0;

	int32_t		softttl=pvt->_cfg->getSoftTtl();
	datetime	startdt;
	startdt.getSystemDateAndTime();

	bool		clientconnectfailed=false;

	for (;;) {

		waitForAvailableDatabase();
		initSession();
		if (!announceAvailability(NULL,0,pvt->_connectionid)) {
			return true;
		}

		// loop to handle suspended sessions
		bool	loopback=false;
		for (;;) {

			int	success=waitForClient();

			if (success==1) {

				pvt->_suspendedsession=false;

				// have a session with the client
				clientSession();

				// break out of the loop unless the client
				// suspended the session
				if (!pvt->_suspendedsession) {
					break;
				}

			} else if (success==2) {

				// This is a special case, basically it means
				// that the listener wants the connection to
				// reconnect to the database.  Just loop back
				// so that can be handled naturally.
				loopback=true;
				break;

			} else if (success==-1) {

				// If waitForClient() errors out, break out of
				// the suspendedsession loop, loop back for
				// another session, and close connection if
				// it is possible.  Otherwise wait for session.
				// But it seems that under heavy load that it's
				// impossible to change handoff socket for pid.
				clientconnectfailed=true;
				break;

			} else if (success==0) {

				// If waitForClient() times out or otherwise
				// fails to wait for someone to pick up the
				// suspended session then roll back and break.
				if (pvt->_conn->isTransactional()) {
					rollback();
				}
				pvt->_suspendedsession=false;
				break;
			}
		}

		if (!loopback && pvt->_cfg->getDynamicScaling()) {

			decrementConnectedClientCount();

			// for dynamically spawned connections, bail on
			// various conditions...
			if (pvt->_scalerspawned) {

				// if the client that this was spawned for
				// failed to connect...
				if (clientconnectfailed) {
					return false;
				}

				// if the ttl is 0...
				if (!pvt->_ttl) {
					return true;
				}

				// if we've already handled some number of
				// client sessions...
				if (pvt->_ttl>0 &&
					pvt->_cfg->getMaxSessionCount()) {
					sessioncount++;
					if (sessioncount==
					pvt->_cfg->getMaxSessionCount()) {
						return true;
					}
				}

				// if we've been alive for too long...
				if (softttl>0) {
					datetime	currentdt;
					currentdt.getSystemDateAndTime();
					if (currentdt.getEpoch()-
						startdt.getEpoch()>=softttl) {
						return true;
					}
				}
			}
		}
	}
}

void sqlrservercontroller::waitForAvailableDatabase() {

	raiseDebugMessageEvent("waiting for available database...");

	setState(WAIT_FOR_AVAIL_DB);

	if (!file::exists(pvt->_updown)) {
		raiseDebugMessageEvent("database is not available");
		reLogIn();
		markDatabaseAvailable();
	}

	raiseDebugMessageEvent("database is available");
}

void sqlrservercontroller::reLogIn() {

	markDatabaseUnavailable();

	// run the session end queries
	// FIXME: only run these if a dead connection prompted
	// a relogin, not if we couldn't login at startup
	sessionEndQueries();

	// get the current db so we can restore it
	char	*currentdb=pvt->_conn->getCurrentDatabase();

	// FIXME: get the isolation level so we can restore it

	raiseDebugMessageEvent("relogging in...");

	// attempt to log in over and over, once every 5 seconds
	int32_t	oldcursorcount=pvt->_cursorcount;
	closeCursors(false);
	logOut();
	for (;;) {
			
		raiseDebugMessageEvent("trying...");

		incrementReLogInCount();

		if (logIn(false)) {
			if (!initCursors(oldcursorcount)) {
				closeCursors(false);
				logOut();
			} else {
				break;
			}
		}
		snooze::macrosnooze(5);
	}

	raiseDebugMessageEvent("done relogging in");

	// run the session-start queries
	// FIXME: only run these if a dead connection prompted
	// a relogin, not if we couldn't login at startup
	sessionStartQueries();

	// restore the db
	pvt->_conn->selectDatabase(currentdb);
	delete[] currentdb;

	// restore initial autocommit behavior
	if (pvt->_initialautocommit) {
		pvt->_conn->autoCommitOn();
	} else {
		pvt->_conn->autoCommitOff();
	}

	// FIXME: restore the isolation level

	markDatabaseAvailable();
}

void sqlrservercontroller::initSession() {

	raiseDebugMessageEvent("initializing session...");

	pvt->_needscommitorrollback=false;
	pvt->_suspendedsession=false;
	for (int32_t i=0; i<pvt->_cursorcount; i++) {
		pvt->_cur[i]->setState(SQLRCURSORSTATE_AVAILABLE);
	}
	pvt->_accepttimeout=5;

	raiseDebugMessageEvent("done initializing session...");
}

bool sqlrservercontroller::announceAvailability(const char *unixsocket,
						unsigned short inetport,
						const char *connectionid) {

	// FIXME: unixsocket and inetport are unused and
	// should be removed in the next minor release

	raiseDebugMessageEvent("announcing availability...");

	// connect to listener if we haven't already
	// and pass it this process's pid
	if (!pvt->_connected) {
		registerForHandoff();
	}

	// save the original ttl
	int32_t	originalttl=pvt->_ttl;

	// get the time before announcing
	time_t	before=0;
	if (originalttl>0) {
		datetime	dt;
		dt.getSystemDateAndTime();
		before=dt.getEpoch();
	}

	// This will fall through if the ttl was reached while waiting.
	// In that case, since we failed to acquire the announce mutex,
	// we don't need to release it.  We also don't need to reset the
	// ttl because we're going to exit.
	if (!acquireAnnounceMutex()) {
		raiseDebugMessageEvent("ttl reached, "
					"aborting announcing availabilty");
		return false;
	}

	setState(ANNOUNCE_AVAILABILITY);

	// write the connectionid and pid into the segment
	charstring::copy(pvt->_shm->connectionid,
				connectionid,MAXCONNECTIONIDLEN);
	pvt->_shm->connectioninfo.connectionpid=process::getProcessId();

	signalListenerToRead();

	// get the time after announcing and update the ttl
	if (originalttl>0) {
		datetime	dt;
		dt.getSystemDateAndTime();
		pvt->_ttl=pvt->_ttl-(dt.getEpoch()-before);
	}

	// This will fall through if the ttl was reached while waiting.
	// Since we acquired the announce mutex earlier though, we need to
	// release it in either case.
	bool	success=false;
	if (originalttl<=0 || pvt->_ttl) {
		success=waitForListenerToFinishReading();
	}

	releaseAnnounceMutex();

	if (success) {
		// reset ttl
		pvt->_ttl=originalttl;

		raiseDebugMessageEvent("done announcing availability...");
	} else {
		// a timeout must have occurred...

		// We signalled earlier in signalListenerToRead() but now we
		// need to undo that operation.  We don't want to rely on
		// undo's though because this isn't a mutex and not all
		// platforms support undo's.
		unSignalListenerToRead();

		// Close the handoff socket.  At this point, the listener
		// will have read the connection data and will attempt to
		// hand off the client to this connection.  The socket must
		// be closed when it tries so the handoff will fail and the
		// listener can loop back and try again with a different
		// connection.
		pvt->_handoffsockun.close();

		raiseDebugMessageEvent("ttl reached, "
					"aborting announcing availabilty");
	}

	// signal the listener to hand off...
	// Do this whether the wait above timed out or not.  At this point,
	// the listener is committed to using this connection.  If a timeout
	// did occur, and this connection is going to exit, that's OK.  Since
	// the handoff socket was closed above, the handoff will fail, and the
	// listener will loop back and try again with a different connection.
	signalListenerToHandoff();

	return success;
}

void sqlrservercontroller::registerForHandoff() {

	raiseDebugMessageEvent("registering for handoff...");

	// construct the name of the socket to connect to
	char	*handoffsockname=NULL;
	charstring::printf(&handoffsockname,
				"%s%s-handoff.sock",
				pvt->_pth->getSocketsDir(),
				pvt->_cmdl->getId());

	pvt->_debugstr.clear();
	pvt->_debugstr.append("handoffsockname: ")->append(handoffsockname);
	raiseDebugMessageEvent(pvt->_debugstr.getString());

	// Try to connect over and over forever on 1 second intervals.
	// If the connect succeeds but the write fails, loop back and
	// try again.
	pvt->_connected=false;
	for (;;) {

		raiseDebugMessageEvent("trying...");

		if (pvt->_handoffsockun.connect(handoffsockname,
							-1,-1,1,0)==
							RESULT_SUCCESS) {
			if (pvt->_handoffsockun.write(
				(uint32_t)process::getProcessId())==
							sizeof(uint32_t)) {
				pvt->_connected=true;
				break;
			}
			pvt->_handoffsockun.flushWriteBuffer(-1,-1);
			deRegisterForHandoff();
		}
		snooze::macrosnooze(1);
	}

	raiseDebugMessageEvent("done registering for handoff");

	delete[] handoffsockname;
}

void sqlrservercontroller::deRegisterForHandoff() {
	
	raiseDebugMessageEvent("de-registering for handoff...");

	// construct the name of the socket to connect to
	char	*removehandoffsockname=NULL;
	charstring::printf(&removehandoffsockname,
				"%s%s-removehandoff.sock",
				pvt->_pth->getSocketsDir(),
				pvt->_cmdl->getId());

	pvt->_debugstr.clear();
	pvt->_debugstr.append("removehandoffsockname: ");
	pvt->_debugstr.append(removehandoffsockname);
	raiseDebugMessageEvent(pvt->_debugstr.getString());

	// attach to the socket and write the process id
	unixsocketclient	removehandoffsockun;
	removehandoffsockun.connect(removehandoffsockname,-1,-1,0,1);
	removehandoffsockun.write((uint32_t)process::getProcessId());
	removehandoffsockun.flushWriteBuffer(-1,-1);

	raiseDebugMessageEvent("done de-registering for handoff");

	delete[] removehandoffsockname;
}

int32_t sqlrservercontroller::waitForClient() {

	raiseDebugMessageEvent("waiting for client...");

	setState(WAIT_CLIENT);

	// reset proxy mode flag
	pvt->_proxymode=false;

	if (!pvt->_suspendedsession) {

		// If we're not in the middle of a suspended session,
		// talk to the listener...


		// the client file descriptor
		int32_t	descriptor;

		// What is this loop all aboout?
		// If the listener is proxying clients, it can't tell whether
		// the client succeeded in transmitting an END_SESSION or
		// whether it even tried, so it sends one when the client
		// disconnects no matter what.  If the client did send one then
		// we'll receive a second END_SESSION here.  Depending on the
		// byte order of the host, we'll receive either a 1536 or 6.
		// If we got an END_SESION then just loop back and read again,
		// the command will follow.
		uint16_t	command;
		do {
			// get the command
			if (pvt->_handoffsockun.read(&command)!=
							sizeof(uint16_t)) {
				raiseInternalErrorEvent(NULL,
					"read handoff command failed");
				raiseDebugMessageEvent("done waiting for client");
				// If this fails, then the listener most likely
				// died because sqlr-stop was run.  Arguably
				// this condition should initiate a shut down
				// of this process as well, but for now we'll
				// just wait to be shut down manually.
				// Unfortunatley, that means looping over and
				// over, with that read failing every time.
				// We'll sleep so as not to slam the machine
				// while we loop.
				snooze::microsnooze(0,100000);
				return -1;
			}
		} while (command==1536 || command==6);

		if (command==HANDOFF_RECONNECT) {

			// if we're supposed to reconnect, then just do that...
			return 2;

		} else if (command==HANDOFF_PASS) {

			if (!getProtocol()) {
				return -1;
			}

			// Receive the client file descriptor and use it.
			if (!pvt->_handoffsockun.receiveSocket(&descriptor)) {
				raiseInternalErrorEvent(NULL,"failed to receive "
						"client file descriptor");
				raiseDebugMessageEvent("done waiting for client");
				// If this fails, then the listener most likely
				// died because sqlr-stop was run.  Arguably
				// this condition should initiate a shut down
				// of this process as well, but for now we'll
				// just wait to be shut down manually.
				// Unfortunatley, that means looping over and
				// over, with that read above failing every
				// time, thus the  sleep so as not to slam the
				// machine while we loop.
				return -1;
			}

		} else if (command==HANDOFF_PROXY) {

			if (!getProtocol()) {
				return -1;
			}

			raiseDebugMessageEvent("listener is proxying the client");

			// get the listener's pid
			if (pvt->_handoffsockun.read(&pvt->_proxypid)!=
							sizeof(uint32_t)) {
				raiseInternalErrorEvent(NULL,
						"failed to read process "
						"id during proxy handoff");
				return -1;
			}

			pvt->_debugstr.clear();
			pvt->_debugstr.append("listener pid: ");
			pvt->_debugstr.append(pvt->_proxypid);
			raiseDebugMessageEvent(pvt->_debugstr.getString());

			// acknowledge
			#define ACK	6
			pvt->_handoffsockun.write((unsigned char)ACK);
			pvt->_handoffsockun.flushWriteBuffer(-1,-1);

			descriptor=pvt->_handoffsockun.getFileDescriptor();

			pvt->_proxymode=true;

		} else {

			raiseInternalErrorEvent(NULL,"received invalid handoff mode");
			return -1;
		}

		// set up the client socket
		pvt->_clientsock=new socketclient;
		pvt->_clientsock->setFileDescriptor(descriptor);

		// On most systems, the file descriptor is in whatever mode
		// it was in the other process, but on FreeBSD < 5.0 and
		// possibly other systems, it ends up in non-blocking mode
		// in this process, independent of its mode in the other
		// process.  So, we force it to blocking mode here.
		pvt->_clientsock->useBlockingMode();

		raiseDebugMessageEvent("done waiting for client");

	} else {

		// If we're in the middle of a suspended session, wait for
		// a client to reconnect...

		if (pvt->_lsnr.listen(pvt->_accepttimeout,0)<1) {
			raiseInternalErrorEvent(NULL,"wait for client connect failed");
			return 0;
		}

		// get the first socket that had data available...
		filedescriptor	*fd=pvt->_lsnr.getReadReadyList()->
						getFirst()->getValue();

		inetsocketserver	*iss=NULL;
		for (uint64_t index=0; index<pvt->_serversockincount; index++) {
			if (fd==pvt->_serversockin[index]) {
				iss=pvt->_serversockin[index];
			}
		}
		if (iss) {
			pvt->_clientsock=iss->accept();
		} else if (fd==pvt->_serversockun) {
			pvt->_clientsock=pvt->_serversockun->accept();
		}

		if (fd) {
			raiseDebugMessageEvent("client reconnect succeeded");
		} else {
			raiseInternalErrorEvent(NULL,"client reconnect failed");
		}
		raiseDebugMessageEvent("done waiting for client");

		if (!fd) {
			return 0;
		}
	}

	return 1;
}

bool sqlrservercontroller::getProtocol() {

	raiseDebugMessageEvent("getting the protocol index...");

	// get protocol index
	if (pvt->_handoffsockun.read(&pvt->_protocolindex)!=sizeof(uint16_t)) {
		raiseDebugMessageEvent("failed to get the client protocol index");
		return false;
	}

	raiseDebugMessageEvent("done getting the client protocol...");
	return true;
}

void sqlrservercontroller::clientSession() {

	raiseDebugMessageEvent("client session...");

	pvt->_inclientsession=true;

	// update various stats
	setState(SESSION_START);
	setClientAddr();
	setClientSessionStartTime();
	incrementOpenClientConnections();

	raiseClientConnectedEvent();

	// have client session using the appropriate protocol
	pvt->_currentprotocol=pvt->_sqlrpr->getProtocol(pvt->_protocolindex);
	clientsessionexitstatus_t	exitstatus=
					CLIENTSESSIONEXITSTATUS_ERROR;
	if (pvt->_currentprotocol) {
		exitstatus=pvt->_currentprotocol->clientSession(
							pvt->_clientsock);
	} else {
		closeClientConnection(0);
	}

	closeSuspendedSessionSockets();

	const char	*info;
	switch (exitstatus) {
		case CLIENTSESSIONEXITSTATUS_CLOSED_CONNECTION:
			info="client closed connection";
			break;
		case CLIENTSESSIONEXITSTATUS_ENDED_SESSION:
			info="client ended the session";
			break;
		case CLIENTSESSIONEXITSTATUS_SUSPENDED_SESSION:
			info="client suspended the session";
			break;
		case CLIENTSESSIONEXITSTATUS_ERROR:
		default:
			// Don't use the word "error" here.
			//
			// Conditions that result in
			// CLIENTSESSIONEXITSTATUS_ERROR
			// might not warrant investigation by operations staff.
			//
			// For example:
			// * Programs can crash or exit at just the right
			// 	moment.
			// * Load balancers often check to be sure a service is
			// 	still running by just connecting and
			// 	disconnecting.
			// * Ad-hock sqlrsh users might supply invalid
			//	credentials.
			//
			// We don't want "error" making its way into the logs
			// or log analyzers will generate a bunch of
			// false-positives.
			//
			// If a "real" error occurs, it will be reported
			// elsewhere.
			info="server closed connection";
			break;
	}
	raiseClientDisconnectedEvent(info);

	decrementOpenClientConnections();

	pvt->_inclientsession=false;

	raiseDebugMessageEvent("done with client session");
}

sqlrservercursor *sqlrservercontroller::getCursor(uint16_t id) {

	// get the specified cursor
	for (uint16_t i=0; i<pvt->_cursorcount; i++) {
		if (pvt->_cur[i]->getId()==id) {
			incrementTimesCursorReused(); 
			return pvt->_cur[i];
		}
	}

	pvt->_debugstr.clear();
	pvt->_debugstr.append("get cursor failed: "
				"client requested an invalid cursor: ");
	pvt->_debugstr.append(id);
	raiseClientProtocolErrorEvent(NULL,pvt->_debugstr.getString(),1);

	return NULL;
}

sqlrservercursor *sqlrservercontroller::getCursor() {

	// find an available cursor
	for (uint16_t i=0; i<pvt->_cursorcount; i++) {
		if (pvt->_cur[i]->getState()==SQLRCURSORSTATE_AVAILABLE) {
			pvt->_debugstr.clear();
			pvt->_debugstr.append("available cursor: ")->append(i);
			raiseDebugMessageEvent(pvt->_debugstr.getString());
			pvt->_cur[i]->setState(SQLRCURSORSTATE_BUSY);
			incrementTimesNewCursorUsed();
			return pvt->_cur[i];
		}
	}

	// apparently there weren't any available cursors...

	// if we can't create any new cursors then return an error
	if (pvt->_cursorcount==pvt->_maxcursorcount) {
		raiseDebugMessageEvent("all cursors are busy");
		return NULL;
	}

	// create new cursors
	uint16_t	expandto=pvt->_cursorcount+
					pvt->_cfg->getCursorsGrowBy();
	if (expandto>=pvt->_maxcursorcount) {
		expandto=pvt->_maxcursorcount;
	}
	uint16_t	firstnewcursor=pvt->_cursorcount;
	do {
		pvt->_cur[pvt->_cursorcount]=newCursor(pvt->_cursorcount);
		pvt->_cur[pvt->_cursorcount]->
				setState(SQLRCURSORSTATE_AVAILABLE);
		if (!open(pvt->_cur[pvt->_cursorcount])) {
			pvt->_debugstr.clear();
			pvt->_debugstr.append("cursor init failure: ");
			pvt->_debugstr.append(pvt->_cursorcount);
			raiseInternalErrorEvent(NULL,
					pvt->_debugstr.getString());
			return NULL;
		}
		pvt->_cursorcount++;
	} while (pvt->_cursorcount<expandto);
	
	// return the first new cursor that we created
	pvt->_cur[firstnewcursor]->setState(SQLRCURSORSTATE_BUSY);
	incrementTimesNewCursorUsed();
	return pvt->_cur[firstnewcursor];
}

sqlrcredentials *sqlrservercontroller::getCredentials(const char *user,
							const char *password,
							bool usegss,
							bool usetls) {

	// try to use gss credentials
	if (usegss) {

		gsscontext	*ctx=getGSSContext();
		if (ctx) {
			sqlrgsscredentials	*gsscred=
					new sqlrgsscredentials();
			gsscred->setInitiator(ctx->getInitiator());
			return gsscred;
		}
		return NULL;
	}

	// try to use tls credentials
	// (unless a user was passed in)
	if (usetls && charstring::isNullOrEmpty(user)) {

		tlscontext	*ctx=getTLSContext();
		if (ctx) {
			tlscertificate	*cert=ctx->getPeerCertificate();
			if (cert) {
				sqlrtlscredentials	*tlscred=
					new sqlrtlscredentials();
				tlscred->setSubjectAlternateNames(
				cert->getSubjectAlternateNames());
				tlscred->setCommonName(
					cert->getCommonName());
				return tlscred;
			}
		}
		return NULL;
	}

	// use user/password credentials
	sqlruserpasswordcredentials	*upcred=
					new sqlruserpasswordcredentials();
	upcred->setUser(user);
	upcred->setPassword(password);
	return upcred;
}

bool sqlrservercontroller::auth(sqlrcredentials *cred) {

	raiseDebugMessageEvent("auth...");

	// authenticate
	const char	*autheduser=NULL;
	if (pvt->_sqlra) {
		autheduser=pvt->_sqlra->auth(cred);
	}
	if (autheduser) {

		raiseDebugMessageEvent("auth success");
		setCurrentUser(autheduser,charstring::length(autheduser));

		// consult connection schedules
		if (pvt->_sqlrs &&
			!pvt->_sqlrs->allowed(pvt->_conn,getCurrentUser())) {
			raiseDebugMessageEvent("connection schedule violation");
			raiseScheduleViolationEvent(getCurrentUser());
			return false;
		}

		return true;
	}

	raiseDebugMessageEvent("auth failed");
	raiseClientConnectionRefusedEvent("auth failed");
	return false;
}

bool sqlrservercontroller::changeUser(const char *newuser,
					const char *newpassword) {
	raiseDebugMessageEvent("change user");
	closeCursors(false);
	logOut();
	setUser(newuser);
	setPassword(newpassword);
	return (logIn(false) && initCursors(pvt->_cursorcount));
}

bool sqlrservercontroller::changeProxiedUser(const char *newuser,
						const char *newpassword) {
	raiseDebugMessageEvent("change proxied user");
	return pvt->_conn->changeProxiedUser(newuser,newpassword);
}

void sqlrservercontroller::beginSession() {
	sessionStartQueries();
}

void sqlrservercontroller::suspendSession(const char **unixsocket,
						uint16_t *inetport) {

	// mark the session suspended
	pvt->_suspendedsession=true;

	// we can't wait forever for the client to resume, set a timeout
	pvt->_accepttimeout=pvt->_cfg->getSessionTimeout();

	// abort all cursors that aren't suspended...
	raiseDebugMessageEvent("aborting busy cursors...");
	for (int32_t i=0; i<pvt->_cursorcount; i++) {
		if (pvt->_cur[i]->getState()==SQLRCURSORSTATE_BUSY) {
			pvt->_cur[i]->abort();
		}
	}
	raiseDebugMessageEvent("done aborting busy cursors");

	// open sockets to resume on
	raiseDebugMessageEvent("opening sockets to resume on...");
	*unixsocket=NULL;
	*inetport=0;
	if (openSockets()) {
		if (pvt->_serversockun) {
			*unixsocket=pvt->_unixsocket.getString();
		}
		*inetport=pvt->_inetport;
	}
	raiseDebugMessageEvent("done opening sockets to resume on");
}

bool sqlrservercontroller::autoCommitOn() {

	if (pvt->_debugsql) {
		stdoutput.printf("\n===================="
				 "===================="
				 "===================="
				 "===================\n\n");
		stdoutput.printf("%d: autocommit on\n",
					process::getProcessId());
	}

	pvt->_autocommitforthissession=true;
	if (pvt->_conn->autoCommitOn()) {
		if (pvt->_intransaction) {
			raiseCommitEvent();
		}
		pvt->_intransaction=false;
		return true;
	}
	return false;
}

bool sqlrservercontroller::autoCommitOff() {

	if (pvt->_debugsql) {
		stdoutput.printf("\n===================="
				 "===================="
				 "===================="
				 "===================\n\n");
		stdoutput.printf("%d: autocommit off\n",
					process::getProcessId());
	}

	pvt->_autocommitforthissession=false;
	if (pvt->_conn->autoCommitOff()) {
		// if the db doesn't support transaction blocks (oracle,
		// firebird, informix) then we are in a transaction here,
		// otherwise we aren't
		//pvt->_intransaction=!pvt->_conn->supportsTransactionBlocks();
		// actually, it seems that in db's that support transaction
		// blocks, setting autocommit off is about the same as running
		// a begin/start-tx query, so we're in a transaction no matter
		// what...
		// FIXME: verify this though, with all db's
		bool	wasintx=pvt->_intransaction;
		pvt->_intransaction=true;
		if (!wasintx) {
			raiseBeginTransactionEvent();
		}
		return true;
	}
	return false;
}

bool sqlrservercontroller::begin() {

	if (pvt->_debugsql) {
		stdoutput.printf("\n===================="
				 "===================="
				 "===================="
				 "===================\n\n");
		stdoutput.printf("%d: begin\n",
					process::getProcessId());
	}

	// if we're faking transaction blocks, do that,
	// otherwise run an actual begin query
	if ((pvt->_faketransactionblocks)?
			beginFakeTransactionBlock():
			pvt->_conn->begin()) {
		pvt->_intransaction=true;
		raiseBeginTransactionEvent();
		return true;
	}
	return false;
}

bool sqlrservercontroller::beginFakeTransactionBlock() {

	// save the current autocommit state
	pvt->_faketransactionblocksautocommiton=pvt->_autocommitforthissession;

	// if autocommit is on, turn it off
	if (pvt->_autocommitforthissession) {
		if (!autoCommitOff()) {
			return false;
		}
	}
	pvt->_infaketransactionblock=true;
	return true;
}

bool sqlrservercontroller::commit() {

	if (pvt->_debugsql) {
		stdoutput.printf("\n===================="
				 "===================="
				 "===================="
				 "===================\n\n");
		stdoutput.printf("%d: commit\n",
					process::getProcessId());
	}

	if (pvt->_conn->commit()) {
		endTransaction(true);
		return true;
	}
	return false;
}

bool sqlrservercontroller::endFakeTransactionBlock() {

	// if we're faking begins and autocommit is on,
	// reset autocommit behavior
	if (pvt->_faketransactionblocks &&
		pvt->_faketransactionblocksautocommiton) {
		if (!autoCommitOn()) {
			return false;
		}
	}
	pvt->_infaketransactionblock=false;
	return true;
}

void sqlrservercontroller::endTransaction(bool commit) {

	// end fake transaction blocks
	// FIXME: this can fail
	endFakeTransactionBlock();

	// raise events
	if (commit) {
		raiseCommitEvent();
	} else {
		raiseRollbackEvent();
	}

	// reset protocol modules
	if (pvt->_sqlrpr) {
		pvt->_sqlrpr->endTransaction(commit);
	}

	// reset translation modules
	if (pvt->_sqlrt) {
		pvt->_sqlrt->endTransaction(commit);
	}

	// reset filter modules
	if (pvt->_sqlrf) {
		pvt->_sqlrf->endTransaction(commit);
	}

	// reset bind variable translation modules
	if (pvt->_sqlrbvt) {
		pvt->_sqlrbvt->endTransaction(commit);
	}

	// reset result set header translation modules
	if (pvt->_sqlrrsht) {
		pvt->_sqlrrsht->endTransaction(commit);
	}

	// reset result set translation modules
	if (pvt->_sqlrrst) {
		pvt->_sqlrrst->endTransaction(commit);
	}

	// reset result set row translation modules
	if (pvt->_sqlrrsrt) {
		pvt->_sqlrrsrt->endTransaction(commit);
	}

	// reset result set row block translation modules
	if (pvt->_sqlrrsrbt) {
		pvt->_sqlrrsrbt->endTransaction(commit);
	}

	// reset trigger modules
	if (pvt->_sqlrtr) {
		pvt->_sqlrtr->endTransaction(commit);
	}

	// reset logger modules
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->endTransaction(commit);
	}

	// reset notification modules
	if (pvt->_sqlrn) {
		pvt->_sqlrn->endTransaction(commit);
	}

	// reset query modules
	if (pvt->_sqlrq) {
		pvt->_sqlrq->endTransaction(commit);
	}

	// clear per-session pool
	pvt->_txpool.clear();

	// set in-tx flag
	pvt->_intransaction=!pvt->_autocommitforthissession;
}

bool sqlrservercontroller::rollback() {

	if (pvt->_debugsql) {
		stdoutput.printf("\n===================="
				 "===================="
				 "===================="
				 "===================\n\n");
		stdoutput.printf("%d: rollback\n",
					process::getProcessId());
	}

	if (pvt->_conn->rollback()) {
		endTransaction(false);
		return true;
	}
	return false;
}

bool sqlrservercontroller::selectDatabase(const char *db) {
	return (pvt->_cfg->getIgnoreSelectDatabase())?
			true:pvt->_conn->selectDatabase(db);
}

void sqlrservercontroller::dbHasChanged() {
	pvt->_dbchanged=true;
}

char *sqlrservercontroller::getCurrentDatabase() {
	return pvt->_conn->getCurrentDatabase();
}

char *sqlrservercontroller::getCurrentSchema() {
	return pvt->_conn->getCurrentSchema();
}

bool sqlrservercontroller::getLastInsertId(uint64_t *id) {
	return pvt->_conn->getLastInsertId(id);
}

bool sqlrservercontroller::setIsolationLevel(const char *isolevel) {
	return pvt->_conn->setIsolationLevel(isolevel);
}

bool sqlrservercontroller::ping() {
	return pvt->_conn->ping();
}

bool sqlrservercontroller::getListsByApiCalls() {
	return pvt->_conn->getListsByApiCalls();
}

bool sqlrservercontroller::fakePrepareAndExecuteForApiCall(
					sqlrservercursor *cursor) {
	cursor->setResultSetHeaderHasBeenHandled(false);
	cursor->getBindMappingsPool()->clear();
	cursor->setQueryLength(0);
	cursor->getQueryBuffer()[0]='\0';
	if (pvt->_sqlrt && !translateQuery(cursor)) {
		return false;
	}
	cursor->clearTotalRowsFetched();
	return true;
}

bool sqlrservercontroller::getDatabaseList(sqlrservercursor *cursor,
						const char *wild) {
	return fakePrepareAndExecuteForApiCall(cursor) &&
		pvt->_conn->getDatabaseList(cursor,wild) &&
		handleResultSetHeader(cursor);
}

bool sqlrservercontroller::getSchemaList(sqlrservercursor *cursor,
						const char *wild) {
	return fakePrepareAndExecuteForApiCall(cursor) &&
		pvt->_conn->getSchemaList(cursor,wild) &&
		handleResultSetHeader(cursor);
}

bool sqlrservercontroller::getTableList(sqlrservercursor *cursor,
						const char *wild,
						uint16_t objecttypes) {
	return fakePrepareAndExecuteForApiCall(cursor) &&
		pvt->_conn->getTableList(cursor,wild,objecttypes) &&
		handleResultSetHeader(cursor);
}

bool sqlrservercontroller::getTableTypeList(sqlrservercursor *cursor,
						const char *wild) {
	return fakePrepareAndExecuteForApiCall(cursor) &&
		pvt->_conn->getTableTypeList(cursor,wild) &&
		handleResultSetHeader(cursor);
}

bool sqlrservercontroller::getColumnList(sqlrservercursor *cursor,
						const char *table,
						const char *wild) {
	return fakePrepareAndExecuteForApiCall(cursor) &&
		pvt->_conn->getColumnList(cursor,table,wild) &&
		handleResultSetHeader(cursor);
}

bool sqlrservercontroller::getPrimaryKeyList(sqlrservercursor *cursor,
						const char *table,
						const char *wild) {
	return fakePrepareAndExecuteForApiCall(cursor) &&
		pvt->_conn->getPrimaryKeyList(cursor,table,wild) &&
		handleResultSetHeader(cursor);
}

bool sqlrservercontroller::getKeyAndIndexList(sqlrservercursor *cursor,
						const char *table,
						const char *wild) {
	return fakePrepareAndExecuteForApiCall(cursor) &&
		pvt->_conn->getKeyAndIndexList(cursor,table,wild) &&
		handleResultSetHeader(cursor);
}

bool sqlrservercontroller::getProcedureBindAndColumnList(
						sqlrservercursor *cursor,
						const char *proc,
						const char *wild) {
	return fakePrepareAndExecuteForApiCall(cursor) &&
		pvt->_conn->getProcedureBindAndColumnList(cursor,proc,wild) &&
		handleResultSetHeader(cursor);
}

bool sqlrservercontroller::getTypeInfoList(sqlrservercursor *cursor,
						const char *type,
						const char *wild) {
	return fakePrepareAndExecuteForApiCall(cursor) &&
		pvt->_conn->getTypeInfoList(cursor,type,wild) &&
		handleResultSetHeader(cursor);
}

bool sqlrservercontroller::getProcedureList(sqlrservercursor *cursor,
						const char *wild) {
	return fakePrepareAndExecuteForApiCall(cursor) &&
		pvt->_conn->getProcedureList(cursor,wild) &&
		handleResultSetHeader(cursor);
}

const char *sqlrservercontroller::getDatabaseListQuery(bool wild) {
	return pvt->_conn->getDatabaseListQuery(wild);
}

const char *sqlrservercontroller::getSchemaListQuery(bool wild) {
	return pvt->_conn->getSchemaListQuery(wild);
}

const char *sqlrservercontroller::getTableListQuery(bool wild,
						uint16_t objecttypes) {
	return pvt->_conn->getTableListQuery(wild,objecttypes);
}

const char *sqlrservercontroller::getTableTypeListQuery(bool wild) {
	return pvt->_conn->getTableTypeListQuery(wild);
}

const char *sqlrservercontroller::getGlobalTempTableListQuery() {
	return pvt->_conn->getGlobalTempTableListQuery();
}

const char *sqlrservercontroller::getColumnListQuery(const char *table,
								bool wild) {
	return pvt->_conn->getColumnListQuery(table,wild);
}

const char *sqlrservercontroller::getPrimaryKeyListQuery(const char *table,
								bool wild) {
	return pvt->_conn->getPrimaryKeyListQuery(table,wild);
}

const char *sqlrservercontroller::getKeyAndIndexListQuery(const char *table,
								bool wild) {
	return pvt->_conn->getKeyAndIndexListQuery(table,wild);
}

const char *sqlrservercontroller::getProcedureBindAndColumnListQuery(
							const char *proc,
							bool wild) {
	return pvt->_conn->getProcedureBindAndColumnListQuery(proc,wild);
}

const char *sqlrservercontroller::getTypeInfoListQuery(const char *type,
							bool wild) {
	return pvt->_conn->getTypeInfoListQuery(type,wild);
}

const char *sqlrservercontroller::getProcedureListQuery(bool wild) {
	return pvt->_conn->getProcedureListQuery(wild);
}

void sqlrservercontroller::saveError() {

	// don't overwrite any message that's already been saved
	if (pvt->_conn->getErrorLength()) {
		return;
	}

	// fetch the error into the connection error buffers and flags
	uint32_t	errorlength;
	int64_t		errorcode;
	bool		liveconnection;
	pvt->_conn->errorMessage(pvt->_conn->getErrorBuffer(),
				pvt->_cfg->getMaxErrorLength(),
				&errorlength,
				&errorcode,
				&liveconnection);
	pvt->_conn->setErrorLength(errorlength);
	pvt->_conn->setErrorNumber(errorcode);
	pvt->_conn->setLiveConnection(liveconnection);

	if (pvt->_debugsql) {
		stdoutput.printf("%d:ERROR:\n%d:",
				process::getProcessId(),errorcode);
		stdoutput.write(pvt->_conn->getErrorBuffer(),errorlength);
		stdoutput.write('\n');
	}
}

void sqlrservercontroller::saveErrorFromCursor(sqlrservercursor *cursor) {

	// don't overwrite any message that's already been saved
	if (pvt->_conn->getErrorLength()) {
		return;
	}

	// fetch the error into the connection error buffers and flags
	uint32_t	errorlength;
	int64_t		errorcode;
	bool		liveconnection;
	errorMessage(cursor,pvt->_conn->getErrorBuffer(),
				pvt->_cfg->getMaxErrorLength(),
				&errorlength,
				&errorcode,
				&liveconnection);
	pvt->_conn->setErrorLength(errorlength);
	pvt->_conn->setErrorNumber(errorcode);
	pvt->_conn->setLiveConnection(liveconnection);
}


void sqlrservercontroller::errorMessage(const char **errorbuffer,
						uint32_t *errorlength,
						int64_t *errorcode,
						bool *liveconnection) {
	saveError();
	*errorbuffer=pvt->_conn->getErrorBuffer();
	*errorlength=pvt->_conn->getErrorLength();
	*errorcode=pvt->_conn->getErrorNumber();
	*liveconnection=pvt->_conn->getLiveConnection();
}

void sqlrservercontroller::errorMessage(char *errorbuffer,
						uint32_t errorbuffersize,
						uint32_t *errorlength,
						int64_t *errorcode,
						bool *liveconnection) {

	// fetch the error
	const char	*errorstring=NULL;
	errorMessage(&errorstring,errorlength,errorcode,liveconnection);

	// copy the error out
	charstring::safeCopy(errorbuffer,errorbuffersize,
				errorstring,*errorlength);
	if (*errorlength>errorbuffersize) {
		*errorlength=errorbuffersize;
	}
}

void sqlrservercontroller::clearError() {
	setError(NULL,0,true);
}

void sqlrservercontroller::setError(const char *err,
					int64_t errn,
					bool liveconn) {

	char		*errorbuffer=pvt->_conn->getErrorBuffer();
	uint32_t	errorlength=charstring::length(err);
	if (errorlength>pvt->_maxerrorlength) {
		errorlength=pvt->_maxerrorlength;
	}
	charstring::safeCopy(errorbuffer,pvt->_maxerrorlength,err,errorlength);
	if (errorlength<pvt->_maxerrorlength) {
		errorbuffer[errorlength]='\0';
	}
	pvt->_conn->setErrorLength(errorlength);
	pvt->_conn->setErrorNumber(errn);
	pvt->_conn->setLiveConnection(liveconn);
}

char *sqlrservercontroller::getErrorBuffer() {
	return pvt->_conn->getErrorBuffer();
}

uint32_t sqlrservercontroller::getErrorLength() {
	return pvt->_conn->getErrorLength();
}

void sqlrservercontroller::setErrorLength(uint32_t errorlength) {
	pvt->_conn->setErrorLength(errorlength);
}

uint32_t sqlrservercontroller::getErrorNumber() {
	return pvt->_conn->getErrorNumber();
}

void sqlrservercontroller::setErrorNumber(uint32_t errnum) {
	pvt->_conn->setErrorNumber(errnum);
}

bool sqlrservercontroller::getLiveConnection() {
	return pvt->_conn->getLiveConnection();
}

void sqlrservercontroller::setLiveConnection(bool liveconnection) {
	pvt->_conn->setLiveConnection(liveconnection);
}

bool sqlrservercontroller::checkInterceptQuery(sqlrservercursor *cursor) {

	// find the start of the actual query
	const char	*ptr=skipWhitespaceAndComments(
					cursor->getQueryBuffer());

	// for now, we only intercept transaction queries
	if (isBeginTransactionQuery(ptr)) {
		cursor->setQueryType(SQLRQUERYTYPE_BEGIN);
		return true;
	} else if (isCommitQuery(ptr)) {
		cursor->setQueryType(SQLRQUERYTYPE_COMMIT);
		return true;
	} else if (isRollbackQuery(ptr)) {
		cursor->setQueryType(SQLRQUERYTYPE_ROLLBACK);
		return true;
	} else if (isAutoCommitOnQuery(ptr)) {
		cursor->setQueryType(SQLRQUERYTYPE_AUTOCOMMIT_ON);
		return true;
	} else if (isAutoCommitOffQuery(ptr)) {
		cursor->setQueryType(SQLRQUERYTYPE_AUTOCOMMIT_OFF);
		return true;
	} else {
		bool	on=false;
		if (isSetIncludingAutoCommitQuery(ptr,&on)) {
			// For these, set the query type, but don't actually
			// return true.  That way they won't actually be
			// intercepted by interceptQuery().  Instead they'll be
			// handled as special cases by executeQuery().
			cursor->setQueryType((on)?
				SQLRQUERYTYPE_SET_INCLUDING_AUTOCOMMIT_ON:
				SQLRQUERYTYPE_SET_INCLUDING_AUTOCOMMIT_OFF);
		}
	}
	return false;
}

bool sqlrservercontroller::interceptQuery(sqlrservercursor *cursor) {

	cursor->setQueryWasIntercepted(false);

	// Get the query type.  It will have been set by checkInterceptQuery().
	sqlrquerytype_t	querytype=cursor->getQueryType();

	// Intercept begins and handle them.  If we're faking begins, commit
	// and rollback queries also need to be intercepted as well, otherwise
	// the query will be sent directly to the db and endFakeBeginTransaction
	// won't get called.
	bool	retval=false;
	switch (querytype) {
		case SQLRQUERYTYPE_BEGIN:
			cursor->setQueryWasIntercepted(true);
			cursor->setInputBindCount(0);
			cursor->setOutputBindCount(0);
			pvt->_sendcolumninfo=DONT_SEND_COLUMN_INFO;
			if (pvt->_faketransactionblocks &&
					pvt->_infaketransactionblock) {
				setError(cursor,
					SQLR_ERROR_BEGIN_IN_TX_BLOCK_STRING,
					SQLR_ERROR_BEGIN_IN_TX_BLOCK,true);
			} else {
				retval=begin();
			}
			// FIXME: if the begin fails and the db api doesn't
			// support a begin command then the connection-level
			// error needs to be copied to the cursor so
			// queryOrBindCursor can report it
			break;
		case SQLRQUERYTYPE_COMMIT:
			cursor->setQueryWasIntercepted(true);
			cursor->setInputBindCount(0);
			cursor->setOutputBindCount(0);
			pvt->_sendcolumninfo=DONT_SEND_COLUMN_INFO;
			if (pvt->_faketransactionblocks &&
					!pvt->_infaketransactionblock) {
				setError(cursor,
				SQLR_ERROR_COMMIT_NOT_IN_TX_BLOCK_STRING,
				SQLR_ERROR_COMMIT_NOT_IN_TX_BLOCK,true);
			} else {
				retval=commit();
			}
			// FIXME: if the commit fails and the db api doesn't
			// support a commit command then the connection-level
			// error needs to be copied to the cursor so
			// queryOrBindCursor can report it
			break;
		case SQLRQUERYTYPE_ROLLBACK:
			cursor->setQueryWasIntercepted(true);
			cursor->setInputBindCount(0);
			cursor->setOutputBindCount(0);
			pvt->_sendcolumninfo=DONT_SEND_COLUMN_INFO;
			if (pvt->_faketransactionblocks &&
					!pvt->_infaketransactionblock) {
				setError(cursor,
				SQLR_ERROR_ROLLBACK_NOT_IN_TX_BLOCK_STRING,
				SQLR_ERROR_ROLLBACK_NOT_IN_TX_BLOCK,true);
			} else {
				retval=rollback();
			}
			// FIXME: if the rollback fails and the db api doesn't
			// support a rollback command then the connection-level
			// error needs to be copied to the cursor so
			// queryOrBindCursor can report it
			break;
		case SQLRQUERYTYPE_AUTOCOMMIT_ON:
			cursor->setQueryWasIntercepted(true);
			cursor->setInputBindCount(0);
			cursor->setOutputBindCount(0);
			pvt->_sendcolumninfo=DONT_SEND_COLUMN_INFO;
			// FIXME: fake tx block issues here???
			retval=autoCommitOn();
			break;
		case SQLRQUERYTYPE_AUTOCOMMIT_OFF:
			cursor->setQueryWasIntercepted(true);
			cursor->setInputBindCount(0);
			cursor->setOutputBindCount(0);
			pvt->_sendcolumninfo=DONT_SEND_COLUMN_INFO;
			// FIXME: fake tx block issues here???
			retval=autoCommitOff();
			break;
		default:
			break;
	}
	return retval;
}

bool sqlrservercontroller::isAutoCommitOnQuery(const char *query) {
	return isAutoCommitQuery(query,true);
}

bool sqlrservercontroller::isAutoCommitOffQuery(const char *query) {
	return isAutoCommitQuery(query,false);
}

bool sqlrservercontroller::isAutoCommitQuery(const char *query, bool on) {

	// look for "autocommit"
	if (!charstring::compareIgnoringCase(query,"autocommit",10)) {

		query+=10;

	}  else {

		// look for "set"
		if (!charstring::compareIgnoringCase(query,"set",3)) {
			query+=3;
		} else {
			return false;
		}

		// skip whitespace
		query=skipWhitespaceAndComments(query);

		// look for "autocommit"/"auto"/"implicit_transactions"
		if (!charstring::compareIgnoringCase(query,"autocommit",10)) {
			query+=10;
		} else if (!charstring::compareIgnoringCase(query,"auto",4)) {
			query+=4;
		} else if (!charstring::compareIgnoringCase(
					query,"implicit_transactions",21)) {
			query+=21;
		} else {
			return false;
		}
	}

	// skip whitespace
	query=skipWhitespaceAndComments(query);

	// look for "="/"to"
	if (*query=='=') {
		query++;
	} else if (!charstring::compareIgnoringCase(query,"to",2)) {
		query+=2;
	}

	// skip whitespace
	query=skipWhitespaceAndComments(query);

	if (on) {
		// look for 1/on/yes/immediate
		if (*query=='1') {
			query++;
		} else if (!charstring::compareIgnoringCase(query,"on",2)) {
			query+=2;
		} else if (!charstring::compareIgnoringCase(query,"yes",3)) {
			query+=3;
		} else if (!charstring::compareIgnoringCase(
							query,"immediate",9)) {
			query+=9;
		} else {
			return false;
		}
	} else {
		// look for 0/off/no
		if (*query=='0') {
			query++;
		} else if (!charstring::compareIgnoringCase(query,"off",3)) {
			query+=3;
		} else if (!charstring::compareIgnoringCase(query,"no",2)) {
			query+=2;
		} else {
			return false;
		}
	}

	// skip whitespace
	query=skipWhitespaceAndComments(query);

	// look for end of query
	if (*query) {
		return false;
	}

	return true;
}

bool sqlrservercontroller::isSetIncludingAutoCommitQuery(
						const char *query, bool *on) {

	*on=false;

	// look for "set"
	if (!charstring::compareIgnoringCase(query,"set",3)) {
		query+=3;
	} else {
		return false;
	}

	for (;;) {

		// skip whitespace
		query=skipWhitespaceAndComments(query);

		// look for "autocommit"
		if (!charstring::compareIgnoringCase(query,"autocommit",10)) {
			query+=10;
			break;
		}

		// look for a comma or end of query
		while (*query && *query!=',') {
			query++;
		}
		if (!*query) {
			return false;
		}

		// skip comma
		query++;
	}

	// skip whitespace
	query=skipWhitespaceAndComments(query);

	// look for "="/"to"
	if (*query=='=') {
		query++;
	} else {
		return false;
	}

	// skip whitespace
	query=skipWhitespaceAndComments(query);

	// look for 1/0
	if (*query=='1') {
		*on=true;
		query++;
	} else if (*query=='0') {
		*on=false;
		query++;
	} else {
		return false;
	}

	// skip whitespace
	query=skipWhitespaceAndComments(query);

	// success if we hit a comma, or are at the end of the query
	return (*query!=',' || *query);
}

bool sqlrservercontroller::isBeginTransactionQuery(sqlrservercursor *cursor) {
	return isBeginTransactionQuery(skipWhitespaceAndComments(
						cursor->getQueryBuffer()));
}

bool sqlrservercontroller::isBeginTransactionQuery(const char *query) {

	// See if it was any of the different queries used to start a
	// transaction.  IMPORTANT: don't just look for the first 5 characters
	// to be "begin", make sure it's the entire query.  Many db's use
	// "begin" to start a stored procedure block, but in those cases,
	// something will follow it.
	if (!charstring::compareIgnoringCase(query,"begin",5)) {

		// make sure there are only spaces, comments or the word "work"
		// after the begin
		const char	*spaceptr=skipWhitespaceAndComments(query+5);
		
		if (*spaceptr=='\0' ||
			!charstring::compareIgnoringCase(spaceptr,"work",4)) {
			return true;
		}
		return false;

	} else if (!charstring::compareIgnoringCase(query,"start ",6)) {
		return true;
	} else if (!charstring::compareIgnoringCase(query,"bt",2) &&
							*(query+2)=='\0') {
		return true;
	}
	return false;
}

bool sqlrservercontroller::isCommitQuery(const char *query) {

	return (!charstring::compareIgnoringCase(query,"commit",6) ||
		(!charstring::compareIgnoringCase(query,"et",2) &&
						*(query+2)=='\0'));
}

bool sqlrservercontroller::isRollbackQuery(const char *query) {
	return !charstring::compareIgnoringCase(query,"rollback",8);
}

bool sqlrservercontroller::skipComment(const char **ptr,
						const char *endptr) {
	while (*ptr<endptr && !charstring::compare(*ptr,"--",2)) {
		while (**ptr && **ptr!='\n') {
			(*ptr)++;
		}
	}
	return *ptr!=endptr;
}

bool sqlrservercontroller::skipWhitespace(const char **ptr,
						const char *endptr) {
	while (character::isWhitespace(**ptr) && *ptr<endptr) {
		(*ptr)++;
	}
	return *ptr!=endptr;
}

const char *sqlrservercontroller::skipWhitespaceAndComments(const char *query) {
	if (!query) {
		return NULL;
	}
	const char	*ptr=query;
	while (*ptr) {
		if (character::isWhitespace(*ptr)) {
			ptr++;
		} else if (!charstring::compare(ptr,"--",2)) {
			while (*ptr && *ptr!='\n') {
				ptr++;
			}
			if (*ptr) {
				ptr++;
			}
		} else {
			return ptr;
		}
	}
	return ptr;
}

bool sqlrservercontroller::parseDateTime(
				const char *datetime, bool ddmm, bool yyyyddmm,
				const char *datedelimiters,
				int16_t *year, int16_t *month, int16_t *day,
				int16_t *hour, int16_t *minute, int16_t *second,
				int32_t *microsecond, bool *isnegative) {
	return ::parseDateTime(datetime,ddmm,yyyyddmm,datedelimiters,
				year,month,day,hour,minute,second,microsecond,
				isnegative);
}

char *sqlrservercontroller::convertDateTime(const char *format,
				int16_t year, int16_t month, int16_t day,
				int16_t hour, int16_t minute, int16_t second,
				int32_t microsecond, bool isnegative) {
	return ::convertDateTime(format,year,month,day,
				hour,minute,second,microsecond,
				isnegative);
}

static const char *asciitohex[]={
	"00","01","02","03","04","05","06","07",
	"08","09","0A","0B","0C","0D","0E","0F",
	"10","11","12","13","14","15","16","17",
	"18","19","1A","1B","1C","1D","1E","1F",
	"20","21","22","23","24","25","26","27",
	"28","29","2A","2B","2C","2D","2E","2F",
	"30","31","32","33","34","35","36","37",
	"38","39","3A","3B","3C","3D","3E","3F",
	"40","41","42","43","44","45","46","47",
	"48","49","4A","4B","4C","4D","4E","4F",
	"50","51","52","53","54","55","56","57",
	"58","59","5A","5B","5C","5D","5E","5F",
	"60","61","62","63","64","65","66","67",
	"68","69","6A","6B","6C","6D","6E","6F",
	"70","71","72","73","74","75","76","77",
	"78","79","7A","7B","7C","7D","7E","7F",
	"80","81","82","83","84","85","86","87",
	"88","89","8A","8B","8C","8D","8E","8F",
	"90","91","92","93","94","95","96","97",
	"98","99","9A","9B","9C","9D","9E","9F",
	"A0","A1","A2","A3","A4","A5","A6","A7",
	"A8","A9","AA","AB","AC","AD","AE","AF",
	"B0","B1","B2","B3","B4","B5","B6","B7",
	"B8","B9","BA","BB","BC","BD","BE","BF",
	"C0","C1","C2","C3","C4","C5","C6","C7",
	"C8","C9","CA","CB","CC","CD","CE","CF",
	"D0","D1","D2","D3","D4","D5","D6","D7",
	"D8","D9","DA","DB","DC","DD","DE","DF",
	"E0","E1","E2","E3","E4","E5","E6","E7",
	"E8","E9","EA","EB","EC","ED","EE","EF",
	"F0","F1","F2","F3","F4","F5","F6","F7",
	"F8","F9","FA","FB","FC","FD","FE","FF"
};

const char *sqlrservercontroller::asciiToHex(unsigned char ch) {
	return asciitohex[ch];
}

static const char *asciitooctal[]={
	"000","001","002","003","004","005","006","007",
	"010","011","012","013","014","015","016","017",
	"020","021","022","023","024","025","026","027",
	"030","031","032","033","034","035","036","037",
	"040","041","042","043","044","045","046","047",
	"050","051","052","053","054","055","056","057",
	"060","061","062","063","064","065","066","067",
	"070","071","072","073","074","075","076","077",
	"100","101","102","103","104","105","106","107",
	"110","111","112","113","114","115","116","117",
	"120","121","122","123","124","125","126","127",
	"130","131","132","133","134","135","136","137",
	"140","141","142","143","144","145","146","147",
	"150","151","152","153","154","155","156","157",
	"160","161","162","163","164","165","166","167",
	"170","171","172","173","174","175","176","177",
	"200","201","202","203","204","205","206","207",
	"210","211","212","213","214","215","216","217",
	"220","221","222","223","224","225","226","227",
	"230","231","232","233","234","235","236","237",
	"240","241","242","243","244","245","246","247",
	"250","251","252","253","254","255","256","257",
	"260","261","262","263","264","265","266","267",
	"270","271","272","273","274","275","276","277",
	"300","301","302","303","304","305","306","307",
	"310","311","312","313","314","315","316","317",
	"320","321","322","323","324","325","326","327",
	"330","331","332","333","334","335","336","337",
	"340","341","342","343","344","345","346","347",
	"350","351","352","353","354","355","356","357",
	"360","361","362","363","364","365","366","367",
	"370","371","372","373","374","375","376","377"
};

const char *sqlrservercontroller::asciiToOctal(unsigned char ch) {
	return asciitooctal[ch];
}

bool sqlrservercontroller::hasBindVariables(const char *query,
						uint32_t querylen) {
	return ::countBindVariables(query,querylen,
				pvt->_questionmarksupported,
				pvt->_colonsupported,
				pvt->_atsignsupported,
				pvt->_dollarsignsupported);
}

uint16_t sqlrservercontroller::countBindVariables(const char *query,
							uint32_t querylen) {
	return ::countBindVariables(query,querylen,
				pvt->_questionmarksupported,
				pvt->_colonsupported,
				pvt->_atsignsupported,
				pvt->_dollarsignsupported);
}

bool sqlrservercontroller::isBitType(const char *type) {
	return ::isBitTypeChar(type);
}

bool sqlrservercontroller::isBitType(int16_t type) {
	return ::isBitTypeInt(type);
}

bool sqlrservercontroller::isBoolType(const char *type) {
	return ::isBoolTypeChar(type);
}

bool sqlrservercontroller::isBoolType(int16_t type) {
	return ::isBoolTypeInt(type);
}

bool sqlrservercontroller::isFloatType(const char *type) {
	return ::isFloatTypeChar(type);
}

bool sqlrservercontroller::isFloatType(int16_t type) {
	return ::isFloatTypeInt(type);
}

bool sqlrservercontroller::isNumberType(const char *type) {
	return ::isNumberTypeChar(type);
}

bool sqlrservercontroller::isNumberType(int16_t type) {
	return ::isNumberTypeInt(type);
}

bool sqlrservercontroller::isBlobType(const char *type) {
	return ::isBlobTypeChar(type);
}

bool sqlrservercontroller::isBlobType(int16_t type) {
	return ::isBlobTypeInt(type);
}

bool sqlrservercontroller::isUnsignedType(const char *type) {
	return ::isUnsignedTypeChar(type);
}

bool sqlrservercontroller::isUnsignedType(int16_t type) {
	return ::isUnsignedTypeInt(type);
}

bool sqlrservercontroller::isBinaryType(const char *type) {
	return ::isBinaryTypeChar(type);
}

bool sqlrservercontroller::isBinaryType(int16_t type) {
	return ::isBinaryTypeInt(type);
}

bool sqlrservercontroller::isDateTimeType(const char *type) {
	return ::isDateTimeTypeChar(type);
}

bool sqlrservercontroller::isDateTimeType(int16_t type) {
	return ::isDateTimeTypeInt(type);
}

const char * const *sqlrservercontroller::dataTypeStrings() {
	return datatypestring;
}

bool sqlrservercontroller::applyDirectives(sqlrservercursor *cursor) {

	if (pvt->_debugsqlrdirectives) {
		stdoutput.printf("\n===================="
				 "===================="
				 "===================="
				 "===================\n\n");
		stdoutput.printf("applying directives...\n");
	}

	// apply translation rules
	const char	*query=cursor->getQueryBuffer();
	if (!pvt->_sqlrd->run(pvt->_conn,cursor,query)) {
		if (pvt->_debugsqlrdirectives) {
			stdoutput.printf("a directive failed\n");
		}
		// FIXME: raise directive failed event...
		// FIXME: return an error somehow
		return false;
	}

	return true;
}

bool sqlrservercontroller::translateQuery(sqlrservercursor *cursor) {

	const char	*query=cursor->getQueryBuffer();
	uint32_t	querylen=cursor->getQueryLength();

	if (pvt->_debugsqlrtranslations) {
		stdoutput.write("\n===================="
				 "===================="
				 "===================="
				 "===================\n\n");
		stdoutput.write("translating query...\n\n");
		stdoutput.write("original:\n\"");
		stdoutput.safePrint(query,querylen);
		stdoutput.write("\"\n");
	}

	// clear the query tree
	cursor->clearQueryTree();

	// apply translation rules
	stringbuffer	*translatedquery=cursor->getTranslatedQueryBuffer();
	translatedquery->clear();
	if (!pvt->_sqlrt->run(pvt->_conn,cursor,pvt->_sqlrp,
					query,querylen,translatedquery)) {
		raiseTranslationFailureEvent(cursor,query);
		if (pvt->_sqlrt->getUseOriginalOnError()) {
			if (pvt->_debugsqlrtranslations) {
				stdoutput.write("translation failed, "
						"using original:\n\"");
				stdoutput.safePrint(query,querylen);
				stdoutput.write("\"\n");
			}
			return true;
		}
		setError(cursor,pvt->_sqlrt->getError(),
				SQLR_ERROR_QUERYTRANSLATION,true);
		return false;
	}

	// update the query tree
	if (pvt->_sqlrp) {
		cursor->setQueryTree(pvt->_sqlrp->detachTree());
	}

	if (pvt->_debugsqlrtranslations) {
		stdoutput.write("translated:\n\"");
		stdoutput.safePrint(translatedquery->getString(),
					translatedquery->getSize());
		stdoutput.write("\"\n");
	}

	// bail if the translated query is too large
	if (translatedquery->getSize()>pvt->_maxquerysize) {
		if (pvt->_debugsqlrtranslations) {
			stdoutput.write("translated query too large\n");
		}
		return false;
	}

	// replace with a noop if the query is empty
	if (charstring::isNullOrEmpty(translatedquery->getString())) {
		translatedquery->append(pvt->_conn->noopQuery());
	}

	// write the translated query to the cursor's query buffer
	// so it'll be there if we decide to re-execute it later
	bytestring::copy(cursor->getQueryBuffer(),
			translatedquery->getString(),
			translatedquery->getSize());
	cursor->setQueryLength(translatedquery->getSize());
	cursor->getQueryBuffer()[cursor->getQueryLength()]='\0';
	return true;
}

void sqlrservercontroller::translateBindVariables(sqlrservercursor *cursor) {

	// clear bind mappings
	cursor->getBindMappings()->clear();

	// get query buffer
	char	*querybuffer=cursor->getQueryBuffer();

	// debug
	if (pvt->_debugbindtranslation) {
		stdoutput.printf("\n===================="
				 "===================="
				 "===================="
				 "===================\n\n");
		stdoutput.printf("translating bind variables...\n");
		stdoutput.printf("original:\n%s\n",querybuffer);
	}
	if (logEnabled()) {
		raiseDebugMessageEvent("translating bind variables...");
		raiseDebugMessageEvent("original:");
		raiseDebugMessageEvent(querybuffer);
	}

	// convert queries from whatever bind variable format they currently
	// use to the format required by the database...

	bool			translated=false;
	queryparsestate_t	parsestate=IN_QUERY;
	stringbuffer		newquery;
	stringbuffer		currentbind;

	// use 1-based index for bind variables
	uint16_t	bindindex=1;
	
	// run through the querybuffer...
	const char	*ptr=querybuffer;
	const char	*endptr=querybuffer+cursor->getQueryLength();
	char		prev='\0';
	do {

		// if we're in the query...
		if (parsestate==IN_QUERY) {

			// if we find a quote, we're in quotes
			if (*ptr=='\'') {
				parsestate=IN_QUOTES;
			}

			// if we find whitespace or a couple of other things
			// then the next thing could be a bind variable
			if (beforeBindVariable(ptr)) {
				parsestate=BEFORE_BIND;
			}

			// append the character
			newquery.append(*ptr);

			// move on
			if (*ptr=='\\' && prev=='\\') {
				prev='\0';
			} else {
				prev=*ptr;
			}
			ptr++;
			continue;
		}

		// copy anything in quotes verbatim
		if (parsestate==IN_QUOTES) {

			// if we find a quote, but not an escaped quote,
			// then we're back in the query
			// (or we're in between one of these: '...''...'
			// which is functionally the same)
			if (*ptr=='\'' && prev!='\\') {
				parsestate=IN_QUERY;
			}

			// append the character
			newquery.append(*ptr);

			// move on
			if (*ptr=='\\' && prev=='\\') {
				prev='\0';
			} else {
				prev=*ptr;
			}
			ptr++;
			continue;
		}

		if (parsestate==BEFORE_BIND) {

			// if we find a bind variable...
			if (isBindDelimiter(ptr,
					pvt->_questionmarksupported,
					pvt->_colonsupported,
					pvt->_atsignsupported,
					pvt->_dollarsignsupported)) {
				parsestate=IN_BIND;
				currentbind.clear();
				continue;
			}

			// if we didn't find a bind variable then we're just
			// back in the query
			parsestate=IN_QUERY;
			continue;
		}

		// if we're in a bind variable...
		if (parsestate==IN_BIND) {

			// If we find whitespace or a few other things
			// then we're done with the bind variable.  Process it.
			// Otherwise get the variable itself in another buffer.
			bool	endofbind=afterBindVariable(ptr);
			if (endofbind || ptr==endptr-1) {

				// special case...
				// last character in the query
				if (!endofbind && ptr==endptr-1) {
					currentbind.append(*ptr);
					if (*ptr=='\\' && prev=='\\') {
						prev='\0';
					} else {
						prev=*ptr;
					}
					ptr++;
				}

				// if the current bind variable format doesn't
				// match the db bind format...
				if (!matchesNativeBindFormat(
						currentbind.getString())) {

					// translate...
					translated=true;
					translateBindVariableInStringAndMap(
								cursor,
								&currentbind,
								bindindex,
								&newquery);
				} else {
					newquery.append(
						currentbind.getString());
				}
				bindindex++;

				parsestate=IN_QUERY;

			} else {

				// move on
				currentbind.append(*ptr);
				if (*ptr=='\\' && prev=='\\') {
					prev='\0';
				} else {
					prev=*ptr;
				}
				ptr++;
			}
			continue;
		}

	} while (ptr<endptr);


	// if no translation was performed
	if (!translated) {
		if (pvt->_debugbindtranslation) {
			stdoutput.printf(
				"\n  no bind translation performed\n\n");
		}
		raiseDebugMessageEvent("no bind translation performed");
		return;
	}


	// if we made it here then some conversion
	// was done - update the querybuffer...
	const char	*newq=newquery.getString();
	uint32_t	newqlen=newquery.getStringLength();
	if (newqlen>pvt->_maxquerysize) {
		newqlen=pvt->_maxquerysize;
	}
	bytestring::copy(querybuffer,newq,newqlen);
	querybuffer[newqlen]='\0';
	cursor->setQueryLength(newqlen);


	// debug
	if (pvt->_debugbindtranslation) {
		stdoutput.printf("\ntranslated:\n%s\n\n",querybuffer);
	}
	if (logEnabled()) {
		raiseDebugMessageEvent("translated:");
		raiseDebugMessageEvent(querybuffer);
	}
}

bool sqlrservercontroller::matchesNativeBindFormat(const char *bind) {

	const char	*bindformat=pvt->_conn->bindFormat();
	size_t		bindformatlen=charstring::length(bindformat);

	// the bind variable name matches the format if...
	// * the first character of the bind variable name matches the 
	//   first character of the bind format
	//
	//	and...
	//
	// * the format is just a single character
	// 	or..
	// * the second character of the format is a 1 and the second character
	//   of the bind variable name is a digit
	// 	or..
	// * the second character of the format is a * and the second character
	//   of the bind varaible name is alphanumeric
	return (bind[0]==bindformat[0]  &&
		(bindformatlen==1 ||
		(bindformat[1]=='1' && character::isDigit(bind[1])) ||
		(bindformat[1]=='*' && character::isAlphanumeric(bind[1]))));
}

void sqlrservercontroller::translateBindVariableInStringAndMap(
						sqlrservercursor *cursor,
						stringbuffer *currentbind,
						uint16_t bindindex,
						stringbuffer *newquery) {

	// get the bind format
	const char	*bindformat=pvt->_conn->bindFormat();

	// replace the bind variable delimiter with whatever we would expect to
	// find for this database
	currentbind->setPosition(0);
	currentbind->write(bindformat[0]);

	// append the first character of the bind format to the new query
	newquery->append(bindformat[0]);

	if (bindformat[1]=='\0') {

		// This section handles single-character bind variable
		// placeholder such as ?'s. (mysql, db2 and firebird format)

		// replace bind variable itself with number
		mapBindVariable(cursor,currentbind->getString(),
					currentbind->getStringLength(),
					bindindex);

	} else if (bindformat[1]=='1' &&
			!charstring::isNumber(currentbind->getString()+1)) {

		// This section handles 2-character placeholders where the
		// second position is a number, such as $1 (postgresql-format).

		// replace bind variable in string with number
		newquery->append(bindindex);

		// replace bind variable itself with number
		mapBindVariable(cursor,currentbind->getString(),
					currentbind->getStringLength(),
					bindindex);

	} else {

		// This section handles everything else, such as :*, @*.
		// (oracle, sybase, and ms sql server formats)

		// If the current bind variable was a single character
		// placeholder (such as a ?) then replace it with a delimited
		// number.  Otherwise use it as-is...

		if (currentbind->getStringLength()==1) {

			// replace bind variable in string with number
			newquery->append(bindindex);

			// replace bind variable itself with number
			mapBindVariable(cursor,currentbind->getString(),
						currentbind->getStringLength(),
						bindindex);
		} else {
			newquery->append(currentbind->getString()+1,
					currentbind->getStringLength()-1);
		}
	}
}

void sqlrservercontroller::mapBindVariable(sqlrservercursor *cursor,
						const char *bindvariable,
						uint64_t bindvariablelen,
						uint16_t bindindex) {

	// if the current bind variable is a ? then just
	// set it NULL for special handling later
	if (!charstring::compare(bindvariable,"?")) {
		bindvariable=NULL;
	}

	// create the new bind var name and get its length
	char		*tempnumber=charstring::parseNumber(bindindex);
	uint16_t	tempnumberlen=charstring::length(tempnumber);

	char	*oldvariable=(char *)cursor->getBindMappingsPool()->
						allocate(bindvariablelen+1);
	char	*newvariable=(char *)cursor->getBindMappingsPool()->
						allocate(tempnumberlen+2);

	charstring::copy(oldvariable,bindvariable,bindvariablelen);
	oldvariable[bindvariablelen]='\0';

	newvariable[0]=bindFormat()[0];
	charstring::copy(newvariable+1,tempnumber);
	newvariable[tempnumberlen+1]='\0';

	// map existing name to new name
	cursor->getBindMappings()->setValue(oldvariable,newvariable);
				
	// clean up
	delete[] tempnumber;
}

void sqlrservercontroller::translateBindVariablesFromMappings(
						sqlrservercursor *cursor) {

	// index variable
	uint16_t	i=0;

	// debug and logging
	if (pvt->_debugbindtranslation) {
		stdoutput.printf("\n===================="
				 "===================="
				 "===================="
				 "===================\n\n");
		stdoutput.printf("remapping bind variables:\n");
		stdoutput.printf("  input binds:\n");
		for (i=0; i<cursor->getInputBindCount(); i++) {
			stdoutput.printf("    %s\n",
				cursor->getInputBinds()[i].variable);
		}
		stdoutput.printf("  output binds:\n");
		for (i=0; i<cursor->getOutputBindCount(); i++) {
			stdoutput.printf("    %s\n",
				cursor->getOutputBinds()[i].variable);
		}
		stdoutput.printf("  input/output binds:\n");
		for (i=0; i<cursor->getInputOutputBindCount(); i++) {
			stdoutput.printf("    %s\n",
				cursor->getInputOutputBinds()[i].variable);
		}
		stdoutput.printf("\n");
	}
	if (logEnabled()) {
		raiseDebugMessageEvent("remapping bind variables...");
		raiseDebugMessageEvent("input binds:");
		for (i=0; i<cursor->getInputBindCount(); i++) {
			raiseDebugMessageEvent(
				cursor->getInputBinds()[i].variable);
		}
		raiseDebugMessageEvent("output binds:");
		for (i=0; i<cursor->getOutputBindCount(); i++) {
			raiseDebugMessageEvent(
				cursor->getOutputBinds()[i].variable);
		}
		raiseDebugMessageEvent("input/output binds:");
		for (i=0; i<cursor->getInputOutputBindCount(); i++) {
			raiseDebugMessageEvent(
				cursor->getInputOutputBinds()[i].variable);
		}
	}

	// run three passes - input binds, output binds, input/output binds
	bool	remapped=false;
	for (i=0; i<3; i++) {

		namevaluepairs		*mappings=cursor->getBindMappings();
		uint16_t		count=0;
		sqlrserverbindvar	*vars=NULL;
		if (i==0) {
			count=cursor->getInputBindCount();
			vars=cursor->getInputBinds();
		} else if (i==1) {
			count=cursor->getOutputBindCount();
			vars=cursor->getOutputBinds();
		} else if (i==2) {
			count=cursor->getInputOutputBindCount();
			vars=cursor->getInputOutputBinds();
		}

		for (uint16_t j=0; j<count; j++) {

			// get the bind var
			sqlrserverbindvar	*b=&(vars[j]);

			// remap it
			char	*newvariable;
			if (mappings->getValue(b->variable,&newvariable)) {
				b->variable=newvariable;
				b->variablesize=charstring::length(b->variable);
				remapped=true;
			}
		}
	}

	// if no remapping was performed
	if (!remapped) {
		if (pvt->_debugbindtranslation) {
			stdoutput.printf("  no variables remapped\n\n");
		}
		raiseDebugMessageEvent("no variables remapped");
		return;
	}

	// debug and logging
	if (pvt->_debugbindtranslation) {
		stdoutput.printf("  remapped input binds:\n");
		for (i=0; i<cursor->getInputBindCount(); i++) {
			stdoutput.printf("    %s\n",
				cursor->getInputBinds()[i].variable);
		}
		stdoutput.printf("  remapped output binds:\n");
		for (i=0; i<cursor->getOutputBindCount(); i++) {
			stdoutput.printf("    %s\n",
				cursor->getOutputBinds()[i].variable);
		}
		stdoutput.printf("  remapped input/output binds:\n");
		for (i=0; i<cursor->getOutputBindCount(); i++) {
			stdoutput.printf("    %s\n",
				cursor->getInputOutputBinds()[i].variable);
		}
		stdoutput.printf("\n");
	}
	if (logEnabled()) {
		raiseDebugMessageEvent("remapped input binds:");
		for (i=0; i<cursor->getInputBindCount(); i++) {
			raiseDebugMessageEvent(
				cursor->getInputBinds()[i].variable);
		}
		raiseDebugMessageEvent("remapped output binds:");
		for (i=0; i<cursor->getOutputBindCount(); i++) {
			raiseDebugMessageEvent(
				cursor->getOutputBinds()[i].variable);
		}
		raiseDebugMessageEvent("remapped input/output binds:");
		for (i=0; i<cursor->getInputOutputBindCount(); i++) {
			raiseDebugMessageEvent(
				cursor->getInputOutputBinds()[i].variable);
		}
	}
}

void sqlrservercontroller::translateBeginTransaction(sqlrservercursor *cursor) {

	// get query buffer
	char	*querybuffer=cursor->getQueryBuffer();

	// debug
	raiseDebugMessageEvent("translating begin tx query...");
	raiseDebugMessageEvent("original:");
	raiseDebugMessageEvent(querybuffer);

	// translate query
	const char	*beginquery=pvt->_conn->beginTransactionQuery();
	uint32_t	querylength=charstring::length(beginquery);
	charstring::copy(querybuffer,beginquery,querylength);
	querybuffer[querylength]='\0';
	cursor->setQueryLength(querylength);

	// debug
	raiseDebugMessageEvent("converted:");
	raiseDebugMessageEvent(querybuffer);
}

bool sqlrservercontroller::filterQuery(sqlrservercursor *cursor, bool before) {

	const char	*query=cursor->getQueryBuffer();

	if (pvt->_debugsqlrfilters) {
		stdoutput.printf("\n===================="
				 "===================="
				 "===================="
				 "===================\n\n");
		stdoutput.printf("filtering:\n\"%s\"\n\n",query);
	}

	// apply filters
	const char	*err=NULL;
	int64_t		errn=0;
	bool		success=(before)?
				pvt->_sqlrf->runBeforeFilters(pvt->_conn,
								cursor,
								pvt->_sqlrp,
								query,
								&err,&errn):
				pvt->_sqlrf->runAfterFilters(pvt->_conn,
								cursor,
								pvt->_sqlrp,
								query,
								&err,&errn);
	if (!success) {
		setError(cursor,err,errn,true);
		raiseFilterViolationEvent(cursor);
		if (pvt->_debugsqlrfilters) {
			stdoutput.printf("query filtered out\n");
		} 
		return false;
	}

	if (pvt->_debugsqlrfilters) {
		stdoutput.printf("query accepted\n");
	}
	return true;
}

sqlrservercursor *sqlrservercontroller::useCustomQueryCursor(	
						sqlrservercursor *cursor) {

	// do we need to use a custom query cursor for this query?

	// not if custom queries aren't enabled...
	if (!pvt->_sqlrq) {
		return cursor;
	}

	// see if the query matches one of the custom queries
	// FIXME: the 0 below isn't safe, none of the custom queries do
	// anything with the id, but they might in the future so it needs to be
	// unique
	sqlrquerycursor	*customcursor=pvt->_sqlrq->match(
						pvt->_conn,
						cursor->getQueryBuffer(),
						cursor->getQueryLength(),0);
				
	// if not...
	if (!customcursor) {
		return cursor;
	}

	// if so...

	// open the custom cursor
	customcursor->open();

	// copy the query that we just got into
	// the custom query cursor's buffers
	bytestring::copy(
		customcursor->getQueryBuffer(),
		cursor->getQueryBuffer(),
		cursor->getQueryLength());
	customcursor->getQueryBuffer()[cursor->getQueryLength()]='\0';
	customcursor->setQueryLength(cursor->getQueryLength());

	// set the custom cursor' state
	customcursor->setState(SQLRCURSORSTATE_BUSY);

	// attach the custom cursor to the cursor
	cursor->setCustomQueryCursor(customcursor);

	// return the custom cursor
	return customcursor;
}

bool sqlrservercontroller::handleBinds(sqlrservercursor *cursor) {

	// translate binds
	if (pvt->_sqlrbvt && cursor->getInputBindCount()) {
		if (pvt->_debugsqlrbindvariabletranslation) {
			stdoutput.printf("\n===================="
				 	"===================="
				 	"===================="
				 	"===================\n\n");
			stdoutput.printf("translating bind variables:\n");
		}
		if (!pvt->_sqlrbvt->run(pvt->_conn,cursor)) {
			setError(cursor,pvt->_sqlrbvt->getError(),
				SQLR_ERROR_BINDVARIABLETRANSLATION,true);
			return false;
		}
	}

	sqlrserverbindvar	*bind=NULL;
	
	// iterate through the arrays, binding values to variables
	for (int16_t in=0; in<cursor->getInputBindCount(); in++) {

		bind=&cursor->getInputBinds()[in];

		// bind the value to the variable
		if (bind->type==SQLRSERVERBINDVARTYPE_STRING ||
				bind->type==SQLRSERVERBINDVARTYPE_NULL) {
			if (!cursor->inputBind(
					bind->variable,
					bind->variablesize,
					bind->value.stringval,
					bind->valuesize,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==SQLRSERVERBINDVARTYPE_INTEGER) {
			if (!cursor->inputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.integerval)) {
				return false;
			}
		} else if (bind->type==SQLRSERVERBINDVARTYPE_DOUBLE) {
			if (!cursor->inputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.doubleval.value,
					bind->value.doubleval.precision,
					bind->value.doubleval.scale)) {
				return false;
			}
		} else if (bind->type==SQLRSERVERBINDVARTYPE_DATE) {
			if (!cursor->inputBind(
					bind->variable,
					bind->variablesize,
					bind->value.dateval.year,
					bind->value.dateval.month,
					bind->value.dateval.day,
					bind->value.dateval.hour,
					bind->value.dateval.minute,
					bind->value.dateval.second,
					bind->value.dateval.microsecond,
					bind->value.dateval.tz,
					bind->value.dateval.isnegative,
					bind->value.dateval.buffer,
					bind->value.dateval.buffersize,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==SQLRSERVERBINDVARTYPE_BLOB) {
			if (!cursor->inputBindBlob(
					bind->variable,
					bind->variablesize,
					bind->value.stringval,
					bind->valuesize,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==SQLRSERVERBINDVARTYPE_CLOB) {
			if (!cursor->inputBindClob(
					bind->variable,
					bind->variablesize,
					bind->value.stringval,
					bind->valuesize,
					&bind->isnull)) {
				return false;
			}
		}
	}

	for (int16_t out=0; out<cursor->getOutputBindCount(); out++) {

		bind=&cursor->getOutputBinds()[out];

		// bind the value to the variable
		if (bind->type==SQLRSERVERBINDVARTYPE_STRING) {
			if (!cursor->outputBind(
					bind->variable,
					bind->variablesize,
					bind->value.stringval,
					bind->valuesize+1,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==SQLRSERVERBINDVARTYPE_INTEGER) {
			if (!cursor->outputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.integerval,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==SQLRSERVERBINDVARTYPE_DOUBLE) {
			if (!cursor->outputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.doubleval.value,
					&bind->value.doubleval.precision,
					&bind->value.doubleval.scale,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==SQLRSERVERBINDVARTYPE_DATE) {
			if (!cursor->outputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.dateval.year,
					&bind->value.dateval.month,
					&bind->value.dateval.day,
					&bind->value.dateval.hour,
					&bind->value.dateval.minute,
					&bind->value.dateval.second,
					&bind->value.dateval.microsecond,
					(const char **)&bind->value.dateval.tz,
					&bind->value.dateval.isnegative,
					bind->value.dateval.buffer,
					bind->value.dateval.buffersize,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==SQLRSERVERBINDVARTYPE_BLOB) {
			if (!cursor->outputBindBlob(
					bind->variable,
					bind->variablesize,out,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==SQLRSERVERBINDVARTYPE_CLOB) {
			if (!cursor->outputBindClob(
					bind->variable,
					bind->variablesize,out,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==SQLRSERVERBINDVARTYPE_CURSOR) {

			bool	found=false;

			// find the cursor that we acquired earlier...
			for (uint16_t j=0; j<pvt->_cursorcount; j++) {

				if (pvt->_cur[j]->getId()==
						bind->value.cursorid) {
					found=true;

					// bind the cursor
					if (!cursor->outputBindCursor(
							bind->variable,
							bind->variablesize,
							pvt->_cur[j])) {
						return false;
					}
					break;
				}
			}

			// this shouldn't happen, but if it does, return false
			if (!found) {
				return false;
			}
		}
	}

	for (int16_t inout=0;
		inout<cursor->getInputOutputBindCount(); inout++) {

		bind=&cursor->getInputOutputBinds()[inout];

		// bind the value to the variable
		if (bind->type==SQLRSERVERBINDVARTYPE_STRING ||
				bind->type==SQLRSERVERBINDVARTYPE_NULL) {
			if (!cursor->inputOutputBind(
					bind->variable,
					bind->variablesize,
					bind->value.stringval,
					bind->valuesize+1,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==SQLRSERVERBINDVARTYPE_INTEGER) {
			if (!cursor->inputOutputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.integerval,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==SQLRSERVERBINDVARTYPE_DOUBLE) {
			if (!cursor->inputOutputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.doubleval.value,
					&bind->value.doubleval.precision,
					&bind->value.doubleval.scale,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==SQLRSERVERBINDVARTYPE_DATE) {
			if (!cursor->inputOutputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.dateval.year,
					&bind->value.dateval.month,
					&bind->value.dateval.day,
					&bind->value.dateval.hour,
					&bind->value.dateval.minute,
					&bind->value.dateval.second,
					&bind->value.dateval.microsecond,
					(const char **)&bind->value.dateval.tz,
					&bind->value.dateval.isnegative,
					bind->value.dateval.buffer,
					bind->value.dateval.buffersize,
					&bind->isnull)) {
				return false;
			}
		} /*else if (bind->type==SQLRSERVERBINDVARTYPE_BLOB) {
			if (!cursor->inputOutputBindBlob(
					bind->variable,
					bind->variablesize,inout,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==SQLRSERVERBINDVARTYPE_CLOB) {
			if (!cursor->inputOutputBindClob(
					bind->variable,
					bind->variablesize,inout,
					&bind->isnull)) {
				return false;
			}
		}*/
	}

	return true;
}

bool sqlrservercontroller::prepareQuery(sqlrservercursor *cursor,
						const char *query,
						uint32_t querylen) {
	return prepareQuery(cursor,query,querylen,false,false,false);
}

bool sqlrservercontroller::prepareQuery(sqlrservercursor *cursor,
						const char *query,
						uint32_t querylen,
						bool enabledirectives,
						bool enabletranslations,
						bool enablefilters) {

	if (pvt->_debugsql) {
		stdoutput.printf("\n===================="
				 "===================="
				 "===================="
				 "===================\n\n");
		stdoutput.printf("%d:%d:prepare:\n",
					process::getProcessId(),
					cursor->getId());
		stdoutput.write(query,querylen);
		stdoutput.write('\n');
	}

	// The standard paradigm is:
	//
	// * prepare(query)
	// * bind(variable,value)
	// * execute()
	//
	// Under various circumstances, we may need to fake binds though.  This
	// requires the query to be rewritten to include the bind values before
	// being prepared.  In that case, we must defer preparing the query
	// until the execution phase so that we'll be sure to have all of the
	// bind values on hand ("lazy prepares").
	//
	// The sqlrclient protocol is fine with that, but other protocols like
	// mysql and tds want to be able to return column info after prepare
	// but before execution.
	//
	// So, we can't just generally do lazy-prepares.  Instead, we'll follow
	// the standard paradigm when we can, and lazy-prepare when we must.
	//
	// That way, non-sqlrclient protocols will work as expected, unless
	// fakeinputbinds="yes" is explicitly set.
	//
	// There are some queries that must be fake-bind'ed, and thus
	// lazy-prepared (eg. Oracle's "create as select", and some MySQL
	// queries...) but they don't return a result set.  So, they'll end up
	// working fine, even with protocols that want to return column info
	// after prepare, because it turns out that there's no column info to
	// return anyway.

	// clean up the previous result set
	closeResultSet(cursor);

	// re-init error data
	clearError(cursor);

	// reset flags
	cursor->setColumnInfoIsValid(false);
	cursor->setQueryHasBeenPreProcessed(false);
	cursor->setQueryHasBeenPrepared(false);
	cursor->setQueryHasBeenExecuted(false);
	cursor->setQueryNeedsIntercept(false);
	cursor->setQueryWasIntercepted(false);
	cursor->setBindsWereFaked(false);
	cursor->setFakeInputBindsForThisQuery(pvt->_fakeinputbinds);
	cursor->setQueryStatus(SQLRQUERYSTATUS_ERROR);
	cursor->setQueryType(SQLRQUERYTYPE_ETC);
	cursor->setResultSetHeaderHasBeenHandled(false);

	// reset column mapping
	pvt->_columnmap=NULL;

	// sanity check
	if (querylen>pvt->_maxquerysize) {
		querylen=pvt->_maxquerysize;
		cursor->setQueryLength(pvt->_maxquerysize);
		// FIXME: Should we throw an error here?  If not, we'll end up
		// trying to execute a partial query, which could fail, or
		// could just truncate part of the where clause, yielding a
		// valid, but incorrect result.
	}

	// copy query to cursor's query buffer if necessary
	if (query!=cursor->getQueryBuffer()) {
		bytestring::copy(cursor->getQueryBuffer(),query,querylen);
		cursor->getQueryBuffer()[querylen]='\0';
		cursor->setQueryLength(querylen);
	}

	// bail if we are just generally configured to fake input binds
	if (cursor->getFakeInputBindsForThisQuery()) {
		return true;
	}

	// do this here instead of inside translateBindVariables
	// because translateQuery might use it
	cursor->getBindMappingsPool()->clear();

	// before-filter query
	if (enablefilters && pvt->_sqlrf) {
		if (!filterQuery(cursor,true)) {

			// log the query
			raiseQueryEvent(cursor);

			cursor->setQueryStatus(
				SQLRQUERYSTATUS_FILTER_VIOLATION);
			return false;
		}
	}

	// apply directives
	if (enabledirectives && pvt->_sqlrd) {
		applyDirectives(cursor);
	}

	// translate query
	if (enabletranslations && pvt->_sqlrt &&
				!translateQuery(cursor)) {

		// log the query
		raiseQueryEvent(cursor);
		return false;
	}

	// translate bind variables
	if (pvt->_translatebinds) {
		translateBindVariables(cursor);
	}

	// translate "begin" queries
	// FIXME: can we just let interceptQuery below handle this?
	if (pvt->_conn->supportsTransactionBlocks() &&
			isBeginTransactionQuery(cursor)) {
		translateBeginTransaction(cursor);
	}

	// after-filter query
	if (enablefilters && pvt->_sqlrf) {
		if (!filterQuery(cursor,false)) {

			// log the query
			raiseQueryEvent(cursor);

			cursor->setQueryStatus(
				SQLRQUERYSTATUS_FILTER_VIOLATION);
			return false;
		}
	}

	// (re)get the query now that it's been translated
	query=cursor->getQueryBuffer();
	querylen=cursor->getQueryLength();
	if (enabletranslations && pvt->_sqlrt &&
			pvt->_debugsql && !pvt->_debugsqlrtranslations) {
		stdoutput.printf("\n%d:%d:translated:\n%.*s\n",
					process::getProcessId(),
					cursor->getId(),
					querylen,query);
	}

	// fake input binds if this specific query doesn't support them
	if (!cursor->supportsNativeBinds(query,querylen)) {
		cursor->setFakeInputBindsForThisQuery(true);
	}

	// set flag indicating that the query has been preprocessed
	cursor->setQueryHasBeenPreProcessed(true);

	// Check to see if the query needs to be intercepted.  Don't
	// actually intercept it yet, but bail if it needs to be.
	cursor->setQueryNeedsIntercept(checkInterceptQuery(cursor));
	if (cursor->getQueryNeedsIntercept()) {
		return true;
	}

	// Bail if we this query should fake input binds.  This an happen if:
	// * the instance is generally configured to fake input binds
	// * one of the translations has set the
	// 	fakeinputbindsforthisquery flag true
	// * the specific query doesn't support native binds
	// In any of these cases, the cursor's fakeinputbindsforthisquery
	// flag will have been set true.
	if (cursor->getFakeInputBindsForThisQuery()) {
		return true;
	}

	raiseDebugMessageEvent("preparing query...");

	// set the query start time (in case the prepare fails)
	datetime	dt;
	dt.getSystemDateAndTime();
	cursor->setQueryStart(dt.getSeconds(),dt.getMicroseconds());

	// prepare the query
	bool	success=cursor->prepareQuery(query,querylen);

	// log result
	raiseDebugMessageEvent((success)?"prepare query succeeded":
						"prepare query failed");
	raiseDebugMessageEvent("done with prepare query");

	if (!success) {

		// set the query end time
		dt.getSystemDateAndTime();
		cursor->setQueryEnd(dt.getSeconds(),
					dt.getMicroseconds());

		// update query and error counts
		incrementQueryCounts(cursor->queryType(query,querylen));
		incrementTotalErrors();

		// save the error
		saveError(cursor);
		pvt->_debugstr.clear();
		pvt->_debugstr.append("prepare failed: ");
		pvt->_debugstr.append("\"");
		pvt->_debugstr.append(
			cursor->getErrorBuffer(),cursor->getErrorLength());
		pvt->_debugstr.append("\"");
		raiseDebugMessageEvent(pvt->_debugstr.getString());

		// log the query (attempt)
		raiseQueryEvent(cursor);

		return false;
	}

	// set flag indicating that the query has been prepared
	cursor->setQueryHasBeenPrepared(true);

	// handle column info now if it's valid at this point
	return (columnInfoIsValidAfterPrepare(cursor))?
				handleResultSetHeader(cursor):true;
}

bool sqlrservercontroller::executeQuery(sqlrservercursor *cursor) {
	return executeQuery(cursor,false,false,false,false);
}

bool sqlrservercontroller::executeQuery(sqlrservercursor *cursor,
						bool enabledirectives,
						bool enabletranslations,
						bool enablefilters,
						bool enabletriggers) {

	// set state
	setState((isCustomQuery(cursor))?PROCESS_CUSTOM:PROCESS_SQL);

	// if we're re-executing
	if (cursor->getQueryHasBeenExecuted()) {

		// clean up the previous result set
		closeResultSet(cursor);

		// re-init error data
		clearError(cursor);

		// if we're faking binds then the original
		// query must be re-prepared
		if (cursor->getFakeInputBindsForThisQuery()) {
			cursor->setQueryHasBeenPrepared(false);
			cursor->setColumnInfoIsValid(false);
		}
	}

	// if the query hasn't been preprocessed then execute various
	// filters, translations, and checks
	if (!cursor->getQueryHasBeenPreProcessed()) {

		// do this here instead of inside translateBindVariables
		// because translateQuery might use it
		cursor->getBindMappingsPool()->clear();

		// before-filter query
		if (enablefilters && pvt->_sqlrf) {
			if (!filterQuery(cursor,true)) {

				// log the query
				raiseQueryEvent(cursor);

				cursor->setQueryStatus(
					SQLRQUERYSTATUS_FILTER_VIOLATION);
				return false;
			}
		}

		// apply directives
		if (enabledirectives && pvt->_sqlrd) {
			applyDirectives(cursor);
		}

		// translate query
		if (enabletranslations && pvt->_sqlrt &&
					!translateQuery(cursor)) {

			// log the query
			raiseQueryEvent(cursor);
			return false;
		}

		// translate bind variables
		if (pvt->_translatebinds) {
			translateBindVariables(cursor);
		}

		// translate "begin" queries
		// FIXME: can we just let interceptQuery below handle this?
		if (pvt->_conn->supportsTransactionBlocks() &&
				isBeginTransactionQuery(cursor)) {
			translateBeginTransaction(cursor);
		}

		// after-filter query
		if (enablefilters && pvt->_sqlrf) {
			if (!filterQuery(cursor,false)) {

				// log the query
				raiseQueryEvent(cursor);

				cursor->setQueryStatus(
					SQLRQUERYSTATUS_FILTER_VIOLATION);
				return false;
			}
		}

		// fake input binds if this specific query doesn't support them
		if (!cursor->supportsNativeBinds(
					cursor->getQueryBuffer(),
					cursor->getQueryLength())) {
			cursor->setFakeInputBindsForThisQuery(true);
		}

		// check to see if the query needs to be intercepted,
		// but don't actually intercept it yet
		cursor->setQueryNeedsIntercept(checkInterceptQuery(cursor));

		// set flag indicating that the query has been preprocessed
		cursor->setQueryHasBeenPreProcessed(true);
	}

	// Do we need to fake input binds?
	// We do if:
	// * the instance is generally configured to fake input binds
	// * one of the translations has set the
	// 	fakeinputbindsforthisquery flag true
	// * the specific query doesn't support native binds
	// In any of these cases, the cursor's fakeinputbindsforthisquery flag
	// will have been set true.
	if (cursor->getFakeInputBindsForThisQuery()) {

		raiseDebugMessageEvent("faking binds...");

		if (cursor->fakeInputBinds()) {
			if (pvt->_debugbindtranslation) {
				stdoutput.printf(
				"after faking input binds:\n%s\n\n",
				cursor->
				getQueryWithFakeInputBindsBuffer()->
							getString());
			}
			cursor->setBindsWereFaked(true);
		}
	}

	// (re)set the query start time
	// (which may have been set earlier during prepare,
	// in case the prepare failed)
	datetime	dt;
	dt.getSystemDateAndTime();
	cursor->setQueryStart(dt.getSeconds(),dt.getMicroseconds());

	// init result
	bool	success=false;

	// if the query needs to be intercepted, then intercept it
	if (cursor->getQueryNeedsIntercept()) {

		success=interceptQuery(cursor);
		if (cursor->getQueryWasIntercepted()) {

			// set the query end time
			dt.getSystemDateAndTime();
			cursor->setQueryEnd(dt.getSeconds(),
						dt.getMicroseconds());

			if (success) {
				cursor->setQueryStatus(SQLRQUERYSTATUS_SUCCESS);
			}

			// log the query
			raiseQueryEvent(cursor);

			return success;
		}
	}

	// get the query
	const char	*query=cursor->getQueryBuffer();
	uint32_t	querylen=cursor->getQueryLength();
	if (cursor->getBindsWereFaked()) {
		query=cursor->getQueryWithFakeInputBindsBuffer()->
							getString();
		querylen=cursor->getQueryWithFakeInputBindsBuffer()->
							getStringLength();
	}

	// if the query still hasn't been prepared (probably
	// because we're faking binds), then prepare it now
	if (!cursor->getQueryHasBeenPrepared()) {

		raiseDebugMessageEvent("preparing query...");

		// set the query start time (in case the prepare fails)
		dt.getSystemDateAndTime();
		cursor->setQueryStart(dt.getSeconds(),dt.getMicroseconds());

		// prepare the query
		success=cursor->prepareQuery(query,querylen);

		// log result
		raiseDebugMessageEvent((success)?"prepare query succeeded":
						"prepare query failed");
		raiseDebugMessageEvent("done with prepare query");

		if (!success) {

			// set the query end time
			dt.getSystemDateAndTime();
			cursor->setQueryEnd(dt.getSeconds(),
						dt.getMicroseconds());

			// update query and error counts
			incrementQueryCounts(cursor->queryType(query,querylen));
			incrementTotalErrors();

			// save the error
			saveError(cursor);
			pvt->_debugstr.clear();
			pvt->_debugstr.append("prepare failed: ");
			pvt->_debugstr.append("\"");
			pvt->_debugstr.append(
				cursor->getErrorBuffer(),
				cursor->getErrorLength());
			pvt->_debugstr.append("\"");
			raiseDebugMessageEvent(pvt->_debugstr.getString());

			// log the query (attempt)
			raiseQueryEvent(cursor);

			return false;
		}

		// set flag indicating that the query has been prepared
		cursor->setQueryHasBeenPrepared(true);
	}

	raiseDebugMessageEvent("executing query...");

	// translate bind variables (from mappings)
	translateBindVariablesFromMappings(cursor);

	// handle binds (unless they were faked during the prepare)
	if (!cursor->getBindsWereFaked()) {

		// set the query start time (in case handleBinds fails)
		dt.getSystemDateAndTime();
		cursor->setQueryStart(dt.getSeconds(),dt.getMicroseconds());

		if (!handleBinds(cursor)) {

			// set the query end time
			dt.getSystemDateAndTime();
			cursor->setQueryEnd(dt.getSeconds(),
						dt.getMicroseconds());

			// update query and error counts
			incrementQueryCounts(cursor->queryType(query,querylen));
			incrementTotalErrors();

			// get the error
			saveError(cursor);
			pvt->_debugstr.clear();
			pvt->_debugstr.append("handle binds failed: ");
			pvt->_debugstr.append("\"");
			pvt->_debugstr.append(
				cursor->getErrorBuffer(),
				cursor->getErrorLength());
			pvt->_debugstr.append("\"");
			raiseDebugMessageEvent(pvt->_debugstr.getString());

			// log the query (attempt)
			raiseQueryEvent(cursor);

			return false;
		}
	}

	// handle before-triggers
	if (enabletriggers && pvt->_sqlrtr) {
		pvt->_sqlrtr->runBeforeTriggers(pvt->_conn,cursor);
	}

	// (re)set the query start time
	dt.getSystemDateAndTime();
	cursor->setQueryStart(dt.getSeconds(),dt.getMicroseconds());

	if (pvt->_debugsql) {
		stdoutput.printf("\n===================="
				 "===================="
				 "===================="
				 "===================\n\n");
		stdoutput.printf("%d:%d:execute:\n",
					process::getProcessId(),
					cursor->getId());
		stdoutput.write(query,querylen);
		stdoutput.write('\n');
	}

	// execute the query
	success=cursor->executeQuery(query,querylen);

	// set flag indicating that the query has been executed
	if (success) {
		cursor->setQueryHasBeenExecuted(true);
	}

	// set the query end time
	dt.getSystemDateAndTime();
	cursor->setQueryEnd(dt.getSeconds(),dt.getMicroseconds());

	// special case intercepts...
	// rather than actually intercepting these, we
	// allow the db to run them and set the flags here
	if (cursor->getQueryType()==
			SQLRQUERYTYPE_SET_INCLUDING_AUTOCOMMIT_ON) {
		pvt->_autocommitforthissession=true;
		if (pvt->_intransaction) {
			raiseCommitEvent();
		}
		pvt->_intransaction=false;
	} else if (cursor->getQueryType()==
			SQLRQUERYTYPE_SET_INCLUDING_AUTOCOMMIT_OFF) {
		pvt->_autocommitforthissession=false;
		bool	wasintx=pvt->_intransaction;
		pvt->_intransaction=true;
		if (wasintx) {
			raiseBeginTransactionEvent();
		}
	}

	// on failure, save the error
	// get it here rather than below because with some db's
	// after-triggers can mask the error
	if (!success) {
		saveError(cursor);
		pvt->_debugstr.clear();
		pvt->_debugstr.append("execute failed: ");
		pvt->_debugstr.append("\"");
		pvt->_debugstr.append(cursor->getErrorBuffer(),
					cursor->getErrorLength());
		pvt->_debugstr.append("\"");
		raiseDebugMessageEvent(pvt->_debugstr.getString());
	}

	// reset total rows fetched
	cursor->clearTotalRowsFetched();

	// update query and error counts
	incrementQueryCounts(cursor->queryType(query,querylen));
	if (!success) {
		incrementTotalErrors();
	}

	// handle after-triggers
	if (enabletriggers && pvt->_sqlrtr) {
		pvt->_sqlrtr->runAfterTriggers(pvt->_conn,cursor,&success);
	}

	// was the query a commit or rollback?
	commitOrRollback(cursor);

	// commit if necessary
	if (success && pvt->_conn->isTransactional() &&
			!pvt->_conn->supportsTransactionBlocks() &&
			pvt->_needscommitorrollback &&
			!pvt->_conn->supportsAutoCommit() &&
			pvt->_fakeautocommit) {
		raiseDebugMessageEvent("commit necessary...");
		success=commit();
	}
	
	raiseDebugMessageEvent((success)?"executing query succeeded":
					"executing query failed");
	raiseDebugMessageEvent("done executing query");

	if (success) {
		cursor->setQueryStatus(SQLRQUERYSTATUS_SUCCESS);
	}

	// log the query
	raiseQueryEvent(cursor);

	return (success)?handleResultSetHeader(cursor):false;
}

void sqlrservercontroller::setNeedsCommitOrRollback(bool needed) {
	pvt->_needscommitorrollback=needed;
}

bool sqlrservercontroller::getNeedsCommitOrRollback() {
	return pvt->_needscommitorrollback;
}

void sqlrservercontroller::commitOrRollback(sqlrservercursor *cursor) {

	raiseDebugMessageEvent("commit or rollback check...");

	// if the query was a commit or rollback, set a flag indicating so
	if (pvt->_conn->isTransactional()) {
		if (cursor->queryIsCommitOrRollback()) {
			raiseDebugMessageEvent("commit or rollback not needed");
			pvt->_needscommitorrollback=false;
		} else if (cursor->queryIsNotSelect()) {
			raiseDebugMessageEvent("commit or rollback needed");
			pvt->_needscommitorrollback=true;
		}
	}

	raiseDebugMessageEvent("done with commit or rollback check");
}

bool sqlrservercontroller::inTransaction() {
	return pvt->_intransaction;
}

bool sqlrservercontroller::columnInfoIsValidAfterPrepare(
					sqlrservercursor *cursor) {
	return !cursor->getExecuteDirect() &&
		cursor->columnInfoIsValidAfterPrepare();
}

uint16_t sqlrservercontroller::getSendColumnInfo() {
	return pvt->_sendcolumninfo;
}

void sqlrservercontroller::setSendColumnInfo(uint16_t sendcolumninfo) {
	pvt->_sendcolumninfo=sendcolumninfo;
}

bool sqlrservercontroller::skipRows(sqlrservercursor *cursor,
						uint64_t rows, bool *error) {

	if (pvt->_sqlrlg) {
		pvt->_debugstr.clear();
		pvt->_debugstr.append("skipping ");
		pvt->_debugstr.append(rows);
		pvt->_debugstr.append(" rows...");
		raiseDebugMessageEvent(pvt->_debugstr.getString());
	}

	for (uint64_t i=0; i<rows; i++) {

		raiseDebugMessageEvent("skip...");

		bool	error;
		if (!skipRow(cursor,&error)) {
			if (error) {
				raiseDebugMessageEvent(
						"skipping rows encountered "
						"an error");
			} else {
				raiseDebugMessageEvent(
						"skipping rows hit the "
						"end of the result set");
			}
			return false;
		}

		cursor->incrementTotalRowsFetched();
	}

	raiseDebugMessageEvent("done skipping rows");
	return true;
}

void sqlrservercontroller::setDatabaseListColumnMap(
					sqlrserverlistformat_t listformat) {

	// for now, don't remap columns if api calls are used to get lists,
	// the columns don't come back in the "native" format
	// FIXME: this "happens to work" for odbc passthrough:
	// ODBC -> sqlrelay client -> sqlrelay server -> ODBC -> some db
	// but wouldn't if either ODBC were replaced with something else
	if (pvt->_conn->getListsByApiCalls()) {
		pvt->_columnmap=NULL;
		return;
	}

	switch (listformat) {
		case SQLRSERVERLISTFORMAT_NULL:
			pvt->_columnmap=NULL;
			break;
		case SQLRSERVERLISTFORMAT_MYSQL:
			pvt->_columnmap=&(pvt->_mysqldatabasescolumnmap);
			break;
		case SQLRSERVERLISTFORMAT_ODBC:
			pvt->_columnmap=&(pvt->_odbcdatabasescolumnmap);
			break;
		default:
			pvt->_columnmap=NULL;
			break;
	}
}

void sqlrservercontroller::setSchemaListColumnMap(
					sqlrserverlistformat_t listformat) {

	// for now, don't remap columns if api calls are used to get lists,
	// the columns don't come back in the "native" format
	// FIXME: this "happens to work" for odbc passthrough:
	// ODBC -> sqlrelay client -> sqlrelay server -> ODBC -> some db
	// but wouldn't if either ODBC were replaced with something else
	if (pvt->_conn->getListsByApiCalls()) {
		pvt->_columnmap=NULL;
		return;
	}

	// FIXME: currently, the only connection that implements this is the
	// odbc connection, which gets lists by api calls, but eventually we
	// should implement it for other connections too...
	switch (listformat) {
		case SQLRSERVERLISTFORMAT_NULL:
			pvt->_columnmap=NULL;
			break;
		case SQLRSERVERLISTFORMAT_MYSQL:
			// FIXME: implement this
			pvt->_columnmap=NULL;
			break;
		case SQLRSERVERLISTFORMAT_ODBC:
			// FIXME: implement this
			pvt->_columnmap=NULL;
			break;
		default:
			pvt->_columnmap=NULL;
			break;
	}
}

void sqlrservercontroller::setTableListColumnMap(
					sqlrserverlistformat_t listformat) {

	// for now, don't remap columns if api calls are used to get lists,
	// the columns don't come back in the "native" format
	// FIXME: this "happens to work" for odbc passthrough:
	// ODBC -> sqlrelay client -> sqlrelay server -> ODBC -> some db
	// but wouldn't if either ODBC were replaced with something else
	if (pvt->_conn->getListsByApiCalls()) {
		pvt->_columnmap=NULL;
		return;
	}

	switch (listformat) {
		case SQLRSERVERLISTFORMAT_NULL:
			pvt->_columnmap=NULL;
			break;
		case SQLRSERVERLISTFORMAT_MYSQL:
			pvt->_columnmap=&(pvt->_mysqltablescolumnmap);
			break;
		case SQLRSERVERLISTFORMAT_ODBC:
			pvt->_columnmap=&(pvt->_odbctablescolumnmap);
			break;
		default:
			pvt->_columnmap=NULL;
			break;
	}
}

void sqlrservercontroller::setTableTypeListColumnMap(
					sqlrserverlistformat_t listformat) {

	// for now, don't remap columns if api calls are used to get lists,
	// the columns don't come back in the "native" format
	// FIXME: this "happens to work" for odbc passthrough:
	// ODBC -> sqlrelay client -> sqlrelay server -> ODBC -> some db
	// but wouldn't if either ODBC were replaced with something else
	if (pvt->_conn->getListsByApiCalls()) {
		pvt->_columnmap=NULL;
		return;
	}

	// FIXME: currently, the only connection that implements this is the
	// odbc connection, which gets lists by api calls, but eventually we
	// should implement it for other connections too...
	switch (listformat) {
		case SQLRSERVERLISTFORMAT_NULL:
			pvt->_columnmap=NULL;
			break;
		case SQLRSERVERLISTFORMAT_MYSQL:
			// FIXME: implement this
			pvt->_columnmap=NULL;
			break;
		case SQLRSERVERLISTFORMAT_ODBC:
			// FIXME: implement this
			pvt->_columnmap=NULL;
			break;
		default:
			pvt->_columnmap=NULL;
			break;
	}
}

void sqlrservercontroller::setColumnListColumnMap(
					sqlrserverlistformat_t listformat) {

	// for now, don't remap columns if api calls are used to get lists,
	// the columns don't come back in the "native" format
	// FIXME: this "happens to work" for odbc passthrough:
	// ODBC -> sqlrelay client -> sqlrelay server -> ODBC -> some db
	// but wouldn't if either ODBC were replaced with something else
	if (pvt->_conn->getListsByApiCalls()) {
		pvt->_columnmap=NULL;
		return;
	}

	switch (listformat) {
		case SQLRSERVERLISTFORMAT_NULL:
			pvt->_columnmap=NULL;
			break;
		case SQLRSERVERLISTFORMAT_MYSQL:
			pvt->_columnmap=&(pvt->_mysqlcolumnscolumnmap);
			break;
		case SQLRSERVERLISTFORMAT_ODBC:
			pvt->_columnmap=&(pvt->_odbccolumnscolumnmap);
			break;
		default:
			pvt->_columnmap=NULL;
			break;
	}
}

void sqlrservercontroller::setPrimaryKeyListColumnMap(
					sqlrserverlistformat_t listformat) {

	// for now, don't remap columns if api calls are used to get lists,
	// the columns don't come back in the "native" format
	// FIXME: this "happens to work" for odbc passthrough:
	// ODBC -> sqlrelay client -> sqlrelay server -> ODBC -> some db
	// but wouldn't if either ODBC were replaced with something else
	if (pvt->_conn->getListsByApiCalls()) {
		pvt->_columnmap=NULL;
		return;
	}

	// FIXME: currently, the only connection that implements this is the
	// odbc connection, which gets lists by api calls, but eventually we
	// should implement it for other connections too...
	switch (listformat) {
		case SQLRSERVERLISTFORMAT_NULL:
			pvt->_columnmap=NULL;
			break;
		case SQLRSERVERLISTFORMAT_MYSQL:
			// FIXME: implement this
			pvt->_columnmap=NULL;
			break;
		case SQLRSERVERLISTFORMAT_ODBC:
			// FIXME: implement this
			pvt->_columnmap=NULL;
			break;
		default:
			pvt->_columnmap=NULL;
			break;
	}
}

void sqlrservercontroller::setKeyAndIndexListColumnMap(
					sqlrserverlistformat_t listformat) {

	// for now, don't remap columns if api calls are used to get lists,
	// the columns don't come back in the "native" format
	// FIXME: this "happens to work" for odbc passthrough:
	// ODBC -> sqlrelay client -> sqlrelay server -> ODBC -> some db
	// but wouldn't if either ODBC were replaced with something else
	if (pvt->_conn->getListsByApiCalls()) {
		pvt->_columnmap=NULL;
		return;
	}

	// FIXME: currently, the only connection that implements this is the
	// odbc connection, which gets lists by api calls, but eventually we
	// should implement it for other connections too...
	switch (listformat) {
		case SQLRSERVERLISTFORMAT_NULL:
			pvt->_columnmap=NULL;
			break;
		case SQLRSERVERLISTFORMAT_MYSQL:
			// FIXME: implement this
			pvt->_columnmap=NULL;
			break;
		case SQLRSERVERLISTFORMAT_ODBC:
			// FIXME: implement this
			pvt->_columnmap=NULL;
			break;
		default:
			pvt->_columnmap=NULL;
			break;
	}
}

void sqlrservercontroller::setProcedureBindAndColumnListColumnMap(
					sqlrserverlistformat_t listformat) {

	// for now, don't remap columns if api calls are used to get lists,
	// the columns don't come back in the "native" format
	// FIXME: this "happens to work" for odbc passthrough:
	// ODBC -> sqlrelay client -> sqlrelay server -> ODBC -> some db
	// but wouldn't if either ODBC were replaced with something else
	if (pvt->_conn->getListsByApiCalls()) {
		pvt->_columnmap=NULL;
		return;
	}

	// FIXME: currently, the only connection that implements this is the
	// odbc connection, which gets lists by api calls, but eventually we
	// should implement it for other connections too...
	switch (listformat) {
		case SQLRSERVERLISTFORMAT_NULL:
			pvt->_columnmap=NULL;
			break;
		case SQLRSERVERLISTFORMAT_MYSQL:
			// FIXME: implement this
			pvt->_columnmap=NULL;
			break;
		case SQLRSERVERLISTFORMAT_ODBC:
			// FIXME: implement this
			pvt->_columnmap=NULL;
			break;
		default:
			pvt->_columnmap=NULL;
			break;
	}
}

void sqlrservercontroller::setTypeInfoListColumnMap(
					sqlrserverlistformat_t listformat) {

	// for now, don't remap columns if api calls are used to get lists,
	// the columns don't come back in the "native" format
	// FIXME: this "happens to work" for odbc passthrough:
	// ODBC -> sqlrelay client -> sqlrelay server -> ODBC -> some db
	// but wouldn't if either ODBC were replaced with something else
	if (pvt->_conn->getListsByApiCalls()) {
		pvt->_columnmap=NULL;
		return;
	}

	// FIXME: currently, the only connection that implements this is the
	// odbc connection, which gets lists by api calls, but eventually we
	// should implement it for other connections too...
	switch (listformat) {
		case SQLRSERVERLISTFORMAT_NULL:
			pvt->_columnmap=NULL;
			break;
		case SQLRSERVERLISTFORMAT_MYSQL:
			// FIXME: implement this
			pvt->_columnmap=NULL;
			break;
		case SQLRSERVERLISTFORMAT_ODBC:
			// FIXME: implement this
			pvt->_columnmap=NULL;
			break;
		default:
			pvt->_columnmap=NULL;
			break;
	}
}

void sqlrservercontroller::setProcedureListColumnMap(
					sqlrserverlistformat_t listformat) {

	// for now, don't remap columns if api calls are used to get lists,
	// the columns don't come back in the "native" format
	// FIXME: this "happens to work" for odbc passthrough:
	// ODBC -> sqlrelay client -> sqlrelay server -> ODBC -> some db
	// but wouldn't if either ODBC were replaced with something else
	if (pvt->_conn->getListsByApiCalls()) {
		pvt->_columnmap=NULL;
		return;
	}

	// FIXME: currently, the only connection that implements this is the
	// odbc connection, which gets lists by api calls, but eventually we
	// should implement it for other connections too...
	switch (listformat) {
		case SQLRSERVERLISTFORMAT_NULL:
			pvt->_columnmap=NULL;
			break;
		case SQLRSERVERLISTFORMAT_MYSQL:
			// FIXME: implement this
			pvt->_columnmap=NULL;
			break;
		case SQLRSERVERLISTFORMAT_ODBC:
			// FIXME: implement this
			pvt->_columnmap=NULL;
			break;
		default:
			pvt->_columnmap=NULL;
			break;
	}
}

void sqlrservercontroller::buildColumnMaps() {

	// Native/MySQL getDatabaseList:
	//
	// Database
	pvt->_mysqldatabasescolumnmap.setValue(0,0);

	// MySQL getTableList:
	//
	// Tables_in_xxx -> TABLE_NAME
	pvt->_mysqltablescolumnmap.setValue(0,2);

	// Native/MySQL getColumnList:
if (!charstring::compare(pvt->_cfg->getDbase(),"postgresql")) {
	//
	// column_name
	pvt->_mysqlcolumnscolumnmap.setValue(0,3);
	// data_type
	pvt->_mysqlcolumnscolumnmap.setValue(1,5);
	// character_maximum_length
	pvt->_mysqlcolumnscolumnmap.setValue(2,6);
	// numeric_precision
	pvt->_mysqlcolumnscolumnmap.setValue(3,6);
	// numeric_scale
	pvt->_mysqlcolumnscolumnmap.setValue(4,8);
	// is_nullable
	pvt->_mysqlcolumnscolumnmap.setValue(5,10);
	// column_key
	pvt->_mysqlcolumnscolumnmap.setValue(6,18);
	// column_default
	pvt->_mysqlcolumnscolumnmap.setValue(7,12);
	// extra
	pvt->_mysqlcolumnscolumnmap.setValue(8,18);
} else {
	//
	// column_name
	pvt->_mysqlcolumnscolumnmap.setValue(0,0);
	// data_type
	pvt->_mysqlcolumnscolumnmap.setValue(1,1);
	// character_maximum_length
	pvt->_mysqlcolumnscolumnmap.setValue(2,2);
	// numeric_precision
	pvt->_mysqlcolumnscolumnmap.setValue(3,3);
	// numeric_scale
	pvt->_mysqlcolumnscolumnmap.setValue(4,4);
	// is_nullable
	pvt->_mysqlcolumnscolumnmap.setValue(5,5);
	// column_key
	pvt->_mysqlcolumnscolumnmap.setValue(6,6);
	// column_default
	pvt->_mysqlcolumnscolumnmap.setValue(7,7);
	// extra
	pvt->_mysqlcolumnscolumnmap.setValue(8,8);
}


	// Native/ODBC getDatabaseList:
	//
	// TABLE_CAT -> Database
	pvt->_odbcdatabasescolumnmap.setValue(0,0);
	// TABLE_SCHEM -> NULL
	pvt->_odbcdatabasescolumnmap.setValue(1,1);
	// TABLE_NAME -> NULL
	pvt->_odbcdatabasescolumnmap.setValue(2,1);
	// TABLE_TYPE -> NULL
	pvt->_odbcdatabasescolumnmap.setValue(3,1);
	// REMARKS -> NULL
	pvt->_odbcdatabasescolumnmap.setValue(4,1);

	// ODBC getTableList:
	// TABLE_CAT
	pvt->_odbctablescolumnmap.setValue(0,0);
	// TABLE_SCHEM
	pvt->_odbctablescolumnmap.setValue(1,1);
	// TABLE_NAME
	pvt->_odbctablescolumnmap.setValue(2,2);
	// TABLE_TYPE
	pvt->_odbctablescolumnmap.setValue(3,3);
	// REMARKS
	pvt->_odbctablescolumnmap.setValue(4,4);

	// ODBC getColumnList:
	//
if (!charstring::compare(pvt->_cfg->getDbase(),"postgresql")) {
	// TABLE_CAT
	pvt->_odbccolumnscolumnmap.setValue(0,0);
	// TABLE_SCHEM
	pvt->_odbccolumnscolumnmap.setValue(1,1);
	// TABLE_NAME
	pvt->_odbccolumnscolumnmap.setValue(2,2);
	// COLUMN_NAME
	pvt->_odbccolumnscolumnmap.setValue(3,3);
	// DATA_TYPE (numeric)
	pvt->_odbccolumnscolumnmap.setValue(4,4);
	// TYPE_NAME
	pvt->_odbccolumnscolumnmap.setValue(5,5);
	// COLUMN_SIZE
	pvt->_odbccolumnscolumnmap.setValue(6,6);
	// BUFFER_LEGTH
	pvt->_odbccolumnscolumnmap.setValue(7,7);
	// DECIMAL_DIGITS - smallint - scale
	pvt->_odbccolumnscolumnmap.setValue(8,8);
	// NUM_PREC_RADIX - smallint - precision
	pvt->_odbccolumnscolumnmap.setValue(9,9);
	// NULLABLE
	pvt->_odbccolumnscolumnmap.setValue(10,10);
	// REMARKS
	pvt->_odbccolumnscolumnmap.setValue(11,11);
	// COLUMN_DEF
	pvt->_odbccolumnscolumnmap.setValue(12,12);
	// SQL_DATA_TYPE
	pvt->_odbccolumnscolumnmap.setValue(13,13);
	// SQL_DATETIME_SUB
	pvt->_odbccolumnscolumnmap.setValue(14,14);
	// CHAR_OCTET_LENGTH
	pvt->_odbccolumnscolumnmap.setValue(15,15);
	// ORDINAL_POSITION
	pvt->_odbccolumnscolumnmap.setValue(16,16);
	// IS_NULLABLE
	pvt->_odbccolumnscolumnmap.setValue(17,17);
} else {
	// TABLE_CAT -> NULL
	pvt->_odbccolumnscolumnmap.setValue(0,9);
	// TABLE_SCHEM -> NULL
	pvt->_odbccolumnscolumnmap.setValue(1,9);
	// TABLE_NAME -> NULL
	pvt->_odbccolumnscolumnmap.setValue(2,9);
	// COLUMN_NAME -> column_name
	pvt->_odbccolumnscolumnmap.setValue(3,0);
	// DATA_TYPE (numeric) -> NULL
	pvt->_odbccolumnscolumnmap.setValue(4,9);
	// TYPE_NAME -> data_type
	pvt->_odbccolumnscolumnmap.setValue(5,1);
	// COLUMN_SIZE -> character_maximum_length
	pvt->_odbccolumnscolumnmap.setValue(6,2);
	// BUFFER_LEGTH -> character_maximum_length
	pvt->_odbccolumnscolumnmap.setValue(7,2);
	// DECIMAL_DIGITS - smallint - scale
	pvt->_odbccolumnscolumnmap.setValue(8,4);
	// NUM_PREC_RADIX - smallint - precision
	pvt->_odbccolumnscolumnmap.setValue(9,3);
	// NULLABLE -> NULL
	pvt->_odbccolumnscolumnmap.setValue(10,9);
	// REMARKS -> extra
	pvt->_odbccolumnscolumnmap.setValue(11,8);
	// COLUMN_DEF -> column_default
	pvt->_odbccolumnscolumnmap.setValue(12,7);
	// SQL_DATA_TYPE -> NULL
	pvt->_odbccolumnscolumnmap.setValue(13,9);
	// SQL_DATETIME_SUB -> NULL
	pvt->_odbccolumnscolumnmap.setValue(14,9);
	// CHAR_OCTET_LENGTH -> character_maximum_length
	pvt->_odbccolumnscolumnmap.setValue(15,2);
	// ORDINAL_POSITION -> NULL
	pvt->_odbccolumnscolumnmap.setValue(16,9);
	// IS_NULLABLE -> NULL
	pvt->_odbccolumnscolumnmap.setValue(17,5);
}
}

uint32_t sqlrservercontroller::mapColumn(uint32_t col) {
	return (pvt->_columnmap)?
			pvt->_columnmap->getValue(col):col;
}

uint32_t sqlrservercontroller::mapColumnCount(uint32_t colcount) {
	return (pvt->_columnmap)?
			pvt->_columnmap->getList()->getLength():colcount;
}

bool sqlrservercontroller::reformatField(sqlrservercursor *cursor,
						const char *name,
						uint32_t index,
						const char **field,
						uint64_t *fieldlength) {
	// run translations
	if (pvt->_sqlrrst) {

		if (pvt->_debugsqlrresultsettranslation) {
			stdoutput.printf("\n===================="
				 	"===================="
				 	"===================="
				 	"===================\n\n");
			stdoutput.printf("translating result set "
					"field %d (%s)...\n",index,name);
			stdoutput.printf("original:\n%s\n",*field);
		}

		if (!pvt->_sqlrrst->run(pvt->_conn,cursor,
					name,index,field,fieldlength)) {
			setError(cursor,pvt->_sqlrrst->getError(),
				SQLR_ERROR_RESULTSETTRANSLATION,true);
			return false;
		}

		if (pvt->_debugsqlrresultsettranslation) {
			stdoutput.printf("translated:\n%.*s\n\n",
						*fieldlength,*field);
		}
	}
	return true;
}

bool sqlrservercontroller::reformatRow(sqlrservercursor *cursor,
						uint32_t colcount,
						const char * const *names,
						const char ***fields,
						uint64_t **fieldlengths) {

	// run translations
	if (pvt->_sqlrrsrt) {
		if (pvt->_debugsqlrresultsetrowtranslation) {
			stdoutput.printf("\n===================="
				 	"===================="
				 	"===================="
				 	"===================\n\n");
			stdoutput.printf("translating result set row\n");
			for (uint32_t i=0; i<colcount; i++) {
				stdoutput.printf("field %d (%s)...\n",
								i,names[i]);
				stdoutput.printf("original:\n%s\n",
								(*fields)[i]);
			}
		}

		if (!pvt->_sqlrrsrt->run(pvt->_conn,cursor,colcount,
						names,fields,fieldlengths)) {
			setError(cursor,pvt->_sqlrrsrt->getError(),
				SQLR_ERROR_RESULTSETROWTRANSLATION,true);
			return false;
		}

		if (pvt->_debugsqlrresultsetrowtranslation) {
			for (uint32_t i=0; i<colcount; i++) {
				stdoutput.printf("translated:\n%.*s\n\n",
					(*fieldlengths)[i],(*fields)[i]);
			}
		}
	}
	return true;
}

bool sqlrservercontroller::reformatDateTimes(sqlrservercursor *cursor,
						uint32_t index,
						const char *field,
						uint64_t fieldlength,
						const char **newfield,
						uint64_t *newfieldlength,
						bool ddmm, bool yyyyddmm,
						bool ignorenondatetime,
						const char *datedelimiters,
						const char *datetimeformat,
						const char *dateformat,
						const char *timeformat) {

	// ignore non-date fields, if specified
	if (ignorenondatetime &&
		!isDateTimeTypeInt(getColumnType(cursor,index))) {
		return true;
	}

	// This weirdness is mainly to address a FreeTDS/MSSQL
	// issue.  See the code for the method
	// freetdscursor::ignoreDateDdMmParameter() for more info.
	if (cursor->ignoreDateDdMmParameter(index,field,fieldlength)) {
		ddmm=false;
		yyyyddmm=false;
	}

	int16_t	year=-1;
	int16_t	month=-1;
	int16_t	day=-1;
	int16_t	hour=-1;
	int16_t	minute=-1;
	int16_t	second=-1;
	int32_t	microsecond=-1;
	bool	isnegative=false;
	if (!parseDateTime(field,ddmm,yyyyddmm,
				datedelimiters,
				&year,&month,&day,
				&hour,&minute,&second,
				&microsecond,&isnegative)) {
		return false;
	}

	// decide which format to use based on what parts
	// were detected in the date/time
	const char	*format=datetimeformat;
	if (hour==-1) {
		format=dateformat;
	} else if (day==-1) {
		format=timeformat;
	}

	// convert to the specified format
	delete[] pvt->_reformattedfield;
	pvt->_reformattedfield=convertDateTime(format,
					year,month,day,
					hour,minute,second,
					microsecond,isnegative);
	pvt->_reformattedfieldlength=charstring::length(pvt->_reformattedfield);

	if (pvt->_debugsqlrresultsettranslation) {
		stdoutput.printf("\nconverted date "
			"\"%s\" to \"%s\"\nusing ddmm=%d and yyyyddmm=%d\n",
			field,pvt->_reformattedfield,ddmm,yyyyddmm);
	}

	// set return values
	*newfield=pvt->_reformattedfield;
	*newfieldlength=pvt->_reformattedfieldlength;

	return true;
}

void sqlrservercontroller::closeAllResultSets() {
	raiseDebugMessageEvent("closing result sets for all cursors...");
	for (int32_t i=0; i<pvt->_cursorcount; i++) {
		if (pvt->_cur[i]) {
			pvt->_cur[i]->closeResultSet();
		}
	}
	raiseDebugMessageEvent("done closing result sets for all cursors...");
}

void sqlrservercontroller::endSession() {

	raiseDebugMessageEvent("ending session...");

	setState(SESSION_END);

	raiseDebugMessageEvent("aborting all cursors...");
	for (int32_t i=0; i<pvt->_cursorcount; i++) {
		if (pvt->_cur[i]) {
			pvt->_cur[i]->abort();
		}
	}
	raiseDebugMessageEvent("done aborting all cursors");

	// must set suspendedsession to false here so resumed sessions won't 
	// automatically re-suspend
	pvt->_suspendedsession=false;

	// Run end-of-session rollback or commit before dropping tables and
	// running session-end-queries.  Some queries, including drop table,
	// cause an implicit commit.  If we need to rollback, then make sure
	// that's done first.
	if (pvt->_infaketransactionblock) {

		// if we're faking transaction blocks and the session was ended
		// but we haven't ended the transaction block, then we need to
		// rollback and end the block
		rollback();
		pvt->_infaketransactionblock=false;

	} else if (pvt->_conn->isTransactional() &&
				pvt->_needscommitorrollback) {

		// otherwise, commit or rollback as necessary
		if (pvt->_cfg->getEndOfSessionCommit()) {
			raiseDebugMessageEvent("committing...");
			commit();
			raiseDebugMessageEvent("done committing...");
		} else {
			raiseDebugMessageEvent("rolling back...");
			rollback();
			raiseDebugMessageEvent("done rolling back...");
		}
	}

	// truncate/drop temp tables
	// (Do this before running the end-session queries becuase
	// with oracle, it may be necessary to log out and log back in to
	// drop a temp table.  With each log-out the session end queries
	// are run and with each log-in the session start queries are run.)
	truncateTempTables(pvt->_cur[0]);
	dropTempTables(pvt->_cur[0]);

	// run session-end queries
	sessionEndQueries();

	// reset database/schema
	if (pvt->_dbchanged) {
		// FIXME: we're ignoring the result and error,
		// should we do something if there's an error?
		pvt->_conn->selectDatabase(pvt->_originaldb);
		pvt->_dbchanged=false;
	}

	// reset initial autocommit behavior
	setAutoCommit(pvt->_initialautocommit);

	// set isolation level
	pvt->_conn->setIsolationLevel(pvt->_isolationlevel);

	// NOTE: For debugging, it's nice to know what the most recent
	// clientinfo was, so lets not reset this.  Hopefully not resetting it
	// doesn't break something.
	// reset the client info
	//setClientInfo("",0);

	// reset protocol modules
	if (pvt->_sqlrpr) {
		pvt->_sqlrpr->endSession();
	}

	// reset parser modules
	if (pvt->_sqlrp) {
		pvt->_sqlrp->endSession();
	}

	// reset translation modules
	if (pvt->_sqlrt) {
		pvt->_sqlrt->endSession();
	}
	for (int32_t i=0; i<pvt->_cursorcount; i++) {
		if (pvt->_cur[i]) {
			pvt->_cur[i]->getTranslatedQueryBuffer()->clear();
		}
	}

	// reset filter modules
	if (pvt->_sqlrf) {
		pvt->_sqlrf->endSession();
	}

	// reset bind variable translation modules
	if (pvt->_sqlrbvt) {
		pvt->_sqlrbvt->endSession();
	}

	// reset result set header translation modules
	if (pvt->_sqlrrsht) {
		pvt->_sqlrrsht->endSession();
	}

	// reset result set translation modules
	if (pvt->_sqlrrst) {
		pvt->_sqlrrst->endSession();
	}

	// reset result set row translation modules
	if (pvt->_sqlrrsrt) {
		pvt->_sqlrrsrt->endSession();
	}

	// reset result set row block translation modules
	if (pvt->_sqlrrsrbt) {
		pvt->_sqlrrsrbt->endSession();
	}

	// reset trigger modules
	if (pvt->_sqlrtr) {
		pvt->_sqlrtr->endSession();
	}

	// reset logger modules
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->endSession();
	}

	// reset notification modules
	if (pvt->_sqlrn) {
		pvt->_sqlrn->endSession();
	}

	// reset schedule modules
	if (pvt->_sqlrs) {
		pvt->_sqlrs->endSession();
	}

	// reset query modules
	if (pvt->_sqlrq) {
		pvt->_sqlrq->endSession();
	}

	// reset auth modules
	if (pvt->_sqlra) {
		pvt->_sqlra->endSession();
	}

	// clear per-session pool
	pvt->_sessionpool.clear();

	// shrink the cursor array, if necessary
	// FIXME: it would probably be more efficient to scale
	// these down gradually rather than all at once
	while (pvt->_cursorcount>pvt->_mincursorcount) {
		pvt->_cursorcount--;
		close(pvt->_cur[pvt->_cursorcount]);
		deleteCursor(pvt->_cur[pvt->_cursorcount]);
		pvt->_cur[pvt->_cursorcount]=NULL;
	}

	// end the session
	pvt->_conn->endSession();

	// if the db is behind a load balancer, re-login
	// periodically to redistribute connections
	if (pvt->_constr->getBehindLoadBalancer()) {
		raiseDebugMessageEvent("relogging in to "
				"redistribute connections");
		datetime	dt;
		if (dt.getSystemDateAndTime()) {
			if (dt.getEpoch()>=pvt->_relogintime) {
				reLogIn();
			}
		}
		raiseDebugMessageEvent("done relogging in to "
				"redistribute connections");
	}

	raiseDebugMessageEvent("done ending session");
}

void sqlrservercontroller::dropTempTables(sqlrservercursor *cursor) {

	// run through the temp table list, dropping tables
	for (singlylinkedlistnode< char * >
				*sln=pvt->_sessiontemptablesfordrop.getFirst();
				sln; sln=sln->getNext()) {

		// some databases (oracle) require us to truncate the
		// table before it can be dropped
		if (pvt->_conn->tempTableTruncateBeforeDrop()) {
			truncateTempTable(cursor,sln->getValue());
		}

		dropTempTable(cursor,sln->getValue());
		delete[] sln->getValue();
	}
	pvt->_sessiontemptablesfordrop.clear();
}

void sqlrservercontroller::dropTempTable(sqlrservercursor *cursor,
						const char *tablename) {

	stringbuffer	dropquery;
	dropquery.append("drop table ");
	dropquery.append(pvt->_conn->tempTableDropPrefix());
	dropquery.append(tablename);

	// kind of a kluge...
	// The cursor might already have a querytree associated with it and
	// if it does then executeQuery below might cause some triggers to
	// be run on that tree rather than on the tree for the drop query
	// we intend to run.
	cursor->clearQueryTree();

	if (prepareQuery(cursor,dropquery.getString(),
					dropquery.getStringLength())) {
		executeQuery(cursor);
	}
	cursor->closeResultSet();
}

void sqlrservercontroller::truncateTempTables(sqlrservercursor *cursor) {

	// run through the temp table list, truncating tables
	for (singlylinkedlistnode< char * >
			*sln=pvt->_sessiontemptablesfortrunc.getFirst();
			sln; sln=sln->getNext()) {
		truncateTempTable(cursor,sln->getValue());
		delete[] sln->getValue();
	}
	pvt->_sessiontemptablesfortrunc.clear();

	// truncate global temp tables...

	// all tables...
	if (pvt->_allglobaltemptables) {

		const char	*tablename=NULL;
		uint64_t	fieldlength;
		bool		blob;
		bool		null;
		const char	*query=getGlobalTempTableListQuery();

		sqlrservercursor	*gttcur=newCursor();
		if (open(gttcur) &&
			prepareQuery(gttcur,query,charstring::length(query)) &&
			executeQuery(gttcur)) {

			bool	error;
			while (fetchRow(gttcur,&error)) {
				getField(gttcur,0,
					&tablename,&fieldlength,&blob,&null);
				truncateTempTable(cursor,tablename);

				// FIXME: kludgy
				nextRow(gttcur);
			}
		}
		closeResultSet(gttcur);
		close(gttcur);
		deleteCursor(gttcur);

		return;
	}

	// specific tables...
	for (singlylinkedlistnode< char * >
			*sln=pvt->_globaltemptables.getFirst();
						sln; sln=sln->getNext()) {
		truncateTempTable(cursor,sln->getValue());
	}
}

void sqlrservercontroller::truncateTempTable(sqlrservercursor *cursor,
						const char *tablename) {
	stringbuffer	truncatequery;
	truncatequery.append(cursor->truncateTableQuery());
	truncatequery.append(" ")->append(tablename);
	if (prepareQuery(cursor,truncatequery.getString(),
					truncatequery.getStringLength())) {
		executeQuery(cursor);
	}
	cursor->closeResultSet();
}

void sqlrservercontroller::closeClientConnection(uint32_t bytes) {

	// Sometimes the server sends the result set and closes the socket
	// while part of it is buffered but not yet transmitted.  This causes
	// the client to receive a partial result set or error.  Telling the
	// socket to linger doesn't always fix it.  Doing a read here should 
	// guarantee that the client will close its end of the connection 
	// before the server closes its end; the server will wait for data 
	// from the client (which it will never receive) and when the client 
	// closes its end (which it will only do after receiving the entire
	// result set) the read will fall through.  This should guarantee 
	// that the client will get the the entire result set without
	// requiring the client to send data back indicating so.
	//
	// Also, if auth fails, the client could send an entire query
	// and bind vars before it reads the error and closes the socket.
	// We have to absorb all of that data.  We shouldn't just loop forever
	// though, that would provide a point of entry for a DOS attack.  We'll
	// read the maximum number of bytes that could be sent.
	raiseDebugMessageEvent("waiting for client to close the connection...");
	uint16_t	dummy;
	uint32_t	counter=0;
	pvt->_clientsock->useNonBlockingMode();
	while (pvt->_clientsock->read(&dummy,pvt->_idleclienttimeout,0)>0 &&
								counter<bytes) {
		counter++;
	}
	pvt->_clientsock->useBlockingMode();
	
	raiseDebugMessageEvent("done waiting for client to close the connection");

	// close the client socket
	raiseDebugMessageEvent("closing the client socket...");
	pvt->_clientsock->close();
	delete pvt->_clientsock;
	raiseDebugMessageEvent("done closing the client socket");

	// in proxy mode, the client socket is pointed at the handoff
	// socket which now needs to be reestablished
	if (pvt->_proxymode) {
		registerForHandoff();
	}
}

void sqlrservercontroller::closeSuspendedSessionSockets() {

	if (pvt->_suspendedsession) {
		return;
	}

	// If we're no longer in a suspended session but had to open a set of
	// sockets to handle a suspended session, close those sockets here.
	if (pvt->_serversockun || pvt->_serversockin) {
		raiseDebugMessageEvent("closing sockets from "
				"a previously suspended session...");
	}
	if (pvt->_serversockun) {
		pvt->_lsnr.removeFileDescriptor(pvt->_serversockun);
		delete pvt->_serversockun;
		pvt->_serversockun=NULL;
	}
	if (pvt->_serversockin) {
		for (uint64_t index=0;
				index<pvt->_serversockincount;
				index++) {
			pvt->_lsnr.removeFileDescriptor(
					pvt->_serversockin[index]);
			delete pvt->_serversockin[index];
			pvt->_serversockin[index]=NULL;
		}
		delete[] pvt->_serversockin;
		pvt->_serversockin=NULL;
		pvt->_serversockincount=0;
	}
	if (pvt->_serversockun || pvt->_serversockin) {
		raiseDebugMessageEvent("done closing sockets from "
				"a previously suspended session...");
	}
}

void sqlrservercontroller::shutDown() {

	raiseDebugMessageEvent("closing connection...");

	if (pvt->_inclientsession) {
		endSession();
		decrementOpenClientConnections();
		pvt->_inclientsession=false;
	}

	// decrement the connection counter or signal the scaler to
	if (pvt->_decrementonclose && 
			pvt->_cfg->getDynamicScaling() &&
			pvt->_semset &&
			pvt->_shmem) {
		decrementConnectionCount();
	}

	// deregister and close the handoff socket if necessary
	if (pvt->_connected) {
		deRegisterForHandoff();
	}

	// close the cursors
	closeCursors(true);

	// try to log out
	logOut();

	// clear the pool
	pvt->_lsnr.removeAllFileDescriptors();

	// close, clean up all sockets
	delete pvt->_serversockun;

	for (uint64_t index=0; index<pvt->_serversockincount; index++) {
		delete pvt->_serversockin[index];
	}
	delete[] pvt->_serversockin;

	// The scaler might need to decrement the connection count after
	// waiting for the child to exit.  On unix-like platforms, we can
	// handle that with SIGCHLD/waitpid().  On other platforms we can
	// do it with a semaphore.
	if (!pvt->_decrementonclose &&
			pvt->_cfg &&
			pvt->_cfg->getDynamicScaling() &&
			pvt->_semset &&
			pvt->_shmem &&
			!process::supportsGetChildStateChange()) {
		pvt->_semset->signal(11);
	}

	raiseDebugMessageEvent("done closing connection");
}

void sqlrservercontroller::closeCursors(bool destroy) {

	raiseDebugMessageEvent("closing cursors...");

	if (pvt->_cur) {
		while (pvt->_cursorcount) {
			pvt->_cursorcount--;

			if (pvt->_cur[pvt->_cursorcount]) {
				pvt->_cur[pvt->_cursorcount]->closeResultSet();
				close(pvt->_cur[pvt->_cursorcount]);
				if (destroy) {
					deleteCursor(
						pvt->_cur[pvt->_cursorcount]);
					pvt->_cur[pvt->_cursorcount]=NULL;
				}
			}
		}
		if (destroy) {
			delete[] pvt->_cur;
			pvt->_cur=NULL;
		}
	}

	raiseDebugMessageEvent("done closing cursors...");
}

void sqlrservercontroller::deleteCursor(sqlrservercursor *curs) {
	pvt->_conn->deleteCursor(curs);
	decrementOpenDatabaseCursors();
}

bool sqlrservercontroller::createSharedMemoryAndSemaphores(const char *id) {

	char	*idfilename=NULL;
	charstring::printf(&idfilename,"%s%s.ipc",pvt->_pth->getIpcDir(),id);

	pvt->_debugstr.clear();
	pvt->_debugstr.append("attaching to shared memory and semaphores ");
	pvt->_debugstr.append("id filename: ")->append(idfilename);
	raiseDebugMessageEvent(pvt->_debugstr.getString());

	// connect to the shared memory
	raiseDebugMessageEvent("attaching to shared memory...");
	pvt->_shmem=new sharedmemory();
	if (!pvt->_shmem->attach(file::generateKey(idfilename,1),
						sizeof(sqlrshm))) {
		char	*err=error::getErrorString();
		stderror.printf("Couldn't attach to shared memory segment: "
				"%s\n",err);
		delete[] err;
		delete pvt->_shmem;
		pvt->_shmem=NULL;
		delete[] idfilename;
		return false;
	}
	pvt->_shm=(sqlrshm *)pvt->_shmem->getPointer();
	if (!pvt->_shm) {
		stderror.printf("Failed to get pointer to shm\n");
		delete pvt->_shmem;
		pvt->_shmem=NULL;
		delete[] idfilename;
		return false;
	}

	// connect to the semaphore set
	raiseDebugMessageEvent("attaching to semaphores...");
	pvt->_semset=new semaphoreset();
	if (!pvt->_semset->attach(file::generateKey(idfilename,1),13)) {
		char	*err=error::getErrorString();
		stderror.printf("Couldn't attach to semaphore set: "
				"%s\n",err);
		delete[] err;
		delete pvt->_semset;
		delete pvt->_shmem;
		pvt->_semset=NULL;
		pvt->_shmem=NULL;
		delete[] idfilename;
		return false;
	}

	raiseDebugMessageEvent("done attaching to shared memory and semaphores");

	delete[] idfilename;

	return true;
}

void sqlrservercontroller::decrementConnectedClientCount() {

	raiseDebugMessageEvent("decrementing session count...");

	if (!pvt->_semset->waitWithUndo(5)) {
		// FIXME: bail somehow
	}

	// increment the connections-in-use count
	if (pvt->_shm->connectedclients) {
		pvt->_shm->connectedclients--;
	}

	// update the peak connections-in-use count
	if (pvt->_shm->connectedclients>pvt->_shm->peak_connectedclients) {
		pvt->_shm->peak_connectedclients=pvt->_shm->connectedclients;
	}

	// update the peak connections-in-use over the previous minute count
	datetime	dt;
	dt.getSystemDateAndTime();
	if (pvt->_shm->connectedclients>
			pvt->_shm->peak_connectedclients_1min ||
		dt.getEpoch()/60>
			pvt->_shm->peak_connectedclients_1min_time/60) {

		pvt->_shm->peak_connectedclients_1min=
				pvt->_shm->connectedclients;
		pvt->_shm->peak_connectedclients_1min_time=
				dt.getEpoch();
	}

	if (!pvt->_semset->signalWithUndo(5)) {
		// FIXME: bail somehow
	}

	raiseDebugMessageEvent("done decrementing session count");
}

bool sqlrservercontroller::acquireAnnounceMutex() {

	raiseDebugMessageEvent("acquiring announce mutex");

	setState(WAIT_SEMAPHORE);

	// Wait.  Bail if ttl is exceeded
	bool	result=false;
	if (pvt->_ttl>0 && pvt->_semset->supportsTimedSemaphoreOperations()) {
		result=pvt->_semset->waitWithUndo(0,pvt->_ttl,0);
	} else if (pvt->_ttl>0 && sys::signalsInterruptSystemCalls()) {
		pvt->_semset->dontRetryInterruptedOperations();
		alarmrang=0;
		signalmanager::alarm(pvt->_ttl);
		do {
			error::setErrorNumber(0);
			result=pvt->_semset->waitWithUndo(0);
		} while (!result &&
				error::getErrorNumber()==EINTR &&
				!alarmrang);
		signalmanager::alarm(0);
		pvt->_semset->retryInterruptedOperations();
	} else {
		result=pvt->_semset->waitWithUndo(0);
	}
	if (result) {
		raiseDebugMessageEvent("done acquiring announce mutex");
	} else {
		raiseDebugMessageEvent("ttl reached, aborting "
				"acquiring announce mutex");
	}
	return result;
}

void sqlrservercontroller::releaseAnnounceMutex() {
	raiseDebugMessageEvent("releasing announce mutex");
	pvt->_semset->signalWithUndo(0);
	raiseDebugMessageEvent("done releasing announce mutex");
}

void sqlrservercontroller::signalListenerToRead() {
	raiseDebugMessageEvent("signalling listener to read");
	pvt->_semset->signal(2);
	raiseDebugMessageEvent("done signalling listener to read");
}

void sqlrservercontroller::unSignalListenerToRead() {
	pvt->_semset->wait(2);
}

bool sqlrservercontroller::waitForListenerToFinishReading() {

	raiseDebugMessageEvent("waiting for listener");

	// Wait.  Bail if ttl is exceeded
	bool	result=false;
	if (pvt->_ttl>0 && pvt->_semset->supportsTimedSemaphoreOperations()) {
		result=pvt->_semset->wait(3,pvt->_ttl,0);
	} else if (pvt->_ttl>0 && sys::signalsInterruptSystemCalls()) {
		pvt->_semset->dontRetryInterruptedOperations();
		alarmrang=0;
		signalmanager::alarm(pvt->_ttl);
		do {
			error::setErrorNumber(0);
			result=pvt->_semset->wait(3);
		} while (!result &&
				error::getErrorNumber()==EINTR &&
				!alarmrang);
		signalmanager::alarm(0);
		pvt->_semset->retryInterruptedOperations();
	} else {
		result=pvt->_semset->wait(3);
	}
	if (result) {
		raiseDebugMessageEvent("done waiting for listener");
	} else {
		raiseDebugMessageEvent("ttl reached, aborting waiting for listener");
	}

	// Reset this semaphore to 0.  It can get left incremented if another
	// sqlr-connection is killed between calls to signalListenerToRead()
	// and this method.  It's ok to reset it here becuase no one except
	// uthis process has access to this semaphore at this time because of
	// the lock on the announce mutex (semaphore 0).
	pvt->_semset->setValue(3,0);

	return result;
}

void sqlrservercontroller::signalListenerToHandoff() {
	raiseDebugMessageEvent("signalling listener to handoff");
	pvt->_semset->signal(12);
	raiseDebugMessageEvent("done signalling listener to handoff");
}

void sqlrservercontroller::acquireConnectionCountMutex() {
	raiseDebugMessageEvent("acquiring connection count mutex");
	pvt->_semset->waitWithUndo(4);
	raiseDebugMessageEvent("done acquiring connection count mutex");
}

void sqlrservercontroller::releaseConnectionCountMutex() {
	raiseDebugMessageEvent("releasing connection count mutex");
	pvt->_semset->signalWithUndo(4);
	raiseDebugMessageEvent("done releasing connection count mutex");
}

void sqlrservercontroller::signalScalerToRead() {
	raiseDebugMessageEvent("signalling scaler to read");
	pvt->_semset->signal(8);
	raiseDebugMessageEvent("done signalling scaler to read");
}

void sqlrservercontroller::initConnStats() {

	pvt->_semset->waitWithUndo(9);

	// Find an available location in the connstats array.
	// It shouldn't be possible for sqlr-start or sqlr-scaler to start
	// more than MAXCONNECTIONS, so unless someone started one manually,
	// it should always be possible to find an open one.
	for (uint32_t i=0; i<MAXCONNECTIONS; i++) {
		pvt->_connstats=&(pvt->_shm->connstats[i]);
		if (!pvt->_connstats->processid) {

			pvt->_semset->signalWithUndo(9);

			// initialize the connection stats
			clearConnStats();
			setState(INIT);
			pvt->_connstats->index=i;
			pvt->_connstats->processid=process::getProcessId();
			pvt->_connstats->loggedinsec=pvt->_loggedinsec;
			pvt->_connstats->loggedinusec=pvt->_loggedinusec;
			return;
		}
	}

	pvt->_semset->signalWithUndo(9);

	// in case someone started a connection manually and
	// exceeded MAXCONNECTIONS, set this NULL here
	pvt->_connstats=NULL;
}

void sqlrservercontroller::clearConnStats() {
	if (!pvt->_connstats) {
		return;
	}
	bytestring::zero(pvt->_connstats,sizeof(struct sqlrconnstatistics));
}

sqlrparser *sqlrservercontroller::newParser() {

	pvt->_sqlrpnode=pvt->_cfg->getParser();
	const char	*module=pvt->_sqlrpnode->getAttributeValue("module");
	if (charstring::isNullOrEmpty(module)) {
		module="default";
	}

	pvt->_debugsqlrparser=pvt->_cfg->getDebugParser();

	if (pvt->_debugsqlrparser) {
		stdoutput.printf("loading parser module: %s\n",module);
	}

#ifdef SQLRELAY_ENABLE_SHARED
	// load the parser module
	stringbuffer	modulename;
	modulename.append(pvt->_pth->getLibExecDir());
	modulename.append(SQLR);
	modulename.append("parser_");
	modulename.append(module)->append(".")->append(SQLRELAY_MODULESUFFIX);
	if (!pvt->_sqlrpdl.open(modulename.getString(),true,true)) {
		char	*error=pvt->_sqlrpdl.getError();
		stderror.printf("failed to load parser module: %s\n%s\n",
						module,(error)?error:"");
		delete[] error;
		return NULL;
	}

	// load the parser itself
	stringbuffer	functionname;
	functionname.append("new_sqlrparser_")->append(module);
	sqlrparser	*(*newParser)(sqlrservercontroller *, domnode *)=
			(sqlrparser *(*)(sqlrservercontroller *, domnode *))
			pvt->_sqlrpdl.getSymbol(functionname.getString());
	if (!newParser) {
		char	*error=pvt->_sqlrpdl.getError();
		stderror.printf("failed to load parser: %s\n%s\n",
				module,(error)?error:"");
		delete[] error;
		return NULL;
	}

	sqlrparser	*parser=(*newParser)(this,pvt->_sqlrpnode);

#else
	sqlrparser	*parser;
	stringbuffer	parsername;
	parsername.append(module);
	#include "sqlrparserassignments.cpp"
	{
		parser=NULL;
	}
#endif

	if (pvt->_debugsqlrparser) {
		stdoutput.printf("success\n");
	}

	return parser;
}

bool sqlrservercontroller::bulkLoadBegin(const char *id,
						const char *errorfieldtable,
						const char *errorrowtable,
						uint64_t maxerrorcount,
						bool droperrortables) {

	// FIXME: validate "errorfieldtable" for safety
	// FIXME: validate "errorrowtable" for safety

	if (pvt->_debugbulkload) {
		stdoutput.printf("%d: bulk load begin:\n"
				"		id: \"%s\"\n",
						process::getProcessId(),id);
		stdoutput.printf("		error table 1: \"%s\"\n"
				"		error table 2: \"%s\"\n"
				"		max error count: %lld\n"
				"		drop error tables: %s\n",
				errorfieldtable,
				errorrowtable,
				maxerrorcount,
				(droperrortables)?"yes":"no");
	}

	// FIXME: bail if error tables already exist

	// get an md5 sum of the id
	// use this rather than using the table directly because:
	// * the id could be sensitive information
	// * the id might not conform to valid file naming conventions
	md5	m;
	m.append((const unsigned char *)id,charstring::length(id));
	char	*md5str=charstring::hexEncode(m.getHash(),m.getHashLength());
	id=md5str;

	// create a key file and key
	delete[] pvt->_bulkserveridfilename;
	charstring::printf(&pvt->_bulkserveridfilename,"%sbulk-%s.ipc",
						pvt->_pth->getIpcDir(),id);
	delete[] md5str;
	if (!file::createFile(pvt->_bulkserveridfilename,
				permissions::ownerReadWrite())) {
		setError(SQLR_ERROR_BULKLOADBEGIN_IPC_FILE_STRING,
				SQLR_ERROR_BULKLOADBEGIN_IPC_FILE,true);
		bulkLoadEnd();
		return false;
	}
	key_t	key=file::generateKey(pvt->_bulkserveridfilename,1);
	if (key==-1) {
		setError(SQLR_ERROR_BULKLOADBEGIN_IPC_KEY_STRING,
				SQLR_ERROR_BULKLOADBEGIN_IPC_KEY,true);
		bulkLoadEnd();
		return false;
	}

	// calculate shared memory segment size
	uint64_t	shmsize=charstring::length(errorfieldtable)+1+
				charstring::length(errorrowtable)+1+
				sizeof(uint64_t)+
				sizeof(bool)+
				pvt->_maxquerysize+1+
				sizeof(uint16_t)+
				pvt->_maxbindcount*
					(sizeof(sqlrserverbindvartype_t)+
					sizeof(uint32_t));

	// create shared memory
	pvt->_bulkservershmem=new sharedmemory;
	if (!pvt->_bulkservershmem->create(key,shmsize,
				permissions::evalPermString("rw-r-----"))) {
		setError(SQLR_ERROR_BULKLOADBEGIN_SHM_STRING,
				SQLR_ERROR_BULKLOADBEGIN_SHM,true);
		bulkLoadEnd();
		return false;
	}
	pvt->_bulkservershm=
		(unsigned char *)pvt->_bulkservershmem->getPointer();
	bytestring::zero(pvt->_bulkservershm,pvt->_maxquerysize+1);

	// put error tables, maxerrorcount,
	// and drop error tables flag in shared memory
	unsigned char	*ptr=pvt->_bulkservershm;

	uint64_t	len=charstring::length(errorfieldtable);
	pvt->_bulkerrorfieldtable=(const char *)ptr;
	bytestring::copy(ptr,errorfieldtable,len);
	ptr+=len;
	*ptr='\0';
	ptr++;

	len=charstring::length(errorrowtable);
	pvt->_bulkerrorrowtable=(const char *)ptr;
	bytestring::copy(ptr,errorrowtable,len);
	ptr+=len;
	*ptr='\0';
	ptr++;

	bytestring::copy(ptr,&maxerrorcount,sizeof(uint64_t));
	ptr+=sizeof(uint64_t);

	bytestring::copy(ptr,&droperrortables,sizeof(bool));
	ptr+=sizeof(bool);

	// get positions for query and data format
	pvt->_bulkservershmquery=ptr;
	pvt->_bulkservershmdataformat=ptr+sizeof(uint64_t)+pvt->_maxquerysize+1;

	return true;
}

bool sqlrservercontroller::bulkLoadCheckpoint(const char *id) {

	if (pvt->_debugbulkload) {
		stdoutput.printf("%d: bulk load checkpoint: \"%s\"\n",
						process::getProcessId(),id);
	}

	// FIXME: not sure what to do here...
	// maybe run a checkpoint query on each joined connection?

	// FIXME: do something...
	return true;
}

bool sqlrservercontroller::bulkLoadPrepareQuery(const char *query,
						uint64_t querylen,
						uint16_t inbindcount,
						sqlrserverbindvar *inbinds) {

	if (pvt->_debugbulkload) {
		stdoutput.printf("%d: bulk load prepare query:\n%.*s\n",
					process::getProcessId(),querylen,query);
	}

	// validate that the query is an insert
	const char	*ptr=skipWhitespaceAndComments(query);
	if (charstring::compareIgnoringCase(ptr,"insert",6) ||
				!character::isWhitespace(*(ptr+6))) {
		setError(SQLR_ERROR_BULKLOADPREPARE_INVALID_QUERY_STRING,
				SQLR_ERROR_BULKLOADPREPARE_INVALID_QUERY,true);
		return false;
	}

	// create error tables
	if (!bulkLoadCreateErrorTables(query,querylen,
					pvt->_bulkerrorfieldtable,
					pvt->_bulkerrorrowtable)) {
		return false;
	}

	// put the query length and query in shared memory
	unsigned char	*qptr=pvt->_bulkservershmquery;
	*((uint64_t *)qptr)=querylen;
	qptr+=sizeof(uint64_t);
	bytestring::copy(qptr,query,querylen);
	qptr[querylen]='\0';

	// put the data format in shared memory...
	ptr=(const char *)pvt->_bulkservershmdataformat;

	// copy in the number of data format elements...
	*((uint16_t *)ptr)=inbindcount;
	ptr+=sizeof(uint16_t);

	// for each data format element
	for (uint16_t i=0; i<inbindcount; i++) {

		sqlrserverbindvar	*inbind=&(inbinds[i]);

		// copy in the data format element type
		*((sqlrserverbindvartype_t *)ptr)=inbind->type;
		ptr+=sizeof(sqlrserverbindvartype_t);

		// copy in the data format element size
		*((uint32_t *)ptr)=inbind->valuesize;
		ptr+=sizeof(uint32_t);

		if (pvt->_debugbulkload) {
			stdoutput.printf("	%.*s - %d(%d)\n",
						inbind->variablesize,
						inbind->variable,
						inbind->type,
						inbind->valuesize);
		}
	}

	return true;
}

bool sqlrservercontroller::bulkLoadCreateErrorTables(
					const char *query,
					uint64_t querylen,
					const char *errorfieldtable,
					const char *errorrowtable) {

	bool	retval=false;
	sqlrservercursor	*cursor=newCursor();
	if (open(cursor)) {
		retval=bulkLoadCreateErrorTable1(
				cursor,query,querylen,errorfieldtable) &&
			bulkLoadCreateErrorTable2(
				cursor,query,querylen,errorrowtable);
	}
	close(cursor);
	deleteCursor(cursor);

	return retval;
}

bool sqlrservercontroller::bulkLoadCreateErrorTable1(
					sqlrservercursor *cursor,
					const char *query,
					uint64_t querylen,
					const char *errorfieldtable) {

	// FIXME: this is right for teradata, but not right in general...
	// teradata doesn't allow DDL inside of a
	// tx unless it's the last thing in the tx
	bool	wasintx=inTransaction();
	if (wasintx) {
		prepareQuery(cursor,"ET",2);
		executeQuery(cursor);
		closeResultSet(cursor);

		prepareQuery(cursor,"BT",2);
		executeQuery(cursor);
		closeResultSet(cursor);
	}

	if (pvt->_debugbulkload) {
		stdoutput.printf("%d: bulk load create error table: %s\n",
							process::getProcessId(),
							errorfieldtable);
	}

	// FIXME: this needs to be overridable per-connection

	const char	*errorfieldtablequery=
			"create table %s ("
			"	ErrorCode integer,"
			"	ErrorFieldName varchar(120),"
			"	DataParcel varbyte(64000)"
			")";

	bool	retval=true;
	stringbuffer	str;
	str.writeFormatted(errorfieldtablequery,errorfieldtable);
	if (!prepareQuery(cursor,str.getString(),str.getStringLength()) ||
							!executeQuery(cursor)) {
		saveErrorFromCursor(cursor);
		retval=false;
	}
	closeResultSet(cursor);

	// FIXME: this is right for teradata, but not right in general...
	// teradata doesn't allow DDL inside of a
	// tx unless it's the last thing in the tx
	if (wasintx) {
		prepareQuery(cursor,"ET",2);
		executeQuery(cursor);
		closeResultSet(cursor);

		prepareQuery(cursor,"BT",2);
		executeQuery(cursor);
		closeResultSet(cursor);
		pvt->_intransaction=true;
		raiseBeginTransactionEvent();
	}

	return retval;
}

bool sqlrservercontroller::bulkLoadCreateErrorTable2(
					sqlrservercursor *cursor,
					const char *query,
					uint64_t querylen,
					const char *errorrowtable) {

	// FIXME: this is right for teradata, but not right in general...
	// teradata doesn't allow DDL inside of a
	// tx unless it's the last thing in the tx
	bool	wasintx=inTransaction();
	if (wasintx) {
		prepareQuery(cursor,"ET",2);
		executeQuery(cursor);
		closeResultSet(cursor);

		prepareQuery(cursor,"BT",2);
		executeQuery(cursor);
		closeResultSet(cursor);
	}

	if (pvt->_debugbulkload) {
		stdoutput.printf("%d: bulk load create error table: %s\n",
					process::getProcessId(),errorrowtable);
	}

	// FIXME: this needs to be overridable per-connection
	// and the default implementation needs to construct a create table
	// from scratch, not relying on create table ... as select ... as this
	// isn't supported by all db's


	// get the table name from the query...
	char	*table=NULL;
	bulkLoadParseInsert(query,querylen,&table,NULL,NULL);

	const char	*errorrowtablequery=
			"create table %s as (select * from %s) with no data";

	bool	retval=true;
	stringbuffer	str;
	str.writeFormatted(errorrowtablequery,errorrowtable,table);
	if (!prepareQuery(cursor,str.getString(),str.getStringLength()) ||
							!executeQuery(cursor)) {
		saveErrorFromCursor(cursor);
		retval=false;
	}
	closeResultSet(cursor);
	delete[] table;

	// FIXME: this is right for teradata, but not right in general...
	// teradata doesn't allow DDL inside of a
	// tx unless it's the last thing in the tx
	if (wasintx) {
		prepareQuery(cursor,"ET",2);
		executeQuery(cursor);
		closeResultSet(cursor);

		prepareQuery(cursor,"BT",2);
		executeQuery(cursor);
		closeResultSet(cursor);
		pvt->_intransaction=true;
		raiseBeginTransactionEvent();
	}

	return retval;
}

bool sqlrservercontroller::bulkLoadJoin(const char *id) {

	if (pvt->_debugbulkload) {
		stdoutput.printf("%d: bulk load join:\n"
				"		id: \"%s\"\n",
					process::getProcessId(),id);
	}

	// get an md5 sum of the id (see bulkLoadBegin for why)
	md5	m;
	m.append((const unsigned char *)id,charstring::length(id));
	char	*md5str=charstring::hexEncode(m.getHash(),m.getHashLength());
	id=md5str;

	// create the key
	char	*idfilename;
	charstring::printf(&idfilename,"%sbulk-%s.ipc",
				pvt->_pth->getIpcDir(),id);
	delete[] md5str;
	key_t	key=file::generateKey(idfilename,1);
	delete[] idfilename;
	if (key==-1) {
		setError(SQLR_ERROR_BULKLOADJOIN_IPC_KEY_STRING,
				SQLR_ERROR_BULKLOADJOIN_IPC_KEY,true);
		return false;
	}

	// (re)init shared memory
	if (pvt->_bulkclientshmem) {
		delete pvt->_bulkclientshmem;
	}

	// attach to shared memory
	pvt->_bulkclientshmem=new sharedmemory;
	if (!pvt->_bulkclientshmem->attach(key,pvt->_maxquerysize+1)) {
		setError(SQLR_ERROR_BULKLOADJOIN_SHM_STRING,
				SQLR_ERROR_BULKLOADJOIN_SHM,true);
		return false;
	}
	pvt->_bulkclientshm=
		(unsigned char *)pvt->_bulkclientshmem->getPointer();

	// get error tables
	const unsigned char	*ptr=pvt->_bulkclientshm;

	pvt->_bulkerrorfieldtable=(const char *)ptr;
	ptr+=charstring::length(ptr);
	ptr++;

	pvt->_bulkerrorrowtable=(const char *)ptr;
	ptr+=charstring::length(ptr);
	ptr++;

	// get max error count
	pvt->_bulkmaxerrorcount=*((uint64_t *)ptr);
	ptr+=sizeof(uint64_t);

	// get drop error tables flag
	pvt->_bulkdroperrortables=*((bool *)ptr);
	ptr+=sizeof(bool);

	// get query length and query
	pvt->_bulkquerylen=*((uint64_t *)ptr);
	ptr+=sizeof(uint64_t);
	pvt->_bulkquery=(const char *)ptr;
	ptr+=pvt->_maxquerysize+1;

	// get data format definitions
	pvt->_bulkdataformat=(const unsigned char *)ptr;

	// clear bulk data lists
	pvt->_bulkdata.clear();
	pvt->_bulkdatalen.clear();

	if (pvt->_debugbulkload) {
		stdoutput.printf("		error table 1: \"%s\"\n"
				"		error table 2: \"%s\"\n"
				"		max error count: %lld\n"
				"		drop error tables : %s\n"
				"		query:\n%.*s\n",
				pvt->_bulkerrorfieldtable,
				pvt->_bulkerrorrowtable,
				pvt->_bulkmaxerrorcount,
				(pvt->_bulkdroperrortables)?"yes":"no",
				pvt->_bulkquerylen,
				pvt->_bulkquery);
	}

	return true;
}

bool sqlrservercontroller::bulkLoadInputBind(const unsigned char *data,
							uint64_t datalen) {

	if (pvt->_debugbulkload) {
		stdoutput.printf("%d: bulk load input bind:\n",
					process::getProcessId());
		stdoutput.safePrint(data,datalen);
		stdoutput.write('\n');
	}

	pvt->_bulkdata.append(data);
	pvt->_bulkdatalen.append(datalen);

	return true;
}

bool sqlrservercontroller::bulkLoadExecuteQuery() {

	if (pvt->_debugbulkload) {
		stdoutput.printf("%d: bulk load execute query - %d rows\n",
						process::getProcessId(),
						pvt->_bulkdata.getLength());
	}

	// init the bulk cursor
	pvt->_bulkcursor=newCursor();

	// open the cursor
	bool	success=true;
	if (!open(pvt->_bulkcursor)) {
		setError(SQLR_ERROR_BULKLOADEXECUTE_OPEN_CURSOR_STRING,
				SQLR_ERROR_BULKLOADEXECUTE_OPEN_CURSOR,true);
		success=false;
	}

	// prepare the query
	if (success && !prepareQuery(pvt->_bulkcursor,
					pvt->_bulkquery,
					pvt->_bulkquerylen,
					true,true,true)) {
		saveErrorFromCursor(pvt->_bulkcursor);
		success=false;
	}

	if (success) {

		bulkLoadInitBinds();

		// run through the bulk data, binding and executing each row
		uint64_t		errorcount=0;
		singlylinkedlistnode<const unsigned char *>
				*datanode=pvt->_bulkdata.getFirst();
		singlylinkedlistnode<uint64_t>
				*datalennode=pvt->_bulkdatalen.getFirst();
		while (datanode) {

			bulkLoadBindRow(datanode->getValue(),
					datalennode->getValue());

			if (!executeQuery(pvt->_bulkcursor)) {
				bulkLoadError();
				errorcount++;
			}


			// bail if too many errors occurred
			if (errorcount>pvt->_bulkmaxerrorcount) {
				setError(
			SQLR_ERROR_BULKLOADEXECUTE_TOO_MANY_ERRORS_STRING,
			SQLR_ERROR_BULKLOADEXECUTE_TOO_MANY_ERRORS,true);
				success=false;
				break;
			}

			datanode=datanode->getNext();
			datalennode=datalennode->getNext();
		}
	}

	// close the bulk cursor and clean up
	closeResultSet(pvt->_bulkcursor);
	close(pvt->_bulkcursor);
	deleteCursor(pvt->_bulkcursor);
	pvt->_bulkcursor=NULL;
	pvt->_bulkdata.clear();
	pvt->_bulkdatalen.clear();

	return success;
}

void sqlrservercontroller::bulkLoadInitBinds() {

	// get the table, column names, and binds from the query...
	char			*table=NULL;
	linkedlist<char *>	cols;
	linkedlist<char *>	binds;
	bulkLoadParseInsert(pvt->_bulkquery,
				pvt->_bulkquerylen,
				&table,&cols,&binds);

	// map columns to binds (if we actually have columns)
	dictionary<char *, char *>	bindtocol;
	bool				havecols=false;
	if (cols.getLength()) {
		havecols=true;
		linkedlistnode<char *> *bind=binds.getFirst();
		linkedlistnode<char *> *col=cols.getFirst();
		while (bind && col) {
			bindtocol.setValue(bind->getValue(),col->getValue());
			bind=bind->getNext();
			col=col->getNext();
		}
	}

	// Get column info for the table and set bind type accordingly...
	// Ideally we'd call getColumnList() rather than running a "select *",
	// but some ODBC drivers (eg. teradata) don't support SQLColumns(),
	// so getColumnList() fails.  Everyone supports "select *", and as
	// long as we don't fetch anything, it's fast enough.
	sqlrservercursor	*cur=newCursor();
	if (open(cur)) {

		stringbuffer	query;
		query.append("select * from ")->append(table);

		if (prepareQuery(cur,query.getString(),
					query.getStringLength()) &&
					executeQuery(cur)) {

			memorypool		*bindpool=
						getBindPool(pvt->_bulkcursor);
			sqlrserverbindvar	*inbinds=
						getInputBinds(pvt->_bulkcursor);

			// run through the binds...
			uint16_t	inbindcount=0;
			linkedlistnode<char *> *bind=binds.getFirst();
			while (bind) {

				// set up the input bind name
				sqlrserverbindvar	*inbind=
							&(inbinds[inbindcount]);
				char		*var=bind->getValue();
				uint16_t	varsize=charstring::length(var);
				inbind->variable=
					(char *)bindpool->allocate(varsize+1);
				charstring::copy(inbind->variable,var);
				inbind->variablesize=varsize;

				// figure out which column the bind maps to
				uint16_t	colindex=inbindcount;
				if (havecols) {

					const char	*col=
						bindtocol.getValue(
							bind->getValue());

					for (uint16_t i=0;
							i<colCount(cur);
							i++) {

						if (!charstring::compare(col,
							getColumnName(cur,i))) {
							colindex=i;
						}
					}

					// FIXME: what if we don't find it?
				}

				// get the type of the column
				uint16_t	type=getColumnType(
								cur,colindex);

				// set the bind type from the column type
				// (the order of these tests is import, eg.
				// floats are numbers and dates are binary)
				if (isFloatType(type)) {
					inbind->type=
						SQLRSERVERBINDVARTYPE_DOUBLE;
				} else if (isNumberType(type)) {
					inbind->type=
						SQLRSERVERBINDVARTYPE_INTEGER;
				} else if (isDateTimeType(type)) {
					inbind->type=
						SQLRSERVERBINDVARTYPE_DATE;
				} else if (isBinaryType(type)) {
					inbind->type=
						SQLRSERVERBINDVARTYPE_BLOB;
				} else {
					inbind->type=
						SQLRSERVERBINDVARTYPE_STRING;
				}

				// bump to the next bind
				bind=bind->getNext();
				inbindcount++;
			}

			// set the input bind count
			setInputBindCount(pvt->_bulkcursor,inbindcount);
		}

		closeResultSet(cur);
	} else {
		// FIXME: error...
	}
	close(cur);
	deleteCursor(cur);

	// clean up
	delete[] table;
	cols.clearAndArrayDelete();
	binds.clearAndArrayDelete();
}

void sqlrservercontroller::bulkLoadParseInsert(const char *query,
						uint64_t querylen,
						char **table,
						linkedlist<char *> *cols,
						linkedlist<char *> *binds) {

	// get query end
	const char	*queryend=query+querylen;

	// skip whitespace and comments
	const char	*ptr=skipWhitespaceAndComments(query);
	if (!*ptr) {
		return;
	}

	// skip "insert"
	ptr+=6;
	if (ptr>=queryend) {
		return;
	}

	// skip whitespace
	ptr=skipWhitespaceAndComments(ptr);
	if (!*ptr) {
		return;
	}

	// skip "into"
	ptr+=4;
	if (ptr>=queryend) {
		return;
	}

	// skip whitespace
	ptr=skipWhitespaceAndComments(ptr);
	if (!*ptr) {
		return;
	}

	// get table
	const char	*start=ptr;
	while (*ptr && !character::isWhitespace(*ptr)) {
		ptr++;
	}
	if (!*ptr) {
		return;
	}

	// return the table
	if (table) {
		// FIXME: make "table" safe to substitute into a query
		*table=charstring::duplicate(start,ptr-start);
	}

	// FIXME: Some db's (teradata) don't require a values keyword.
	// If it is missing then the parenthesized list following the
	// table is the column values, rather than the list of columns.

	// skip to column list
	ptr=skipWhitespaceAndComments(ptr);
	if (!*ptr) {
		return;
	}

	// parse column list
	if (*ptr=='(') {

		// skip (
		ptr++;
		if (!*ptr) {
			return;
		}

		// skip to )
		start=ptr;
		while (*ptr && *ptr!=')') {
			ptr++;
		}
		if (!*ptr) {
			return;
		}

		// parse out columns
		if (cols) {
			char		**parts;
			uint64_t	partcount;
			charstring::split(start,ptr-start,
						",",false,&parts,&partcount);
			for (uint64_t i=0; i<partcount; i++) {
				charstring::bothTrim(parts[i]);
				cols->append(parts[i]);
			}
		}
	}

	// skip )
	ptr++;
	if (!*ptr) {
		return;
	}

	// skip to "values"
	ptr=skipWhitespaceAndComments(ptr);
	if (!*ptr) {
		return;
	}

	// skip "values"
	ptr+=6;
	if (ptr>=queryend) {
		return;
	}

	// skip to actual values
	ptr=skipWhitespaceAndComments(ptr);
	if (!*ptr) {
		return;
	}

	// parse values list
	// FIXME: currently this assumes that all values are binds
	if (*ptr=='(') {

		// skip (
		ptr++;
		if (!*ptr) {
			return;
		}

		// skip to )
		start=ptr;
		while (*ptr && *ptr!=')') {
			ptr++;
		}
		if (!*ptr) {
			return;
		}

		// parse out binds
		if (binds) {
			char		**parts;
			uint64_t	partcount;
			charstring::split(start,ptr-start,
						",",false,&parts,&partcount);
			for (uint64_t i=0; i<partcount; i++) {
				charstring::bothTrim(parts[i]);
				// override whatever bind prefix was in the
				// query with the correct one (in case a
				// non-native bind format was used)
				parts[i][0]=bindFormat()[0];
				binds->append(parts[i]);
			}
		}
	}
}

void sqlrservercontroller::bulkLoadBindRow(const unsigned char *data,
							uint64_t datalen) {

	if (pvt->_debugbulkload) {
		stdoutput.printf("%d: bind row {\n",process::getProcessId());
	}

	// get the number of data format elements...
	const unsigned char	*ptr=pvt->_bulkdataformat;
	uint16_t		formatcount=*((uint16_t *)ptr);
	ptr+=sizeof(uint16_t);

	// get the input bind count and input binds
	uint16_t		inbindcount=getInputBindCount(pvt->_bulkcursor);
	sqlrserverbindvar	*inbinds=getInputBinds(pvt->_bulkcursor);
	memorypool		*bindpool=getBindPool(pvt->_bulkcursor);

	uint16_t formatindex=0;
	uint16_t inbindindex=0;
	for (;;) {

		// get the format element type
		sqlrserverbindvartype_t	type=*((sqlrserverbindvartype_t *)ptr);
		ptr+=sizeof(sqlrserverbindvartype_t);

		// get the format element size
		uint32_t	size=*((uint32_t *)ptr);
		ptr+=sizeof(uint32_t);
		
		// skip delimiters and newlines
		if (type==SQLRSERVERBINDVARTYPE_DELIMITER ||
				type==SQLRSERVERBINDVARTYPE_NEWLINE) {
			if (pvt->_debugbulkload) {
				if (type==SQLRSERVERBINDVARTYPE_DELIMITER) {
					stdoutput.printf(
						"	delimiter: %.*s\n",
						size,data);
				}
				if (type==SQLRSERVERBINDVARTYPE_NEWLINE) {
					stdoutput.printf("	newline\n");
				}
			}
			formatindex++;
			data+=size;
			continue;
		}

		// for now we only support static-length strings
		if (type!=SQLRSERVERBINDVARTYPE_STRING) {
			// FIXME: set error...
			break;
		}

		// get the bind value
		sqlrserverbindvar	*inbind=&(inbinds[inbindindex]);
		const unsigned char	*val=data;
		inbind->valuesize=size;
		data+=size;

		if (pvt->_debugbulkload) {
			stdoutput.printf("	%.*s %d(%d): ",
						inbind->variablesize,
						inbind->variable,
						inbind->type,
						size);
		}

		char	*temp=NULL;
		switch (inbind->type) {

			case SQLRSERVERBINDVARTYPE_STRING:
			case SQLRSERVERBINDVARTYPE_BLOB:
			case SQLRSERVERBINDVARTYPE_CLOB:
				inbind->value.stringval=(char *)val;
				if (pvt->_debugbulkload) {
					stdoutput.printf("(%d) ",
						inbind->valuesize);
					stdoutput.printf("%.*s\n",
						inbind->valuesize,
						inbind->value.stringval);
				}
				break;

			case SQLRSERVERBINDVARTYPE_INTEGER:
				inbind->value.integerval=
					charstring::toInteger((char *)val);
				inbind->isnull=nonNullBindValue();
				if (pvt->_debugbulkload) {
					stdoutput.printf("%d\n",
						inbind->value.integerval);
				}
				break;

			case SQLRSERVERBINDVARTYPE_DOUBLE:
				{
				temp=charstring::duplicate(
					(char *)val,inbind->valuesize);
				inbind->value.doubleval.value=
					charstring::toFloat(temp);
				inbind->value.doubleval.precision=
					inbind->valuesize-
					((inbind->value.
						doubleval.value<0)?1:0);
				const char	*dot=
					charstring::findFirst(temp,'.');
				if (dot) {
					inbind->value.doubleval.
							precision--;
					dot++;
					inbind->value.doubleval.scale=
						temp+inbind->valuesize-dot;
				} else {
					inbind->value.
						doubleval.scale=0;
				}
				delete[] temp;
				if (pvt->_debugbulkload) {
					stdoutput.printf("%*.*f\n",
						inbind->value.
							doubleval.precision,
						inbind->value.
							doubleval.scale,
						inbind->value.
							doubleval.value);
				}
				}
				break;

			case SQLRSERVERBINDVARTYPE_DATE:
				{
				temp=charstring::duplicate(
					(char *)val,inbind->valuesize);
				charstring::bothTrim(temp);

				// copy out the timezone
				// (and null terminate the date before it)
				char	*firstspace=
					charstring::findFirst(temp," ");
				char	*lastspace=
					charstring::findLast(temp," ");
				if (lastspace && lastspace!=firstspace) {
					const char	*tz=lastspace+1;
					inbind->value.dateval.tz=
						(char *)bindpool->allocate(
						temp+inbind->valuesize-tz+1);
					charstring::copy(inbind->value.
								dateval.tz,tz);
					*lastspace='\0';
				} else {
					inbind->value.dateval.tz=NULL;
				}

				// FIXME: this assumes ISO-ish date/time format
				// but who knows what we'll actually get, the
				// ddmm, yyyyddmm, and delimiter parameters need
				// to be configurable...
				if (!parseDateTime(
						temp,
						false,
						false,
						"-",
						&inbind->value.dateval.year,
						&inbind->value.dateval.month,
						&inbind->value.dateval.day,
						&inbind->value.dateval.hour,
						&inbind->value.dateval.minute,
						&inbind->value.dateval.second,
						&inbind->value.dateval.
								microsecond,
						&inbind->value.dateval.
								isnegative)) {
					// FIXME: what if this fails?
				}
				delete[] temp;

				// allocate enough space to store the date/time
				// string or whatever buffer a child might need
				// to store a date 512 bytes ought to be enough
				inbind->value.dateval.buffersize=512;
				inbind->value.dateval.buffer=
					(char *)bindpool->
						allocate(inbind->value.dateval.
								buffersize);

				if (pvt->_debugbulkload) {
					stdoutput.printf(
						"%04hd-%02hd-%02hd "
						"%02hd:%02hd:%02hd.%06d %s\n",
						inbind->value.dateval.year,
						inbind->value.dateval.month,
						inbind->value.dateval.day,
						inbind->value.dateval.hour,
						inbind->value.dateval.minute,
						inbind->value.dateval.second,
						inbind->value.dateval.
								microsecond,
						((inbind->value.dateval.tz)?
							inbind->value.
								dateval.tz:""));
				}
				}

			default:
				break;
		}

		if (type==SQLRSERVERBINDVARTYPE_NULL) {
			inbind->isnull=nullBindValue();
		} else {
			inbind->isnull=nonNullBindValue();
		}

		// move on
		inbindindex++;
		if (inbindindex==inbindcount) {
			break;
		}
		formatindex++;
		if (formatindex==formatcount) {
			break;
		}
	}

	if (pvt->_debugbulkload) {
		stdoutput.printf("}\n");
	}
}

void sqlrservercontroller::bulkLoadError() {

	// get the error
	uint32_t	errorlength;
	int64_t		errnum;
	bool		liveconnection;
	errorMessage(pvt->_bulkcursor,
			pvt->_bulkcursor->getErrorBuffer(),
			pvt->_maxerrorlength,
			&errorlength,&errnum,&liveconnection);

	if (pvt->_debugbulkload) {
		stdoutput.printf("%d: bulk load error\n%d\n%.*s\n",
					process::getProcessId(),
					errnum,errorlength,
					pvt->_bulkcursor->getErrorBuffer());
	}

	// store the error
	if (!bulkLoadStoreError(errnum,
				pvt->_bulkcursor->getErrorBuffer(),
				errorlength,
				pvt->_bulkerrorfieldtable,
				pvt->_bulkerrorrowtable)) {
		// FIXME: error...
	}
}

bool sqlrservercontroller::bulkLoadStoreError(int64_t errorcode,
						const char *error,
						uint32_t errorlength,
						const char *bulkerrorfieldtable,
						const char *bulkerrorrowtable) {

	// FIXME: this needs to be overridable per-connection

	// FIXME: reuse a cursor...
	sqlrservercursor	*cur=newCursor();
	if (open(cur)) {

		// insert into errorfieldtable...

		// FIXME: use binds...
		const char	*errorquery=
				"insert into %s values (%lld,'%s','%s')";
		stringbuffer	query;
		query.writeFormatted(errorquery,
					bulkerrorfieldtable,
					errorcode,
					// FIXME: get the column somehow
					"bad_column",
					// FIXME: get the data somehow
					"bad_data");

		if (!prepareQuery(cur,query.getString(),
					query.getStringLength()) ||
					!executeQuery(cur)) {
			// FIXME: error...
		}
		closeResultSet(cur);

		// FIXME: insert into errorrowtable...
	}
	close(cur);
	deleteCursor(cur);

	return true;
}

bool sqlrservercontroller::bulkLoadEnd() {

	if (pvt->_debugbulkload) {
		stdoutput.printf("%d: bulk load end\n",
					process::getProcessId());
	}

	if (pvt->_bulkdroperrortables) {
		if (!bulkLoadDropErrorTables(pvt->_bulkerrorfieldtable,
						pvt->_bulkerrorrowtable)) {
			// FIXME: error...
		}
	}

	// delete shared memory
	delete pvt->_bulkservershmem;
	pvt->_bulkservershmem=NULL;
	pvt->_bulkservershm=NULL;

	// remove the id file
	if (pvt->_bulkserveridfilename) {
		file::remove(pvt->_bulkserveridfilename);
		delete[] pvt->_bulkserveridfilename;
	}
	pvt->_bulkserveridfilename=NULL;

	return true;
}

bool sqlrservercontroller::bulkLoadDropErrorTables(
					const char *errorfieldtable,
					const char *errorrowtable) {
	// FIXME: implement this...
	// this needs to be overridable per-connection
	return true;
}

void sqlrservercontroller::setState(enum sqlrconnectionstate_t state) {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->state=state;
	datetime	dt;
	dt.getSystemDateAndTime();
	pvt->_connstats->statestartsec=dt.getSeconds();
	pvt->_connstats->statestartusec=dt.getMicroseconds();
}

enum sqlrconnectionstate_t sqlrservercontroller::getState() {
	if (!pvt->_connstats) {
		return NOT_AVAILABLE;
	}
	return pvt->_connstats->state;
}

void sqlrservercontroller::setClientSessionStartTime() {
	if (!pvt->_connstats) {
		return;
	}
	datetime	dt;
	dt.getSystemDateAndTime();
	pvt->_connstats->clientsessionsec=dt.getSeconds();
	pvt->_connstats->clientsessionusec=dt.getMicroseconds();
}

void sqlrservercontroller::setCurrentUser(const char *user,
						uint32_t userlen) {
	if (!pvt->_connstats) {
		return;
	}
	uint32_t	len=userlen;
	if (len>USERSIZE-1) {
		len=USERSIZE-1;
	}
	charstring::copy(pvt->_connstats->user,user,len);
	pvt->_connstats->user[len]='\0';
}

void sqlrservercontroller::setCurrentQuery(const char *query,
						uint32_t querylen) {
	if (!pvt->_connstats) {
		return;
	}
	uint32_t	len=querylen;
	if (len>STATSQLTEXTLEN-1) {
		len=STATSQLTEXTLEN-1;
	}
	charstring::copy(pvt->_connstats->sqltext,query,len);
	pvt->_connstats->sqltext[len]='\0';
}

void sqlrservercontroller::setClientInfo(const char *info,
						uint32_t infolen) {
	if (!pvt->_connstats) {
		return;
	}
	uint64_t	len=infolen;
	if (len>STATCLIENTINFOLEN-1) {
		len=STATCLIENTINFOLEN-1;
	}
	charstring::copy(pvt->_connstats->clientinfo,info,len);
	pvt->_connstats->clientinfo[len]='\0';
}

void sqlrservercontroller::setClientAddr() {
	if (!pvt->_connstats) {
		return;
	}
	if (pvt->_clientsock) {
		char	*clientaddrbuf=pvt->_clientsock->getPeerAddress();
		if (clientaddrbuf) {
			charstring::copy(
				pvt->_connstats->clientaddr,clientaddrbuf);
			delete[] clientaddrbuf;
		} else {
			charstring::copy(
				pvt->_connstats->clientaddr,"UNIX");
		}
	} else {
		charstring::copy(pvt->_connstats->clientaddr,"internal");
	}
}

const char *sqlrservercontroller::getCurrentUser() {
	return (pvt->_connstats)?pvt->_connstats->user:NULL;
}

const char *sqlrservercontroller::getCurrentQuery() {
	return (pvt->_connstats)?pvt->_connstats->sqltext:NULL;
}

const char *sqlrservercontroller::getClientInfo() {
	return (pvt->_connstats)?pvt->_connstats->clientinfo:NULL;
}

const char *sqlrservercontroller::getClientAddr() {
	return (pvt->_connstats)?pvt->_connstats->clientaddr:NULL;
}

void sqlrservercontroller::setInstanceDisabled(bool disabled) {
	pvt->_shm->disabled=disabled;
}

bool sqlrservercontroller::getInstanceDisabled() {
	return pvt->_shm->disabled;
}

void sqlrservercontroller::incrementOpenDatabaseConnections() {
	pvt->_semset->waitWithUndo(9);
	pvt->_shm->open_db_connections++;
	pvt->_shm->opened_db_connections++;
	pvt->_semset->signalWithUndo(9);
}

void sqlrservercontroller::decrementOpenDatabaseConnections() {
	pvt->_semset->waitWithUndo(9);
	if (pvt->_shm->open_db_connections) {
		pvt->_shm->open_db_connections--;
	}
	pvt->_semset->signalWithUndo(9);
}

void sqlrservercontroller::incrementOpenClientConnections() {
	pvt->_semset->waitWithUndo(9);
	pvt->_shm->open_cli_connections++;
	pvt->_shm->opened_cli_connections++;
	pvt->_semset->signalWithUndo(9);
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nconnect++;
}

void sqlrservercontroller::decrementOpenClientConnections() {
	pvt->_semset->waitWithUndo(9);
	if (pvt->_shm->open_cli_connections) {
		pvt->_shm->open_cli_connections--;
	}
	pvt->_semset->signalWithUndo(9);
}

void sqlrservercontroller::incrementOpenDatabaseCursors() {
	pvt->_semset->waitWithUndo(9);
	pvt->_shm->open_db_cursors++;
	pvt->_shm->opened_db_cursors++;
	pvt->_semset->signalWithUndo(9);
}

void sqlrservercontroller::decrementOpenDatabaseCursors() {
	pvt->_semset->waitWithUndo(9);
	if (pvt->_shm->open_db_cursors) {
		pvt->_shm->open_db_cursors--;
	}
	pvt->_semset->signalWithUndo(9);
}

void sqlrservercontroller::incrementTimesNewCursorUsed() {
	pvt->_semset->waitWithUndo(9);
	pvt->_shm->times_new_cursor_used++;
	pvt->_semset->signalWithUndo(9);
}

void sqlrservercontroller::incrementTimesCursorReused() {
	pvt->_semset->waitWithUndo(9);
	pvt->_shm->times_cursor_reused++;
	pvt->_semset->signalWithUndo(9);
}

void sqlrservercontroller::incrementQueryCounts(sqlrquerytype_t querytype) {

	pvt->_semset->waitWithUndo(9);

	// update total queries
	pvt->_shm->total_queries++;

	// update queries-per-second stats...

	// re-init stats if necessary
	datetime	dt;
	dt.getSystemDateAndTime();
	time_t	now=dt.getEpoch();
	int	index=now%STATQPSKEEP;
	if (pvt->_shm->timestamp[index]!=now) {
		pvt->_shm->timestamp[index]=now;
		pvt->_shm->qps_select[index]=0;
		pvt->_shm->qps_update[index]=0;
		pvt->_shm->qps_insert[index]=0;
		pvt->_shm->qps_delete[index]=0;
		pvt->_shm->qps_create[index]=0;
		pvt->_shm->qps_drop[index]=0;
		pvt->_shm->qps_alter[index]=0;
		pvt->_shm->qps_custom[index]=0;
		pvt->_shm->qps_etc[index]=0;
	}

	// increment per-query-type stats
	switch (querytype) {
		case SQLRQUERYTYPE_SELECT:
			pvt->_shm->qps_select[index]++;
			break;
		case SQLRQUERYTYPE_INSERT:
			pvt->_shm->qps_insert[index]++;
			break;
		case SQLRQUERYTYPE_UPDATE:
			pvt->_shm->qps_update[index]++;
			break;
		case SQLRQUERYTYPE_DELETE:
			pvt->_shm->qps_delete[index]++;
			break;
		case SQLRQUERYTYPE_CREATE:
			pvt->_shm->qps_create[index]++;
			break;
		case SQLRQUERYTYPE_DROP:
			pvt->_shm->qps_drop[index]++;
			break;
		case SQLRQUERYTYPE_ALTER:
			pvt->_shm->qps_alter[index]++;
			break;
		case SQLRQUERYTYPE_CUSTOM:
			pvt->_shm->qps_custom[index]++;
			break;
		case SQLRQUERYTYPE_ETC:
		default:
			pvt->_shm->qps_etc[index]++;
			break;
	}

	pvt->_semset->signalWithUndo(9);

	if (!pvt->_connstats) {
		return;
	}
	if (querytype==SQLRQUERYTYPE_CUSTOM) {
		pvt->_connstats->ncustomsql++;
	} else {
		pvt->_connstats->nsql++;
	}
}

void sqlrservercontroller::incrementTotalErrors() {
	pvt->_semset->waitWithUndo(9);
	pvt->_shm->total_errors++;
	pvt->_semset->signalWithUndo(9);
}

void sqlrservercontroller::incrementAuthCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nauth++;
}

void sqlrservercontroller::incrementSuspendSessionCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nsuspend_session++;
}

void sqlrservercontroller::incrementEndSessionCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nend_session++;
}

void sqlrservercontroller::incrementPingCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nping++;
}

void sqlrservercontroller::incrementIdentifyCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nidentify++;
}

void sqlrservercontroller::incrementAutocommitCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nautocommit++;
}

void sqlrservercontroller::incrementBeginCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nbegin++;
}

void sqlrservercontroller::incrementCommitCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->ncommit++;
}

void sqlrservercontroller::incrementRollbackCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nrollback++;
}

void sqlrservercontroller::incrementDbVersionCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->ndbversion++;
}

void sqlrservercontroller::incrementBindFormatCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nbindformat++;
}

void sqlrservercontroller::incrementServerVersionCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nserverversion++;
}

void sqlrservercontroller::incrementSelectDatabaseCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nselectdatabase++;
}

void sqlrservercontroller::incrementGetCurrentDatabaseCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->ngetcurrentdatabase++;
}

void sqlrservercontroller::incrementGetLastInsertIdCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->ngetlastinsertid++;
}

void sqlrservercontroller::incrementDbHostNameCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->ndbhostname++;
}

void sqlrservercontroller::incrementDbIpAddressCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->ndbipaddress++;
}

void sqlrservercontroller::incrementNewQueryCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nnewquery++;
}

void sqlrservercontroller::incrementReexecuteQueryCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nreexecutequery++;
}

void sqlrservercontroller::incrementFetchFromBindCursorCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nfetchfrombindcursor++;
}

void sqlrservercontroller::incrementFetchResultSetCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nfetchresultset++;
}

void sqlrservercontroller::incrementAbortResultSetCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nabortresultset++;
}

void sqlrservercontroller::incrementSuspendResultSetCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nsuspendresultset++;
}

void sqlrservercontroller::incrementResumeResultSetCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nresumeresultset++;
}

void sqlrservercontroller::incrementGetDbListCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->ngetdblist++;
}

void sqlrservercontroller::incrementGetTableListCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->ngettablelist++;
}

void sqlrservercontroller::incrementGetColumnListCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->ngetcolumnlist++;
}

void sqlrservercontroller::incrementGetQueryTreeCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->ngetquerytree++;
}

void sqlrservercontroller::incrementReLogInCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nrelogin++;
}

void sqlrservercontroller::incrementNextResultSetCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nnextresultset++;
}

void sqlrservercontroller::incrementNextResultSetAvailableCount() {
	if (!pvt->_connstats) {
		return;
	}
	pvt->_connstats->nnextresultsetavailable++;
}

uint32_t sqlrservercontroller::getStatisticsIndex() {
	if (!pvt->_connstats) {
		return 0;
	}
	return pvt->_connstats->index;
}

void sqlrservercontroller::sessionStartQueries() {
	// run a configurable set of queries at the start of each session
	for (linkedlistnode< char * > *node=
			pvt->_cfg->getSessionStartQueries()->getFirst();
			node; node=node->getNext()) {
		sessionQuery(node->getValue());
	}
}

void sqlrservercontroller::sessionEndQueries() {
	// run a configurable set of queries at the end of each session
	for (linkedlistnode< char * > *node=
			pvt->_cfg->getSessionEndQueries()->getFirst();
			node; node=node->getNext()) {
		sessionQuery(node->getValue());
	}
}

void sqlrservercontroller::sessionQuery(const char *query) {

	// create the select database query
	size_t	querylen=charstring::length(query);

	sqlrservercursor	*cur=newCursor();
	if (open(cur) &&
		prepareQuery(cur,query,querylen) && executeQuery(cur)) {
		closeResultSet(cur);
	}
	close(cur);
	deleteCursor(cur);
}

const char *sqlrservercontroller::getConnectStringValue(const char *variable) {

	// If we're using password encryption and the password is requested,
	// and the password encryption module supports two-way encryption,
	// then return the decrypted version of the password, otherwise just
	// return the value as-is.
	const char	*peid=pvt->_constr->getPasswordEncryption();
	if (pvt->_sqlrpe && charstring::length(peid) &&
			!charstring::compare(variable,"password")) {
		sqlrpwdenc	*pe=
			pvt->_sqlrpe->getPasswordEncryptionById(peid);
		if (!pe->oneWay()) {
			delete[] pvt->_decrypteddbpassword;
			pvt->_decrypteddbpassword=pe->decrypt(
				pvt->_constr->getConnectStringValue(variable));
			return pvt->_decrypteddbpassword;
		}
	}
	return pvt->_constr->getConnectStringValue(variable);
}

void sqlrservercontroller::setUser(const char *user) {
	pvt->_user=user;
}

void sqlrservercontroller::setPassword(const char *password) {
	pvt->_password=password;
}

const char *sqlrservercontroller::getUser() {
	return pvt->_user;
}

const char *sqlrservercontroller::getPassword() {
	return pvt->_password;
}

void sqlrservercontroller::setConnectTimeout(uint64_t connecttimeout) {
	pvt->_connecttimeout=connecttimeout;
}

uint64_t sqlrservercontroller::getConnectTimeout() {
	return pvt->_connecttimeout;
}

void sqlrservercontroller::setQueryTimeout(uint64_t querytimeout) {
	pvt->_querytimeout=querytimeout;
}

uint64_t sqlrservercontroller::getQueryTimeout() {
	return pvt->_querytimeout;
}

void sqlrservercontroller::setExecuteDirect(bool executedirect) {
	pvt->_executedirect=executedirect;
}

bool sqlrservercontroller::getExecuteDirect() {
	return pvt->_executedirect;
}

void sqlrservercontroller::setFakeTransactionBlocks(bool ftb) {
	pvt->_faketransactionblocks=ftb;
}

bool sqlrservercontroller::getFakeTransactionBlocks() {
	return pvt->_faketransactionblocks;
}

void sqlrservercontroller::setFakeAutoCommit(bool fac) {
	pvt->_fakeautocommit=fac;
}

bool sqlrservercontroller::getFakeAutoCommit() {
	return pvt->_fakeautocommit;
}

void sqlrservercontroller::setInitialAutoCommit(bool iac) {
	pvt->_initialautocommit=iac;
}

bool sqlrservercontroller::getInitialAutoCommit() {
	return pvt->_initialautocommit;
}

const char *sqlrservercontroller::bindFormat() {
	return pvt->_conn->bindFormat();
}

int16_t sqlrservercontroller::nonNullBindValue() {
	return pvt->_conn->nonNullBindValue();
}

int16_t sqlrservercontroller::nullBindValue() {
	return pvt->_conn->nullBindValue();
}

bool sqlrservercontroller::bindValueIsNull(int16_t isnull) {
	return pvt->_conn->bindValueIsNull(isnull);
}

void sqlrservercontroller::setFakeInputBinds(bool fake) {
	pvt->_fakeinputbinds=fake;
}

bool sqlrservercontroller::getFakeInputBinds() {
	return pvt->_fakeinputbinds;
}

void sqlrservercontroller::setFetchAtOnce(uint32_t fao) {
	pvt->_fetchatonce=fao;
}

void sqlrservercontroller::setMaxColumnCount(uint32_t mcc) {
	pvt->_maxcolumncount=mcc;
}

void sqlrservercontroller::setMaxFieldLength(uint32_t mfl) {
	pvt->_maxfieldlength=mfl;
}

uint32_t sqlrservercontroller::getFetchAtOnce() {
	return pvt->_fetchatonce;
}

uint32_t sqlrservercontroller::getMaxColumnCount() {
	return pvt->_maxcolumncount;
}

uint32_t sqlrservercontroller::getMaxFieldLength() {
	return pvt->_maxfieldlength;
}

bool sqlrservercontroller::getColumnNames(const char *query,
					stringbuffer *output) {

	// sanity check on the query
	if (!query) {
		return false;
	}

	size_t		querylen=charstring::length(query);

	sqlrservercursor	*gcncur=newCursor();
	if (open(gcncur) &&
		prepareQuery(gcncur,query,querylen) && executeQuery(gcncur)) {

		// build column list...
		getColumnNameList(gcncur,output);
	}
	closeResultSet(gcncur);
	close(gcncur);
	deleteCursor(gcncur);
	return true;
}

void sqlrservercontroller::addSessionTempTableForDrop(const char *table) {
	pvt->_sessiontemptablesfordrop.append(charstring::duplicate(table));
}

void sqlrservercontroller::addTransactionTempTableForDrop(const char *table) {
	pvt->_transtemptablesfordrop.append(charstring::duplicate(table));
}

void sqlrservercontroller::addGlobalTempTables(const char *gtts) {

	// all tables
	if (!charstring::compare(gtts,"%")) {
		pvt->_allglobaltemptables=true;
		return;
	}

	// specific tables
	char		**gttlist=NULL;
	uint64_t	gttlistcount=0;
	charstring::split(gtts,",",true,&gttlist,&gttlistcount);
	for (uint64_t i=0; i<gttlistcount; i++) {
		pvt->_globaltemptables.append(gttlist[i]);
	}
	delete[] gttlist;
}

void sqlrservercontroller::addSessionTempTableForTrunc(const char *table) {
	pvt->_sessiontemptablesfortrunc.append(charstring::duplicate(table));
}

void sqlrservercontroller::addTransactionTempTableForTrunc(const char *table) {
	pvt->_transtemptablesfortrunc.append(charstring::duplicate(table));
}

bool sqlrservercontroller::logEnabled() {
	return (pvt->_sqlrlg!=NULL);
}

bool sqlrservercontroller::notificationsEnabled() {
	return (pvt->_sqlrn!=NULL);
}

void sqlrservercontroller::raiseDebugMessageEvent(const char *info) {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,NULL,
					SQLRLOGGER_LOGLEVEL_DEBUG,
					SQLREVENT_DEBUG_MESSAGE,
					info);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,NULL,
					SQLREVENT_DEBUG_MESSAGE,
					info);
	}
}

void sqlrservercontroller::raiseClientConnectedEvent() {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,NULL,
					SQLRLOGGER_LOGLEVEL_INFO,
					SQLREVENT_CLIENT_CONNECTED,
					NULL);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,NULL,
					SQLREVENT_CLIENT_CONNECTED,
					NULL);
	}
}

void sqlrservercontroller::raiseClientConnectionRefusedEvent(const char *info) {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,NULL,
					SQLRLOGGER_LOGLEVEL_WARNING,
					SQLREVENT_CLIENT_CONNECTION_REFUSED,
					info);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,NULL,
					SQLREVENT_CLIENT_CONNECTION_REFUSED,
					info);
	}
}

void sqlrservercontroller::raiseClientDisconnectedEvent(const char *info) {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,NULL,
					SQLRLOGGER_LOGLEVEL_INFO,
					SQLREVENT_CLIENT_DISCONNECTED,
					info);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,NULL,
					SQLREVENT_CLIENT_DISCONNECTED,
					info);
	}
}

void sqlrservercontroller::raiseClientProtocolErrorEvent(
						sqlrservercursor *cursor,
						const char *info,
						ssize_t result) {
	if (!pvt->_sqlrlg && !pvt->_sqlrn) {
		return;
	}
	stringbuffer	errorbuffer;
	errorbuffer.append(info);
	if (result==0) {
		errorbuffer.append(": client closed connection");
	} else if (result==RESULT_ERROR) {
		errorbuffer.append(": error");
	} else if (result==RESULT_TIMEOUT) {
		errorbuffer.append(": timeout");
	} else if (result==RESULT_ABORT) {
		errorbuffer.append(": abort");
	}
	if (error::getErrorNumber()) {
		char	*error=error::getErrorString();
		errorbuffer.append(": ")->append((error)?error:"");
		delete[] error;
	}
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,cursor,
					SQLRLOGGER_LOGLEVEL_ERROR,
					SQLREVENT_CLIENT_PROTOCOL_ERROR,
					errorbuffer.getString());
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,cursor,
					SQLREVENT_CLIENT_PROTOCOL_ERROR,
					errorbuffer.getString());
	}
}

void sqlrservercontroller::raiseDbLogInEvent() {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,NULL,
					SQLRLOGGER_LOGLEVEL_INFO,
					SQLREVENT_DB_LOGIN,
					NULL);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,NULL,
					SQLREVENT_DB_LOGIN,
					NULL);
	}
}

void sqlrservercontroller::raiseDbLogOutEvent() {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,NULL,
					SQLRLOGGER_LOGLEVEL_INFO,
					SQLREVENT_DB_LOGOUT,
					NULL);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,NULL,
					SQLREVENT_DB_LOGOUT,
					NULL);
	}
}

void sqlrservercontroller::raiseDbErrorEvent(sqlrservercursor *cursor,
							const char *info) {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,cursor,
					SQLRLOGGER_LOGLEVEL_ERROR,
					SQLREVENT_DB_ERROR,
					info);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,cursor,
					SQLREVENT_DB_ERROR,
					info);
	}
}

void sqlrservercontroller::raiseDbWarningEvent(sqlrservercursor *cursor,
							const char *info) {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,cursor,
					SQLRLOGGER_LOGLEVEL_WARNING,
					SQLREVENT_DB_WARNING,
					info);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,cursor,
					SQLREVENT_DB_WARNING,
					info);
	}
}

void sqlrservercontroller::raiseQueryEvent(sqlrservercursor *cursor) {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,cursor,
					SQLRLOGGER_LOGLEVEL_INFO,
					SQLREVENT_QUERY,
					NULL);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,cursor,
					SQLREVENT_QUERY,
					NULL);
	}
}

void sqlrservercontroller::raiseFilterViolationEvent(sqlrservercursor *cursor) {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,cursor,
					SQLRLOGGER_LOGLEVEL_INFO,
					SQLREVENT_FILTER_VIOLATION,
					NULL);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,cursor,
					SQLREVENT_FILTER_VIOLATION,
					NULL);
	}
}

void sqlrservercontroller::raiseInternalErrorEvent(sqlrservercursor *cursor,
							const char *info) {
	if (!pvt->_sqlrlg && !pvt->_sqlrn) {
		return;
	}
	stringbuffer	errorbuffer;
	errorbuffer.append(info);
	if (error::getErrorNumber()) {
		char	*error=error::getErrorString();
		errorbuffer.append(": ")->append((error)?error:"");
		delete[] error;
	}
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,cursor,
					SQLRLOGGER_LOGLEVEL_ERROR,
					SQLREVENT_INTERNAL_ERROR,
					errorbuffer.getString());
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,cursor,
					SQLREVENT_INTERNAL_ERROR,
					errorbuffer.getString());
	}
}

void sqlrservercontroller::raiseInternalWarningEvent(sqlrservercursor *cursor,
							const char *info) {
	if (!pvt->_sqlrlg && !pvt->_sqlrn) {
		return;
	}
	stringbuffer	warningbuffer;
	warningbuffer.append(info);
	if (error::getErrorNumber()) {
		char	*error=error::getErrorString();
		warningbuffer.append(": ")->append((error)?error:"");
		delete[] error;
	}
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,cursor,
					SQLRLOGGER_LOGLEVEL_WARNING,
					SQLREVENT_INTERNAL_WARNING,
					warningbuffer.getString());
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,cursor,
					SQLREVENT_INTERNAL_WARNING,
					warningbuffer.getString());
	}
}

void sqlrservercontroller::raiseScheduleViolationEvent(const char *info) {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,NULL,
					SQLRLOGGER_LOGLEVEL_WARNING,
					SQLREVENT_SCHEDULE_VIOLATION,
					info);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,NULL,
					SQLREVENT_SCHEDULE_VIOLATION,
					info);
	}
}

void sqlrservercontroller::raiseIntegrityViolationEvent(const char *info) {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,NULL,
					SQLRLOGGER_LOGLEVEL_ERROR,
					SQLREVENT_INTEGRITY_VIOLATION,
					info);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,NULL,
					SQLREVENT_INTEGRITY_VIOLATION,
					info);
	}
}

void sqlrservercontroller::raiseTranslationFailureEvent(
						sqlrservercursor *cursor,
						const char *info) {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,cursor,
					SQLRLOGGER_LOGLEVEL_ERROR,
					SQLREVENT_TRANSLATION_FAILURE,
					info);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,cursor,
					SQLREVENT_TRANSLATION_FAILURE,
					info);
	}
}

void sqlrservercontroller::raiseParseFailureEvent(
						sqlrservercursor *cursor,
						const char *info) {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,cursor,
					SQLRLOGGER_LOGLEVEL_ERROR,
					SQLREVENT_PARSE_FAILURE,
					info);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,cursor,
					SQLREVENT_PARSE_FAILURE,
					info);
	}
}

void sqlrservercontroller::raiseCursorOpenEvent(sqlrservercursor *cursor) {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,cursor,
					SQLRLOGGER_LOGLEVEL_INFO,
					SQLREVENT_CURSOR_OPEN,
					NULL);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,cursor,
					SQLREVENT_CURSOR_OPEN,
					NULL);
	}
}

void sqlrservercontroller::raiseCursorCloseEvent(sqlrservercursor *cursor) {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,cursor,
					SQLRLOGGER_LOGLEVEL_INFO,
					SQLREVENT_CURSOR_CLOSE,
					NULL);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,cursor,
					SQLREVENT_CURSOR_CLOSE,
					NULL);
	}
}

void sqlrservercontroller::raiseBeginTransactionEvent() {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,NULL,
					SQLRLOGGER_LOGLEVEL_INFO,
					SQLREVENT_BEGIN_TRANSACTION,
					NULL);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,NULL,
					SQLREVENT_BEGIN_TRANSACTION,
					NULL);
	}
}

void sqlrservercontroller::raiseCommitEvent() {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,NULL,
					SQLRLOGGER_LOGLEVEL_INFO,
					SQLREVENT_COMMIT,
					NULL);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,NULL,
					SQLREVENT_COMMIT,
					NULL);
	}
}

void sqlrservercontroller::raiseRollbackEvent() {
	if (pvt->_sqlrlg) {
		pvt->_sqlrlg->run(NULL,pvt->_conn,NULL,
					SQLRLOGGER_LOGLEVEL_INFO,
					SQLREVENT_ROLLBACK,
					NULL);
	}
	if (pvt->_sqlrn) {
		pvt->_sqlrn->run(NULL,pvt->_conn,NULL,
					SQLREVENT_ROLLBACK,
					NULL);
	}
}

void sqlrservercontroller::alarmHandler(int32_t signum) {
	alarmrang=1;
}

const char *sqlrservercontroller::dbHostName() {
	if (!pvt->_dbhostname || !pvt->_conn->cacheDbHostInfo()) {
		pvt->_dbhostname=pvt->_conn->dbHostName();
		pvt->_dbipaddress=pvt->_conn->dbIpAddress();
	}
	return pvt->_dbhostname;
}

const char *sqlrservercontroller::dbIpAddress() {
	if (!pvt->_dbipaddress || !pvt->_conn->cacheDbHostInfo()) {
		pvt->_dbhostname=pvt->_conn->dbHostName();
		pvt->_dbipaddress=pvt->_conn->dbIpAddress();
	}
	return pvt->_dbipaddress;
}

const char *sqlrservercontroller::identify() {
	return pvt->_conn->identify();
}

const char *sqlrservercontroller::dbVersion() {
	return pvt->_conn->dbVersion();
}

const char *sqlrservercontroller::translateTableName(const char *table) {
	if (pvt->_sqlrt) {
		const char	*newname=NULL;
		if (pvt->_sqlrt->getReplacementTableName(
					NULL,NULL,table,&newname)) {
			return newname;
		}
	}
	return NULL;
}

bool sqlrservercontroller::removeReplacementTable(const char *database,
							const char *schema,
							const char *table) {
	if (pvt->_sqlrt) {
		return pvt->_sqlrt->removeReplacementTable(
						database,schema,table);
	}
	return false;
}

bool sqlrservercontroller::removeReplacementIndex(const char *database,
							const char *schema,
							const char *table) {
	if (pvt->_sqlrt) {
		return pvt->_sqlrt->removeReplacementIndex(
						database,schema,table);
	}
	return false;
}

const char *sqlrservercontroller::getId() {
	return pvt->_cmdl->getId();
}

const char *sqlrservercontroller::getConnectionId() {
	return pvt->_connectionid;
}

const char *sqlrservercontroller::getLogDir() {
	return pvt->_pth->getLogDir();
}

const char *sqlrservercontroller::getDebugDir() {
	return pvt->_pth->getDebugDir();
}

bool sqlrservercontroller::isCustomQuery(sqlrservercursor *cursor) {
	return cursor->isCustomQuery();
}

bool sqlrservercontroller::fetchFromBindCursor(sqlrservercursor *cursor) {

	// set state
	setState(PROCESS_SQL);

	raiseDebugMessageEvent("fetching from bind cursor...");

	// reset flags
	cursor->setColumnInfoIsValid(false);
	cursor->setResultSetHeaderHasBeenHandled(false);

	// clear query buffer just so some future operation doesn't
	// get confused into thinking this cursor actually ran one
	cursor->getQueryBuffer()[0]='\0';
	cursor->setQueryLength(0);

	bool	success=cursor->fetchFromBindCursor();
	
	// reset total rows fetched
	cursor->clearTotalRowsFetched();

	if (success) {
		success=handleResultSetHeader(cursor);
	} else {
		// on failure save the error
		saveError(cursor);
	}

	raiseDebugMessageEvent((success)?"fetching from bind cursor succeeded":
					"fetching from bind cursor failed");
	raiseDebugMessageEvent("done fetching from bind cursor");

	return success;
}

bool sqlrservercontroller::nextResultSet(sqlrservercursor *cursor,
						bool *nextresultsetavailable) {

	bool	success=cursor->nextResultSet(nextresultsetavailable);

	// on failure get the error (unless it's already been set)
	if (!success && !cursor->getErrorNumber()) {
		uint32_t	errorlength;
		int64_t		errnum;
		bool		liveconnection;
		errorMessage(cursor,
				cursor->getErrorBuffer(),
				pvt->_maxerrorlength,
				&errorlength,&errnum,&liveconnection);
		cursor->setErrorLength(errorlength);
		cursor->setErrorNumber(errnum);
		cursor->setLiveConnection(liveconnection);
	}

	raiseDebugMessageEvent((success)?"nextResultSet cursor succeeded":
						"nextResultSet cursor failed");
	raiseDebugMessageEvent("done nextResultSet");

	return success;
}

void sqlrservercontroller::saveError(sqlrservercursor *cursor) {

	// don't overwrite any message that's already been saved
	if (cursor->getErrorLength()) {
		return;
	}

	// fetch the error into the per-cursor error buffers and flags
	uint32_t	errorlength;
	int64_t		errorcode;
	bool		liveconnection;
	cursor->errorMessage(cursor->getErrorBuffer(),
				pvt->_cfg->getMaxErrorLength(),
				&errorlength,
				&errorcode,
				&liveconnection);
	cursor->setErrorLength(errorlength);
	cursor->setErrorNumber(errorcode);
	cursor->setLiveConnection(liveconnection);

	if (pvt->_debugsql) {
		stdoutput.printf("\n%d:%d:ERROR:\n%d:",
					process::getProcessId(),
					cursor->getId(),
					errorcode);
		stdoutput.write(cursor->getErrorBuffer(),errorlength);
		stdoutput.write('\n');
	}
}

void sqlrservercontroller::errorMessage(sqlrservercursor *cursor,
						const char **errorbuffer,
						uint32_t *errorlength,
						int64_t *errorcode,
						bool *liveconnection) {
	saveError(cursor);
	*errorbuffer=cursor->getErrorBuffer();
	*errorlength=cursor->getErrorLength();
	*errorcode=cursor->getErrorNumber();
	*liveconnection=cursor->getLiveConnection();
}

void sqlrservercontroller::errorMessage(sqlrservercursor *cursor,
						char *errorbuffer,
						uint32_t errorbuffersize,
						uint32_t *errorlength,
						int64_t *errorcode,
						bool *liveconnection) {

	// fetch the error
	const char	*errorstring=NULL;
	errorMessage(cursor,&errorstring,errorlength,errorcode,liveconnection);

	// copy the error out
	charstring::safeCopy(errorbuffer,errorbuffersize,
				errorstring,*errorlength);
	if (*errorlength>errorbuffersize) {
		*errorlength=errorbuffersize;
	}
}

bool sqlrservercontroller::knowsRowCount(sqlrservercursor *cursor) {
	return cursor->knowsRowCount();
}

uint64_t sqlrservercontroller::rowCount(sqlrservercursor *cursor) {
	return cursor->rowCount();
}

bool sqlrservercontroller::knowsAffectedRows(sqlrservercursor *cursor) {
	return cursor->knowsAffectedRows();
}

uint64_t sqlrservercontroller::affectedRows(sqlrservercursor *cursor) {
	return cursor->affectedRows();
}

uint32_t sqlrservercontroller::colCount(sqlrservercursor *cursor) {

	// "db"cursor::prepareQuery() resets the column count to 0 before
	// preparing the query.  If the database knows the column count
	// post-prepare, then "db"cursor::prepareQuery() also sets it
	// immediately after preparing the query.
	//
	// But...
	//
	// If we're faking binds, then sqlrservercontroller::prepareQuery()
	// doesn't actually call "db"cursor::prepareQuery().  Instead,
	// sqlrservercontroller::executeQuery() calls it after faking the
	// binds.
	//
	// The app won't know that though, and if we're using a front end
	// (like the mysql fron end) which talks to the server after prepare,
	// then the app might do something like:
	//
	// * mysql_stmt_prepare(...)
	// * mysql_stmt_field_count(...)
	// * mysql_stmt_execute(...);
	//
	// ...expecting mysql_stmt_field_count to return the valid column count.
	//
	// Since we're faking binds though, and "db"cursor::prepareQuery()
	// hasn't actually been called by the time mysql_stmt_field_count is
	// called, then the column count returned by "db"cursor::colCount()
	// will still be the column count from the previous query.
	//
	// We can't just set it to 0 in closeResultSet because we want to be
	// able to access the column metadata after calling that.
	//
	// So, we'll handle it here.
	//
	// When the client prepares the query,
	// sqlrserverconnection::prepareQuery() will be called.  When it
	// executes the query, sqlrserverconnection::executeQuery() will be
	// called.  During that period, if bind-faking is enabled, then
	// cursor->getQueryHasBeenPrepared() will return false.  In that case,
	// return 0 for the column count.
	//
	// This results in the expected behavior when faking binds - the column
	// metadata isn't available until after execute.
	//
	// Arguably, the various getColumn*() methods should return 0 or NULL
	// as well at this time...
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return mapColumnCount(cursor->colCount());
}

uint16_t sqlrservercontroller::columnTypeFormat(sqlrservercursor *cursor) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return cursor->columnTypeFormat();
}

const char *sqlrservercontroller::getColumnName(sqlrservercursor *cursor,
							uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return NULL;
	}
	return cursor->getColumnNameFromBuffer(mapColumn(col));
}

uint16_t sqlrservercontroller::getColumnNameLength(sqlrservercursor *cursor,
							uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return cursor->getColumnNameLengthFromBuffer(mapColumn(col));
}

uint16_t sqlrservercontroller::getColumnType(sqlrservercursor *cursor,
							uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return cursor->getColumnTypeFromBuffer(mapColumn(col));
}

const char *sqlrservercontroller::getColumnTypeName(sqlrservercursor *cursor,
							uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return NULL;
	}
	return cursor->getColumnTypeNameFromBuffer(mapColumn(col));
}

uint16_t sqlrservercontroller::getColumnTypeNameLength(sqlrservercursor *cursor,
							uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return cursor->getColumnTypeNameLengthFromBuffer(mapColumn(col));
}

uint32_t sqlrservercontroller::getColumnLength(sqlrservercursor *cursor,
							uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return cursor->getColumnLengthFromBuffer(mapColumn(col));
}

uint32_t sqlrservercontroller::getColumnPrecision(sqlrservercursor *cursor,
							uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return cursor->getColumnPrecisionFromBuffer(mapColumn(col));
}

uint32_t sqlrservercontroller::getColumnScale(sqlrservercursor *cursor,
							uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return cursor->getColumnScaleFromBuffer(mapColumn(col));
}

uint16_t sqlrservercontroller::getColumnIsNullable(sqlrservercursor *cursor,
							uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return cursor->getColumnIsNullableFromBuffer(mapColumn(col));
}

uint16_t sqlrservercontroller::getColumnIsPrimaryKey(sqlrservercursor *cursor,
							uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return cursor->getColumnIsPrimaryKeyFromBuffer(mapColumn(col));
}

uint16_t sqlrservercontroller::getColumnIsUnique(sqlrservercursor *cursor,
							uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return cursor->getColumnIsUniqueFromBuffer(mapColumn(col));
}

uint16_t sqlrservercontroller::getColumnIsPartOfKey(sqlrservercursor *cursor,
							uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return cursor->getColumnIsPartOfKeyFromBuffer(mapColumn(col));
}

uint16_t sqlrservercontroller::getColumnIsUnsigned(sqlrservercursor *cursor,
							uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return cursor->getColumnIsUnsignedFromBuffer(mapColumn(col));
}

uint16_t sqlrservercontroller::getColumnIsZeroFilled(sqlrservercursor *cursor,
							uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return cursor->getColumnIsZeroFilledFromBuffer(mapColumn(col));
}

uint16_t sqlrservercontroller::getColumnIsBinary(sqlrservercursor *cursor,
							uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return cursor->getColumnIsBinaryFromBuffer(mapColumn(col));
}

uint16_t sqlrservercontroller::getColumnIsAutoIncrement(
						sqlrservercursor *cursor,
							uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return cursor->getColumnIsAutoIncrementFromBuffer(mapColumn(col));
}

const char *sqlrservercontroller::getColumnTable(sqlrservercursor *cursor,
								uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return cursor->getColumnTableFromBuffer(mapColumn(col));
}

uint16_t sqlrservercontroller::getColumnTableLength(sqlrservercursor *cursor,
								uint32_t col) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return 0;
	}
	return cursor->getColumnTableLengthFromBuffer(mapColumn(col));
}

void sqlrservercontroller::getColumnNameList(sqlrservercursor *cursor,
							stringbuffer *output) {
	// see comment in colCount()
	if (!cursor->getColumnInfoIsValid()) {
		return;
	}
	for (uint32_t i=0; i<colCount(cursor); i++) {
		if (i) {
			output->append(',');
		}
		output->append(getColumnName(cursor,i),
				getColumnNameLength(cursor,i));
	}
}

bool sqlrservercontroller::handleResultSetHeader(sqlrservercursor *cursor) {

	// set flag indicating that the column info is now valid
	cursor->setColumnInfoIsValid(true);

	// This could get called multiple times, depending on whether
	// column info is valid post-prepare or post-execute.  It's easier
	// to just call it and bail if its already been called, than to
	// keep track of whether it needs to be called or not and not call
	// it if it doesn't.
	if (cursor->getResultSetHeaderHasBeenHandled()) {
		return true;
	}
	cursor->setResultSetHeaderHasBeenHandled(true);

	// get arrays of field pointers,
	// helpfully provided for us by the cursor
	cursor->getColumnPointers(&(pvt->_columnnames),
				&(pvt->_columnnamelengths),
				&(pvt->_columntypes),
				&(pvt->_columntypenames),
				&(pvt->_columntypenamelengths),
				&(pvt->_columnlengths),
				&(pvt->_columnprecisions),
				&(pvt->_columnscales),
				&(pvt->_columnisnullables),
				&(pvt->_columnisprimarykeys),
				&(pvt->_columnisuniques),
				&(pvt->_columnispartofkeys),
				&(pvt->_columnisunsigneds),
				&(pvt->_columniszerofilleds),
				&(pvt->_columnisbinarys),
				&(pvt->_columnisautoincrements),
				&(pvt->_columntables),
				&(pvt->_columntablelengths));

	// remap columns
	uint32_t	colcount=colCount(cursor);
	for (uint32_t col=0; col<colcount; col++) {
		pvt->_columnnames[col]=
			cursor->getColumnName(mapColumn(col));
		pvt->_columnnamelengths[col]=
			cursor->getColumnNameLength(mapColumn(col));
		pvt->_columntypes[col]=
			cursor->getColumnType(mapColumn(col));
		pvt->_columntypenames[col]=
			cursor->getColumnTypeName(mapColumn(col));
		pvt->_columntypenamelengths[col]=
			cursor->getColumnTypeNameLength(mapColumn(col));
		pvt->_columnlengths[col]=
			cursor->getColumnLength(mapColumn(col));
		pvt->_columnprecisions[col]=
			cursor->getColumnPrecision(mapColumn(col));
		pvt->_columnscales[col]=
			cursor->getColumnScale(mapColumn(col));
		pvt->_columnisnullables[col]=
			cursor->getColumnIsNullable(mapColumn(col));
		pvt->_columnisprimarykeys[col]=
			cursor->getColumnIsPrimaryKey(mapColumn(col));
		pvt->_columnisuniques[col]=
			cursor->getColumnIsUnique(mapColumn(col));
		pvt->_columnispartofkeys[col]=
			cursor->getColumnIsPartOfKey(mapColumn(col));
		pvt->_columnisunsigneds[col]=
			cursor->getColumnIsUnsigned(mapColumn(col));
		pvt->_columniszerofilleds[col]=
			cursor->getColumnIsZeroFilled(mapColumn(col));
		pvt->_columnisbinarys[col]=
			cursor->getColumnIsBinary(mapColumn(col));
		pvt->_columnisautoincrements[col]=
			cursor->getColumnIsAutoIncrement(mapColumn(col));
		pvt->_columntables[col]=
			cursor->getColumnTable(mapColumn(col));
		pvt->_columntablelengths[col]=
			cursor->getColumnTableLength(mapColumn(col));
	}

	// translate columns
	if (pvt->_sqlrrsht && colcount) {

		if (pvt->_debugsqlrresultsetheadertranslation) {
			stdoutput.printf("\n===================="
				 	"===================="
				 	"===================="
				 	"===================\n\n");
			stdoutput.printf("translating result set header...\n");
		}

		if (!pvt->_sqlrrsht->run(pvt->_conn,
					cursor,colcount,
					&pvt->_columnnames,
					&pvt->_columnnamelengths,
					&pvt->_columntypes,
					&pvt->_columntypenames,
					&pvt->_columntypenamelengths,
					&pvt->_columnlengths,
					&pvt->_columnprecisions,
					&pvt->_columnscales,
					&pvt->_columnisnullables,
					&pvt->_columnisprimarykeys,
					&pvt->_columnisuniques,
					&pvt->_columnispartofkeys,
					&pvt->_columnisunsigneds,
					&pvt->_columniszerofilleds,
					&pvt->_columnisbinarys,
					&pvt->_columnisautoincrements,
					&pvt->_columntables,
					&pvt->_columntablelengths)) {
			setError(cursor,pvt->_sqlrrsht->getError(),
				SQLR_ERROR_RESULTSETHEADERTRANSLATION,true);
			return false;
		}
	}
	return true;
}

bool sqlrservercontroller::noRowsToReturn(sqlrservercursor *cursor) {
	return cursor->noRowsToReturn();
}

bool sqlrservercontroller::skipRow(sqlrservercursor *cursor, bool *error) {
	return cursor->skipRow(error);
}

bool sqlrservercontroller::fetchRow(sqlrservercursor *cursor, bool *error) {

	// initialize error
	*error=false;

	// get arrays of field pointers,
	// helpfully provided for us by the cursor
	cursor->getFieldPointers(&(pvt->_fieldnames),
					&(pvt->_fields),
					&(pvt->_fieldlengths),
					&(pvt->_blobs),
					&(pvt->_nulls));

	// get the column count
	// NOTE: Don't just set colcount=colCount(cursor) here.  If a column
	// map is being used, then it returns the column count of the map, which
	// could be smaller than the actual column count.  We need to be sure we
	// fetch all columns, so we need to use the raw column count here.
	uint32_t	colcount=(cursor->getColumnInfoIsValid())?
						cursor->colCount():0;

	// for timings...
	datetime	dt;

	if (pvt->_sqlrrsrbt) {

		// if we have row block translations, then
		// this is a little complex...

		// if we're on the first row of a block...
		if (!(cursor->getTotalRowsFetched()%
				pvt->_sqlrrsrbt->getRowBlockSize())) {

			if (pvt->_debugsqlrresultsetrowblocktranslation) {
				stdoutput.printf("\n===================="
				 		"===================="
				 		"===================="
				 		"===================\n\n");
				stdoutput.printf("translating result "
						"set row block...\n");
			}

			// for each row in the block...
			for (uint32_t j=0;
				j<pvt->_sqlrrsrbt->getRowBlockSize(); j++) {

				// fetch the row, bail if fetch failed
				if (!cursor->fetchRow(error)) {
					break;
				}

				// handle errors
				if (*error) {
					return false;
				}

				// use the provided field pointer arrays to get
				// the pointers to the column names and actual
				// field data
				for (uint32_t i=0; i<colcount; i++) {

					pvt->_fieldnames[i]=
						getColumnName(cursor,i);
					pvt->_fields[i]=NULL;
					pvt->_fieldlengths[i]=0;
					pvt->_blobs[i]=false;
					pvt->_nulls[i]=false;
					cursor->getField(i,
						&(pvt->_fields[i]),
						&(pvt->_fieldlengths[i]),
						&(pvt->_blobs[i]),
						&(pvt->_nulls[i]));

					// A connection module might return the
					// actual field length, even if its
					// larger than the buffer that the data
					// was copied into.  Override
					// fieldlength, if necessary, just to
					// be safe.
					if (pvt->_maxfieldlength &&
						pvt->_fieldlengths[i]>
							pvt->_maxfieldlength) {
						pvt->_fieldlengths[i]=
							pvt->_maxfieldlength;
					}
				}

				// send the row to the translators
				if (!pvt->_sqlrrsrbt->setRow(
							cursor->conn,
							cursor,
							colcount,
							pvt->_fieldnames,
							pvt->_fields,
							pvt->_fieldlengths,
							pvt->_blobs,
							pvt->_nulls)) {
					*error=true;
					setError(cursor,
						pvt->_sqlrrsrbt->getError(),
					SQLR_ERROR_RESULTSETROWBLOCKTRANSLATION,
						true);
					return false;
				}
			}

			// run the translators
			if (!pvt->_sqlrrsrbt->run(cursor->conn,cursor,
						colcount,pvt->_fieldnames)) {
				*error=true;
				setError(cursor,pvt->_sqlrrsrbt->getError(),
					SQLR_ERROR_RESULTSETROWBLOCKTRANSLATION,
					true);
				return false;
			}
		}

		// get the row from the translators
		if (!pvt->_sqlrrsrbt->getRow(cursor->conn,cursor,
						colcount,
						&(pvt->_fields),
						&(pvt->_fieldlengths),
						&(pvt->_blobs),
						&(pvt->_nulls))) {
			if (pvt->_sqlrrsrbt->getError()) {
				*error=true;
				setError(cursor,pvt->_sqlrrsrbt->getError(),
					SQLR_ERROR_RESULTSETROWBLOCKTRANSLATION,
					true);
			}
			return false;
		}

	} else {

		// if we don't have any row block translations, then
		// this is a little more straightforward...

		// fetch the row, bail if fetch failed
		if (!cursor->fetchRow(error)) {
			return false;
		}

		// handle errors
		if (*error) {
			return false;
		}

		// use the provided field pointer arrays to get the
		// pointers to the column names and actual field data
		for (uint32_t i=0; i<colcount; i++) {

			pvt->_fieldnames[i]=getColumnName(cursor,i);
			pvt->_fields[i]=NULL;
			pvt->_fieldlengths[i]=0;
			pvt->_blobs[i]=false;
			pvt->_nulls[i]=false;
			cursor->getField(i,&(pvt->_fields[i]),
						&(pvt->_fieldlengths[i]),
						&(pvt->_blobs[i]),
						&(pvt->_nulls[i]));

			// A connection module might return the actual field
			// length, even if its larger than the buffer that the
			// data was copied into.  Override fieldlength, if
			// necessary, just to be safe.
			if (pvt->_maxfieldlength &&
				pvt->_fieldlengths[i]>pvt->_maxfieldlength) {
				pvt->_fieldlengths[i]=pvt->_maxfieldlength;
			}
		}
	}

	// reformat the row
	if (!reformatRow(cursor,colcount,pvt->_fieldnames,
				&(pvt->_fields),&(pvt->_fieldlengths))) {
		*error=true;
		return false;
	}

	// bump total rows fetched
	cursor->incrementTotalRowsFetched();

	return true;
}

void sqlrservercontroller::nextRow(sqlrservercursor *cursor) {
	cursor->nextRow();
}

bool sqlrservercontroller::getField(sqlrservercursor *cursor,
						uint32_t col,
						const char **field,
						uint64_t *fieldlength,
						bool *blob,
						bool *null) {

	// return the requested field (which these pointers
	// were set to during the previous call to fetchRow)
	uint32_t	actualcol=mapColumn(col);
	*field=pvt->_fields[actualcol];
	*fieldlength=pvt->_fieldlengths[actualcol];
	*blob=pvt->_blobs[actualcol];
	*null=pvt->_nulls[actualcol];

	// reformat the field
	return reformatField(cursor,pvt->_fieldnames[col],
					col,field,fieldlength);
}

bool sqlrservercontroller::getLobFieldLength(sqlrservercursor *cursor,
							uint32_t col,
							uint64_t *length) {
	return cursor->getLobFieldLength(mapColumn(col),length);
}

bool sqlrservercontroller::getLobFieldSegment(sqlrservercursor *cursor,
							uint32_t col,
							char *buffer,
							uint64_t buffersize,
							uint64_t offset,
							uint64_t charstoread,
							uint64_t *charsread) {
	return cursor->getLobFieldSegment(mapColumn(col),buffer,buffersize,
						offset,charstoread,charsread);
}

void sqlrservercontroller::closeLobField(sqlrservercursor *cursor,
							uint32_t col) {
	cursor->closeLobField(mapColumn(col));
}

void sqlrservercontroller::closeResultSet(sqlrservercursor *cursor) {
	cursor->closeResultSet();
}

uint16_t sqlrservercontroller::getId(sqlrservercursor *cursor) {
	return cursor->getId();
}

memorypool *sqlrservercontroller::getBindPool(sqlrservercursor *cursor) {
	return cursor->getBindPool();
}

memorypool *sqlrservercontroller::getBindMappingsPool(
						sqlrservercursor *cursor) {
	return cursor->getBindMappingsPool();
}

namevaluepairs *sqlrservercontroller::getBindMappings(
						sqlrservercursor *cursor) {
	return cursor->getBindMappings();
}

void sqlrservercontroller::setFakeInputBindsForThisQuery(
					sqlrservercursor *cursor, bool fake) {
	cursor->setFakeInputBindsForThisQuery(fake);
}

bool sqlrservercontroller::getFakeInputBindsForThisQuery(
					sqlrservercursor *cursor) {
	return cursor->getFakeInputBindsForThisQuery();
}

void sqlrservercontroller::setInputBindCount(sqlrservercursor *cursor,
						uint16_t inbindcount) {
	cursor->setInputBindCount(inbindcount);
}

uint16_t sqlrservercontroller::getInputBindCount(sqlrservercursor *cursor) {
	return cursor->getInputBindCount();
}

sqlrserverbindvar *sqlrservercontroller::getInputBinds(
						sqlrservercursor *cursor) {
	return cursor->getInputBinds();
}

void sqlrservercontroller::setOutputBindCount(sqlrservercursor *cursor,
						uint16_t outbindcount) {
	cursor->setOutputBindCount(outbindcount);
}

uint16_t sqlrservercontroller::getOutputBindCount(sqlrservercursor *cursor) {
	return cursor->getOutputBindCount();
}

sqlrserverbindvar *sqlrservercontroller::getOutputBinds(
						sqlrservercursor *cursor) {
	return cursor->getOutputBinds();
}

bool sqlrservercontroller::getLobOutputBindLength(sqlrservercursor *cursor,
							uint16_t index,
							uint64_t *length) {
	return cursor->getLobOutputBindLength(index,length);
}

bool sqlrservercontroller::getLobOutputBindSegment(sqlrservercursor *cursor,
							uint16_t index,
							char *buffer,
							uint64_t buffersize,
							uint64_t offset,
							uint64_t charstoread,
							uint64_t *charsread) {
	return cursor->getLobOutputBindSegment(index,buffer,buffersize,
						offset,charstoread,charsread);
}

void sqlrservercontroller::closeLobOutputBind(sqlrservercursor *cursor,
							uint16_t index) {
	cursor->closeLobOutputBind(index);
}

void sqlrservercontroller::setInputOutputBindCount(sqlrservercursor *cursor,
						uint16_t outbindcount) {
	cursor->setInputOutputBindCount(outbindcount);
}

uint16_t sqlrservercontroller::getInputOutputBindCount(
						sqlrservercursor *cursor) {
	return cursor->getInputOutputBindCount();
}

sqlrserverbindvar *sqlrservercontroller::getInputOutputBinds(
						sqlrservercursor *cursor) {
	return cursor->getInputOutputBinds();
}

bool sqlrservercontroller::getLobInputOutputBindLength(
						sqlrservercursor *cursor,
						uint16_t index,
						uint64_t *length) {
	return cursor->getLobInputOutputBindLength(index,length);
}

bool sqlrservercontroller::getLobInputOutputBindSegment(
						sqlrservercursor *cursor,
						uint16_t index,
						char *buffer,
						uint64_t buffersize,
						uint64_t offset,
						uint64_t charstoread,
						uint64_t *charsread) {
	return cursor->getLobInputOutputBindSegment(index,buffer,buffersize,
						offset,charstoread,charsread);
}

void sqlrservercontroller::closeLobInputOutputBind(sqlrservercursor *cursor,
							uint16_t index) {
	cursor->closeLobInputOutputBind(index);
}

bool sqlrservercontroller::open(sqlrservercursor *cursor) {
	raiseCursorOpenEvent(cursor);
	return cursor->open();
}

bool sqlrservercontroller::close(sqlrservercursor *cursor) {
	raiseCursorCloseEvent(cursor);
	return cursor->close();
}

void sqlrservercontroller::suspendResultSet(sqlrservercursor *cursor) {
	cursor->setState(SQLRCURSORSTATE_SUSPENDED);
	if (cursor->getCustomQueryCursor()) {
		cursor->getCustomQueryCursor()->
			setState(SQLRCURSORSTATE_SUSPENDED);
	}
}

void sqlrservercontroller::abort(sqlrservercursor *cursor) {
	cursor->abort();
}

char *sqlrservercontroller::getQueryBuffer(sqlrservercursor *cursor) {
	return cursor->getQueryBuffer();
}

uint32_t  sqlrservercontroller::getQueryLength(sqlrservercursor *cursor) {
	return cursor->getQueryLength();
}

void sqlrservercontroller::setQueryLength(sqlrservercursor *cursor,
						uint32_t querylength) {
	cursor->setQueryLength(querylength);
}

sqlrquerystatus_t sqlrservercontroller::getQueryStatus(
						sqlrservercursor *cursor) {
	return cursor->getQueryStatus();
}

xmldom *sqlrservercontroller::getQueryTree(sqlrservercursor *cursor) {
	return cursor->getQueryTree();
}

const char *sqlrservercontroller::getTranslatedQuery(
					sqlrservercursor *cursor) {
	return cursor->getTranslatedQueryBuffer()->getString();
}

void sqlrservercontroller::setCommandStart(sqlrservercursor *cursor,
						uint64_t sec, uint64_t usec) {
	cursor->setCommandStart(sec,usec);
}

uint64_t sqlrservercontroller::getCommandStartSec(sqlrservercursor *cursor) {
	return cursor->getCommandStartSec();
}

uint64_t sqlrservercontroller::getCommandStartUSec(sqlrservercursor *cursor) {
	return cursor->getCommandStartUSec();
}

void sqlrservercontroller::setCommandEnd(sqlrservercursor *cursor,
						uint64_t sec, uint64_t usec) {
	cursor->setCommandEnd(sec,usec);
}

uint64_t sqlrservercontroller::getCommandEndSec(sqlrservercursor *cursor) {
	return cursor->getCommandEndSec();
}

uint64_t sqlrservercontroller::getCommandEndUSec(sqlrservercursor *cursor) {
	return cursor->getCommandEndUSec();
}

void sqlrservercontroller::setQueryStart(sqlrservercursor *cursor,
						uint64_t sec, uint64_t usec) {
	cursor->setQueryStart(sec,usec);
}

uint64_t sqlrservercontroller::getQueryStartSec(sqlrservercursor *cursor) {
	return cursor->getQueryStartSec();
}

uint64_t sqlrservercontroller::getQueryStartUSec(sqlrservercursor *cursor) {
	return cursor->getQueryStartUSec();
}

void sqlrservercontroller::setQueryEnd(sqlrservercursor *cursor,
						uint64_t sec, uint64_t usec) {
	cursor->setQueryEnd(sec,usec);
}

uint64_t sqlrservercontroller::getQueryEndSec(sqlrservercursor *cursor) {
	return cursor->getQueryEndSec();
}

uint64_t sqlrservercontroller::getQueryEndUSec(sqlrservercursor *cursor) {
	return cursor->getQueryEndUSec();
}

void sqlrservercontroller::setState(sqlrservercursor *cursor,
						sqlrcursorstate_t state) {
	cursor->setState(state);
}

sqlrcursorstate_t sqlrservercontroller::getState(sqlrservercursor *cursor) {
	return cursor->getState();
}

uint64_t sqlrservercontroller::getTotalRowsFetched(sqlrservercursor *cursor) {
	return cursor->getTotalRowsFetched();
}

void sqlrservercontroller::clearError(sqlrservercursor *cursor) {
	setError(cursor,NULL,0,true);
}

void sqlrservercontroller::setError(sqlrservercursor *cursor,
						const char *err,
						int64_t errn,
						bool liveconn) {

	char		*errorbuffer=cursor->getErrorBuffer();
	uint32_t	errorlength=charstring::length(err);
	if (errorlength>pvt->_maxerrorlength) {
		errorlength=pvt->_maxerrorlength;
	}
	charstring::safeCopy(errorbuffer,pvt->_maxerrorlength,err,errorlength);
	if (errorlength<pvt->_maxerrorlength) {
		errorbuffer[errorlength]='\0';
	}
	cursor->setErrorLength(errorlength);
	cursor->setErrorNumber(errn);
	cursor->setLiveConnection(liveconn);
}

char *sqlrservercontroller::getErrorBuffer(sqlrservercursor *cursor) {
	return cursor->getErrorBuffer();
}

uint32_t sqlrservercontroller::getErrorLength(sqlrservercursor *cursor) {
	return cursor->getErrorLength();
}

void sqlrservercontroller::setErrorLength(sqlrservercursor *cursor,
						uint32_t errorlength) {
	cursor->setErrorLength(errorlength);
}

uint32_t sqlrservercontroller::getErrorNumber(sqlrservercursor *cursor) {
	return cursor->getErrorNumber();
}

void sqlrservercontroller::setErrorNumber(sqlrservercursor *cursor,
						uint32_t errnum) {
	cursor->setErrorNumber(errnum);
}

bool sqlrservercontroller::getLiveConnection(sqlrservercursor *cursor) {
	return cursor->getLiveConnection();
}

void sqlrservercontroller::setLiveConnection(sqlrservercursor *cursor,
						bool liveconnection) {
	cursor->setLiveConnection(liveconnection);
}

memorypool *sqlrservercontroller::getPerTransactionMemoryPool() {
	return &pvt->_txpool;
}

memorypool *sqlrservercontroller::getPerSessionMemoryPool() {
	return &pvt->_sessionpool;
}

sqlrparser *sqlrservercontroller::getParser() {
	return pvt->_sqlrp;
}

gsscontext *sqlrservercontroller::getGSSContext() {
	return (pvt->_currentprotocol)?
			pvt->_currentprotocol->getGSSContext():NULL;
}

tlscontext *sqlrservercontroller::getTLSContext() {
	return (pvt->_currentprotocol)?
			pvt->_currentprotocol->getTLSContext():NULL;
}

sqlrconfig *sqlrservercontroller::getConfig() {
	return pvt->_cfg;
}

sqlrpaths *sqlrservercontroller::getPaths() {
	return pvt->_pth;
}

sqlrshm *sqlrservercontroller::getShm() {
	return pvt->_shm;
}

sqlrmoduledata *sqlrservercontroller::getModuleData(const char *id) {
	return pvt->_sqlrmd->getModuleData(id);
}

bool sqlrservercontroller::send(unsigned char *data, size_t size) {
	return pvt->_conn->send(data,size);
}

bool sqlrservercontroller::recv(unsigned char **data, size_t *size) {
	return pvt->_conn->recv(data,size);
}
