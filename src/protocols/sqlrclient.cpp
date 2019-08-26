// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <config.h>

#include <sqlrelay/sqlrserver.h>

#include <rudiments/stringbuffer.h>
#include <rudiments/memorypool.h>
#include <rudiments/datetime.h>
#include <rudiments/userentry.h>
#include <rudiments/process.h>
#include <rudiments/file.h>
//#define DEBUG_MESSAGES 1
#include <rudiments/debugprint.h>

#include <datatypes.h>
#include <defaults.h>
#include <defines.h>

enum sqlrclientquerytype_t {
	SQLRCLIENTQUERYTYPE_QUERY=0,
	SQLRCLIENTQUERYTYPE_DATABASE_LIST,
	SQLRCLIENTQUERYTYPE_SCHEMA_LIST,
	SQLRCLIENTQUERYTYPE_TABLE_LIST,
	SQLRCLIENTQUERYTYPE_TABLE_LIST_2,
	SQLRCLIENTQUERYTYPE_TABLE_TYPE_LIST,
	SQLRCLIENTQUERYTYPE_COLUMN_LIST,
	SQLRCLIENTQUERYTYPE_PRIMARY_KEY_LIST,
	SQLRCLIENTQUERYTYPE_KEY_AND_INDEX_LIST,
	SQLRCLIENTQUERYTYPE_PROCEDURE_BIND_AND_COLUMN_LIST,
	SQLRCLIENTQUERYTYPE_TYPE_INFO_LIST,
	SQLRCLIENTQUERYTYPE_PROCEDURE_LIST
};

class SQLRSERVER_DLLSPEC sqlrprotocol_sqlrclient : public sqlrprotocol {
	public:
			sqlrprotocol_sqlrclient(sqlrservercontroller *cont,
							sqlrprotocols *ps,
							domnode *parameters);
		virtual	~sqlrprotocol_sqlrclient();

		clientsessionexitstatus_t	clientSession(
							filedescriptor *cs);

		gsscontext	*getGSSContext();
		tlscontext	*getTLSContext();

	private:
		bool	acceptSecurityContext();
		bool	getCommand(uint16_t *command);
		sqlrservercursor	*getCursor(uint16_t command);
		void	noAvailableCursors(uint16_t command);
		bool	authCommand();
		bool	getUserFromClient();
		bool	getPasswordFromClient();
		void	suspendSessionCommand();
		void	pingCommand();
		void	identifyCommand();
		void	autoCommitCommand();
		void	beginCommand();
		void	commitCommand();
		void	rollbackCommand();
		void	dbVersionCommand();
		void	bindFormatCommand();
		void	serverVersionCommand();
		void	selectDatabaseCommand();
		void	getCurrentDatabaseCommand();
		void	getCurrentSchemaCommand();
		void	getLastInsertIdCommand();
		void	dbHostNameCommand();
		void	dbIpAddressCommand();
		bool	newQueryCommand(sqlrservercursor *cursor);
		bool	reExecuteQueryCommand(sqlrservercursor *cursor);
		bool	fetchFromBindCursorCommand(sqlrservercursor *cursor);
		bool	processQueryOrBindCursor(sqlrservercursor *cursor,
					sqlrclientquerytype_t querytype,
					sqlrserverlistformat_t listformat,
					bool reexecute,
					bool bindcursor);
		bool	getClientInfo(sqlrservercursor *cursor);
		bool	getQuery(sqlrservercursor *cursor);
		bool	getInputBinds(sqlrservercursor *cursor);
		bool	getOutputBinds(sqlrservercursor *cursor);
		bool	getInputOutputBinds(sqlrservercursor *cursor);
		bool	getBindVarCount(sqlrservercursor *cursor,
						uint16_t *count);
		bool	getBindVarName(sqlrservercursor *cursor,
						sqlrserverbindvar *bv,
						memorypool *bindpool);
		bool	getBindVarType(sqlrserverbindvar *bv);
		bool	getBindSize(sqlrservercursor *cursor,
						sqlrserverbindvar *bv,
						uint32_t *maxsize);
		void	getNullBind(sqlrserverbindvar *bv,
						memorypool *bindpool);
		bool	getStringBind(sqlrservercursor *cursor,
						sqlrserverbindvar *bv,
						memorypool *bindpool);
		bool	getIntegerBind(sqlrserverbindvar *bv);
		bool	getDoubleBind(sqlrserverbindvar *bv);
		bool	getDateBind(sqlrserverbindvar *bv,
						memorypool *bindpool);
		bool	getLobBind(sqlrservercursor *cursor,
						sqlrserverbindvar *bv,
						memorypool *bindpool);
		bool	getSendColumnInfo();
		bool	getSkipAndFetch(bool initial, sqlrservercursor *cursor);
		void	returnResultSetHeader(sqlrservercursor *cursor);
		void	returnColumnInfo(sqlrservercursor *cursor,
							uint16_t format);
		uint16_t	protocolAppropriateColumnType(uint16_t coltype);
		void	sendRowCounts(bool knowsactual, uint64_t actual,
					bool knowsaffected, uint64_t affected);
		void	returnOutputBindValues(sqlrservercursor *cursor);
		void	returnOutputBindBlob(sqlrservercursor *cursor,
							uint16_t index);
		void	returnOutputBindClob(sqlrservercursor *cursor,
							uint16_t index);
		void	sendLobOutputBind(sqlrservercursor *cursor,
							uint16_t index);
		void	returnInputOutputBindValues(sqlrservercursor *cursor);
		void	sendColumnDefinition(const char *name,
						uint16_t namelen,
						uint16_t type, 
						uint32_t size,
						uint32_t precision,
						uint32_t scale,
						uint16_t nullable,
						uint16_t primarykey,
						uint16_t unique,
						uint16_t partofkey,
						uint16_t unsignednumber,
						uint16_t zerofill,
						uint16_t binary,
						uint16_t autoincrement,
						const char *table,
						uint16_t tablelength);
		void	sendColumnDefinitionString(const char *name,
						uint16_t namelen,
						const char *type, 
						uint16_t typelen,
						uint32_t size,
						uint32_t precision,
						uint32_t scale,
						uint16_t nullable,
						uint16_t primarykey,
						uint16_t unique,
						uint16_t partofkey,
						uint16_t unsignednumber,
						uint16_t zerofill,
						uint16_t binary,
						uint16_t autoincrement,
						const char *table,
						uint16_t tablelength);
		bool	returnResultSetData(sqlrservercursor *cursor,
						bool getskipandfetch,
						bool overridelazyfetch);
		void	returnFetchError(sqlrservercursor *cursor);
		void	returnRow(sqlrservercursor *cursor);
		void	sendField(const char *data, uint32_t size);
		void	sendNullField();
		void	sendLobField(sqlrservercursor *cursor, uint32_t col);
		void	startSendingLong(uint64_t longlength);
		void	sendLongSegment(const char *data, uint32_t size);
		void	endSendingLong();
		void	returnError(bool forcedisconnect);
		void	returnError(sqlrservercursor *cursor,
						bool forcedisconnect);
		bool	fetchResultSetCommand(sqlrservercursor *cursor);
		void	abortResultSetCommand(sqlrservercursor *cursor);
		void	suspendResultSetCommand(sqlrservercursor *cursor);
		bool	resumeResultSetCommand(sqlrservercursor *cursor);
		bool	getDatabaseListCommand(sqlrservercursor *cursor);
		bool	getSchemaListCommand(sqlrservercursor *cursor);
		bool	getTableListCommand(sqlrservercursor *cursor);
		bool	getTableList2Command(sqlrservercursor *cursor);
		bool	getTableTypeListCommand(sqlrservercursor *cursor);
		bool	getColumnListCommand(sqlrservercursor *cursor);
		bool	getPrimaryKeyListCommand(sqlrservercursor *cursor);
		bool	getKeyAndIndexListCommand(sqlrservercursor *cursor);
		bool	getProcedureBindAndColumnListCommand(
						sqlrservercursor *cursor);
		bool	getTypeInfoListCommand(sqlrservercursor *cursor);
		bool	getProcedureListCommand(sqlrservercursor *cursor);
		bool	getListCommand(sqlrservercursor *cursor,
					sqlrclientquerytype_t querytype,
					bool getobject);
		bool	getListByApiCall(sqlrservercursor *cursor,
					sqlrclientquerytype_t querytype,
					const char *object,
					const char *wild,
					sqlrserverlistformat_t listformat,
					uint16_t objecttypes);
		bool	getListByQuery(sqlrservercursor *cursor,
					sqlrclientquerytype_t querytype,
					const char *object,
					const char *wild,
					sqlrserverlistformat_t listformat,
					uint16_t objecttypes);
		bool	buildListQuery(sqlrservercursor *cursor,
						const char *query,
						const char *wild,
						const char *object);
		void	escapeParameter(stringbuffer *buffer,
						const char *parameter);
		bool	getQueryTreeCommand(sqlrservercursor *cursor);
		bool	getTranslatedQueryCommand(sqlrservercursor *cursor);
		bool	nextResultSetCommand(sqlrservercursor *cursor);

		stringbuffer	debugstr;

		filedescriptor	*clientsock;

		securitycontext	*ctx;
		bool		usekrb;
		bool		usetls;

		gsscredentials	gcred;
		gssmechanism	gmech;
		gsscontext	gctx;
		tlscontext	tctx;

		int32_t		idleclienttimeout;

		uint64_t	maxclientinfolength;
		uint32_t	maxquerysize;
		uint16_t	maxbindcount;
		uint16_t	maxbindnamelength;
		uint32_t	maxstringbindvaluelength;
		uint32_t	maxlobbindvaluelength;
		uint32_t	maxerrorlength;
		bool		waitfordowndb;

		char		userbuffer[USERSIZE];
		char		passwordbuffer[USERSIZE];

		char		*clientinfo;
		uint64_t	clientinfolen;

		uint64_t	skip;
		uint64_t	fetch;
		bool		lazyfetch;

		char		lobbuffer[32768];

		uint16_t	protocolversion;
		uint16_t	endresultset;
};

sqlrprotocol_sqlrclient::sqlrprotocol_sqlrclient(
					sqlrservercontroller *cont,
					sqlrprotocols *ps,
					domnode *parameters) :
					sqlrprotocol(cont,ps,parameters) {
	debugFunction();

	idleclienttimeout=cont->getConfig()->getIdleClientTimeout();
	maxclientinfolength=cont->getConfig()->getMaxClientInfoLength();
	maxquerysize=cont->getConfig()->getMaxQuerySize();
	maxbindcount=cont->getConfig()->getMaxBindCount();
	maxbindnamelength=cont->getConfig()->getMaxBindNameLength();
	maxstringbindvaluelength=
			cont->getConfig()->getMaxStringBindValueLength();
	maxlobbindvaluelength=cont->getConfig()->getMaxLobBindValueLength();
	lazyfetch=false;
	maxerrorlength=cont->getConfig()->getMaxErrorLength();
	waitfordowndb=cont->getConfig()->getWaitForDownDatabase();
	clientinfo=new char[maxclientinfolength+1];
	clientsock=NULL;
	ctx=NULL;
	usekrb=charstring::isYes(parameters->getAttributeValue("krb"));
	usetls=charstring::isYes(parameters->getAttributeValue("tls"));

	if (usekrb) {
		if (gss::supported()) {

			// set the keytab file to use
			const char	*keytab=
				parameters->getAttributeValue("krbkeytab");
			if (!charstring::isNullOrEmpty(keytab)) {
				gcred.setKeytab(keytab);
			}

			// set the service to use
			const char	*service=
				parameters->getAttributeValue("krbservice");
			if (charstring::isNullOrEmpty(service)) {
				service=DEFAULT_KRBSERVICE;
			}

			// acquire service credentials
			if (!gcred.acquireForService(service)) {
				const char	*status=
					gcred.getMechanismMinorStatus();
				stderror.printf("kerberos acquire-"
						"service %s failed:\n%s",
						service,status);
				if (charstring::contains(status,
							"Permission denied")) {
					char	*user=userentry::getName(
							process::getUserId());
					stderror.printf("(keytab file likely "
							"not readable by user "
							"%s)\n",user);
					delete[] user;
				}
			}

			// initialize the gss context
			gmech.initialize(
				parameters->getAttributeValue("krbmech"));
			gctx.setDesiredMechanism(&gmech);
			gctx.setDesiredFlags(
				parameters->getAttributeValue("krbflags"));
			gctx.setCredentials(&gcred);

			// use the gss context
			ctx=&gctx;

		} else {
			stderror.printf("Warning: kerberos support requested "
					"but platform doesn't support "
					"kerberos\n");
		}
	} else if (usetls) {
		if (tls::supported()) {

			// get the protocol version to use
			tctx.setProtocolVersion(
				parameters->getAttributeValue("tlsversion"));

			// get the certificate chain file to use
			const char	*tlscert=
				parameters->getAttributeValue("tlscert");
			if (file::readable(tlscert)) {
				tctx.setCertificateChainFile(tlscert);
			} else if (!charstring::isNullOrEmpty(tlscert)) {
				stderror.printf("Warning: TLS certificate "
						"file %s is not readable.\n",
						tlscert);
			}

			// get the private key file to use
			const char	*tlskey=
				parameters->getAttributeValue("tlskey");
			if (file::readable(tlskey)) {
				tctx.setPrivateKeyFile(tlskey);
			} else if (!charstring::isNullOrEmpty(tlskey)) {
				stderror.printf("Warning: TLS private key "
						"file %s is not readable.\n",
						tlskey);
			}

			// get the private key password to use
			tctx.setPrivateKeyPassword(
				parameters->getAttributeValue("tlspassword"));

			// get whether to validate
			tctx.setValidatePeer(
				charstring::isYes(
				parameters->getAttributeValue("tlsvalidate")));

			// get the certificate authority file to use
			// FIXME: not-found warning
			tctx.setCertificateAuthority(
				parameters->getAttributeValue("tlsca"));

			// get the cipher list to use
			tctx.setCiphers(
				parameters->getAttributeValue("tlsciphers"));

			// get the validation depth
			tctx.setValidationDepth(
				charstring::toUnsignedInteger(
				parameters->getAttributeValue("tlsdepth")));

			// use the tls context
			ctx=&tctx;

		} else {
			stderror.printf("Warning: TLS support requested "
					"but platform doesn't support "
					"TLS\n");
		}
	}

	protocolversion=0;
	endresultset=END_RESULT_SET;
}

sqlrprotocol_sqlrclient::~sqlrprotocol_sqlrclient() {
	debugFunction();
	delete[] clientinfo;
}

