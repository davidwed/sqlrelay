// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/bytestring.h>
#include <rudiments/character.h>
#include <rudiments/snooze.h>
#include <rudiments/regularexpression.h>

#include <datatypes.h>
#include <defines.h>
#include <config.h>

#include <sqlrelay/sqlrclient.h>

struct outputbindvar {
	const char	*variable;
	union {
		char		*stringvalue;
		int64_t		*intvalue;
		double		*doublevalue;
		struct {
			int16_t		*year;
			int16_t		*month;
			int16_t		*day;
			int16_t		*hour;
			int16_t		*minute;
			int16_t		*second;
			int32_t		*microsecond;
			const char	**tz;
			bool		*isnegative;
		} datevalue;
	} value;
	uint32_t		valuesize;
	sqlrserverbindvartype_t	type;
	int16_t			*isnull;
};

struct cursorbindvar {
	const char	*variable;
	sqlrservercursor	*cursor;
};

class routercursor;

class SQLRSERVER_DLLSPEC routerconnection : public sqlrserverconnection {
	friend class routercursor;
	public:
			routerconnection(sqlrservercontroller *cont);
			~routerconnection();
	private:
		bool		supportsAuthOnDatabase();
		void		handleConnectString();
		bool		logIn(const char **error, const char **warning);
		sqlrservercursor	*newCursor(uint16_t id);
		void		deleteCursor(sqlrservercursor *curs);
		void		logOut();
		bool		autoCommitOn();
		bool		autoCommitOff();
		bool		supportsAutoCommit();
		bool		begin();
		bool		commit();
		bool		rollback();
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
		const char	*identify();
		const char	*dbVersion();
		const char	*dbHostName();
		const char	*dbIpAddress();
		bool		cacheDbHostInfo();
		bool		getListsByApiCalls();
		bool		getDatabaseList(sqlrservercursor *cursor,
						const char *wild);
		bool		getTableList(sqlrservercursor *cursor,
						const char *wild,
						uint16_t objecttypes);
		bool		getColumnList(sqlrservercursor *cursor,
						const char *table,
						const char *wild);
		bool		ping();
		const char	*selectDatabaseQuery();
		char		*getCurrentDatabase();
		bool		getLastInsertId(uint64_t *id);
		void		endSession();

		void	route(bool *routed, bool *err);

		void	autoCommitOnFailed(uint16_t index);
		void	autoCommitOffFailed(uint16_t index);
		void	beginFailed(uint16_t index);
		void	commitFailed(uint16_t index);
		void	rollbackFailed(uint16_t index);
		void	beginQueryFailed(uint16_t index);
		void	raiseIntegrityViolationEvent(const char *command,
								uint16_t index);
		const char	*identity;

		const char	**conids;
		sqlrconnection	**cons;
		uint16_t	concount;
		const char	**beginquery;
		bool		anymustbegin;

		sqlrconnection	*currentcon;
		uint16_t	currentconindex;

		bool		justloggedin;

		int16_t		nullbindvalue;
		int16_t		nonnullbindvalue;

		sqlrrouters	*sqlrr;

		bool		routeentiresession;

		bool		debug;

		linkedlist< routercursor * >	routercursors;
};