clientsessionexitstatus_t sqlrprotocol_sqlrclient::clientSession(
							filedescriptor *cs) {
	debugFunction();

	clientsock=cs;

	// set up the socket
	clientsock->translateByteOrder();
	clientsock->dontUseNaglesAlgorithm();
	clientsock->setSocketReadBufferSize(65536);
	clientsock->setSocketWriteBufferSize(65536);
	clientsock->setReadBufferSize(65536);
	clientsock->setWriteBufferSize(65536);
	//clientsock->useAsyncWrite();

	clientsessionexitstatus_t	status=CLIENTSESSIONEXITSTATUS_ERROR;

	// accept security context, if necessary
	if (!acceptSecurityContext()) {
		return status;
	}

	// During each session, the client will send a series of commands.
	// The session ends when the client ends it or when certain commands
	// fail.
	bool			loop=true;
	bool			endsession=true;
	uint16_t		command;
	do {

		// handle disabled instance
		// FIXME: push up?
		if (cont->getInstanceDisabled()) {
			endsession=true;
			break;
		}

		// get a command from the client
		if (!getCommand(&command)) {
			break;
		}

		// get the command start time
		datetime	dt;
		dt.getSystemDateAndTime();

		// handle client protocol version as a command, for now
		if (command==PROTOCOLVERSION) {
			if (clientsock->read(&protocolversion,
						idleclienttimeout,0)==
						sizeof(uint16_t)) {
				// END_RESULT_SET was 3 in protocol version 1,
				// but changed in version 2
				endresultset=(protocolversion==1)?
							3:END_RESULT_SET;
				continue;
			}
			endsession=false;
			break;
		} else

		// these commands are all handled at the connection level
		if (command==AUTH) {
			cont->incrementAuthCount();
			if (authCommand()) {
				cont->beginSession();
				continue;
			}
			endsession=false;
			break;
		} else if (command==SUSPEND_SESSION) {
			cont->incrementSuspendSessionCount();
			suspendSessionCommand();
			status=CLIENTSESSIONEXITSTATUS_SUSPENDED_SESSION;
			endsession=false;
			break;
		} else if (command==END_SESSION) {
			cont->incrementEndSessionCount();
			status=CLIENTSESSIONEXITSTATUS_ENDED_SESSION;
			break;
		} else if (command==PING) {
			cont->incrementPingCount();
			pingCommand();
			continue;
		} else if (command==IDENTIFY) {
			cont->incrementIdentifyCount();
			identifyCommand();
			continue;
		} else if (command==AUTOCOMMIT) {
			cont->incrementAutocommitCount();
			autoCommitCommand();
			continue;
		} else if (command==BEGIN) {
			cont->incrementBeginCount();
			beginCommand();
			continue;
		} else if (command==COMMIT) {
			cont->incrementCommitCount();
			commitCommand();
			continue;
		} else if (command==ROLLBACK) {
			cont->incrementRollbackCount();
			rollbackCommand();
			continue;
		} else if (command==DBVERSION) {
			cont->incrementDbVersionCount();
			dbVersionCommand();
			continue;
		} else if (command==BINDFORMAT) {
			cont->incrementBindFormatCount();
			bindFormatCommand();
			continue;
		} else if (command==SERVERVERSION) {
			cont->incrementServerVersionCount();
			serverVersionCommand();
			continue;
		} else if (command==SELECT_DATABASE) {
			cont->incrementSelectDatabaseCount();
			selectDatabaseCommand();
			continue;
		} else if (command==GET_CURRENT_DATABASE) {
			cont->incrementGetCurrentDatabaseCount();
			getCurrentDatabaseCommand();
			continue;
		} else if (command==GET_CURRENT_SCHEMA) {
			//cont->incrementGetCurrentSchemaCount();
			getCurrentSchemaCommand();
			continue;
		} else if (command==GET_LAST_INSERT_ID) {
			cont->incrementGetLastInsertIdCount();
			getLastInsertIdCommand();
			continue;
		} else if (command==DBHOSTNAME) {
			cont->incrementDbHostNameCount();
			dbHostNameCommand();
			continue;
		} else if (command==DBIPADDRESS) {
			cont->incrementDbIpAddressCount();
			dbIpAddressCommand();
			continue;
		}

		// For the rest of the commands,
		// the client will request a cursor
		sqlrservercursor	*cursor=getCursor(command);
		if (!cursor) {
			// Don't worry about reporting that a cursor wasn't
			// available for abort-result-set commands. Those
			// commands don't look for a response from the server
			// and it doesn't matter if a non-existent result set
			// was aborted.
			if (command!=ABORT_RESULT_SET) {
				noAvailableCursors(command);
			}
			continue;
		}

		// set the command start-time
		cont->setCommandStart(cursor,
				dt.getSeconds(),dt.getMicroseconds());

		// these commands are all handled at the cursor level
		if (command==NEW_QUERY) {
			cont->incrementNewQueryCount();
			loop=newQueryCommand(cursor);
		} else if (command==REEXECUTE_QUERY) {
			cont->incrementReexecuteQueryCount();
			loop=reExecuteQueryCommand(cursor);
		} else if (command==FETCH_FROM_BIND_CURSOR) {
			cont->incrementFetchFromBindCursorCount();
			loop=fetchFromBindCursorCommand(cursor);
		} else if (command==FETCH_RESULT_SET) {
			cont->incrementFetchResultSetCount();
			loop=fetchResultSetCommand(cursor);
		} else if (command==ABORT_RESULT_SET) {
			cont->incrementAbortResultSetCount();
			abortResultSetCommand(cursor);
		} else if (command==SUSPEND_RESULT_SET) {
			cont->incrementSuspendResultSetCount();
			suspendResultSetCommand(cursor);
		} else if (command==RESUME_RESULT_SET) {
			cont->incrementResumeResultSetCount();
			loop=resumeResultSetCommand(cursor);
		} else if (command==GETDBLIST) {
			cont->incrementGetDbListCount();
			loop=getDatabaseListCommand(cursor);
		} else if (command==GETSCHEMALIST) {
			//cont->incrementGetSchemaListCount();
			loop=getSchemaListCommand(cursor);
		} else if (command==GETTABLELIST) {
			cont->incrementGetTableListCount();
			loop=getTableListCommand(cursor);
		} else if (command==GETTABLELIST2) {
			cont->incrementGetTableListCount();
			loop=getTableList2Command(cursor);
		} else if (command==GETTABLETYPELIST) {
			//cont->incrementGetTableTypeListCount();
			loop=getTableTypeListCommand(cursor);
		} else if (command==GETCOLUMNLIST) {
			cont->incrementGetColumnListCount();
			loop=getColumnListCommand(cursor);
		} else if (command==GETPRIMARYKEYLIST) {
			//cont->incrementGetPrimaryKeyListCount();
			loop=getPrimaryKeyListCommand(cursor);
		} else if (command==GETKEYANDINDEXLIST) {
			//cont->incrementGetKeyAndIndexListCount();
			loop=getKeyAndIndexListCommand(cursor);
		} else if (command==GETPROCEDUREBINDANDCOLUMNLIST) {
			//cont->incrementGetProcedureBindAndColumnListCount();
			loop=getProcedureBindAndColumnListCommand(cursor);
		} else if (command==GETTYPEINFOLIST) {
			//cont->incrementGetTypeInfoListCount();
			loop=getTypeInfoListCommand(cursor);
		} else if (command==GETPROCEDURELIST) {
			//cont->incrementGetProcedureListCount();
			loop=getProcedureListCommand(cursor);
		} else if (command==GET_QUERY_TREE) {
			cont->incrementGetQueryTreeCount();
			loop=getQueryTreeCommand(cursor);
		} else if (command==GET_TRANSLATED_QUERY) {
			//cont->incrementGetTranslatedQueryCount();
			loop=getTranslatedQueryCommand(cursor);
		} else if (command==NEXT_RESULT_SET) {
			loop=nextResultSetCommand(cursor);
		} else {
			loop=false;
		}

		// set the command end-time
		dt.getSystemDateAndTime();
		cont->setCommandEnd(cursor,
				dt.getSeconds(),dt.getMicroseconds());

		// free memory used by binds...
		// FIXME: can we move this inside of processQueryOrBindCursor?
		// verify that log/notification modules activated by
		// raise*Event calls don't still need the bind values
		cont->getBindPool(cursor)->clear();

	} while (loop);

	// close the client connection
	//
	// If an error occurred, the client could still be sending an entire
	// session's worth of data before it reads the error and closes the
	// socket.  We have to absorb all of that data.  We shouldn't just loop
	// forever though, that would provide a point of entry for a DOS attack.
	// We'll read the maximum number of bytes that could be sent.
	cont->closeClientConnection(
				// sending auth
				(sizeof(uint16_t)+
				// user/password
				2*(sizeof(uint32_t)+USERSIZE)+
				// sending query
				sizeof(uint16_t)+
				// need a cursor
				sizeof(uint16_t)+
				// executing new query
				sizeof(uint16_t)+
				// query size and query
				sizeof(uint32_t)+maxquerysize+
				// input bind var count
				sizeof(uint16_t)+
				// input bind vars
				maxbindcount*(2*sizeof(uint16_t)+
						maxbindnamelength)+
				// output bind var count
				sizeof(uint16_t)+
				// output bind vars
				maxbindcount*(2*sizeof(uint16_t)+
						maxbindnamelength)+
				// get column info
				sizeof(uint16_t)+
				// skip/fetch
				2*sizeof(uint32_t)
				// divide by two because we're
				// reading 2 bytes at a time
				)/2);

	// end the session if necessary
	if (endsession) {
		cont->endSession();
	}

	// return the exit status
	return status;
}

bool sqlrprotocol_sqlrclient::acceptSecurityContext() {

	if (!usekrb && !usetls) {
		return true;
	}

	cont->raiseDebugMessageEvent("accepting security context");

	if (usekrb && !gss::supported()) {
		cont->raiseInternalErrorEvent(NULL,
				"failed to accept gss security "
				"context (kerberos requested but "
				"not supported)");
		return false;
	} else if (usetls && !tls::supported()) {
		cont->raiseInternalErrorEvent(NULL,
				"failed to accept tls security "
				"context (tls requested but "
				"not supported)");
		return false;
	}

	// attach the context and file descriptor to each other
	clientsock->setSecurityContext(ctx);
	ctx->setFileDescriptor(clientsock);

	// accept the security context
	bool	retval=ctx->accept();
	if (!retval) {
		cont->raiseInternalErrorEvent(NULL,
			"failed to accept security context");
	}

	cont->raiseDebugMessageEvent("done accepting security context");
	return retval;
}

bool sqlrprotocol_sqlrclient::getCommand(uint16_t *command) {
	debugFunction();

	cont->raiseDebugMessageEvent("getting command...");

	cont->setState(GET_COMMAND);

	// get the command
	ssize_t	result=clientsock->read(command,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {

		// Return false but don't consider it an error if we get a
		// timeout or a 0 (meaning that the client closed the socket)
		// as either would be natural to do here.
		if (result!=RESULT_TIMEOUT && result!=0) {
			cont->raiseClientProtocolErrorEvent(
				NULL,"get command failed",result);
		}

		*command=NO_COMMAND;
		return false;
	}

	debugstr.clear();
	debugstr.append("command: ")->append(*command);
	cont->raiseDebugMessageEvent(debugstr.getString());

	cont->raiseDebugMessageEvent("done getting command");
	return true;
}

sqlrservercursor *sqlrprotocol_sqlrclient::getCursor(uint16_t command) {
	debugFunction();

	cont->raiseDebugMessageEvent("getting a cursor...");

	// does the client need a cursor or does it already have one
	uint16_t	neednewcursor=DONT_NEED_NEW_CURSOR;
	if (command==NEW_QUERY ||
		command==GETDBLIST ||
		command==GETSCHEMALIST ||
		command==GETTABLELIST ||
		command==GETTABLELIST2 ||
		command==GETTABLETYPELIST ||
		command==GETCOLUMNLIST ||
		command==GETPRIMARYKEYLIST ||
		command==GETKEYANDINDEXLIST ||
		command==GETPROCEDUREBINDANDCOLUMNLIST ||
		command==GETTYPEINFOLIST ||
		command==GETPROCEDURELIST ||
		command==ABORT_RESULT_SET ||
		command==GET_QUERY_TREE ||
		command==GET_TRANSLATED_QUERY) {
		ssize_t	result=clientsock->read(&neednewcursor,
						idleclienttimeout,0);
		if (result!=sizeof(uint16_t)) {
			cont->raiseClientProtocolErrorEvent(NULL,
					"get cursor failed: "
					"failed to get whether client "
					"needs  new cursor or not",result);
			return NULL;
		}
	}

	sqlrservercursor	*cursor=NULL;

	if (neednewcursor==DONT_NEED_NEW_CURSOR) {

		// which cursor is the client requesting?
		uint16_t	id;
		ssize_t		result=clientsock->read(&id,
						idleclienttimeout,0);
		if (result!=sizeof(uint16_t)) {
			cont->raiseClientProtocolErrorEvent(NULL,
					"get cursor failed: "
					"failed to get cursor id",result);
			return NULL;
		}

		// get the requested cursor
		cursor=cont->getCursor(id);

	} else {

		// find an available cursor
		cursor=cont->getCursor();
	}

	cont->raiseDebugMessageEvent("done getting a cursor");
	return cursor;
}

void sqlrprotocol_sqlrclient::noAvailableCursors(uint16_t command) {
	debugFunction();

	// If no cursor was available, the client
	// cound send an entire query and bind vars
	// before it reads the error and closes the
	// socket.  We have to absorb all of that
	// data.  We shouldn't just loop forever
	// though, that would provide a point of entry
	// for a DOS attack.  We'll read the maximum
	// number of bytes that could be sent.
	uint32_t	size=(
				// query size and query
				sizeof(uint32_t)+maxquerysize+
				// input bind var count
				sizeof(uint16_t)+
				// input bind vars
				maxbindcount*(2*sizeof(uint16_t)+
						maxbindnamelength)+
				// output bind var count
				sizeof(uint16_t)+
				// output bind vars
				maxbindcount*(2*sizeof(uint16_t)+
						maxbindnamelength)+
				// get column info
				sizeof(uint16_t)+
				// skip/fetch
				2*sizeof(uint32_t));

	clientsock->useNonBlockingMode();
	unsigned char	*dummy=new unsigned char[size];
	clientsock->read(dummy,size,idleclienttimeout,0);
	clientsock->useBlockingMode();
	delete[] dummy;

	// indicate that an error has occurred
	clientsock->write((uint16_t)ERROR_OCCURRED);

	// send the error code
	clientsock->write((uint64_t)SQLR_ERROR_NOCURSORS);

	// send the error itself
	uint16_t	len=charstring::length(SQLR_ERROR_NOCURSORS_STRING);
	clientsock->write(len);
	clientsock->write(SQLR_ERROR_NOCURSORS_STRING,len);
	clientsock->flushWriteBuffer(-1,-1);
}

bool sqlrprotocol_sqlrclient::authCommand() {
	debugFunction();

	cont->raiseDebugMessageEvent("auth");

	// get the user/password from the client
	if (!getUserFromClient() || !getPasswordFromClient()) {
		return false;
	}

	// build credentials...
	sqlrcredentials	*cred=cont->getCredentials(
					userbuffer,passwordbuffer,
					usekrb,usetls);

	// auth
	bool	success=cont->auth(cred);

	// clean up
	delete cred;

	// success
	if (success) {
		return true;
	}

	// indicate that an error has occurred
	clientsock->write((uint16_t)ERROR_OCCURRED_DISCONNECT);
	clientsock->write((uint64_t)SQLR_ERROR_AUTHENTICATIONERROR);
	clientsock->write((uint16_t)charstring::length(
				SQLR_ERROR_AUTHENTICATIONERROR_STRING));
	clientsock->write(SQLR_ERROR_AUTHENTICATIONERROR_STRING);
	clientsock->flushWriteBuffer(-1,-1);
	return false;
}

bool sqlrprotocol_sqlrclient::getUserFromClient() {
	debugFunction();
	uint32_t	size=0;
	ssize_t		result=clientsock->read(&size,idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->raiseClientProtocolErrorEvent(NULL,
			"authentication failed: "
			"failed to get user size",result);
		return false;
	}
	if (size>=sizeof(userbuffer)) {
		debugstr.clear();
		debugstr.append("authentication failed: user size too long: ");
		debugstr.append(size);
		cont->raiseClientConnectionRefusedEvent(debugstr.getString());
		return false;
	}
	result=clientsock->read(userbuffer,size,idleclienttimeout,0);
	if ((uint32_t)result!=size) {
		cont->raiseClientProtocolErrorEvent(NULL,
			"authentication failed: "
			"failed to get user",result);
		return false;
	}
	userbuffer[size]='\0';
	return true;
}

bool sqlrprotocol_sqlrclient::getPasswordFromClient() {
	debugFunction();
	uint32_t	size=0;
	ssize_t		result=clientsock->read(&size,idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->raiseClientProtocolErrorEvent(NULL,
			"authentication failed: "
			"failed to get password size",result);
		return false;
	}
	if (size>=sizeof(passwordbuffer)) {
		debugstr.clear();
		debugstr.append("authentication failed: "
				"password size too long: ");
		debugstr.append(size);
		cont->raiseClientConnectionRefusedEvent(debugstr.getString());
		return false;
	}
	result=clientsock->read(passwordbuffer,size,idleclienttimeout,0);
	if ((uint32_t)result!=size) {
		cont->raiseClientProtocolErrorEvent(NULL,
			"authentication failed: "
			"failed to get password",result);
		return false;
	}
	passwordbuffer[size]='\0';
	return true;
}

void sqlrprotocol_sqlrclient::suspendSessionCommand() {
	debugFunction();

	cont->raiseDebugMessageEvent("suspending session...");

	// suspend the session
	const char	*unixsocketname=NULL;
	uint16_t	inetportnumber=0;
	cont->suspendSession(&unixsocketname,&inetportnumber);
	uint16_t	unixsocketsize=charstring::length(unixsocketname);

	// pass the socket info to the client
	cont->raiseDebugMessageEvent("passing socket info to client...");
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	clientsock->write(unixsocketsize);
	if (unixsocketsize) {
		clientsock->write(unixsocketname,unixsocketsize);
	}
	clientsock->write(inetportnumber);
	clientsock->flushWriteBuffer(-1,-1);
	cont->raiseDebugMessageEvent("done passing socket info to client");

	cont->raiseDebugMessageEvent("done suspending session");
}

void sqlrprotocol_sqlrclient::pingCommand() {
	debugFunction();
	cont->raiseDebugMessageEvent("ping");
	bool	pingresult=cont->ping();
	if (pingresult) {
		cont->raiseDebugMessageEvent("ping succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		cont->raiseDebugMessageEvent("ping failed");
		returnError(false);
	}
	if (!pingresult) {
		cont->reLogIn();
	}
}

void sqlrprotocol_sqlrclient::identifyCommand() {
	debugFunction();

	cont->raiseDebugMessageEvent("identify");

	// get the identification
	const char	*ident=cont->identify();

	// send it to the client
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	uint16_t	idlen=charstring::length(ident);
	clientsock->write(idlen);
	clientsock->write(ident,idlen);
	clientsock->flushWriteBuffer(-1,-1);
}

void sqlrprotocol_sqlrclient::autoCommitCommand() {
	debugFunction();
	cont->raiseDebugMessageEvent("autocommit...");
	bool	on;
	ssize_t	result=clientsock->read(&on,idleclienttimeout,0);
	if (result!=sizeof(bool)) {
		cont->raiseClientProtocolErrorEvent(NULL,
				"get autocommit failed: "
				"failed to get autocommit setting",result);
		return;
	}
	bool	success=false;
	if (on) {
		cont->raiseDebugMessageEvent("autocommit on");
		success=cont->autoCommitOn();
	} else {
		cont->raiseDebugMessageEvent("autocommit off");
		success=cont->autoCommitOff();
	}
	if (success) {
		cont->raiseDebugMessageEvent("succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		cont->raiseDebugMessageEvent("failed");
		returnError(false);
	}
}

void sqlrprotocol_sqlrclient::beginCommand() {
	debugFunction();
	cont->raiseDebugMessageEvent("begin...");
	if (cont->begin()) {
		cont->raiseDebugMessageEvent("succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		cont->raiseDebugMessageEvent("failed");
		returnError(false);
	}
}

void sqlrprotocol_sqlrclient::commitCommand() {
	debugFunction();
	cont->raiseDebugMessageEvent("commit...");
	if (cont->commit()) {
		cont->raiseDebugMessageEvent("succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		cont->raiseDebugMessageEvent("failed");
		returnError(false);
	}
}

void sqlrprotocol_sqlrclient::rollbackCommand() {
	debugFunction();
	cont->raiseDebugMessageEvent("rollback...");
	if (cont->rollback()) {
		cont->raiseDebugMessageEvent("succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		cont->raiseDebugMessageEvent("failed");
		returnError(false);
	}
}

void sqlrprotocol_sqlrclient::dbVersionCommand() {
	debugFunction();

	cont->raiseDebugMessageEvent("db version");

	// get the db version
	const char	*dbversion=cont->dbVersion();

	// send it to the client
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	uint16_t	dbvlen=charstring::length(dbversion);
	clientsock->write(dbvlen);
	clientsock->write(dbversion,dbvlen);
	clientsock->flushWriteBuffer(-1,-1);
}

void sqlrprotocol_sqlrclient::bindFormatCommand() {
	debugFunction();

	cont->raiseDebugMessageEvent("bind format");

	// get the bind format
	const char	*bf=cont->bindFormat();

	// send it to the client
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	uint16_t	bflen=charstring::length(bf);
	clientsock->write(bflen);
	clientsock->write(bf,bflen);
	clientsock->flushWriteBuffer(-1,-1);
}

void sqlrprotocol_sqlrclient::serverVersionCommand() {
	debugFunction();

	cont->raiseDebugMessageEvent("server version");

	// get the server version
	const char	*svrversion=SQLR_VERSION;

	// send it to the client
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	uint16_t	svrvlen=charstring::length(svrversion);
	clientsock->write(svrvlen);
	clientsock->write(svrversion,svrvlen);
	clientsock->flushWriteBuffer(-1,-1);
}

void sqlrprotocol_sqlrclient::selectDatabaseCommand() {
	debugFunction();

	cont->raiseDebugMessageEvent("select database");

	// get length of db parameter
	uint32_t	dblen;
	ssize_t		result=clientsock->read(&dblen,idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		clientsock->write(false);
		cont->raiseClientProtocolErrorEvent(NULL,
				"select database failed: "
				"failed to get db length",result);
		return;
	}

	// bounds checking
	if (dblen>maxquerysize) {
		clientsock->write(false);
		debugstr.clear();
		debugstr.append("select database failed: "
				"client sent bad db length: ");
		debugstr.append(dblen);
		cont->raiseClientProtocolErrorEvent(NULL,debugstr.getString(),1);
		return;
	}

	// read the db parameter into the buffer
	char	*db=new char[dblen+1];
	if (dblen) {
		result=clientsock->read(db,dblen,idleclienttimeout,0);
		if ((uint32_t)result!=dblen) {
			clientsock->write(false);
			clientsock->flushWriteBuffer(-1,-1);
			delete[] db;
			cont->raiseClientProtocolErrorEvent(NULL,
				"select database failed: "
				"failed to get database name",result);
			return;
		}
	}
	db[dblen]='\0';
	
	// Select the db and send back the result.
	if (cont->selectDatabase(db)) {
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		returnError(false);
	}

	delete[] db;

	return;
}

void sqlrprotocol_sqlrclient::getCurrentDatabaseCommand() {
	debugFunction();

	cont->raiseDebugMessageEvent("get current database");

	// get the current database
	char	*currentdb=cont->getCurrentDatabase();

	// send it to the client
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	uint16_t	currentdbsize=charstring::length(currentdb);
	clientsock->write(currentdbsize);
	clientsock->write(currentdb,currentdbsize);
	clientsock->flushWriteBuffer(-1,-1);

	// clean up
	delete[] currentdb;
}

void sqlrprotocol_sqlrclient::getCurrentSchemaCommand() {
	debugFunction();

	cont->raiseDebugMessageEvent("get current schema");

	// get the current schema
	char	*currentschema=cont->getCurrentSchema();

	// send it to the client
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	uint16_t	currentschemasize=charstring::length(currentschema);
	clientsock->write(currentschemasize);
	clientsock->write(currentschema,currentschemasize);
	clientsock->flushWriteBuffer(-1,-1);

	// clean up
	delete[] currentschema;
}

void sqlrprotocol_sqlrclient::getLastInsertIdCommand() {
	debugFunction();
	cont->raiseDebugMessageEvent("getting last insert id...");
	uint64_t	id;
	if (cont->getLastInsertId(&id)) {
		cont->raiseDebugMessageEvent("get last insert id succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->write(id);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		cont->raiseDebugMessageEvent("get last insert id failed");
		returnError(false);
	}
}

void sqlrprotocol_sqlrclient::dbHostNameCommand() {
	debugFunction();

	cont->raiseDebugMessageEvent("getting db host name");

	const char	*hostname=cont->dbHostName();
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	uint16_t	hostnamelen=charstring::length(hostname);
	clientsock->write(hostnamelen);
	clientsock->write(hostname,hostnamelen);
	clientsock->flushWriteBuffer(-1,-1);
}

void sqlrprotocol_sqlrclient::dbIpAddressCommand() {
	debugFunction();

	cont->raiseDebugMessageEvent("getting db host name");

	const char	*ipaddress=cont->dbIpAddress();
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	uint16_t	ipaddresslen=charstring::length(ipaddress);
	clientsock->write(ipaddresslen);
	clientsock->write(ipaddress,ipaddresslen);
	clientsock->flushWriteBuffer(-1,-1);
}

bool sqlrprotocol_sqlrclient::newQueryCommand(sqlrservercursor *cursor) {
	debugFunction();

	cont->raiseDebugMessageEvent("new query");

	// if we're using a custom cursor then close it
	// FIXME: push up?
	sqlrservercursor	*customcursor=cursor->getCustomQueryCursor();
	if (customcursor) {
		customcursor->close();
		cursor->clearCustomQueryCursor();
	}

	// get the client info and query from the client
	bool	success=(getClientInfo(cursor) && getQuery(cursor));

	// do we need to use a custom query handler for this query?
	if (success) {
		cursor=cont->useCustomQueryCursor(cursor);
	}

	// get binds and whether to get column info
	if (success) {
		success=(getInputBinds(cursor) &&
				getOutputBinds(cursor) &&
				getInputOutputBinds(cursor) &&
				getSendColumnInfo());
	}

	if (success) {
		return processQueryOrBindCursor(cursor,
				SQLRCLIENTQUERYTYPE_QUERY,
				SQLRSERVERLISTFORMAT_NULL,
				false,false);
	}

	// The client is apparently sending us something we
	// can't handle.  Return an error if there was one,
	// instruct the client to disconnect and return false
	// to end the session on this side.
	if (cont->getErrorNumber(cursor)) {
		returnError(cursor,true);
	}
	cont->raiseDebugMessageEvent("new query failed");
	return false;
}

bool sqlrprotocol_sqlrclient::reExecuteQueryCommand(sqlrservercursor *cursor) {
	debugFunction();

	cont->raiseDebugMessageEvent("rexecute query");

	// if we're using a custom cursor then operate on it
	// FIXME: push up?
	sqlrservercursor	*customcursor=cursor->getCustomQueryCursor();
	if (customcursor) {
		cursor=customcursor;
	}

	// get binds and whether to get column info
	if (getInputBinds(cursor) &&
			getOutputBinds(cursor) &&
			getInputOutputBinds(cursor) &&
			getSendColumnInfo()) {
		return processQueryOrBindCursor(cursor,
				SQLRCLIENTQUERYTYPE_QUERY,
				SQLRSERVERLISTFORMAT_NULL,
				true,false);
	}

	// The client is apparently sending us something we
	// can't handle.  Return an error if there was one,
	// instruct the client to disconnect and return false
	// to end the session on this side.
	if (cont->getErrorNumber(cursor)) {
		returnError(cursor,true);
	}
	cont->raiseDebugMessageEvent("reexecute query failed");
	return false;
}

bool sqlrprotocol_sqlrclient::nextResultSetCommand(sqlrservercursor *cursor) {
	debugFunction();

	cont->raiseDebugMessageEvent("nextResultSet");

	// if we're using a custom cursor then operate on it
	// FIXME: push up?
	sqlrservercursor	*customcursor=cursor->getCustomQueryCursor();
	if (customcursor) {
		cursor=customcursor;
	}

	bool nextresultsetavailable;
	if (cont->nextResultSet(cursor,&nextresultsetavailable)) {
		cont->raiseDebugMessageEvent("nextResultSet succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->write(nextresultsetavailable);
		clientsock->flushWriteBuffer(-1,-1);
		if (nextresultsetavailable) {
			cont->incrementNextResultSetAvailableCount();
		}
		cont->incrementNextResultSetCount();
		return true;
	}

	cont->raiseDebugMessageEvent("nextResultSet failed");
	returnError(!cont->getLiveConnection());
	cont->incrementNextResultSetCount();
	return false;
}

bool sqlrprotocol_sqlrclient::fetchFromBindCursorCommand(
					sqlrservercursor *cursor) {
	debugFunction();

	cont->raiseDebugMessageEvent("fetch from bind cursor");

	// if we're using a custom cursor then close it
	// FIXME: push up?
	sqlrservercursor	*customcursor=cursor->getCustomQueryCursor();
	if (customcursor) {
		customcursor->close();
		cursor->clearCustomQueryCursor();
	}

	// get whether to get column info
	if (getSendColumnInfo()) {
		return processQueryOrBindCursor(cursor,
				SQLRCLIENTQUERYTYPE_QUERY,
				SQLRSERVERLISTFORMAT_NULL,
				false,true);
	}

	// The client is apparently sending us something we
	// can't handle.  Return an error if there was one,
	// instruct the client to disconnect and return false
	// to end the session on this side.
	if (cont->getErrorNumber(cursor)) {
		returnError(cursor,true);
	}
	cont->raiseDebugMessageEvent("failed to fetch from bind cursor");
	return false;
}

bool sqlrprotocol_sqlrclient::processQueryOrBindCursor(
					sqlrservercursor *cursor,
					sqlrclientquerytype_t querytype,
					sqlrserverlistformat_t listformat,
					bool reexecute,
					bool bindcursor) {
	debugFunction();

	// loop here to handle down databases
	for (;;) {

		// process the query or bind cursor...
		bool	success=false;
		if (bindcursor) {
			success=cont->fetchFromBindCursor(cursor);
		} else if (reexecute) {
			success=cont->executeQuery(cursor,true,true,true,true);
		} else {
			success=(cont->prepareQuery(cursor,
					cont->getQueryBuffer(cursor),
					cont->getQueryLength(cursor),
					true,true,true) &&
				cont->executeQuery(cursor,true,true,true,true));
		}

		// get the skip and fetch parameters here so everything can be
		// done in one round trip without relying on buffering
		if (success) {
			success=getSkipAndFetch(true,cursor);
		}

		if (success) {

			// success...

			cont->raiseDebugMessageEvent("process query succeeded");

			// indicate that no error has occurred
			clientsock->write((uint16_t)NO_ERROR_OCCURRED);

			// send the client the id of the cursor
			// that it's going to use so it can request
			// it again later for re-execute
			clientsock->write(cont->getId(cursor));

			// tell the client that this is not a
			// suspended result set
			clientsock->write((uint16_t)NO_SUSPENDED_RESULT_SET);

			// remap columns
			switch (querytype) {
				case SQLRCLIENTQUERYTYPE_DATABASE_LIST:
					cont->setDatabaseListColumnMap(
								listformat);
					break;
				case SQLRCLIENTQUERYTYPE_SCHEMA_LIST:
					cont->setSchemaListColumnMap(
								listformat);
					break;
				case SQLRCLIENTQUERYTYPE_TABLE_LIST:
				case SQLRCLIENTQUERYTYPE_TABLE_LIST_2:
					cont->setTableListColumnMap(
								listformat);
					break;
				case SQLRCLIENTQUERYTYPE_TABLE_TYPE_LIST:
					cont->setTableListColumnMap(
								listformat);
					break;
				case SQLRCLIENTQUERYTYPE_COLUMN_LIST:
					cont->setColumnListColumnMap(
								listformat);
					break;
				case SQLRCLIENTQUERYTYPE_PRIMARY_KEY_LIST:
					cont->setPrimaryKeyListColumnMap(
								listformat);
					break;
				case SQLRCLIENTQUERYTYPE_KEY_AND_INDEX_LIST:
					cont->setKeyAndIndexListColumnMap(
								listformat);
					break;
				case SQLRCLIENTQUERYTYPE_PROCEDURE_BIND_AND_COLUMN_LIST:
					cont->setProcedureBindAndColumnListColumnMap(
								listformat);
					break;
				case SQLRCLIENTQUERYTYPE_TYPE_INFO_LIST:
					cont->setTypeInfoListColumnMap(
								listformat);
					break;
				case SQLRCLIENTQUERYTYPE_PROCEDURE_LIST:
					cont->setProcedureListColumnMap(
								listformat);
					break;
				default:
					break;
			}

			// send a result set header
			returnResultSetHeader(cursor);

			// return the result set
			return returnResultSetData(cursor,false,false);

		} else {

			// an error occurred...

			// is the db still up?
			bool	dbup=cont->getLiveConnection(cursor);

			// if the db is still up, or if we're not configured
			// to wait for them if they're down, then return the
			// error
			if (dbup || !waitfordowndb) {

				// return the error
				returnError(cursor,false);
			}

			// if the error was a dead connection
			// then re-establish the connection
			if (!dbup) {

				cont->raiseDebugMessageEvent(
						"database is down...");

				cont->raiseDbErrorEvent(cursor,
						cont->getErrorBuffer(cursor));

				cont->reLogIn();

				// if we're waiting for down databases then
				// loop back and try the query again
				if (waitfordowndb) {
					continue;
				}
			}
			return true;
		}
	}
}

bool sqlrprotocol_sqlrclient::getClientInfo(sqlrservercursor *cursor) {
	debugFunction();

	cont->raiseDebugMessageEvent("getting client info...");

	// init
	clientinfo[0]='\0';
	clientinfolen=0;

	// get the length of the client info
	ssize_t	result=clientsock->read(&clientinfolen);
	if (result!=sizeof(uint64_t)) {
		cont->raiseClientProtocolErrorEvent(cursor,
				"get client info failed: "
				"failed to get clientinfo length",result);
		return false;
	}

	// bounds checking
	if (clientinfolen>maxclientinfolength) {

		stringbuffer	err;
		err.append(SQLR_ERROR_MAXCLIENTINFOLENGTH_STRING);
		err.append(" (")->append(clientinfolen)->append('>');
		err.append(maxclientinfolength)->append(')');
		cont->setError(cursor,err.getString(),
				SQLR_ERROR_MAXCLIENTINFOLENGTH,true);

		debugstr.clear();
		debugstr.append("get client info failed: "
				"client sent bad client info size: ");
		debugstr.append(clientinfolen);
		cont->raiseClientProtocolErrorEvent(cursor,debugstr.getString(),1);
		return false;
	}

	// read the client info into the buffer
	result=clientsock->read(clientinfo,clientinfolen);
	if ((uint64_t)result!=clientinfolen) {
		cont->raiseClientProtocolErrorEvent(cursor,
				"get client info failed: "
				"failed to get client info",result);
		return false;
	}
	clientinfo[clientinfolen]='\0';

	if (cont->logEnabled() || cont->notificationsEnabled()) {
		debugstr.clear();
		debugstr.append("clientinfolen: ")->append(clientinfolen);
		cont->raiseDebugMessageEvent(debugstr.getString());
		debugstr.clear();
		debugstr.append("clientinfo: ")->append(clientinfo);
		cont->raiseDebugMessageEvent(debugstr.getString());
		cont->raiseDebugMessageEvent("getting clientinfo succeeded");
	}

	// FIXME: push up?
	// update the stats with the client info
	cont->setClientInfo(clientinfo,clientinfolen);

	return true;
}

bool sqlrprotocol_sqlrclient::getQuery(sqlrservercursor *cursor) {
	debugFunction();

	cont->raiseDebugMessageEvent("getting query...");

	// init
	uint32_t	querylength=0;
	char		*querybuffer=cont->getQueryBuffer(cursor);
	querybuffer[0]='\0';
	cont->setQueryLength(cursor,0);

	// get the length of the query
	ssize_t	result=clientsock->read(&querylength,idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->raiseClientProtocolErrorEvent(cursor,
				"get query failed: "
				"failed to get query length",result);
		return false;
	}

	// bounds checking
	if (querylength>maxquerysize) {

		stringbuffer	err;
		err.append(SQLR_ERROR_MAXQUERYLENGTH_STRING);
		err.append(" (")->append(querylength)->append('>');
		err.append(maxquerysize)->append(')');
		cont->setError(cursor,err.getString(),
				SQLR_ERROR_MAXQUERYLENGTH,true);

		debugstr.clear();
		debugstr.append("get query failed: "
				"client sent bad query length: ");
		debugstr.append(querylength);
		cont->raiseClientProtocolErrorEvent(cursor,
						debugstr.getString(),1);

		return false;
	}

	// read the query into the buffer
	result=clientsock->read(querybuffer,querylength,idleclienttimeout,0);
	if ((uint32_t)result!=querylength) {

		querybuffer[0]='\0';

		cont->raiseClientProtocolErrorEvent(cursor,
				"get query failed: "
				"failed to get query",result);
		return false;
	}

	// update query buffer and length
	querybuffer[querylength]='\0';
	cont->setQueryLength(cursor,querylength);

	if (cont->logEnabled() || cont->notificationsEnabled()) {
		debugstr.clear();
		debugstr.append("querylength: ")->append(querylength);
		cont->raiseDebugMessageEvent(debugstr.getString());
		debugstr.clear();
		debugstr.append("query: ")->append(querybuffer);
		cont->raiseDebugMessageEvent(debugstr.getString());
		cont->raiseDebugMessageEvent("getting query succeeded");
	}

	// FIXME: push up?
	// update the stats with the current query
	cont->setCurrentQuery(querybuffer,querylength);

	return true;
}

bool sqlrprotocol_sqlrclient::getInputBinds(sqlrservercursor *cursor) {
	debugFunction();

	cont->raiseDebugMessageEvent("getting input binds...");

	// get the number of input bind variable/values
	uint16_t	inbindcount=0;
	if (!getBindVarCount(cursor,&inbindcount)) {
		return false;
	}
	cont->setInputBindCount(cursor,inbindcount);

	// get the input bind buffers
	memorypool		*bindpool=cont->getBindPool(cursor);
	sqlrserverbindvar	*inbinds=cont->getInputBinds(cursor);

	// fill the buffers
	for (uint16_t i=0; i<inbindcount && i<maxbindcount; i++) {

		sqlrserverbindvar	*bv=&(inbinds[i]);

		// get the variable name and type
		if (!(getBindVarName(cursor,bv,bindpool) &&
					getBindVarType(bv))) {
			return false;
		}

		// get the value
		if (bv->type==SQLRSERVERBINDVARTYPE_NULL) {
			getNullBind(bv,bindpool);
		} else if (bv->type==SQLRSERVERBINDVARTYPE_STRING) {
			if (!getStringBind(cursor,bv,bindpool)) {
				return false;
			}
		} else if (bv->type==SQLRSERVERBINDVARTYPE_INTEGER) {
			if (!getIntegerBind(bv)) {
				return false;
			}
		} else if (bv->type==SQLRSERVERBINDVARTYPE_DOUBLE) {
			if (!getDoubleBind(bv)) {
				return false;
			}
		} else if (bv->type==SQLRSERVERBINDVARTYPE_DATE) {
			if (!getDateBind(bv,bindpool)) {
				return false;
			}
		} else if (bv->type==SQLRSERVERBINDVARTYPE_BLOB) {
			if (!getLobBind(cursor,bv,bindpool)) {
				return false;
			}
		} else if (bv->type==SQLRSERVERBINDVARTYPE_CLOB) {
			if (!getLobBind(cursor,bv,bindpool)) {
				return false;
			}
		}		  
	}

	cont->raiseDebugMessageEvent("done getting input binds");
	return true;
}

bool sqlrprotocol_sqlrclient::getOutputBinds(sqlrservercursor *cursor) {
	debugFunction();

	cont->raiseDebugMessageEvent("getting output binds...");

	// get the number of output bind variable/values
	uint16_t	outbindcount=0;
	if (!getBindVarCount(cursor,&outbindcount)) {
		return false;
	}
	cont->setOutputBindCount(cursor,outbindcount);

	// get the output bind buffers
	memorypool		*bindpool=cont->getBindPool(cursor);
	sqlrserverbindvar	*outbinds=cont->getOutputBinds(cursor);

	// fill the buffers
	for (uint16_t i=0; i<outbindcount && i<maxbindcount; i++) {

		sqlrserverbindvar	*bv=&(outbinds[i]);

		// get the variable name and type
		if (!(getBindVarName(cursor,bv,bindpool) &&
					getBindVarType(bv))) {
			return false;
		}

		// get the size of the value
		if (bv->type==SQLRSERVERBINDVARTYPE_STRING) {
			bv->value.stringval=NULL;
			if (!getBindSize(cursor,bv,&maxstringbindvaluelength)) {
				return false;
			}
			// This must be a allocated and zeroed because oracle
			// gets angry if these aren't initialized to NULL's.
			// It's possible that just the first character needs to
			// be NULL, but for now I'm just going to go ahead and
			// allocate/zero.
			bv->value.stringval=
				(char *)bindpool->allocate(bv->valuesize+1);
			bytestring::zero(bv->value.stringval,bv->valuesize+1);
			cont->raiseDebugMessageEvent("STRING");
		} else if (bv->type==SQLRSERVERBINDVARTYPE_INTEGER) {
			cont->raiseDebugMessageEvent("INTEGER");
		} else if (bv->type==SQLRSERVERBINDVARTYPE_DOUBLE) {
			cont->raiseDebugMessageEvent("DOUBLE");
			// these don't typically get set, but they get used
			// when building debug strings, so we need to
			// initialize them
			bv->value.doubleval.precision=0;
			bv->value.doubleval.scale=0;
		} else if (bv->type==SQLRSERVERBINDVARTYPE_DATE) {
			cont->raiseDebugMessageEvent("DATE");
			bv->value.dateval.year=0;
			bv->value.dateval.month=0;
			bv->value.dateval.day=0;
			bv->value.dateval.hour=0;
			bv->value.dateval.minute=0;
			bv->value.dateval.second=0;
			bv->value.dateval.microsecond=0;
			bv->value.dateval.tz=NULL;
			bv->value.dateval.isnegative=false;
			// allocate enough space to store the date/time string
			// or whatever buffer a child might need to store a
			// date 512 bytes ought to be enough
			bv->value.dateval.buffersize=512;
			bv->value.dateval.buffer=
				(char *)bindpool->
					allocate(bv->value.dateval.buffersize);
		} else if (bv->type==SQLRSERVERBINDVARTYPE_BLOB ||
					bv->type==SQLRSERVERBINDVARTYPE_CLOB) {
			if (!getBindSize(cursor,bv,&maxlobbindvaluelength)) {
				return false;
			}
			if (bv->type==SQLRSERVERBINDVARTYPE_BLOB) {
				cont->raiseDebugMessageEvent("BLOB");
			} else if (bv->type==SQLRSERVERBINDVARTYPE_CLOB) {
				cont->raiseDebugMessageEvent("CLOB");
			}
		} else if (bv->type==SQLRSERVERBINDVARTYPE_CURSOR) {
			cont->raiseDebugMessageEvent("CURSOR");
			sqlrservercursor	*curs=cont->getCursor();
			if (!curs) {
				// FIXME: set error here
				return false;
			}
			cont->setState(curs,SQLRCURSORSTATE_BUSY);
			bv->value.cursorid=cont->getId(curs);
		}

		// init the null indicator
		bv->isnull=cont->nonNullBindValue();
	}

	cont->raiseDebugMessageEvent("done getting output binds");
	return true;
}

bool sqlrprotocol_sqlrclient::getInputOutputBinds(sqlrservercursor *cursor) {
	debugFunction();

	if (protocolversion<2) {
		cont->raiseDebugMessageEvent("not getting input/output binds "
						"(client protocol too old)");
		return true;
	}

	cont->raiseDebugMessageEvent("getting input/output binds...");

	// get the number of input/output bind variable/values
	uint16_t	inoutbindcount=0;
	if (!getBindVarCount(cursor,&inoutbindcount)) {
		return false;
	}
	cont->setInputOutputBindCount(cursor,inoutbindcount);

	// get the input/output bind buffers
	memorypool		*bindpool=cont->getBindPool(cursor);
	sqlrserverbindvar	*inoutbinds=cont->getInputOutputBinds(cursor);

	// fill the buffers
	for (uint16_t i=0; i<inoutbindcount && i<maxbindcount; i++) {

		sqlrserverbindvar	*bv=&(inoutbinds[i]);

		// get the variable name and type
		if (!(getBindVarName(cursor,bv,bindpool) &&
					getBindVarType(bv))) {
			return false;
		}

		// get the size of the value
		if (bv->type==SQLRSERVERBINDVARTYPE_NULL) {
			bv->type=SQLRSERVERBINDVARTYPE_STRING;
			bv->value.stringval=NULL;
			if (!getBindSize(cursor,bv,&maxstringbindvaluelength)) {
				return false;
			}
			// This must be a allocated and zeroed because oracle
			// gets angry if these aren't initialized to NULL's.
			// It's possible that just the first character needs to
			// be NULL, but for now I'm just going to go ahead and
			// allocate/zero.
			bv->value.stringval=
				(char *)bindpool->allocate(bv->valuesize+1);
			bytestring::zero(bv->value.stringval,bv->valuesize+1);
			bv->isnull=cont->nullBindValue();
			cont->raiseDebugMessageEvent("NULL");
		} else if (bv->type==SQLRSERVERBINDVARTYPE_STRING) {
			bv->value.stringval=NULL;
			if (!getBindSize(cursor,bv,&maxstringbindvaluelength)) {
				return false;
			}
			// This must be a allocated and zeroed because oracle
			// gets angry if these aren't initialized to NULL's.
			// It's possible that just the first character needs to
			// be NULL, but for now I'm just going to go ahead and
			// allocate/zero.
			bv->value.stringval=
				(char *)bindpool->allocate(bv->valuesize+1);
			bytestring::zero(bv->value.stringval,bv->valuesize+1);

			// get the bind value
			ssize_t	result=clientsock->read(bv->value.stringval,
							bv->valuesize,
							idleclienttimeout,0);
			if ((uint32_t)result!=(uint32_t)(bv->valuesize)) {
				bv->value.stringval[0]='\0';
				const char	*info="get binds failed: "
							"failed to get bind "
							"value";
				cont->raiseClientProtocolErrorEvent(
							cursor,info,result);
				return false;
			}
			bv->value.stringval[bv->valuesize]='\0';
			bv->isnull=cont->nonNullBindValue();
			cont->raiseDebugMessageEvent("STRING");
		} else if (bv->type==SQLRSERVERBINDVARTYPE_INTEGER) {

			// get the bind value
			ssize_t	result=clientsock->read(&(bv->value.integerval),
							idleclienttimeout,0);
			if (result!=sizeof(uint64_t)) {
				const char	*info="get binds failed: "
							"failed to get bind "
							"value";
				cont->raiseClientProtocolErrorEvent(
							cursor,info,result);
				return false;
			}
			bv->isnull=cont->nonNullBindValue();
			cont->raiseDebugMessageEvent("INTEGER");
		} else if (bv->type==SQLRSERVERBINDVARTYPE_DOUBLE) {

			// get the bind value
			ssize_t	result=clientsock->read(
					&(bv->value.doubleval.value),
					idleclienttimeout,0);
			if ((uint32_t)result!=(uint32_t)sizeof(double)) {
				const char	*info="get binds failed: "
							"failed to get bind "
							"value";
				cont->raiseClientProtocolErrorEvent(
							cursor,info,result);
				return false;
			}

			// get the precision
			result=clientsock->read(
					&(bv->value.doubleval.precision),
					idleclienttimeout,0);
			if ((uint32_t)result!=(uint32_t)sizeof(uint32_t)) {
				const char	*info="get binds failed: "
							"failed to get "
							"precision";
				cont->raiseClientProtocolErrorEvent(
							cursor,info,result);
				return false;
			}

			// get the scale
			result=clientsock->read(
					&(bv->value.doubleval.scale),
					idleclienttimeout,0);
			if ((uint32_t)result!=(uint32_t)sizeof(uint32_t)) {
				const char	*info="get binds failed: "
							"failed to get "
							"scale";
				cont->raiseClientProtocolErrorEvent(
							cursor,info,result);
				return false;
			}

			bv->isnull=cont->nonNullBindValue();
			cont->raiseDebugMessageEvent("DOUBLE");
		} else if (bv->type==SQLRSERVERBINDVARTYPE_DATE) {

			// get the year
			ssize_t	result=clientsock->read(
					&(bv->value.dateval.year),
					idleclienttimeout,0);
			if ((uint32_t)result!=(uint32_t)sizeof(uint16_t)) {
				const char	*info="get binds failed: "
							"failed to get bind "
							"year";
				cont->raiseClientProtocolErrorEvent(
							cursor,info,result);
				return false;
			}

			// get the month
			result=clientsock->read(
					&(bv->value.dateval.month),
					idleclienttimeout,0);
			if ((uint32_t)result!=(uint32_t)sizeof(uint16_t)) {
				const char	*info="get binds failed: "
							"failed to get bind "
							"month";
				cont->raiseClientProtocolErrorEvent(
							cursor,info,result);
				return false;
			}

			// get the day
			result=clientsock->read(
					&(bv->value.dateval.day),
					idleclienttimeout,0);
			if ((uint32_t)result!=(uint32_t)sizeof(uint16_t)) {
				const char	*info="get binds failed: "
							"failed to get bind "
							"day";
				cont->raiseClientProtocolErrorEvent(
							cursor,info,result);
				return false;
			}

			// get the hour
			result=clientsock->read(
					&(bv->value.dateval.hour),
					idleclienttimeout,0);
			if ((uint32_t)result!=(uint32_t)sizeof(uint16_t)) {
				const char	*info="get binds failed: "
							"failed to get bind "
							"hour";
				cont->raiseClientProtocolErrorEvent(
							cursor,info,result);
				return false;
			}

			// get the minute
			result=clientsock->read(
					&(bv->value.dateval.minute),
					idleclienttimeout,0);
			if ((uint32_t)result!=(uint32_t)sizeof(uint16_t)) {
				const char	*info="get binds failed: "
							"failed to get bind "
							"minute";
				cont->raiseClientProtocolErrorEvent(
							cursor,info,result);
				return false;
			}

			// get the second
			result=clientsock->read(
					&(bv->value.dateval.second),
					idleclienttimeout,0);
			if ((uint32_t)result!=(uint32_t)sizeof(uint16_t)) {
				const char	*info="get binds failed: "
							"failed to get bind "
							"second";
				cont->raiseClientProtocolErrorEvent(
							cursor,info,result);
				return false;
			}

			// get the microsecond
			result=clientsock->read(
					&(bv->value.dateval.microsecond),
					idleclienttimeout,0);
			if ((uint32_t)result!=(uint32_t)sizeof(uint32_t)) {
				const char	*info="get binds failed: "
							"failed to get bind "
							"microsecond";
				cont->raiseClientProtocolErrorEvent(
							cursor,info,result);
				return false;
			}

			// get the tz length
			uint16_t	tzlen=0;
			result=clientsock->read(&tzlen,idleclienttimeout,0);
			if ((uint32_t)result!=(uint32_t)sizeof(uint16_t)) {
				const char	*info="get binds failed: "
							"failed to get bind "
							"tz length";
				cont->raiseClientProtocolErrorEvent(
							cursor,info,result);
				return false;
			}

			// get the tz
			bv->value.dateval.tz=(char *)bindpool->allocate(tzlen);
			result=clientsock->read(bv->value.dateval.tz,tzlen,
						idleclienttimeout,0);
			if ((uint32_t)result!=(uint32_t)tzlen) {
				bv->value.dateval.tz[0]='\0';
				const char	*info="get binds failed: "
							"failed to get bind "
							"tz";
				cont->raiseClientProtocolErrorEvent(
							cursor,info,result);
				return false;
			}

			// get the is-negative flag
			result=clientsock->read(&bv->value.dateval.isnegative,
							idleclienttimeout,0);
			if (result!=sizeof(bool)) {
				const char	*info="get binds failed: "
							"failed to get "
							"is-negative flag";
				cont->raiseClientProtocolErrorEvent(
							cursor,info,result);
				return false;
			}

			// allocate enough space to store the date/time string
			// or whatever buffer a child might need to store a
			// date 512 bytes ought to be enough
			bv->value.dateval.buffersize=512;
			bv->value.dateval.buffer=
				(char *)bindpool->allocate(
						bv->value.dateval.buffersize);

			bv->isnull=cont->nonNullBindValue();
			cont->raiseDebugMessageEvent("DATE");
		} /*else if (bv->type==SQLRSERVERBINDVARTYPE_BLOB ||
					bv->type==SQLRSERVERBINDVARTYPE_CLOB) {
			if (!getBindSize(cursor,bv,&maxlobbindvaluelength)) {
				return false;
			}
			if (bv->type==SQLRSERVERBINDVARTYPE_BLOB) {
				cont->raiseDebugMessageEvent("BLOB");
			} else if (bv->type==SQLRSERVERBINDVARTYPE_CLOB) {
				cont->raiseDebugMessageEvent("CLOB");
			}
			bv->isnull=cont->nonNullBindValue();
		}*/
	}

	cont->raiseDebugMessageEvent("done getting input/output binds");
	return true;
}

bool sqlrprotocol_sqlrclient::getBindVarCount(sqlrservercursor *cursor,
							uint16_t *count) {
	debugFunction();

	// init
	*count=0;

	// get the number of input bind variable/values
	ssize_t	result=clientsock->read(count,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->raiseClientProtocolErrorEvent(cursor,
				"get binds failed: "
				"failed to get bind count",result);
		*count=0;
		return false;
	}

	// bounds checking
	if (*count>maxbindcount) {

		stringbuffer	err;
		err.append(SQLR_ERROR_MAXBINDCOUNT_STRING);
		err.append(" (")->append(*count)->append('>');
		err.append(maxbindcount)->append(')');
		cont->setError(cursor,err.getString(),
				SQLR_ERROR_MAXBINDCOUNT,true);

		debugstr.clear();
		debugstr.append("get binds failed: "
				"client tried to send too many binds: ");
		debugstr.append(*count);
		cont->raiseClientProtocolErrorEvent(cursor,debugstr.getString(),1);

		*count=0;
		return false;
	}

	return true;
}

bool sqlrprotocol_sqlrclient::getBindVarName(sqlrservercursor *cursor,
						sqlrserverbindvar *bv,
						memorypool *bindpool) {
	debugFunction();

	// init
	bv->variablesize=0;
	bv->variable=NULL;

	// get the variable name size
	uint16_t	bindnamesize;
	ssize_t		result=clientsock->read(&bindnamesize,
						idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->raiseClientProtocolErrorEvent(cursor,
				"get binds failed: "
				"failed to get variable name length",result);
		return false;
	}

	// bounds checking
	if (bindnamesize>maxbindnamelength) {

		stringbuffer	err;
		err.append(SQLR_ERROR_MAXBINDNAMELENGTH_STRING);
		err.append(" (")->append(bindnamesize)->append('>');
		err.append(maxbindnamelength)->append(')');
		cont->setError(cursor,err.getString(),
					SQLR_ERROR_MAXBINDNAMELENGTH,true);

		debugstr.clear();
		debugstr.append("get binds failed: bad variable name length: ");
		debugstr.append(bindnamesize);
		cont->raiseClientProtocolErrorEvent(cursor,debugstr.getString(),1);
		return false;
	}

	// get the variable name
	bv->variablesize=bindnamesize+1;
	bv->variable=(char *)bindpool->allocate(bindnamesize+2);
	bv->variable[0]=cont->bindFormat()[0];
	result=clientsock->read(bv->variable+1,bindnamesize,
					idleclienttimeout,0);
	if (result!=bindnamesize) {
		bv->variablesize=0;
		bv->variable[0]='\0';
		cont->raiseClientProtocolErrorEvent(cursor,
				"get binds failed: "
				"failed to get variable name",result);
		return false;
	}
	bv->variable[bindnamesize+1]='\0';

	cont->raiseDebugMessageEvent(bv->variable);

	return true;
}

bool sqlrprotocol_sqlrclient::getBindVarType(sqlrserverbindvar *bv) {
	debugFunction();

	// get the type
	uint16_t	type;
	ssize_t	result=clientsock->read(&type,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->raiseClientProtocolErrorEvent(NULL,
				"get binds failed: "
				"failed to get type",result);
		return false;
	}
	bv->type=(sqlrserverbindvartype_t)type;
	return true;
}

bool sqlrprotocol_sqlrclient::getBindSize(sqlrservercursor *cursor,
						sqlrserverbindvar *bv,
						uint32_t *maxsize) {
	debugFunction();

	// init
	bv->valuesize=0;

	// get the size of the value
	ssize_t	result=clientsock->read(&(bv->valuesize),idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		bv->valuesize=0;
		cont->raiseClientProtocolErrorEvent(cursor,
				"get binds failed: "
				"failed to get bind value length",result);
		return false;
	}

	// bounds checking
	if (bv->valuesize>*maxsize) {
		if (maxsize==&maxstringbindvaluelength) {
			stringbuffer	err;
			err.append(SQLR_ERROR_MAXSTRINGBINDVALUELENGTH_STRING);
			err.append(" (")->append(bv->valuesize)->append('>');
			err.append(*maxsize)->append(')');
			cont->setError(cursor,err.getString(),
				SQLR_ERROR_MAXSTRINGBINDVALUELENGTH,true);
		} else {
			stringbuffer	err;
			err.append(SQLR_ERROR_MAXLOBBINDVALUELENGTH_STRING);
			err.append(" (")->append(bv->valuesize)->append('>');
			err.append(*maxsize)->append(')');
			cont->setError(cursor,err.getString(),
				SQLR_ERROR_MAXLOBBINDVALUELENGTH,true);
		}
		debugstr.clear();
		debugstr.append("get binds failed: bad value length: ");
		debugstr.append(bv->valuesize);
		cont->raiseClientProtocolErrorEvent(
				cursor,debugstr.getString(),1);
		return false;
	}

	return true;
}

void sqlrprotocol_sqlrclient::getNullBind(sqlrserverbindvar *bv,
						memorypool *bindpool) {
	debugFunction();

	cont->raiseDebugMessageEvent("NULL");

	bv->value.stringval=(char *)bindpool->allocate(1);
	bv->value.stringval[0]='\0';
	bv->valuesize=0;
	bv->isnull=cont->nullBindValue();
}

bool sqlrprotocol_sqlrclient::getStringBind(sqlrservercursor *cursor,
						sqlrserverbindvar *bv,
						memorypool *bindpool) {
	debugFunction();

	cont->raiseDebugMessageEvent("STRING");

	// init
	bv->value.stringval=NULL;

	// get the size of the value
	if (!getBindSize(cursor,bv,&maxstringbindvaluelength)) {
		return false;
	}

	// allocate space to store the value
	bv->value.stringval=(char *)bindpool->allocate(bv->valuesize+1);

	// get the bind value
	ssize_t	result=clientsock->read(bv->value.stringval,
					bv->valuesize,
					idleclienttimeout,0);
	if ((uint32_t)result!=(uint32_t)(bv->valuesize)) {
		bv->value.stringval[0]='\0';
		const char	*info="get binds failed: "
					"failed to get bind value";
		cont->raiseClientProtocolErrorEvent(cursor,info,result);
		return false;
	}
	bv->value.stringval[bv->valuesize]='\0';

	bv->isnull=cont->nonNullBindValue();

	cont->raiseDebugMessageEvent(bv->value.stringval);

	return true;
}

bool sqlrprotocol_sqlrclient::getIntegerBind(sqlrserverbindvar *bv) {
	debugFunction();

	cont->raiseDebugMessageEvent("INTEGER");

	// get the value itself
	uint64_t	value;
	ssize_t		result=clientsock->read(&value,idleclienttimeout,0);
	if (result!=sizeof(uint64_t)) {
		cont->raiseClientProtocolErrorEvent(NULL,
				"get binds failed: "
				"failed to get bind value",result);
		return false;
	}

	// set the value
	bv->value.integerval=(int64_t)value;

	char	*intval=charstring::parseNumber(bv->value.integerval);
	cont->raiseDebugMessageEvent(intval);
	delete[] intval;

	return true;
}

bool sqlrprotocol_sqlrclient::getDoubleBind(sqlrserverbindvar *bv) {
	debugFunction();

	cont->raiseDebugMessageEvent("DOUBLE");

	// get the value
	ssize_t	result=clientsock->read(&(bv->value.doubleval.value),
						idleclienttimeout,0);
	if (result!=sizeof(double)) {
		cont->raiseClientProtocolErrorEvent(NULL,
				"get binds failed: "
				"failed to get bind value",result);
		return false;
	}

	// get the precision
	result=clientsock->read(&(bv->value.doubleval.precision),
						idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->raiseClientProtocolErrorEvent(NULL,
				"get binds failed: "
				"failed to get precision",result);
		return false;
	}

	// get the scale
	result=clientsock->read(&(bv->value.doubleval.scale),
						idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->raiseClientProtocolErrorEvent(NULL,
				"get binds failed: "
				"failed to get scale",result);
		return false;
	}

	char	*doubleval=charstring::parseNumber(bv->value.doubleval.value);
	cont->raiseDebugMessageEvent(doubleval);
	delete[] doubleval;

	return true;
}

bool sqlrprotocol_sqlrclient::getDateBind(sqlrserverbindvar *bv,
						memorypool *bindpool) {
	debugFunction();

	cont->raiseDebugMessageEvent("DATE");

	// init
	bv->value.dateval.tz=NULL;

	uint16_t	temp;

	// get the year
	ssize_t	result=clientsock->read(&temp,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->raiseClientProtocolErrorEvent(NULL,
				"get binds failed: "
				"failed to get year",result);
		return false;
	}
	bv->value.dateval.year=(int16_t)temp;

	// get the month
	result=clientsock->read(&temp,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->raiseClientProtocolErrorEvent(NULL,
				"get binds failed: "
				"failed to get month",result);
		return false;
	}
	bv->value.dateval.month=(int16_t)temp;

	// get the day
	result=clientsock->read(&temp,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->raiseClientProtocolErrorEvent(NULL,
				"get binds failed: "
				"failed to get day",result);
		return false;
	}
	bv->value.dateval.day=(int16_t)temp;

	// get the hour
	result=clientsock->read(&temp,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->raiseClientProtocolErrorEvent(NULL,
				"get binds failed: "
				"failed to get hour",result);
		return false;
	}
	bv->value.dateval.hour=(int16_t)temp;

	// get the minute
	result=clientsock->read(&temp,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->raiseClientProtocolErrorEvent(NULL,
				"get binds failed: "
				"failed to get minute",result);
		return false;
	}
	bv->value.dateval.minute=(int16_t)temp;

	// get the second
	result=clientsock->read(&temp,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->raiseClientProtocolErrorEvent(NULL,
				"get binds failed: "
				"failed to get second",result);
		return false;
	}
	bv->value.dateval.second=(int16_t)temp;

	// get the microsecond
	uint32_t	temp32;
	result=clientsock->read(&temp32,idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->raiseClientProtocolErrorEvent(NULL,
				"get binds failed: "
				"failed to get microsecond",result);
		return false;
	}
	bv->value.dateval.microsecond=(int32_t)temp32;

	// get the size of the time zone
	uint16_t	length;
	result=clientsock->read(&length,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->raiseClientProtocolErrorEvent(NULL,
				"get binds failed: "
				"failed to get timezone size",result);
		return false;
	}

	// FIXME: do bounds checking here

	// allocate space to store the time zone
	bv->value.dateval.tz=(char *)bindpool->allocate(length+1);

	// get the time zone
	result=clientsock->read(bv->value.dateval.tz,length,
					idleclienttimeout,0);
	if ((uint16_t)result!=length) {
		bv->value.dateval.tz[0]='\0';
		cont->raiseClientProtocolErrorEvent(NULL,
				"get binds failed: "
				"failed to get timezone",result);
		return false;
	}
	bv->value.dateval.tz[length]='\0';

	// get the is-negative flag
	bool	tempbool;
	result=clientsock->read(&tempbool,idleclienttimeout,0);
	if (result!=sizeof(bool)) {
		cont->raiseClientProtocolErrorEvent(NULL,
				"get binds failed: "
				"failed to get is-negative flag",result);
		return false;
	}
	bv->value.dateval.isnegative=tempbool;

	// allocate enough space to store the date/time string
	// 64 bytes ought to be enough
	bv->value.dateval.buffersize=64;
	bv->value.dateval.buffer=(char *)bindpool->
					allocate(bv->value.dateval.buffersize);

	bv->isnull=cont->nonNullBindValue();

	debugstr.clear();
	debugstr.append(bv->value.dateval.year)->append('-');
	debugstr.append(bv->value.dateval.month)->append('-');
	debugstr.append(bv->value.dateval.day)->append(' ');
	if (bv->value.dateval.isnegative) {
		debugstr.append('-');
	}
	debugstr.append(bv->value.dateval.hour)->append(':');
	debugstr.append(bv->value.dateval.minute)->append(':');
	debugstr.append(bv->value.dateval.second)->append(':');
	debugstr.append(bv->value.dateval.microsecond)->append(' ');
	debugstr.append(bv->value.dateval.tz);
	cont->raiseDebugMessageEvent(debugstr.getString());

	return true;
}

bool sqlrprotocol_sqlrclient::getLobBind(sqlrservercursor *cursor,
						sqlrserverbindvar *bv,
						memorypool *bindpool) {
	debugFunction();

	// init
	bv->value.stringval=NULL;

	if (bv->type==SQLRSERVERBINDVARTYPE_BLOB) {
		cont->raiseDebugMessageEvent("BLOB");
	}
	if (bv->type==SQLRSERVERBINDVARTYPE_CLOB) {
		cont->raiseDebugMessageEvent("CLOB");
	}

	// get the size of the value
	if (!getBindSize(cursor,bv,&maxlobbindvaluelength)) {
		return false;
	}

	// allocate space to store the value
	// (the +1 is to store the NULL-terminator for CLOBS)
	bv->value.stringval=(char *)bindpool->allocate(bv->valuesize+1);

	// get the bind value
	ssize_t	result=clientsock->read(bv->value.stringval,
					bv->valuesize,
					idleclienttimeout,0);
	if ((uint32_t)result!=(uint32_t)(bv->valuesize)) {
		bv->value.stringval[0]='\0';
		cont->raiseClientProtocolErrorEvent(cursor,
				"get binds failed: bad value",result);
		return false;
	}

	// It shouldn't hurt to NULL-terminate the lob because the actual size
	// (which doesn't include the NULL terminator) should be used when
	// binding.
	bv->value.stringval[bv->valuesize]='\0';

	bv->isnull=cont->nonNullBindValue();

	return true;
}

bool sqlrprotocol_sqlrclient::getSendColumnInfo() {
	debugFunction();

	cont->raiseDebugMessageEvent("get send column info...");

	uint16_t	sendcolumninfo;
	ssize_t	result=clientsock->read(&sendcolumninfo,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->raiseClientProtocolErrorEvent(NULL,
				"get send column info failed",result);
		return false;
	}

	if (sendcolumninfo==SEND_COLUMN_INFO) {
		cont->raiseDebugMessageEvent("send column info");
	} else {
		cont->raiseDebugMessageEvent("don't send column info");
	}
	cont->raiseDebugMessageEvent("done getting send column info...");

	cont->setSendColumnInfo(sendcolumninfo);

	return true;
}

bool sqlrprotocol_sqlrclient::getSkipAndFetch(bool initial,
						sqlrservercursor *cursor) {
	debugFunction();

	ssize_t	result=0;
	if (initial) {

		// get some flags
		uint64_t	flags=0;
		result=clientsock->read(&flags,idleclienttimeout,0);
		if (result!=sizeof(uint64_t)) {
			cont->raiseClientProtocolErrorEvent(cursor,
					"return result set data failed: "
					"failed to get flags",result);
			return false;
		}

		// for now the only flag is whether or not to do lazy fetches
		lazyfetch=flags;

		// in this situation, skip should always be 0
		skip=0;

	} else {

		// get the number of rows to skip
		result=clientsock->read(&skip,idleclienttimeout,0);
		if (result!=sizeof(uint64_t)) {
			cont->raiseClientProtocolErrorEvent(cursor,
					"return result set data failed: "
					"failed to get rows to skip",result);
			return false;
		}
	}

	// get the number of rows to fetch
	result=clientsock->read(&fetch,idleclienttimeout,0);
	if (result!=sizeof(uint64_t)) {
		cont->raiseClientProtocolErrorEvent(cursor,
				"return result set data failed: "
				"failed to get rows to fetch",result);
		return false;
	}
	return true;
}

void sqlrprotocol_sqlrclient::returnResultSetHeader(sqlrservercursor *cursor) {
	debugFunction();

	cont->raiseDebugMessageEvent("returning result set header...");

	// return the row counts
	cont->raiseDebugMessageEvent("returning row counts...");
	sendRowCounts(cont->knowsRowCount(cursor),
			cont->rowCount(cursor),
			cont->knowsAffectedRows(cursor),
			cont->affectedRows(cursor));
	cont->raiseDebugMessageEvent("done returning row counts");

	// tell the client whether or not the column information will be sent
	uint16_t	sendcolumninfo=cont->getSendColumnInfo();
	clientsock->write(sendcolumninfo);
	cont->raiseDebugMessageEvent((sendcolumninfo==SEND_COLUMN_INFO)?
					"column info will be sent":
					"column info will not be sent");

	// return the column count
	cont->raiseDebugMessageEvent("returning column counts...");
	clientsock->write(cont->colCount(cursor));
	cont->raiseDebugMessageEvent("done returning column counts");

	if (sendcolumninfo==SEND_COLUMN_INFO) {

		// return the column type format
		cont->raiseDebugMessageEvent("sending column type format...");
		uint16_t	format=cont->columnTypeFormat(cursor);
		cont->raiseDebugMessageEvent((format==COLUMN_TYPE_IDS)?
							"id's":"names");
		clientsock->write(format);
		cont->raiseDebugMessageEvent("done sending column type format");

		// return the column info
		cont->raiseDebugMessageEvent("returning column info...");
		returnColumnInfo(cursor,format);
		cont->raiseDebugMessageEvent("done returning column info");
	}

	// return the output bind vars
	returnOutputBindValues(cursor);
	returnInputOutputBindValues(cursor);

	cont->raiseDebugMessageEvent("done returning result set header");
}

void sqlrprotocol_sqlrclient::returnColumnInfo(sqlrservercursor *cursor,
							uint16_t format) {
	debugFunction();

	for (uint32_t i=0; i<cont->colCount(cursor); i++) {

		const char	*name=cont->getColumnName(cursor,i);
		uint16_t	namelen=cont->getColumnNameLength(cursor,i);
		uint32_t	length=cont->getColumnLength(cursor,i);
		uint32_t	precision=cont->getColumnPrecision(cursor,i);
		uint32_t	scale=cont->getColumnScale(cursor,i);
		uint16_t	nullable=cont->getColumnIsNullable(cursor,i);
		uint16_t	primarykey=
				cont->getColumnIsPrimaryKey(cursor,i);
		uint16_t	unique=cont->getColumnIsUnique(cursor,i);
		uint16_t	partofkey=cont->getColumnIsPartOfKey(cursor,i);
		uint16_t	unsignednumber=
				cont->getColumnIsUnsigned(cursor,i);
		uint16_t	zerofill=cont->getColumnIsZeroFilled(cursor,i);
		uint16_t	binary=cont->getColumnIsBinary(cursor,i);
		uint16_t	autoincrement=
				cont->getColumnIsAutoIncrement(cursor,i);
		const char	*table=cont->getColumnTable(cursor,i);
		uint16_t	tablelen=cont->getColumnTableLength(cursor,i);

		if (format==COLUMN_TYPE_IDS) {
			sendColumnDefinition(name,namelen,
					protocolAppropriateColumnType(
						cont->getColumnType(cursor,i)),
					length,precision,scale,
					nullable,primarykey,unique,partofkey,
					unsignednumber,zerofill,binary,
					autoincrement,table,tablelen);
		} else {
			sendColumnDefinitionString(name,namelen,
					cont->getColumnTypeName(cursor,i),
					cont->getColumnTypeNameLength(cursor,i),
					length,precision,scale,
					nullable,primarykey,unique,partofkey,
					unsignednumber,zerofill,binary,
					autoincrement,table,tablelen);
		}
	}
}

uint16_t sqlrprotocol_sqlrclient::protocolAppropriateColumnType(
							uint16_t coltype) {

	if (protocolversion>=2) {
		return coltype;
	}

	// these types didn't exist in earlier protocol verions
	switch (coltype) {
		// also added by mysql
		case TINYTEXT_DATATYPE:
			return TINY_BLOB_DATATYPE;
		case MEDIUMTEXT_DATATYPE:
			return MEDIUM_BLOB_DATATYPE;
		case LONGTEXT_DATATYPE:
			return LONG_BLOB_DATATYPE;
		case JSON_DATATYPE:
			return UNKNOWN_DATATYPE;
		case GEOMETRY_DATATYPE:
			return UNKNOWN_DATATYPE;
		// also added by oracle
		case SDO_GEOMETRY_DATATYPE:
			return BLOB_DATATYPE;
		// added by mssql
		case NCHAR_DATATYPE:
			return CHAR_DATATYPE;
		case NVARCHAR_DATATYPE:
			return VARCHAR_DATATYPE;
		case NTEXT_DATATYPE:
			return TEXT_DATATYPE;
		case XML_DATATYPE:
			return VARCHAR_DATATYPE;
		case DATETIMEOFFSET_DATATYPE:
			return DATETIME_DATATYPE;
		default:
			return coltype;
	}
}

void sqlrprotocol_sqlrclient::sendRowCounts(bool knowsactual,
						uint64_t actual,
						bool knowsaffected,
						uint64_t affected) {
	debugFunction();

	cont->raiseDebugMessageEvent("sending row counts...");

	// send actual rows, if that is known
	if (knowsactual) {

		char	string[30];
		charstring::printf(string,sizeof(string),
				"actual rows: %lld",(long long)actual);
		cont->raiseDebugMessageEvent(string);

		clientsock->write((uint16_t)ACTUAL_ROWS);
		clientsock->write(actual);

	} else {

		cont->raiseDebugMessageEvent("actual rows unknown");

		clientsock->write((uint16_t)NO_ACTUAL_ROWS);
	}

	
	// send affected rows, if that is known
	if (knowsaffected) {

		char	string[46];
		charstring::printf(string,46,
				"affected rows: %lld",(long long)affected);
		cont->raiseDebugMessageEvent(string);

		clientsock->write((uint16_t)AFFECTED_ROWS);
		clientsock->write(affected);

	} else {

		cont->raiseDebugMessageEvent("affected rows unknown");

		clientsock->write((uint16_t)NO_AFFECTED_ROWS);
	}

	cont->raiseDebugMessageEvent("done sending row counts");
}

void sqlrprotocol_sqlrclient::returnOutputBindValues(sqlrservercursor *cursor) {
	debugFunction();

	if (cont->logEnabled() || cont->notificationsEnabled()) {
		debugstr.clear();
		debugstr.append("returning ");
		debugstr.append(cont->getOutputBindCount(cursor));
		debugstr.append(" output bind values: ");
		cont->raiseDebugMessageEvent(debugstr.getString());
	}

	// run through the output bind values, sending them back
	for (uint16_t i=0; i<cont->getOutputBindCount(cursor); i++) {

		sqlrserverbindvar	*bv=&(cont->getOutputBinds(cursor)[i]);

		if (cont->logEnabled() || cont->notificationsEnabled()) {
			debugstr.clear();
			debugstr.append(i);
			debugstr.append(":");
		}

		if (cont->bindValueIsNull(bv->isnull)) {

			if (cont->logEnabled() ||
				cont->notificationsEnabled()) {
				debugstr.append("NULL");
			}

			clientsock->write((uint16_t)NULL_DATA);

		} else if (bv->type==SQLRSERVERBINDVARTYPE_BLOB) {

			if (cont->logEnabled() ||
				cont->notificationsEnabled()) {
				debugstr.append("BLOB:");
			}

			returnOutputBindBlob(cursor,i);

		} else if (bv->type==SQLRSERVERBINDVARTYPE_CLOB) {

			if (cont->logEnabled() ||
				cont->notificationsEnabled()) {
				debugstr.append("CLOB:");
			}

			returnOutputBindClob(cursor,i);

		} else if (bv->type==SQLRSERVERBINDVARTYPE_STRING) {

			if (cont->logEnabled() ||
				cont->notificationsEnabled()) {
				debugstr.append("STRING:");
				debugstr.append(bv->value.stringval);
			}

			clientsock->write((uint16_t)STRING_DATA);
			bv->valuesize=charstring::length(
						(char *)bv->value.stringval);
			clientsock->write(bv->valuesize);
			clientsock->write(bv->value.stringval,bv->valuesize);

		} else if (bv->type==SQLRSERVERBINDVARTYPE_INTEGER) {

			if (cont->logEnabled() ||
				cont->notificationsEnabled()) {
				debugstr.append("INTEGER:");
				debugstr.append(bv->value.integerval);
			}

			clientsock->write((uint16_t)INTEGER_DATA);
			clientsock->write((uint64_t)bv->value.integerval);

		} else if (bv->type==SQLRSERVERBINDVARTYPE_DOUBLE) {

			if (cont->logEnabled() ||
				cont->notificationsEnabled()) {
				debugstr.append("DOUBLE:");
				debugstr.append(bv->value.doubleval.value);
				debugstr.append("(");
				debugstr.append(bv->value.doubleval.precision);
				debugstr.append(",");
				debugstr.append(bv->value.doubleval.scale);
				debugstr.append(")");
			}

			clientsock->write((uint16_t)DOUBLE_DATA);
			clientsock->write(bv->value.doubleval.value);
			clientsock->write((uint32_t)bv->value.
						doubleval.precision);
			clientsock->write((uint32_t)bv->value.
						doubleval.scale);

		} else if (bv->type==SQLRSERVERBINDVARTYPE_DATE) {

			if (cont->logEnabled() ||
				cont->notificationsEnabled()) {
				debugstr.append("DATE:");
				debugstr.append(bv->value.dateval.year);
				debugstr.append("-");
				debugstr.append(bv->value.dateval.month);
				debugstr.append("-");
				debugstr.append(bv->value.dateval.day);
				debugstr.append(" ");
				if (bv->value.dateval.isnegative) {
					debugstr.append('-');
				}
				debugstr.append(bv->value.dateval.hour);
				debugstr.append(":");
				debugstr.append(bv->value.dateval.minute);
				debugstr.append(":");
				debugstr.append(bv->value.dateval.second);
				debugstr.append(":");
				debugstr.append(bv->value.dateval.microsecond);
				debugstr.append(" ");
				debugstr.append(bv->value.dateval.tz);
			}

			clientsock->write((uint16_t)DATE_DATA);
			clientsock->write((uint16_t)bv->value.dateval.year);
			clientsock->write((uint16_t)bv->value.dateval.month);
			clientsock->write((uint16_t)bv->value.dateval.day);
			clientsock->write((uint16_t)bv->value.dateval.hour);
			clientsock->write((uint16_t)bv->value.dateval.minute);
			clientsock->write((uint16_t)bv->value.dateval.second);
			clientsock->write((uint32_t)bv->value.
							dateval.microsecond);
			uint16_t	length=charstring::length(
							bv->value.dateval.tz);
			clientsock->write(length);
			clientsock->write(bv->value.dateval.tz,length);
			clientsock->write((bool)bv->value.dateval.isnegative);

		} else if (bv->type==SQLRSERVERBINDVARTYPE_CURSOR) {

			if (cont->logEnabled() ||
				cont->notificationsEnabled()) {
				debugstr.append("CURSOR:");
				debugstr.append(bv->value.cursorid);
			}

			clientsock->write((uint16_t)CURSOR_DATA);
			clientsock->write(bv->value.cursorid);
		}

		if (cont->logEnabled() || cont->notificationsEnabled()) {
			cont->raiseDebugMessageEvent(debugstr.getString());
		}
	}

	// terminate the bind vars
	clientsock->write((uint16_t)END_BIND_VARS);

	cont->raiseDebugMessageEvent("done returning output bind values");
}

void sqlrprotocol_sqlrclient::returnOutputBindBlob(sqlrservercursor *cursor,
							uint16_t index) {
	debugFunction();
	sendLobOutputBind(cursor,index);
	cont->closeLobOutputBind(cursor,index);
}

void sqlrprotocol_sqlrclient::returnOutputBindClob(sqlrservercursor *cursor,
							uint16_t index) {
	debugFunction();
	sendLobOutputBind(cursor,index);
	cont->closeLobOutputBind(cursor,index);
}

#define MAX_BYTES_PER_CHAR	4

void sqlrprotocol_sqlrclient::sendLobOutputBind(sqlrservercursor *cursor,
							uint16_t index) {
	debugFunction();

	// Get lob length.  If this fails, send a NULL field.
	uint64_t	loblength;
	if (!cont->getLobOutputBindLength(cursor,index,&loblength)) {
		sendNullField();
		return;
	}

	// for lobs of 0 length
	if (!loblength) {
		startSendingLong(0);
		sendLongSegment("",0);
		endSendingLong();
		return;
	}

	// initialize sizes and status
	uint64_t	charstoread=sizeof(lobbuffer)/MAX_BYTES_PER_CHAR;
	uint64_t	charsread=0;
	uint64_t	offset=0;
	bool		start=true;

	for (;;) {

		// read a segment from the lob
		if (!cont->getLobOutputBindSegment(cursor,index,
					lobbuffer,sizeof(lobbuffer),
					offset,charstoread,&charsread) ||
					!charsread) {

			// if we fail to get a segment or got nothing...
			// if we haven't started sending yet, then send a NULL,
			// otherwise just end normally
			if (start) {
				sendNullField();
			} else {
				endSendingLong();
			}
			return;

		} else {

			// if we haven't started sending yet, then do that now
			if (start) {
				startSendingLong(loblength);
				start=false;
			}

			// send the segment we just got
			sendLongSegment(lobbuffer,charsread);

			// FIXME: or should this be charsread?
			offset=offset+charstoread;
		}
	}
}

void sqlrprotocol_sqlrclient::returnInputOutputBindValues(
						sqlrservercursor *cursor) {
	debugFunction();

	if (protocolversion<2) {
		cont->raiseDebugMessageEvent("not returning input/output binds "
						"(client protocol too old)");
		return;
	}

	if (cont->logEnabled() || cont->notificationsEnabled()) {
		debugstr.clear();
		debugstr.append("returning ");
		debugstr.append(cont->getInputOutputBindCount(cursor));
		debugstr.append(" input/output bind values: ");
		cont->raiseDebugMessageEvent(debugstr.getString());
	}

	// run through the input/output bind values, sending them back
	for (uint16_t i=0; i<cont->getInputOutputBindCount(cursor); i++) {

		sqlrserverbindvar	*bv=
				&(cont->getInputOutputBinds(cursor)[i]);

		if (cont->logEnabled() || cont->notificationsEnabled()) {
			debugstr.clear();
			debugstr.append(i);
			debugstr.append(":");
		}

		if (cont->bindValueIsNull(bv->isnull)) {

			if (cont->logEnabled() ||
				cont->notificationsEnabled()) {
				debugstr.append("NULL");
			}

			clientsock->write((uint16_t)NULL_DATA);

		} else /*if (bv->type==SQLRSERVERBINDVARTYPE_BLOB) {

			if (cont->logEnabled() ||
				cont->notificationsEnabled()) {
				debugstr.append("BLOB:");
			}

			returnInputOutputBindBlob(cursor,i);

		} else if (bv->type==SQLRSERVERBINDVARTYPE_CLOB) {

			if (cont->logEnabled() ||
				cont->notificationsEnabled()) {
				debugstr.append("CLOB:");
			}

			returnInputOutputBindClob(cursor,i);

		} else*/ if (bv->type==SQLRSERVERBINDVARTYPE_STRING) {

			if (cont->logEnabled() ||
				cont->notificationsEnabled()) {
				debugstr.append("STRING:");
				debugstr.append(bv->value.stringval);
			}

			clientsock->write((uint16_t)STRING_DATA);
			bv->valuesize=charstring::length(
						(char *)bv->value.stringval);
			clientsock->write(bv->valuesize);
			clientsock->write(bv->value.stringval,bv->valuesize);

		} else if (bv->type==SQLRSERVERBINDVARTYPE_INTEGER) {

			if (cont->logEnabled() ||
				cont->notificationsEnabled()) {
				debugstr.append("INTEGER:");
				debugstr.append(bv->value.integerval);
			}

			clientsock->write((uint16_t)INTEGER_DATA);
			clientsock->write((uint64_t)bv->value.integerval);

		} else if (bv->type==SQLRSERVERBINDVARTYPE_DOUBLE) {

			if (cont->logEnabled() ||
				cont->notificationsEnabled()) {
				debugstr.append("DOUBLE:");
				debugstr.append(bv->value.doubleval.value);
				debugstr.append("(");
				debugstr.append(bv->value.doubleval.precision);
				debugstr.append(",");
				debugstr.append(bv->value.doubleval.scale);
				debugstr.append(")");
			}

			clientsock->write((uint16_t)DOUBLE_DATA);
			clientsock->write(bv->value.doubleval.value);
			clientsock->write((uint32_t)bv->value.
						doubleval.precision);
			clientsock->write((uint32_t)bv->value.
						doubleval.scale);

		} else if (bv->type==SQLRSERVERBINDVARTYPE_DATE) {

			if (cont->logEnabled() ||
				cont->notificationsEnabled()) {
				debugstr.append("DATE:");
				debugstr.append(bv->value.dateval.year);
				debugstr.append("-");
				debugstr.append(bv->value.dateval.month);
				debugstr.append("-");
				debugstr.append(bv->value.dateval.day);
				debugstr.append(" ");
				if (bv->value.dateval.isnegative) {
					debugstr.append('-');
				}
				debugstr.append(bv->value.dateval.hour);
				debugstr.append(":");
				debugstr.append(bv->value.dateval.minute);
				debugstr.append(":");
				debugstr.append(bv->value.dateval.second);
				debugstr.append(":");
				debugstr.append(bv->value.dateval.microsecond);
				debugstr.append(" ");
				debugstr.append(bv->value.dateval.tz);
			}

			clientsock->write((uint16_t)DATE_DATA);
			clientsock->write((uint16_t)bv->value.dateval.year);
			clientsock->write((uint16_t)bv->value.dateval.month);
			clientsock->write((uint16_t)bv->value.dateval.day);
			clientsock->write((uint16_t)bv->value.dateval.hour);
			clientsock->write((uint16_t)bv->value.dateval.minute);
			clientsock->write((uint16_t)bv->value.dateval.second);
			clientsock->write((uint32_t)bv->value.
							dateval.microsecond);
			uint16_t	length=charstring::length(
							bv->value.dateval.tz);
			clientsock->write(length);
			clientsock->write(bv->value.dateval.tz,length);
			clientsock->write((bool)bv->value.dateval.isnegative);

		}

		if (cont->logEnabled() || cont->notificationsEnabled()) {
			cont->raiseDebugMessageEvent(debugstr.getString());
		}
	}

	// terminate the bind vars
	clientsock->write((uint16_t)END_BIND_VARS);

	cont->raiseDebugMessageEvent("done returning input/output bind values");
}

void sqlrprotocol_sqlrclient::sendColumnDefinition(
						const char *name,
						uint16_t namelen,
						uint16_t type, 
						uint32_t size,
						uint32_t precision,
						uint32_t scale,
						uint16_t nullable,
						uint16_t primarykey,
						uint16_t unique,
						uint16_t partofkey,
						uint16_t unsignednumber,
						uint16_t zerofill,
						uint16_t binary,
						uint16_t autoincrement,
						const char *table,
						uint16_t tablelen) {
	debugFunction();

	if (cont->logEnabled() || cont->notificationsEnabled()) {
		debugstr.clear();
		for (uint16_t i=0; i<namelen; i++) {
			debugstr.append(name[i]);
		}
		debugstr.append(":");
		debugstr.append(type);
		debugstr.append(":");
		debugstr.append(size);
		debugstr.append(" (");
		debugstr.append(precision);
		debugstr.append(",");
		debugstr.append(scale);
		debugstr.append(") ");
		if (!nullable) {
			debugstr.append("NOT NULL ");
		}
		if (primarykey) {
			debugstr.append("Primary key ");
		}
		if (unique) {
			debugstr.append("Unique");
		}
		cont->raiseDebugMessageEvent(debugstr.getString());
	}

	clientsock->write(namelen);
	clientsock->write(name,namelen);
	clientsock->write(type);
	clientsock->write(size);
	clientsock->write(precision);
	clientsock->write(scale);
	clientsock->write(nullable);
	clientsock->write(primarykey);
	clientsock->write(unique);
	clientsock->write(partofkey);
	clientsock->write(unsignednumber);
	clientsock->write(zerofill);
	clientsock->write(binary);
	clientsock->write(autoincrement);

	if (protocolversion<2) {
		return;
	}

	clientsock->write(tablelen);
	clientsock->write(table,tablelen);
}

void sqlrprotocol_sqlrclient::sendColumnDefinitionString(
						const char *name,
						uint16_t namelen,
						const char *type, 
						uint16_t typelen,
						uint32_t size,
						uint32_t precision,
						uint32_t scale,
						uint16_t nullable,
						uint16_t primarykey,
						uint16_t unique,
						uint16_t partofkey,
						uint16_t unsignednumber,
						uint16_t zerofill,
						uint16_t binary,
						uint16_t autoincrement,
						const char *table,
						uint16_t tablelen) {
	debugFunction();

	if (cont->logEnabled() || cont->notificationsEnabled()) {
		debugstr.clear();
		for (uint16_t ni=0; ni<namelen; ni++) {
			debugstr.append(name[ni]);
		}
		debugstr.append(":");
		for (uint16_t ti=0; ti<typelen; ti++) {
			debugstr.append(type[ti]);
		}
		debugstr.append(":");
		debugstr.append(size);
		debugstr.append(" (");
		debugstr.append(precision);
		debugstr.append(",");
		debugstr.append(scale);
		debugstr.append(") ");
		if (!nullable) {
			debugstr.append("NOT NULL ");
		}
		if (primarykey) {
			debugstr.append("Primary key ");
		}
		if (unique) {
			debugstr.append("Unique");
		}
		cont->raiseDebugMessageEvent(debugstr.getString());
	}

	clientsock->write(namelen);
	clientsock->write(name,namelen);
	clientsock->write(typelen);
	clientsock->write(type,typelen);
	clientsock->write(size);
	clientsock->write(precision);
	clientsock->write(scale);
	clientsock->write(nullable);
	clientsock->write(primarykey);
	clientsock->write(unique);
	clientsock->write(partofkey);
	clientsock->write(unsignednumber);
	clientsock->write(zerofill);
	clientsock->write(binary);
	clientsock->write(autoincrement);

	if (protocolversion<2) {
		return;
	}

	clientsock->write(tablelen);
	clientsock->write(table,tablelen);
}

bool sqlrprotocol_sqlrclient::returnResultSetData(sqlrservercursor *cursor,
						bool getskipandfetch,
						bool overridelazyfetch) {
	debugFunction();

	cont->raiseDebugMessageEvent("returning result set data...");

	// FIXME: push up?
	cont->setState(RETURN_RESULT_SET);

	// decide whether to use the cursor itself
	// or an attached custom query cursor
	// FIXME: push up?
	sqlrservercursor	*customcursor=cursor->getCustomQueryCursor();
	if (customcursor) {
		cursor=customcursor;
	}

	// get the number of rows to skip and fetch
	if (getskipandfetch) {
		if (!getSkipAndFetch(false,cursor)) {
			return false;
		}
	}

	// reinit cursor state (in case it was suspended)
	// FIXME: push up?
	cont->setState(cursor,SQLRCURSORSTATE_BUSY);

	if (!lazyfetch || overridelazyfetch) {

		bool	error=false;

		// for some queries, there are no rows to return, 
		if (cont->noRowsToReturn(cursor)) {
			clientsock->write(endresultset);
			clientsock->flushWriteBuffer(-1,-1);
			cont->raiseDebugMessageEvent(
				"done returning result set data");
			return true;
		}

		// skip the specified number of rows
		if (!cont->skipRows(cursor,skip,&error)) {
			if (error) {
				returnFetchError(cursor);
			} else {
				clientsock->write(endresultset);
				cont->raiseDebugMessageEvent(
					"done returning result set data");
			}
			clientsock->flushWriteBuffer(-1,-1);
			return true;
		}

		if (cont->logEnabled() || cont->notificationsEnabled()) {
			debugstr.clear();
			debugstr.append("fetching ");
			debugstr.append(fetch);
			debugstr.append(" rows...");
			cont->raiseDebugMessageEvent(debugstr.getString());
		}

		// send the specified number of rows back
		for (uint64_t i=0; (!fetch || i<fetch); i++) {
			if (cont->fetchRow(cursor,&error)) {
				returnRow(cursor);
				// FIXME: kludgy
				cont->nextRow(cursor);
			} else {
				if (error) {
					returnFetchError(cursor);
				} else {
					clientsock->write(endresultset);
				}
				break;
			}
		}
	}
	clientsock->flushWriteBuffer(-1,-1);

	cont->raiseDebugMessageEvent("done returning result set data");
	return true;
}

void sqlrprotocol_sqlrclient::returnFetchError(sqlrservercursor *cursor) {

	clientsock->write((uint16_t)FETCH_ERROR);

	cont->raiseDebugMessageEvent("returning error...");

	// FIXME: this is a little kludgy, ideally we'd just call returnError()
	// but it has some side effects

	// get the error
	const char	*errorstring;
	uint32_t	errorlength;
	int64_t		errnum;
	bool		liveconnection;
	cont->errorMessage(cursor,&errorstring,&errorlength,
					&errnum,&liveconnection);

	// send the error status
	if (!liveconnection) {
		clientsock->write((uint16_t)ERROR_OCCURRED_DISCONNECT);
	} else {
		clientsock->write((uint16_t)ERROR_OCCURRED);
	}

	// send the error code
	clientsock->write((uint64_t)errnum);

	// send the error string
	clientsock->write((uint16_t)errorlength);
	clientsock->write(errorstring,errorlength);

	cont->raiseDebugMessageEvent("done returning error");
}

void sqlrprotocol_sqlrclient::returnRow(sqlrservercursor *cursor) {
	debugFunction();

	if (cont->logEnabled() || cont->notificationsEnabled()) {
		debugstr.clear();
	}

	// send fields
	uint32_t	colcount=cont->colCount(cursor);
	for (uint32_t i=0; i<colcount; i++) {

		const char	*field=NULL;
		uint64_t	fieldlength=0;
		bool		blob=false;
		bool		null=false;
		if (!cont->getField(cursor,i,&field,&fieldlength,&blob,&null)) {
			// FIXME: handle error
		}

		// send data to the client
		if (null) {
			sendNullField();
		} else if (blob) {
			sendLobField(cursor,i);
		} else {
			sendField(field,fieldlength);
		}
	}

	if (cont->logEnabled() || cont->notificationsEnabled()) {
		cont->raiseDebugMessageEvent(debugstr.getString());
	}
}

void sqlrprotocol_sqlrclient::sendField(const char *data, uint32_t size) {
	debugFunction();

	if (cont->logEnabled() || cont->notificationsEnabled()) {
		debugstr.append("\"");
		debugstr.append(data,size);
		debugstr.append("\",");
	}

	clientsock->write((uint16_t)STRING_DATA);
	clientsock->write(size);
	clientsock->write(data,size);
}

void sqlrprotocol_sqlrclient::sendNullField() {
	debugFunction();

	if (cont->logEnabled() || cont->notificationsEnabled()) {
		debugstr.append("NULL,");
	}
	clientsock->write((uint16_t)NULL_DATA);
}

#define MAX_BYTES_PER_CHAR	4

void sqlrprotocol_sqlrclient::sendLobField(sqlrservercursor *cursor,
							uint32_t col) {
	debugFunction();

	// Get lob length.  If this fails, send a NULL field.
	uint64_t	loblength;
	if (!cont->getLobFieldLength(cursor,col,&loblength)) {
		sendNullField();
		cont->closeLobField(cursor,col);
		return;
	}

	// for lobs of 0 length
	if (!loblength) {
		startSendingLong(0);
		sendLongSegment("",0);
		endSendingLong();
		cont->closeLobField(cursor,col);
		return;
	}

	// initialize sizes and status
	uint64_t	charstoread=sizeof(lobbuffer)/MAX_BYTES_PER_CHAR;
	uint64_t	charsread=0;
	uint64_t	offset=0;
	bool		start=true;

	for (;;) {

		// read a segment from the lob
		if (!cont->getLobFieldSegment(cursor,col,
					lobbuffer,sizeof(lobbuffer),
					offset,charstoread,&charsread) ||
					!charsread) {

			// if we fail to get a segment or got nothing...
			// if we haven't started sending yet, then send a NULL,
			// otherwise just end normally
			if (start) {
				sendNullField();
			} else {
				endSendingLong();
			}
			cont->closeLobField(cursor,col);
			return;

		} else {

			// if we haven't started sending yet, then do that now
			if (start) {
				startSendingLong(loblength);
				start=false;
			}

			// send the segment we just got
			sendLongSegment(lobbuffer,charsread);

			// FIXME: or should this be charsread?
			offset=offset+charstoread;
		}
	}
}

void sqlrprotocol_sqlrclient::startSendingLong(uint64_t longlength) {
	debugFunction();
	clientsock->write((uint16_t)START_LONG_DATA);
	clientsock->write(longlength);
}

void sqlrprotocol_sqlrclient::sendLongSegment(const char *data, uint32_t size) {
	debugFunction();

	if (cont->logEnabled() || cont->notificationsEnabled()) {
		debugstr.append(data,size);
	}

	clientsock->write((uint16_t)STRING_DATA);
	clientsock->write(size);
	clientsock->write(data,size);
}

void sqlrprotocol_sqlrclient::endSendingLong() {
	debugFunction();

	if (cont->logEnabled() || cont->notificationsEnabled()) {
		debugstr.append(",");
	}

	clientsock->write((uint16_t)END_LONG_DATA);
}

void sqlrprotocol_sqlrclient::returnError(bool forcedisconnect) {
	debugFunction();

	cont->raiseDebugMessageEvent("returning error...");

	const char	*errorstring;
	uint32_t	errorlength;
	int64_t		errnum;
	bool		liveconnection;
	cont->errorMessage(&errorstring,&errorlength,&errnum,&liveconnection);

	// send the appropriate error status
	if (forcedisconnect || !liveconnection) {
		clientsock->write((uint16_t)ERROR_OCCURRED_DISCONNECT);
	} else {
		clientsock->write((uint16_t)ERROR_OCCURRED);
	}

	// send the error code and error string
	clientsock->write((uint64_t)errnum);

	// send the error string
	clientsock->write((uint16_t)errorlength);
	clientsock->write(errorstring,errorlength);
	clientsock->flushWriteBuffer(-1,-1);

	cont->raiseDebugMessageEvent("done returning error");

	cont->raiseDbErrorEvent(NULL,errorstring);
}

void sqlrprotocol_sqlrclient::returnError(sqlrservercursor *cursor,
						bool forcedisconnect) {
	debugFunction();

	cont->raiseDebugMessageEvent("returning error...");

	const char	*errorstring;
	uint32_t	errorlength;
	int64_t		errnum;
	bool		liveconnection;
	cont->errorMessage(cursor,&errorstring,&errorlength,
					&errnum,&liveconnection);

	// send the appropriate error status
	if (forcedisconnect || !liveconnection) {
		clientsock->write((uint16_t)ERROR_OCCURRED_DISCONNECT);
	} else {
		clientsock->write((uint16_t)ERROR_OCCURRED);
	}

	// send the error code
	clientsock->write((uint64_t)errnum);

	// send the error string
	clientsock->write((uint16_t)errorlength);
	clientsock->write(errorstring,errorlength);

	// client will be sending skip/fetch, better get
	// it even though we're not going to use it
	uint64_t	skipfetch;
	clientsock->read(&skipfetch,idleclienttimeout,0);
	clientsock->read(&skipfetch,idleclienttimeout,0);

	// Even though there was an error, we still 
	// need to send the client the id of the 
	// cursor that it's going to use.
	clientsock->write(cont->getId(cursor));
	clientsock->flushWriteBuffer(-1,-1);

	cont->raiseDebugMessageEvent("done returning error");

	cont->raiseDbErrorEvent(cursor,errorstring);
}

bool sqlrprotocol_sqlrclient::fetchResultSetCommand(
					sqlrservercursor *cursor) {
	debugFunction();
	cont->raiseDebugMessageEvent("fetching result set...");
	bool	retval=returnResultSetData(cursor,true,true);
	cont->raiseDebugMessageEvent("done fetching result set");
	return retval;
}

void sqlrprotocol_sqlrclient::abortResultSetCommand(
					sqlrservercursor *cursor) {
	debugFunction();
	cont->raiseDebugMessageEvent("aborting result set...");
	cont->abort(cursor);
	cont->raiseDebugMessageEvent("done aborting result set");
}

void sqlrprotocol_sqlrclient::suspendResultSetCommand(
					sqlrservercursor *cursor) {
	debugFunction();
	cont->raiseDebugMessageEvent("suspend result set...");
	cont->suspendResultSet(cursor);
	cont->raiseDebugMessageEvent("done suspending result set");
}

bool sqlrprotocol_sqlrclient::resumeResultSetCommand(
					sqlrservercursor *cursor) {
	debugFunction();
	cont->raiseDebugMessageEvent("resume result set...");

	bool	retval=true;

	if (cont->getState(cursor)==SQLRCURSORSTATE_SUSPENDED) {

		cont->raiseDebugMessageEvent(
				"previous result set was suspended");

		// indicate that no error has occurred
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);

		// send the client the id of the 
		// cursor that it's going to use
		clientsock->write(cont->getId(cursor));
		clientsock->write((uint16_t)SUSPENDED_RESULT_SET);

		// if the requested cursor really had a suspended
		// result set, send the index of the last row that
		// was fetched to the client then resume the result set
		// (FIXME: 0 will be sent if no rows were fetched or if
		// only the first row was fetched. This probably isn't
		// correct.)
		uint64_t	totalrowsfetched=
				cont->getTotalRowsFetched(cursor);
		clientsock->write((totalrowsfetched)?totalrowsfetched-1:0);

		returnResultSetHeader(cursor);
		retval=returnResultSetData(cursor,true,false);

	} else {

		cont->raiseDebugMessageEvent(
				"previous result set was not suspended");

		// indicate that an error has occurred
		clientsock->write((uint16_t)ERROR_OCCURRED);

		// send the error code (zero for now)
		clientsock->write((uint64_t)SQLR_ERROR_RESULTSETNOTSUSPENDED);

		// send the error itself
		uint16_t	len=charstring::length(
				SQLR_ERROR_RESULTSETNOTSUSPENDED_STRING);
		clientsock->write(len);
		clientsock->write(SQLR_ERROR_RESULTSETNOTSUSPENDED_STRING,len);

		retval=false;
	}

	cont->raiseDebugMessageEvent("done resuming result set");
	return retval;
}

bool sqlrprotocol_sqlrclient::getDatabaseListCommand(
					sqlrservercursor *cursor) {
	debugFunction();
	cont->raiseDebugMessageEvent("get db list...");
	bool	retval=getListCommand(cursor,
				SQLRCLIENTQUERYTYPE_DATABASE_LIST,false);
	cont->raiseDebugMessageEvent("done getting db list");
	return retval;
}

bool sqlrprotocol_sqlrclient::getSchemaListCommand(
					sqlrservercursor *cursor) {
	debugFunction();
	cont->raiseDebugMessageEvent("get schema list...");
	bool	retval=getListCommand(cursor,
				SQLRCLIENTQUERYTYPE_SCHEMA_LIST,false);
	cont->raiseDebugMessageEvent("done getting schema list");
	return retval;
}

bool sqlrprotocol_sqlrclient::getTableListCommand(
					sqlrservercursor *cursor) {
	debugFunction();
	cont->raiseDebugMessageEvent("get table list...");
	bool	retval=getListCommand(cursor,
				SQLRCLIENTQUERYTYPE_TABLE_LIST,false);
	cont->raiseDebugMessageEvent("done getting table list");
	return retval;
}

bool sqlrprotocol_sqlrclient::getTableList2Command(
					sqlrservercursor *cursor) {
	debugFunction();
	cont->raiseDebugMessageEvent("get table list...");
	bool	retval=getListCommand(cursor,
				SQLRCLIENTQUERYTYPE_TABLE_LIST_2,false);
	cont->raiseDebugMessageEvent("done getting table list");
	return retval;
}

bool sqlrprotocol_sqlrclient::getTableTypeListCommand(
					sqlrservercursor *cursor) {
	debugFunction();
	cont->raiseDebugMessageEvent("get table type list...");
	bool	retval=getListCommand(cursor,
				SQLRCLIENTQUERYTYPE_TABLE_TYPE_LIST,false);
	cont->raiseDebugMessageEvent("done getting table type list");
	return retval;
}

bool sqlrprotocol_sqlrclient::getColumnListCommand(
					sqlrservercursor *cursor) {
	debugFunction();
	cont->raiseDebugMessageEvent("get column list...");
	bool	retval=getListCommand(cursor,
				SQLRCLIENTQUERYTYPE_COLUMN_LIST,true);
	cont->raiseDebugMessageEvent("done getting column list");
	return retval;
}

bool sqlrprotocol_sqlrclient::getPrimaryKeyListCommand(
					sqlrservercursor *cursor) {
	debugFunction();
	cont->raiseDebugMessageEvent("get primary key list...");
	bool	retval=getListCommand(cursor,
				SQLRCLIENTQUERYTYPE_PRIMARY_KEY_LIST,true);
	cont->raiseDebugMessageEvent("done getting primary key list");
	return retval;
}

bool sqlrprotocol_sqlrclient::getKeyAndIndexListCommand(
					sqlrservercursor *cursor) {
	debugFunction();
	cont->raiseDebugMessageEvent("get key and index list...");
	bool	retval=getListCommand(cursor,
				SQLRCLIENTQUERYTYPE_KEY_AND_INDEX_LIST,true);
	cont->raiseDebugMessageEvent("done getting key and index list");
	return retval;
}

bool sqlrprotocol_sqlrclient::getProcedureBindAndColumnListCommand(
						sqlrservercursor *cursor) {
	debugFunction();
	cont->raiseDebugMessageEvent("get procedure bind and column list...");
	bool	retval=getListCommand(cursor,
		SQLRCLIENTQUERYTYPE_PROCEDURE_BIND_AND_COLUMN_LIST,true);
	cont->raiseDebugMessageEvent("done getting procedure "
					"bind and column list");
	return retval;
}

bool sqlrprotocol_sqlrclient::getTypeInfoListCommand(
					sqlrservercursor *cursor) {
	debugFunction();
	cont->raiseDebugMessageEvent("get type info list...");
	bool	retval=getListCommand(cursor,
				SQLRCLIENTQUERYTYPE_TYPE_INFO_LIST,true);
	cont->raiseDebugMessageEvent("done getting type info list");
	return retval;
}

bool sqlrprotocol_sqlrclient::getProcedureListCommand(
					sqlrservercursor *cursor) {
	debugFunction();
	cont->raiseDebugMessageEvent("get procedure list...");
	bool	retval=getListCommand(cursor,
				SQLRCLIENTQUERYTYPE_PROCEDURE_LIST,false);
	cont->raiseDebugMessageEvent("done getting procedure list");
	return retval;
}


bool sqlrprotocol_sqlrclient::getListCommand(sqlrservercursor *cursor,
					sqlrclientquerytype_t querytype,
					bool getobject) {
	debugFunction();

	// if we're using a custom cursor then close it
	// FIXME: push up?
	sqlrservercursor	*customcursor=cursor->getCustomQueryCursor();
	if (customcursor) {
		customcursor->close();
		cursor->clearCustomQueryCursor();
	}

	// get the list format
	uint16_t	listformat;
	ssize_t		result=clientsock->read(&listformat,
						idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->raiseClientProtocolErrorEvent(cursor,
				"get list failed: "
				"failed to get list format",result);
		return false;
	}
	
	// get length of wild parameter
	uint32_t	wildlen;
	result=clientsock->read(&wildlen,idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->raiseClientProtocolErrorEvent(cursor,
				"get list failed: "
				"failed to get wild length",result);
		return false;
	}

	// bounds checking
	if (wildlen>maxquerysize) {
		debugstr.clear();
		debugstr.append("get list failed: wild length too large: ");
		debugstr.append(wildlen);
		cont->raiseClientProtocolErrorEvent(
					cursor,debugstr.getString(),1);
		return false;
	}

	// read the wild parameter into the buffer
	char	*wild=new char[wildlen+1];
	if (wildlen) {
		result=clientsock->read(wild,wildlen,idleclienttimeout,0);
		if ((uint32_t)result!=wildlen) {
			cont->raiseClientProtocolErrorEvent(cursor,
					"get list failed: "
					"failed to get wild parameter",result);
			return false;
		}
	}
	wild[wildlen]='\0';

	// read the object parameter into the buffer
	char	*object=NULL;
	if (getobject) {

		// get length of object parameter
		uint32_t	objectlen;
		result=clientsock->read(&objectlen,idleclienttimeout,0);
		if (result!=sizeof(uint32_t)) {
			cont->raiseClientProtocolErrorEvent(cursor,
					"get list failed: "
					"failed to get object length",result);
			return false;
		}

		// bounds checking
		if (objectlen>maxquerysize) {
			debugstr.clear();
			debugstr.append("get list failed: "
					"object length too large: ");
			debugstr.append(objectlen);
			cont->raiseClientProtocolErrorEvent(
					cursor,debugstr.getString(),1);
			return false;
		}

		// read the object parameter into the buffer
		object=new char[objectlen+1];
		if (objectlen) {
			result=clientsock->read(object,objectlen,
						idleclienttimeout,0);
			if ((uint32_t)result!=objectlen) {
				cont->raiseClientProtocolErrorEvent(cursor,
					"get list failed: "
					"failed to get object parameter",
					result);
				return false;
			}
		}
		object[objectlen]='\0';

		// some apps aren't well behaved, trim spaces off of both sides
		charstring::bothTrim(object);

		// translate object name, if necessary
		const char	*newobject=cont->translateTableName(object);
		if (newobject) {
			delete[] object;
			object=charstring::duplicate(newobject);
		}
	}

	// read the object types
	uint16_t	objecttypes=0;
	if (querytype==SQLRCLIENTQUERYTYPE_TABLE_LIST_2) {

		result=clientsock->read(&objecttypes,idleclienttimeout,0);
		if (result!=sizeof(uint16_t)) {
			cont->raiseClientProtocolErrorEvent(cursor,
					"get list failed: "
					"failed to get object types",
					result);
			return false;
		}
	}

	// set the values that we won't get from the client
	cont->setInputBindCount(cursor,0);
	cont->setOutputBindCount(cursor,0);
	cont->setInputOutputBindCount(cursor,0);
	cont->setSendColumnInfo(SEND_COLUMN_INFO);

	// get the list and return it
	bool	retval=true;
	if (cont->getListsByApiCalls()) {
		retval=getListByApiCall(cursor,querytype,object,wild,
					(sqlrserverlistformat_t)listformat,
					objecttypes);
	} else {
		retval=getListByQuery(cursor,querytype,object,wild,
					(sqlrserverlistformat_t)listformat,
					objecttypes);
	}

	// clean up
	delete[] wild;
	delete[] object;

	return retval;
}

bool sqlrprotocol_sqlrclient::getListByApiCall(sqlrservercursor *cursor,
					sqlrclientquerytype_t querytype,
					const char *object,
					const char *wild,
					sqlrserverlistformat_t listformat,
					uint16_t objecttypes) {
	debugFunction();

	// initialize flags andbuffers
	bool	success=false;

	// get the appropriate list
	switch (querytype) {
		case SQLRCLIENTQUERYTYPE_DATABASE_LIST:
			cont->setDatabaseListColumnMap(listformat);
			success=cont->getDatabaseList(cursor,wild);
			break;
		case SQLRCLIENTQUERYTYPE_SCHEMA_LIST:
			cont->setSchemaListColumnMap(listformat);
			success=cont->getSchemaList(cursor,wild);
			break;
		case SQLRCLIENTQUERYTYPE_TABLE_LIST:
		case SQLRCLIENTQUERYTYPE_TABLE_LIST_2:
			cont->setTableListColumnMap(listformat);
			success=cont->getTableList(cursor,wild,objecttypes);
			break;
		case SQLRCLIENTQUERYTYPE_TABLE_TYPE_LIST:
			cont->setTableTypeListColumnMap(listformat);
			success=cont->getTableTypeList(cursor,wild);
			break;
		case SQLRCLIENTQUERYTYPE_COLUMN_LIST:
			cont->setColumnListColumnMap(listformat);
			success=cont->getColumnList(cursor,object,wild);
			break;
		case SQLRCLIENTQUERYTYPE_PRIMARY_KEY_LIST:
			cont->setPrimaryKeyListColumnMap(listformat);
			success=cont->getPrimaryKeyList(cursor,object,wild);
			break;
		case SQLRCLIENTQUERYTYPE_KEY_AND_INDEX_LIST:
			cont->setKeyAndIndexListColumnMap(listformat);
			success=cont->getKeyAndIndexList(cursor,object,wild);
			break;
		case SQLRCLIENTQUERYTYPE_PROCEDURE_BIND_AND_COLUMN_LIST:
			cont->setProcedureBindAndColumnListColumnMap(
								listformat);
			success=cont->getProcedureBindAndColumnList(
							cursor,object,wild);
			break;
		case SQLRCLIENTQUERYTYPE_TYPE_INFO_LIST:
			cont->setTypeInfoListColumnMap(listformat);
			success=cont->getTypeInfoList(cursor,object,wild);
			break;
		case SQLRCLIENTQUERYTYPE_PROCEDURE_LIST:
			cont->setProcedureListColumnMap(listformat);
			success=cont->getProcedureList(cursor,wild);
		default:
			break;
	}

	if (success) {
		success=getSkipAndFetch(true,cursor);
	}

	// if an error occurred...
	if (!success) {
		returnError(cursor,false);

		// this is actually OK, only return false on a network error
		return true;
	}

	// indicate that no error has occurred
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);

	// send the client the id of the 
	// cursor that it's going to use
	clientsock->write(cont->getId(cursor));

	// tell the client that this is not a
	// suspended result set
	clientsock->write((uint16_t)NO_SUSPENDED_RESULT_SET);

	// if the query processed ok then send a result set header and return...
	returnResultSetHeader(cursor);
	if (!returnResultSetData(cursor,false,false)) {
		return false;
	}
	return true;
}

bool sqlrprotocol_sqlrclient::getListByQuery(sqlrservercursor *cursor,
					sqlrclientquerytype_t querytype,
					const char *object,
					const char *wild,
					sqlrserverlistformat_t listformat,
					uint16_t objecttypes) {
	debugFunction();

	// build the appropriate query
	const char	*query=NULL;
	bool		havewild=charstring::length(wild);
	switch (querytype) {
		case SQLRCLIENTQUERYTYPE_DATABASE_LIST:
			query=cont->getDatabaseListQuery(havewild);
			break;
		case SQLRCLIENTQUERYTYPE_SCHEMA_LIST:
			query=cont->getSchemaListQuery(havewild);
			break;
		case SQLRCLIENTQUERYTYPE_TABLE_LIST:
		case SQLRCLIENTQUERYTYPE_TABLE_LIST_2:
			query=cont->getTableListQuery(havewild,objecttypes);
			break;
		case SQLRCLIENTQUERYTYPE_TABLE_TYPE_LIST:
			query=cont->getTableTypeListQuery(havewild);
			break;
		case SQLRCLIENTQUERYTYPE_COLUMN_LIST:
			query=cont->getColumnListQuery(object,havewild);
			break;
		case SQLRCLIENTQUERYTYPE_PRIMARY_KEY_LIST:
			query=cont->getPrimaryKeyListQuery(object,havewild);
			break;
		case SQLRCLIENTQUERYTYPE_KEY_AND_INDEX_LIST:
			query=cont->getKeyAndIndexListQuery(object,havewild);
			break;
		case SQLRCLIENTQUERYTYPE_PROCEDURE_BIND_AND_COLUMN_LIST:
			query=cont->getProcedureBindAndColumnListQuery(
							object,havewild);
			break;
		case SQLRCLIENTQUERYTYPE_TYPE_INFO_LIST:
			query=cont->getTypeInfoListQuery(object,havewild);
			break;
		case SQLRCLIENTQUERYTYPE_PROCEDURE_LIST:
			query=cont->getProcedureListQuery(havewild);
			break;
		default:
			break;
	}

	// FIXME: this can fail
	buildListQuery(cursor,query,wild,object);

	return processQueryOrBindCursor(cursor,querytype,
					listformat,false,false);
}

bool sqlrprotocol_sqlrclient::buildListQuery(sqlrservercursor *cursor,
						const char *query,
						const char *wild,
						const char *object) {
	debugFunction();

	// If the object was given like catalog.schema.object, then just
	// get the object.
	const char	*realobject=charstring::findLast(object,".");
	if (realobject) {
		realobject++;
	} else {
		realobject=object;
	}

	// clean up buffers to avoid SQL injection
	stringbuffer	wildbuf;
	escapeParameter(&wildbuf,wild);
	stringbuffer	objectbuf;
	escapeParameter(&objectbuf,realobject);

	// bounds checking
	cont->setQueryLength(cursor,charstring::length(query)+
					wildbuf.getStringLength()+
					objectbuf.getStringLength());
	if (cont->getQueryLength(cursor)>maxquerysize) {
		return false;
	}

	// fill the query buffer and update the length
	char	*querybuffer=cont->getQueryBuffer(cursor);
	if (objectbuf.getStringLength()) {
		charstring::printf(querybuffer,maxquerysize+1,
						query,objectbuf.getString(),
						wildbuf.getString());
	} else {
		charstring::printf(querybuffer,maxquerysize+1,
						query,wildbuf.getString());
	}
	cont->setQueryLength(cursor,charstring::length(querybuffer));
	return true;
}

void sqlrprotocol_sqlrclient::escapeParameter(stringbuffer *buffer,
						const char *parameter) {
	debugFunction();

	if (!parameter) {
		return;
	}

	// escape single quotes
	for (const char *ptr=parameter; *ptr; ptr++) {
		if (*ptr=='\'') {
			buffer->append('\'');
		}
		buffer->append(*ptr);
	}
}

bool sqlrprotocol_sqlrclient::getQueryTreeCommand(sqlrservercursor *cursor) {
	debugFunction();

	cont->raiseDebugMessageEvent("getting query tree");

	// get the tree as a string
	xmldom	*tree=cont->getQueryTree(cursor);
	domnode	*root=(tree)?tree->getRootNode():NULL;
	stringbuffer	xml;
	if (root) {
		root->write(&xml);
	}

	// send the tree
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	clientsock->write((uint64_t)xml.getStringLength());
	clientsock->write(xml.getString(),xml.getStringLength());
	clientsock->flushWriteBuffer(-1,-1);

	return true;
}

bool sqlrprotocol_sqlrclient::getTranslatedQueryCommand(
					sqlrservercursor *cursor) {
	debugFunction();

	cont->raiseDebugMessageEvent("getting translated query");

	// get the query
	const char	*query=cont->getTranslatedQuery(cursor);
	uint64_t	querylen=charstring::length(query);

	// send the tree
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	clientsock->write(querylen);
	clientsock->write(query,querylen);
	clientsock->flushWriteBuffer(-1,-1);

	return true;
}

gsscontext *sqlrprotocol_sqlrclient::getGSSContext() {
	return &gctx;
}

tlscontext *sqlrprotocol_sqlrclient::getTLSContext() {
	return &tctx;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrprotocol	*new_sqlrprotocol_sqlrclient(
						sqlrservercontroller *cont,
						sqlrprotocols *ps,
						domnode *parameters) {
		return new sqlrprotocol_sqlrclient(cont,ps,parameters);
	}
}