class SQLRSERVER_DLLSPEC routercursor : public sqlrservercursor {
	friend class routerconnection;
	private:
				routercursor(sqlrserverconnection *conn,
								uint16_t id);
				~routercursor();
		bool		prepareQuery(const char *query,
						uint32_t length);
		void		route(bool *routed, bool *err);
		bool		supportsNativeBinds(const char *query,
							uint32_t length);
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value);
		bool		inputBind(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale);
		bool		inputBind(const char *variable,
						uint16_t variablesize,
						int64_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz,
						bool isnegative,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		bool		inputBindBlob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		bool		inputBindClob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		bool		outputBind(const char *variable, 
						uint16_t variablesize,
						char *value,
						uint32_t valuesize,
						int16_t *isnull);
		bool		outputBind(const char *variable, 
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);
		bool		outputBind(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);
		bool		outputBind(const char *variable,
						uint16_t variablesize,
						int16_t *year,
						int16_t *month,
						int16_t *day,
						int16_t *hour,
						int16_t *minute,
						int16_t *second,
						int32_t *microsecond,
						const char **tz,
						bool *isnegative,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		bool		outputBindBlob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		bool		outputBindClob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		bool		outputBindCursor(const char *variable,
						uint16_t variablesize,
						sqlrservercursor *cursor);
		bool		getLobOutputBindLength(uint16_t index,
						uint64_t *length);
		bool		getLobOutputBindSegment(uint16_t index,
						char *buffer,
						uint64_t buffersize,
						uint64_t offset,
						uint64_t charstoread,
						uint64_t *charsread);
		bool		executeQuery(const char *query,
						uint32_t length);
		void		errorMessage(char *errorbuffer,
						uint32_t errorbufferlength,
						uint32_t *errorlength,
						int64_t	*errorcode,
						bool *liveconnection);
		bool		knowsRowCount();
		uint64_t	rowCount();
		uint64_t	affectedRows();
		uint32_t	colCount();
		uint16_t	columnTypeFormat();
		const char	*getColumnName(uint32_t col);
		const char	*getColumnTypeName(uint32_t col);
		uint32_t	getColumnLength(uint32_t col);
		uint32_t	getColumnPrecision(uint32_t col);
		uint32_t	getColumnScale(uint32_t col);
		uint16_t	getColumnIsNullable(uint32_t col);
		uint16_t	getColumnIsPrimaryKey(uint32_t col);
		uint16_t	getColumnIsUnique(uint32_t col);
		uint16_t	getColumnIsPartOfKey(uint32_t col);
		uint16_t	getColumnIsUnsigned(uint32_t col);
		uint16_t	getColumnIsZeroFilled(uint32_t col);
		uint16_t	getColumnIsBinary(uint32_t col);
		uint16_t	getColumnIsAutoIncrement(uint32_t col);
		const char	*getColumnTable(uint32_t col);
		bool		noRowsToReturn();
		bool		fetchRow(bool *error);
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		closeResultSet();

		routerconnection	*routerconn;

		sqlrconnection	*currentcon;
		sqlrcursor	*currentcur;

		bool		isbindcur;

		sqlrcursor	**curs;

		uint64_t	nextrow;

		outputbindvar	*obv;
		uint16_t	obcount;

		cursorbindvar	*cbv;
		uint16_t	cbcount;

		bool		emptyquery;
};

routerconnection::routerconnection(sqlrservercontroller *cont) :
					sqlrserverconnection(cont) {
	identity=NULL;

	conids=NULL;
	cons=NULL;
	concount=0;
	currentcon=NULL;
	currentconindex=0;
	beginquery=NULL;
	anymustbegin=false;
	justloggedin=false;
	nullbindvalue=nullBindValue();
	nonnullbindvalue=nonNullBindValue();

	sqlrr=NULL;
	routeentiresession=false;

	debug=cont->getConfig()->getDebugRouters();
}

routerconnection::~routerconnection() {
	for (uint16_t index=0; index<concount; index++) {
		delete cons[index];
	}
	delete[] conids;
	delete[] cons;
	delete[] beginquery;
	routercursors.clear();
	delete sqlrr;
}

bool routerconnection::supportsAuthOnDatabase() {
	return false;
}

void routerconnection::handleConnectString() {

	identity=cont->getConnectStringValue("identity");

	// re-get fetchatonce, defaulting to 10, and allowing it to be set to 0
	uint32_t	fetchatonce=10;
	const char	*fao=cont->getConnectStringValue("fetchatonce");
	if (fao) {
		fetchatonce=charstring::toUnsignedInteger(fao);
	}
	cont->setFetchAtOnce(fetchatonce);

	cont->setMaxColumnCount(0);
	cont->setMaxFieldLength(0);


	// build the connections that we'll route to
	// (this is just a convenient place to do it)
	linkedlist< connectstringcontainer * >	*cslist=
				cont->getConfig()->getConnectStringList();
	concount=cslist->getLength();

	conids=new const char *[concount];
	cons=new sqlrconnection *[concount];
	beginquery=new const char *[concount];
	anymustbegin=false;

	uint16_t index=0;
	connectstringnode	*csln=cslist->getFirst();
	while (index<concount) {

		connectstringcontainer	*csc=csln->getValue();

		conids[index]=csc->getConnectionId();

		cons[index]=new sqlrconnection(
				csc->getConnectStringValue("server"),
				charstring::toUnsignedInteger(
					csc->getConnectStringValue("port")),
				csc->getConnectStringValue("socket"),
				csc->getConnectStringValue("user"),
				csc->getConnectStringValue("password"),
				0,1);

		const char	*id=cons[index]->identify();
		if (!charstring::compare(id,"sap") ||
				!charstring::compare(id,"sybase") ||
				!charstring::compare(id,"freetds")) {
			beginquery[index]="begin tran";
		} else if (!charstring::compare(id,"sqlite")) {
			beginquery[index]="begin transaction";
		} else if (!charstring::compare(id,"postgresql") ||
				!charstring::compare(id,"router")) {
			beginquery[index]="begin";
		} else {
			beginquery[index]=NULL;
		}

		if (beginquery[index]) {
			anymustbegin=true;
		}

		index++;
		csln=csln->getNext();
	}

	// load the router modules
	// (this is just a convenient place to do it)
	domnode	*routers=cont->getConfig()->getRouters();
	if (!routers->isNullNode()) {
		sqlrr=new sqlrrouters(cont,conids,cons,concount);
		sqlrr->load(routers);
		routeentiresession=sqlrr->routeEntireSession();
	}
}

bool routerconnection::logIn(const char **error, const char **warning) {

	justloggedin=true;

	while (!ping()) {
		snooze::macrosnooze(1);
	}
	endSession();
	return true;
}

sqlrservercursor *routerconnection::newCursor(uint16_t id) {
	return (sqlrservercursor *)new routercursor(
					(sqlrserverconnection *)this,id);
}

void routerconnection::deleteCursor(sqlrservercursor *curs) {
	delete (routercursor *)curs;
}

void routerconnection::logOut() {
}

bool routerconnection::autoCommitOn() {

	if (debug) {
		stdoutput.printf("autoCommitOn {\n");
	}

	if (justloggedin) {
		justloggedin=false;
	}

	// route
	bool	routed=false;
	bool	err=false;
	route(&routed,&err);
	if (err) {
		if (debug) {
			stdoutput.printf("	routing error\n}\n");
		}
		return false;
	}

	// if routing entire sessions, then just enable for
	// the appropriate connection
	if (routed && routeentiresession) {
		if (debug) {
			stdoutput.printf("	only executing on: %s\n}\n",
				(currentcon)?conids[currentconindex]:NULL);
		}
		return (currentcon)?currentcon->autoCommitOn():true;
	}

	// otherwise, turn autocommit on for all connections,
	// if any fail, return failure
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {

		if (debug) {
			stdoutput.printf("	executing on: %s\n",
							conids[index]);
		}

		bool	res=cons[index]->autoCommitOn();
		if (!res) {
			if (debug) {
				stdoutput.printf("	failed\n");
			}
			autoCommitOnFailed(index);
		}
		// The connection class calls autoCommitOn or autoCommitOff
		// immediately after logging in, which will cause the 
		// cons to connect to the relay's and tie them up unless we
		// call endSession.  We'd rather not tie them up until a
		// client connects, so if we just logged in, we'll call
		// endSession.
		if (justloggedin) {
			// if any of the connections must begin transactions,
			// then those connections will start off in auto-commit
			// mode no matter what, so put all connections in
			// autocommit mode
			// (this is a convenient place to do this...)
			if (anymustbegin) {
				cons[index]->autoCommitOn();
			}
			cons[index]->endSession();
		}
		if (result) {
			result=res;
		}
	}

	if (debug) {
		stdoutput.printf("}\n");
	}
	return result;
}

bool routerconnection::autoCommitOff() {

	if (debug) {
		stdoutput.printf("autoCommitOff {\n");
	}

	if (justloggedin) {
		justloggedin=false;
	}

	// route
	bool	routed=false;
	bool	err=false;
	route(&routed,&err);
	if (err) {
		if (debug) {
			stdoutput.printf("	routing error\n}\n");
		}
		return false;
	}

	// if routing entire sessions, then just disable for
	// the appropriate connection
	if (routed && routeentiresession) {
		if (debug) {
			stdoutput.printf("	only executing on: %s\n}\n",
				(currentcon)?conids[currentconindex]:NULL);
		}
		return (currentcon)?currentcon->autoCommitOff():true;
	}

	// otherwise, turn autocommit on for all connections,
	// if any fail, return failure
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {

		if (debug) {
			stdoutput.printf("	executing on: %s\n",
							conids[index]);
		}

		bool	res=cons[index]->autoCommitOff();
		if (!res) {
			if (debug) {
				stdoutput.printf("	failed\n");
			}
			autoCommitOffFailed(index);
		}
		// The connection class calls autoCommitOn or autoCommitOff
		// immediately after logging in, which will cause the 
		// cons to connect to the relay's and tie them up unless we
		// call endSession.  We'd rather not tie them up until a
		// client connects, so if we just logged in, we'll call
		// endSession.
		if (justloggedin) {
			// if any of the connections must begin transactions,
			// then those connections will start off in auto-commit
			// mode no matter what, so put all connections in
			// autocommit mode, even if autocommit-off is called
			// here
			// (this is a convenient place to do this...)
			if (anymustbegin) {
				cons[index]->autoCommitOn();
			}
			cons[index]->endSession();
		}
		if (result) {
			result=res;
		}
	}

	if (debug) {
		stdoutput.printf("}\n");
	}
	return result;
}

bool routerconnection::supportsAutoCommit() {
	return true;
}

bool routerconnection::begin() {

	if (debug) {
		stdoutput.printf("begin {\n");
	}

	// route
	bool	routed=false;
	bool	err=false;
	route(&routed,&err);
	if (err) {
		if (debug) {
			stdoutput.printf("	routing error\n}\n");
		}
		return false;
	}

	// if routing entire sessions, then just begin for
	// the appropriate connection
	if (routed && routeentiresession) {
		if (debug) {
			stdoutput.printf("	only executing on: %s\n}\n",
				(currentcon)?conids[currentconindex]:NULL);
		}
		return (currentcon)?currentcon->begin():true;
	}

	// otherwise, begin all connections, if any fail, return failure
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {

		if (debug) {
			stdoutput.printf("	executing on: %s\n",
							conids[index]);
		}

		bool	res=cons[index]->begin();
		if (!res) {
			if (debug) {
				stdoutput.printf("	failed\n");
			}
			beginFailed(index);
		}
		if (result) {
			result=res;
		}
	}

	if (debug) {
		stdoutput.printf("}\n");
	}
	return result;
}

bool routerconnection::commit() {

	if (debug) {
		stdoutput.printf("commit {\n");
	}

	// route
	bool	routed=false;
	bool	err=false;
	route(&routed,&err);
	if (err) {
		if (debug) {
			stdoutput.printf("	routing error\n}\n");
		}
		return false;
	}

	// if routing entire sessions, then just commit for
	// the appropriate connection
	if (routed && routeentiresession) {
		if (debug) {
			stdoutput.printf("	only executing on: %s\n}\n",
				(currentcon)?conids[currentconindex]:NULL);
		}
		return (currentcon)?currentcon->commit():true;
	}

	// otherwise, commit all connections, if any fail, return failure
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {

		if (debug) {
			stdoutput.printf("	executing on: %s\n",
							conids[index]);
		}

		bool	res=cons[index]->commit();
		if (!res) {
			if (debug) {
				stdoutput.printf("	failed\n");
			}
			commitFailed(index);
		}
		if (result) {
			result=res;
		}
	}

	if (debug) {
		stdoutput.printf("}\n");
	}
	return result;
}

bool routerconnection::rollback() {

	if (debug) {
		stdoutput.printf("rollback {\n");
	}

	// route
	bool	routed=false;
	bool	err=false;
	route(&routed,&err);
	if (err) {
		if (debug) {
			stdoutput.printf("	routing error\n}\n");
		}
		return false;
	}

	// if routing entire sessions, then just rollback for
	// the appropriate connection
	if (routed && routeentiresession) {
		if (debug) {
			stdoutput.printf("	only executing on: %s\n}\n",
				(currentcon)?conids[currentconindex]:NULL);
		}
		return (currentcon)?currentcon->rollback():true;
	}

	// otherwise, rollback all connections, if any fail, return failure
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {

		if (debug) {
			stdoutput.printf("	executing on: %s\n",
							conids[index]);
		}

		bool	res=cons[index]->rollback();
		if (!res) {
			if (debug) {
				stdoutput.printf("	failed\n");
			}
			rollbackFailed(index);
		}
		if (result) {
			result=res;
		}
	}

	if (debug) {
		stdoutput.printf("}\n");
	}
	return result;
}

void routerconnection::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {

	for (uint16_t index=0; index<concount; index++) {
		const char	*errormessage=cons[index]->errorMessage();
		if (!charstring::length(errormessage)) {
			*errorlength=charstring::length(errormessage);
			charstring::safeCopy(errorbuffer,errorbufferlength,
						errormessage,*errorlength);
			*errorcode=cons[index]->errorNumber();
			break;
		}
	}
	*liveconnection=true;
}

const char *routerconnection::identify() {
	// FIXME: maybe this should only return router if there's no currentcon
	return (identity)?identity:"router";
}

const char *routerconnection::dbVersion() {

	if (debug) {
		stdoutput.printf("dbVersion {\n");
	}

	// route
	bool	routed=false;
	bool	err=false;
	route(&routed,&err);
	if (err) {
		if (debug) {
			stdoutput.printf("	routing error\n}\n");
		}
		return NULL;
	}

	// if routing entire sessions, then get this for
	// the appropriate connection
	if (routeentiresession) {
		if (debug) {
			stdoutput.printf("	only executing on: %s\n}\n",
				(currentcon)?conids[currentconindex]:NULL);
		}
		return (currentcon)?currentcon->dbVersion():NULL;
	}

	// otherwise, try to find a usable connection
	if (!currentcon) {
		for (uint16_t index=0; !currentcon && index<concount; index++) {
			currentcon=cons[index];
			currentconindex=index;
		}
	}

	if (debug) {
		stdoutput.printf("	executing on: %s\n",
			(currentcon)?conids[currentconindex]:NULL);
	}

	const char	*retval=(currentcon)?currentcon->dbVersion():NULL;
	if (debug) {
		stdoutput.printf("	db version: %s\n}\n",retval);
	}
	return retval;
}

const char *routerconnection::dbHostName() {

	if (debug) {
		stdoutput.printf("dbHostName {\n");
	}

	// route
	bool	routed=false;
	bool	err=false;
	route(&routed,&err);
	if (err) {
		if (debug) {
			stdoutput.printf("	routing error\n}\n");
		}
		return NULL;
	}

	// if routing entire sessions, then get this for
	// the appropriate connection
	if (routeentiresession) {
		if (debug) {
			stdoutput.printf("	only executing on: %s\n}\n",
				(currentcon)?conids[currentconindex]:NULL);
		}
		return (currentcon)?currentcon->dbHostName():NULL;
	}

	// otherwise, try to find a usable connection
	if (!currentcon) {
		for (uint16_t index=0; !currentcon && index<concount; index++) {
			currentcon=cons[index];
			currentconindex=index;
		}
	}

	if (debug) {
		stdoutput.printf("	executing on: %s\n",
			(currentcon)?conids[currentconindex]:NULL);
	}

	const char	*retval=(currentcon)?currentcon->dbHostName():NULL;
	if (debug) {
		stdoutput.printf("	db hostname: %s\n}\n",retval);
	}
	return retval;
}

const char *routerconnection::dbIpAddress() {

	if (debug) {
		stdoutput.printf("dbIpAddress {\n");
	}

	// route
	bool	routed=false;
	bool	err=false;
	route(&routed,&err);
	if (err) {
		if (debug) {
			stdoutput.printf("	routing error\n}\n");
		}
		return NULL;
	}

	// if routing entire sessions, then get this for
	// the appropriate connection
	if (routeentiresession) {
		if (debug) {
			stdoutput.printf("	only executing on: %s\n}\n",
				(currentcon)?conids[currentconindex]:NULL);
		}
		return (currentcon)?currentcon->dbIpAddress():NULL;
	}

	// otherwise, try to find a usable connection
	if (!currentcon) {
		for (uint16_t index=0; !currentcon && index<concount; index++) {
			currentcon=cons[index];
			currentconindex=index;
		}
	}

	if (debug) {
		stdoutput.printf("	executing on: %s\n",
			(currentcon)?conids[currentconindex]:NULL);
	}

	const char	*retval=(currentcon)?currentcon->dbIpAddress():NULL;
	if (debug) {
		stdoutput.printf("	db ip address: %s\n}\n",retval);
	}
	return retval;
}

bool  routerconnection::cacheDbHostInfo() {
	return false;
}

bool routerconnection::getListsByApiCalls() {
	return true;
}

bool routerconnection::getDatabaseList(sqlrservercursor *cursor,
						const char *wild) {
	// FIXME: implement this
	cont->setError(cursor,SQLR_ERROR_NOTIMPLEMENTED_STRING,
				SQLR_ERROR_NOTIMPLEMENTED,true);
	return false;
}

bool routerconnection::getTableList(sqlrservercursor *cursor,
						const char *wild,
						uint16_t objecttypes) {
	// FIXME: implement this
	cont->setError(cursor,SQLR_ERROR_NOTIMPLEMENTED_STRING,
				SQLR_ERROR_NOTIMPLEMENTED,true);
	return false;
}

bool routerconnection::getColumnList(sqlrservercursor *cursor,
						const char *table,
						const char *wild) {
	// FIXME: implement this
	cont->setError(cursor,SQLR_ERROR_NOTIMPLEMENTED_STRING,
				SQLR_ERROR_NOTIMPLEMENTED,true);
	return false;
}

bool routerconnection::ping() {

	if (debug) {
		stdoutput.printf("ping {\n");
	}

	// route
	bool	routed=false;
	bool	err=false;
	route(&routed,&err);
	if (err) {
		if (debug) {
			stdoutput.printf("	routing error\n}\n");
		}
		return false;
	}

	// if routing entire sessions, then ping the appropriate connection
	if (routed && routeentiresession) {
		if (debug) {
			stdoutput.printf("	only executing on: %s\n}\n",
				(currentcon)?conids[currentconindex]:NULL);
		}
		return (currentcon)?currentcon->ping():true;
	}

	// ping all connections, if any fail, return failure
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {

		if (debug) {
			stdoutput.printf("	executing on: %s\n",
							conids[index]);
		}

		bool	res=cons[index]->ping();
		if (!res) {
			if (debug) {
				stdoutput.printf("	failed\n");
			}
			result=res;
		}
	}

	if (debug) {
		stdoutput.printf("}\n");
	}
	return result;
}

const char *routerconnection::selectDatabaseQuery() {
	return "use %s";
}

char *routerconnection::getCurrentDatabase() {
	return (currentcon)?
		charstring::duplicate(currentcon->getCurrentDatabase()):NULL;
}

bool routerconnection::getLastInsertId(uint64_t *id) {
	// get this from the most recently used connection
	if (!currentcon) {
		*id=0;
		return true;
	}
	*id=currentcon->getLastInsertId();
	return (*id!=0);
}

void routerconnection::endSession() {

	if (debug) {
		stdoutput.printf("endSession {\n");
	}

	// route
	bool	routed=false;
	bool	err=false;
	route(&routed,&err);
	if (err) {
		if (debug) {
			stdoutput.printf("	routing error\n}\n");
		}
		return;
	}

	if (routed && routeentiresession) {

		// if routing entire sessions, then end-session
		// on the appropriate connection
		if (debug) {
			stdoutput.printf("	only executing on: %s\n}\n",
				(currentcon)?conids[currentconindex]:NULL);
		}
		currentcon->endSession();

	} else {

		// otherwise end-session on all connections
		for (uint16_t index=0; index<concount; index++) {
			if (debug) {
				stdoutput.printf("	executing on: %s\n",
								conids[index]);
			}
			cons[index]->endSession();
		}
	}

	// reset pointers and index
	currentcon=NULL;
	currentconindex=0;
	for (linkedlistnode< routercursor * > *node=routercursors.getFirst();
						node; node=node->getNext()) {
		routercursor	*rcur=node->getValue();
		rcur->currentcon=NULL;
		rcur->currentcur=NULL;
	}
	sqlrr->setCurrentConnectionId(NULL);

	if (debug) {
		stdoutput.printf("}\n");
	}
}

void routerconnection::route(bool *routed, bool *err) {

	if (debug) {
		stdoutput.printf("	route (connection) {\n");
	}

	// initialize return values
	*err=false;
	*routed=false;

	// bail if we're routing the entire session
	// and we already have a currentcon
	if (routeentiresession && currentcon) {
		if (debug) {
			stdoutput.printf("		"
					"routing entire session and "
					"have currentcon\n	}\n");
		}
		return;
	}

	// otherwise, sort ourselves out...

	// reset pointers and index
	currentcon=NULL;
	currentconindex=0;

	// route...
	const char	*errm=NULL;
	int64_t		errn=0;
	const char	*connectionid=sqlrr->route(this,NULL,&errm,&errn);
	if (!connectionid) {
		if (debug) {
			stdoutput.printf("		"
					"no connection id returned\n");
		}
		if (errm) {
			if (debug) {
				stdoutput.printf("		"
						"an error occurred: "
						"%d - %s\n",errn,errm);
			}
			cont->setError(errm,errn,true);
			*err=true;
		}
		if (debug) {
			stdoutput.printf("	}\n");
		}
		return;
	}
	if (debug) {
		stdoutput.printf("		routing to: %s\n",connectionid);
	}

	// get the corresponding connection
	for (uint16_t i=0; i<concount; i++) {
		if (!charstring::compare(connectionid,conids[i])) {
			currentcon=cons[i];
			currentconindex=i;
			sqlrr->setCurrentConnectionId(connectionid);
			*routed=true;
			if (debug) {
				stdoutput.printf("	}\n");
			}
			return;
		}
	}

	if (debug) {
		stdoutput.printf("		"
				"%s not found\n	}\n",connectionid);
	}
}

void routerconnection::autoCommitOnFailed(uint16_t index) {
	raiseIntegrityViolationEvent("autocommit-on",index);
}

void routerconnection::autoCommitOffFailed(uint16_t index) {
	raiseIntegrityViolationEvent("autocommit-off",index);
}

void routerconnection::beginFailed(uint16_t index) {
	raiseIntegrityViolationEvent("begin",index);
}

void routerconnection::commitFailed(uint16_t index) {
	raiseIntegrityViolationEvent("commit",index);
}

void routerconnection::rollbackFailed(uint16_t index) {
	raiseIntegrityViolationEvent("rollback",index);
}

void routerconnection::beginQueryFailed(uint16_t index) {
	raiseIntegrityViolationEvent("begin",index);
}

void routerconnection::raiseIntegrityViolationEvent(const char *command,
							uint16_t index) {
	stringbuffer	info;
	info.append(command);
	info.append(" failed on connectionid: ");
	info.append(conids[index]);
	cont->raiseIntegrityViolationEvent(info.getString());

	cont->setInstanceDisabled(true);
}


routercursor::routercursor(sqlrserverconnection *conn, uint16_t id) :
						sqlrservercursor(conn,id) {
	routerconn=(routerconnection *)conn;
	nextrow=0;
	currentcon=NULL;
	currentcur=NULL;
	isbindcur=false;
	curs=new sqlrcursor *[routerconn->concount];
	for (uint16_t index=0; index<routerconn->concount; index++) {
		curs[index]=NULL;
		if (!routerconn->cons[index]) {
			continue;
		}
		curs[index]=new sqlrcursor(routerconn->cons[index]);
		curs[index]->setResultSetBufferSize(
					conn->cont->getFetchAtOnce());
	}

	obv=new outputbindvar[conn->cont->getConfig()->getMaxBindCount()];
	obcount=0;

	cbv=new cursorbindvar[conn->cont->getConfig()->getMaxBindCount()];
	cbcount=0;

	emptyquery=false;

	routerconn->routercursors.append(this);
}

routercursor::~routercursor() {
	for (uint16_t index=0; index<routerconn->concount; index++) {
		delete curs[index];
	}
	delete[] curs;
	delete[] obv;
	delete[] cbv;
	routerconn->routercursors.remove(this);
}

bool routercursor::prepareQuery(const char *query, uint32_t length) {

	if (routerconn->debug) {
		stdoutput.printf("prepareQuery {\n");
	}

	// FIXME: remove this and use a translation

	// convert to lowercase and normalize whitespace, for regex matching
	char	*nquery=new char[length+1];
	if (query) {
		for (uint32_t i=0; i<length; ++i) {
			char	c=query[i];
			if (character::isWhitespace(c)) {
				nquery[i]=' ';
			} else {
				nquery[i]=character::toLowerCase(c);
			}
		}
	}
	nquery[length]='\0';

	// reset bind cursor
	if (isbindcur) {
		delete currentcur;
		currentcur=NULL;
		isbindcur=false;
	}

	// initialize the output bind count
	obcount=0;

	// initialize the cursor bind count
	cbcount=0;

	// initialize the empty query flag
	emptyquery=false;

	// route
	bool	routed=false;
	bool	err=false;
	route(&routed,&err);
	if (err) {
		if (routerconn->debug) {
			stdoutput.printf("	routing error\n}\n");
		}
		return false;
	}

	// free normalized query
	delete[] nquery;

	// currentcur could be NULL here if no
	// connection could be found to run the query.
	if (!currentcur) {
		if (routerconn->debug) {
			stdoutput.printf("	no connection "
					"found, bailing\n}\n");
		}
		return false;
	}

	// Did a module make the query empty?  If so, then we won't actually
	// prepare/execute it, just return true.  The usedatabase module does
	// this.
	emptyquery=!getQueryLength();
	if (routerconn->debug) {
		stdoutput.printf("%s",(emptyquery)?"	query set empty\n":"");
	}

	// prepare the query using the cursor from whichever
	// connection turned out to be the right one
	if (!emptyquery) {
		if (routerconn->debug) {
			stdoutput.printf("	query: %.*s\n",length,query);
		}
		currentcur->prepareQuery(query,length);
	}

	if (routerconn->debug) {
		stdoutput.printf("}\n");
	}
	return true;
}

void routercursor::route(bool *routed, bool *err) {

	if (routerconn->debug) {
		stdoutput.printf("	route (cursor) {\n");
	}

	// initialize return values
	*err=false;
	*routed=false;

	// if we're routing the entire session and this particular routercursor
	// hasn't sorted itself out, but the routerconnection has, then get
	// which connection and cursor to use from the routerconnection
	if (routerconn->routeentiresession) {

		if (routerconn->debug) {
			stdoutput.printf("		"
					"routing entire session ");
		}
		if (currentcon) {
			if (routerconn->debug) {
				stdoutput.printf("and have currentcon\n	}\n");
			}
			return;
		} else if (routerconn->currentcon) {
			if (routerconn->debug) {
				stdoutput.printf("and conn has "
						"currentcon\n	}\n");
			}
			currentcon=routerconn->currentcon;
			currentcur=curs[routerconn->currentconindex];
			return;
		}
		if (routerconn->debug) {
			stdoutput.printf("but need to get currentcon\n");
		}
	}

	// otherwise, sort this routercursor out...

	// reset pointers and index
	currentcon=NULL;
	currentcur=NULL;
	routerconn->currentcon=NULL;
	routerconn->currentconindex=0;

	// route...
	const char	*errm=NULL;
	int64_t		errn=0;
	const char	*connectionid=routerconn->sqlrr->route(
						routerconn,this,&errm,&errn);
	if (!connectionid) {
		if (routerconn->debug) {
			stdoutput.printf("		"
					"no connection id returned\n");
		}
		if (errm) {
			if (routerconn->debug) {
				stdoutput.printf("		"
						"an error occurred: "
						"%d - %s\n",errn,errm);
			}
			conn->cont->setError(this,errm,errn,true);
			*err=true;
		}
		if (routerconn->debug) {
			stdoutput.printf("	}\n");
		}
		return;
	}
	if (routerconn->debug) {
		stdoutput.printf("		routing to: %s\n",connectionid);
	}

	// get the corresponding connection and cursor
	for (uint16_t i=0; i<routerconn->concount; i++) {
		if (!charstring::compare(connectionid,routerconn->conids[i])) {
			currentcon=routerconn->cons[i];
			currentcur=curs[i];
			routerconn->currentcon=currentcon;
			routerconn->currentconindex=i;
			routerconn->sqlrr->setCurrentConnectionId(connectionid);
			*routed=true;
			if (routerconn->debug) {
				stdoutput.printf("	}\n");
			}
			return;
		}
	}

	if (routerconn->debug) {
		stdoutput.printf("		"
				"%s not found\n	}\n",connectionid);
	}
}

bool routercursor::supportsNativeBinds(const char *query, uint32_t length) {
	return true;
}

bool routercursor::inputBind(const char *variable, 
				uint16_t variablesize,
				const char *value, 
				uint32_t valuesize,
				int16_t *isnull) {
	currentcur->inputBind(variable+1,value);
	return true;
}

bool routercursor::inputBind(const char *variable, 
				uint16_t variablesize,
				int64_t *value) {
	currentcur->inputBind(variable+1,*value);
	return true;
}

bool routercursor::inputBind(const char *variable, 
				uint16_t variablesize,
				double *value,
				uint32_t precision,
				uint32_t scale) {
	currentcur->inputBind(variable+1,*value,precision,scale);
	return true;
}

bool routercursor::inputBind(const char *variable,
				uint16_t variablesize,
				int64_t year,
				int16_t month,
				int16_t day,
				int16_t hour,
				int16_t minute,
				int16_t second,
				int32_t microsecond,
				const char *tz,
				bool isnegative,
				char *buffer,
				uint16_t buffersize,
				int16_t *isnull) {
	currentcur->inputBind(variable+1,year,month,day,
			hour,minute,second,microsecond,tz,isnegative);
	return true;
}

bool routercursor::inputBindBlob(const char *variable, 
					uint16_t variablesize,
					const char *value, 
					uint32_t valuesize,
					int16_t *isnull) {
	currentcur->inputBindBlob(variable+1,value,valuesize);
	return true;
}

bool routercursor::inputBindClob(const char *variable, 
					uint16_t variablesize,
					const char *value, 
					uint32_t valuesize,
					int16_t *isnull) {
	currentcur->inputBindClob(variable+1,value,valuesize);
	return true;
}

bool routercursor::outputBind(const char *variable, 
				uint16_t variablesize,
				char *value,
				uint32_t valuesize,
				int16_t *isnull) {
	currentcur->defineOutputBindString(variable+1,valuesize);
	obv[obcount].variable=variable+1;
	obv[obcount].type=SQLRSERVERBINDVARTYPE_STRING;
	obv[obcount].value.stringvalue=value;
	obv[obcount].valuesize=valuesize;
	obv[obcount].isnull=isnull;
	obcount++;
	return true;
}

bool routercursor::outputBind(const char *variable, 
				uint16_t variablesize,
				int64_t *value,
				int16_t *isnull) {
	currentcur->defineOutputBindInteger(variable+1);
	obv[obcount].variable=variable+1;
	obv[obcount].type=SQLRSERVERBINDVARTYPE_INTEGER;
	obv[obcount].value.intvalue=value;
	obv[obcount].isnull=isnull;
	obcount++;
	return true;
}

bool routercursor::outputBind(const char *variable, 
				uint16_t variablesize,
				double *value,
				uint32_t *precision,
				uint32_t *scale,
				int16_t *isnull) {
	currentcur->defineOutputBindDouble(variable+1);
	obv[obcount].variable=variable+1;
	obv[obcount].type=SQLRSERVERBINDVARTYPE_DOUBLE;
	obv[obcount].value.doublevalue=value;
	obv[obcount].isnull=isnull;
	obcount++;
	return true;
}

bool routercursor::outputBind(const char *variable,
				uint16_t variablesize,
				int16_t *year,
				int16_t *month,
				int16_t *day,
				int16_t *hour,
				int16_t *minute,
				int16_t *second,
				int32_t *microsecond,
				const char **tz,
				bool *isnegative,
				char *buffer,
				uint16_t buffersize,
				int16_t *isnull) {
	currentcur->defineOutputBindDouble(variable+1);
	obv[obcount].variable=variable+1;
	obv[obcount].type=SQLRSERVERBINDVARTYPE_DATE;
	obv[obcount].value.datevalue.year=year;
	obv[obcount].value.datevalue.month=month;
	obv[obcount].value.datevalue.day=day;
	obv[obcount].value.datevalue.hour=hour;
	obv[obcount].value.datevalue.minute=minute;
	obv[obcount].value.datevalue.second=second;
	obv[obcount].value.datevalue.microsecond=microsecond;
	obv[obcount].value.datevalue.tz=tz;
	obv[obcount].isnull=isnull;
	obv[obcount].value.datevalue.isnegative=isnegative;
	obcount++;
	return true;
}


bool routercursor::outputBindBlob(const char *variable, 
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {
	currentcur->defineOutputBindBlob(variable+1);
	obv[obcount].variable=variable+1;
	obv[obcount].type=SQLRSERVERBINDVARTYPE_BLOB;
	obv[obcount].isnull=isnull;
	obcount++;
	return true;
}

bool routercursor::outputBindClob(const char *variable, 
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {
	currentcur->defineOutputBindClob(variable+1);
	obv[obcount].variable=variable+1;
	obv[obcount].type=SQLRSERVERBINDVARTYPE_CLOB;
	obv[obcount].isnull=isnull;
	obcount++;
	return true;
}

bool routercursor::outputBindCursor(const char *variable,
					uint16_t variablesize,
					sqlrservercursor *cursor) {
	currentcur->defineOutputBindCursor(variable+1);
	cbv[cbcount].variable=variable+1;
	cbv[cbcount].cursor=cursor;
	cbcount++;
	return true;
}

bool routercursor::getLobOutputBindLength(uint16_t index, uint64_t *length) {
	*length=currentcur->getOutputBindLength(obv[index].variable);
	return true;
}

bool routercursor::getLobOutputBindSegment(uint16_t index,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread) {
	const char	*varname=obv[index].variable;
	const char	*var=currentcur->getOutputBindClob(varname);
	if (!var) {
		var=currentcur->getOutputBindBlob(varname);
	}
	uint32_t	length=currentcur->getOutputBindLength(varname);
	if (offset+charstoread>length) {
		charstoread=length-offset;
	}
	bytestring::copy(buffer,var,charstoread);
	*charsread=charstoread;
	return true;
}

bool routercursor::executeQuery(const char *query, uint32_t length) {

	// FIXME: if routing entire sessions, then compare and just do this for
	// the appropriate connection

	if (!currentcur) {
		if (!prepareQuery(query,length)) {
			return false;
		}
	}

	if (!currentcur) {
		return false;
	}

	if (!emptyquery) {
		if (!currentcur->executeQuery()) {
			return false;
		}
	}

	nextrow=0;

	// populate output bind values
	for (uint16_t outi=0; outi<obcount; outi++) {
		const char	*variable=obv[outi].variable;
		*(obv[outi].isnull)=routerconn->nonnullbindvalue;
		if (obv[outi].type==SQLRSERVERBINDVARTYPE_STRING) {
			const char	*str=
				currentcur->getOutputBindString(variable);
			uint32_t	len=
				currentcur->getOutputBindLength(variable);
			if (str) {
				charstring::copy(obv[outi].value.stringvalue,
								str,len);
			} else {
				obv[outi].value.stringvalue[0]='\0';
				*(obv[outi].isnull)=routerconn->nullbindvalue;
			} 
		} else if (obv[outi].type==SQLRSERVERBINDVARTYPE_INTEGER) {
			*(obv[outi].value.intvalue)=
				currentcur->getOutputBindInteger(variable);
		} else if (obv[outi].type==SQLRSERVERBINDVARTYPE_DOUBLE) {
			*(obv[outi].value.doublevalue)=
				currentcur->getOutputBindDouble(variable);
		} else if (obv[outi].type==SQLRSERVERBINDVARTYPE_DATE) {
			currentcur->getOutputBindDate(variable,
					obv[outi].value.datevalue.year,
					obv[outi].value.datevalue.month,
					obv[outi].value.datevalue.day,
					obv[outi].value.datevalue.hour,
					obv[outi].value.datevalue.minute,
					obv[outi].value.datevalue.second,
					obv[outi].value.datevalue.microsecond,
					obv[outi].value.datevalue.tz,
					obv[outi].value.datevalue.isnegative);
		}
	}

	// handle cursor bind values
	for (uint16_t curi=0; curi<cbcount; curi++) {
		routercursor	*rcur=(routercursor *)cbv[curi].cursor;
		rcur->currentcon=currentcon;
		rcur->currentcur=
			currentcur->getOutputBindCursor(cbv[curi].variable);
		if (!rcur->currentcur) {
			return false;
		}
		rcur->currentcur->setResultSetBufferSize(
					conn->cont->getFetchAtOnce());
		rcur->isbindcur=true;
		rcur->nextrow=0;
		if (!rcur->currentcur->fetchFromBindCursor()) {
			return false;
		}
	}
	return true;
}

void routercursor::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {
	const char	*errormessage=
			(currentcur)?currentcur->errorMessage():"";
	*errorlength=charstring::length(errormessage);
	charstring::safeCopy(errorbuffer,errorbufferlength,
					errormessage,*errorlength);
	*errorcode=(currentcur)?currentcur->errorNumber():0;
	*liveconnection=true;
}

bool routercursor::knowsRowCount() {
	return true;
}

uint64_t routercursor::rowCount() {
	return (currentcur)?currentcur->rowCount():0;
}

uint64_t routercursor::affectedRows() {
	return (currentcur)?currentcur->affectedRows():0;
}

uint32_t routercursor::colCount() {
	return (currentcur)?currentcur->colCount():0;
}

uint16_t routercursor::columnTypeFormat() {
	return (uint16_t)COLUMN_TYPE_NAMES;
}

const char *routercursor::getColumnName(uint32_t col) {
	return (currentcur)?currentcur->getColumnName(col):NULL;
}

const char *routercursor::getColumnTypeName(uint32_t col) {
	return (currentcur)?currentcur->getColumnType(col):NULL;
}

uint32_t routercursor::getColumnLength(uint32_t col) {
	return (currentcur)?currentcur->getColumnLength(col):0;
}

uint32_t routercursor::getColumnPrecision(uint32_t col) {
	return (currentcur)?currentcur->getColumnPrecision(col):0;
}

uint32_t routercursor::getColumnScale(uint32_t col) {
	return (currentcur)?currentcur->getColumnScale(col):0;
}

uint16_t routercursor::getColumnIsNullable(uint32_t col) {
	return (currentcur)?currentcur->getColumnIsNullable(col):0;
}

uint16_t routercursor::getColumnIsPrimaryKey(uint32_t col) {
	return (currentcur)?currentcur->getColumnIsPrimaryKey(col):0;
}

uint16_t routercursor::getColumnIsUnique(uint32_t col) {
	return (currentcur)?currentcur->getColumnIsUnique(col):0;
}

uint16_t routercursor::getColumnIsPartOfKey(uint32_t col) {
	return (currentcur)?currentcur->getColumnIsPartOfKey(col):0;
}

uint16_t routercursor::getColumnIsUnsigned(uint32_t col) {
	return (currentcur)?currentcur->getColumnIsUnsigned(col):0;
}

uint16_t routercursor::getColumnIsZeroFilled(uint32_t col) {
	return (currentcur)?currentcur->getColumnIsZeroFilled(col):0;
}

uint16_t routercursor::getColumnIsBinary(uint32_t col) {
	return (currentcur)?currentcur->getColumnIsBinary(col):0;
}

uint16_t routercursor::getColumnIsAutoIncrement(uint32_t col) {
	return (currentcur)?currentcur->getColumnIsAutoIncrement(col):0;
}

const char *routercursor::getColumnTable(uint32_t col) {
	return (currentcur)?currentcur->getColumnTable(col):NULL;
}

bool routercursor::noRowsToReturn() {
	return (((currentcur)?currentcur->rowCount():0)==0);
}

bool routercursor::fetchRow(bool *error) {

	*error=false;

	if (!currentcur) {
		return false;
	}
	if (currentcur->getField(nextrow,(uint32_t)0)) {
		nextrow++;
		return true;
	}
	if (currentcur->errorMessage()) {
		*error=true;
	}
	return false;
}

void routercursor::getField(uint32_t col,
				const char **field, uint64_t *fieldlength,
				bool *blob, bool *null) {
	const char	*fld=currentcur->getField(nextrow-1,col);
	uint32_t	len=currentcur->getFieldLength(nextrow-1,col);
	if (len) {
		*field=fld;
		*fieldlength=len;
	} else {
		*null=true;
	}
}

void routercursor::closeResultSet() {
	if (currentcur) {
		currentcur->clearBinds();
	}
	obcount=0;
	cbcount=0;
}


extern "C" {
	SQLRSERVER_DLLSPEC sqlrserverconnection *new_routerconnection(
						sqlrservercontroller *cont) {
		return new routerconnection(cont);
	}
}
