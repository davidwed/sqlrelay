// Copyright (c) 2006-2015 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/bytestring.h>
#include <rudiments/character.h>
#include <rudiments/regularexpression.h>

#include <datatypes.h>
#include <defines.h>
#include <config.h>

#include <sqlrelay/sqlrclient.h>

#define FETCH_AT_ONCE	10

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
		} datevalue;
	} value;
	uint16_t		valuesize;
	sqlrserverbindvartype_t	type;
	int16_t			*isnull;
};

struct cursorbindvar {
	const char	*variable;
	sqlrservercursor	*cursor;
};

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
		bool		ping();
		bool		getLastInsertId(uint64_t *id);
		void		endSession();

		void	autoCommitOnFailed(uint16_t index);
		void	autoCommitOffFailed(uint16_t index);
		void	commitFailed(uint16_t index);
		void	rollbackFailed(uint16_t index);
		void	beginQueryFailed(uint16_t index);

		const char	*identity;

		sqlrconnection	**cons;
		sqlrconnection	*cur;
		const char	**beginquery;
		bool		anymustbegin;
		uint16_t	concount;

		sqlrconfig	*cfg;

		bool		justloggedin;

		int16_t		nullbindvalue;
		int16_t		nonnullbindvalue;

		regularexpression	beginregex;
		regularexpression	beginendregex;

		const char	*error;

		sqlrrouters	*sqlrr;
};

class SQLRSERVER_DLLSPEC routercursor : public sqlrservercursor {
	friend class routerconnection;
	private:
				routercursor(sqlrserverconnection *conn,
								uint16_t id);
				~routercursor();
		bool		prepareQuery(const char *query,
						uint32_t length);
		bool		supportsNativeBinds(const char *query,
							uint32_t length);
		bool		begin(const char *query, uint32_t length);
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
						uint16_t valuesize,
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
		void		checkForTempTable(const char *query,
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
		bool		noRowsToReturn();
		bool		fetchRow();
		void		getField(uint32_t col,
					const char **field,
					uint64_t *fieldlength,
					bool *blob,
					bool *null);
		void		closeResultSet();

		routerconnection	*routerconn;

		sqlrconnection	*con;
		sqlrcursor	*cur;
		bool		isbindcur;
		uint16_t	curindex;
		sqlrcursor	**curs;

		uint64_t	nextrow;

		bool		beginquery;

		outputbindvar	*obv;
		uint16_t	obcount;

		cursorbindvar	*cbv;
		uint16_t	cbcount;

		regularexpression	createoratemp;
		regularexpression	preserverows;
};

routerconnection::routerconnection(sqlrservercontroller *cont) :
					sqlrserverconnection(cont) {
	identity=NULL;

	cons=NULL;
	cur=NULL;
	beginquery=NULL;
	anymustbegin=false;
	concount=0;
	cfg=NULL;
	justloggedin=false;
	nullbindvalue=nullBindValue();
	nonnullbindvalue=nonNullBindValue();

	beginregex.compile("^\\s*(begin|start\\s*transaction)");
	beginregex.study();
	beginendregex.compile("^\\s*begin\\s*.*\\s*end;\\s*$");
	beginendregex.study();

	sqlrr=NULL;
}

routerconnection::~routerconnection() {
	for (uint16_t index=0; index<concount; index++) {
		delete cons[index];
	}
	delete[] cons;
	delete[] beginquery;
	delete sqlrr;
}

bool routerconnection::supportsAuthOnDatabase() {
	return false;
}

void routerconnection::handleConnectString() {

	identity=cont->getConnectStringValue("identity");

	cfg=cont->cfg;

	// FIXME: use the router modules...
	xmldomnode	*routers=cfg->getRouters();
	if (!routers->isNullNode()) {
		// FIXME: set debug correctly
		sqlrr=new sqlrrouters(cont->pth,false);
		sqlrr->load(routers);
		sqlrr->init(this);
	}

	linkedlist< routecontainer * >	*routelist=cont->cfg->getRouteList();
	concount=routelist->getLength();

	cons=new sqlrconnection *[concount];
	beginquery=new const char *[concount];
	anymustbegin=false;
	uint16_t index=0;
	linkedlistnode< routecontainer * >	*rln=routelist->getFirst();
	while (index<concount) {

		cons[index]=NULL;
		beginquery[index]=NULL;

		routecontainer	*rc=rln->getValue();

		if (rc->getIsFilter()) {
			index++;
			continue;
		}

		cons[index]=new sqlrconnection(rc->getHost(),rc->getPort(),
						rc->getSocket(),rc->getUser(),
						rc->getPassword(),0,1);

		const char	*id=cons[index]->identify();
		if (!charstring::compare(id,"sybase") ||
				!charstring::compare(id,"freetds")) {
			beginquery[index]="begin tran";
		} else if (!charstring::compare(id,"sqlite")) {
			beginquery[index]="begin transaction";
		} else if (!charstring::compare(id,"postgresql") ||
				!charstring::compare(id,"router")) {
			beginquery[index]="begin";
		}

		if (beginquery[index]) {
			anymustbegin=true;
		}

		index++;
		rln=rln->getNext();
	}
}

bool routerconnection::logIn(const char **error, const char **warning) {
	justloggedin=true;
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

	// turn autocommit on for all connections, if any fail, return failure
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {
		if (!cons[index]) {
			continue;
		}
		bool	res=cons[index]->autoCommitOn();
		if (!res) {
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
	if (justloggedin) {
		justloggedin=false;
	}
	return result;
}

bool routerconnection::autoCommitOff() {

	// turn autocommit on for all connections, if any fail, return failure
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {
		if (!cons[index]) {
			continue;
		}
		bool	res=cons[index]->autoCommitOff();
		if (!res) {
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
	if (justloggedin) {
		justloggedin=false;
	}
	return result;
}

bool routerconnection::commit() {

	// commit all connections, if any fail, return failure
	// FIXME: use 2 stage commit...
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {
		if (!cons[index]) {
			continue;
		}
		bool	res=cons[index]->commit();
		if (!res) {
			commitFailed(index);
		}
		if (result) {
			result=res;
		}
	}
	return result;
}

bool routerconnection::rollback() {

	// rollback all connections, if any fail, return failure
	// FIXME: use 2 stage rollback...
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {
		if (!cons[index]) {
			continue;
		}
		bool	res=cons[index]->rollback();
		if (!res) {
			rollbackFailed(index);
		}
		if (result) {
			result=res;
		}
	}
	return result;
}

void routerconnection::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {
	for (uint16_t index=0; index<concount; index++) {
		if (!cons[index]) {
			continue;
		}
		const char	*errormessage=cons[index]->errorMessage();
		if (!charstring::length(errormessage)) {
			*errorlength=charstring::length(errormessage);
			charstring::safeCopy(errorbuffer,errorbufferlength,
						errormessage,*errorlength);
			*errorcode=cons[index]->errorNumber();
			break;
		}
	}
	// FIXME: detect downed database or downed relay
	*liveconnection=true;
}

void routerconnection::endSession() {
	for (uint16_t index=0; index<concount; index++) {
		if (!cons[index]) {
			continue;
		}
		cons[index]->endSession();
	}
}

const char *routerconnection::identify() {
	return (identity)?identity:"router";
}

const char *routerconnection::dbVersion() {
	if (!cur) {
		// try to find a usable connection
		for (uint16_t index=0; !cur && index<concount; index++) {
			cur=cons[index];
		}
	}
	return (cur)?cur->dbVersion():NULL;
}

const char *routerconnection::dbHostName() {
	if (!cur) {
		// try to find a usable connection
		for (uint16_t index=0; !cur && index<concount; index++) {
			cur=cons[index];
		}
	}
	return (cur)?cur->dbHostName():NULL;
}

const char *routerconnection::dbIpAddress() {
	if (!cur) {
		// try to find a usable connection
		for (uint16_t index=0; !cur && index<concount; index++) {
			cur=cons[index];
		}
	}
	return (cur)?cur->dbIpAddress():NULL;
}

bool routerconnection::ping() {

	// ping all connections, if any fail, return failure
	bool	result=true;
	for (uint16_t index=0; index<concount; index++) {
		if (!cons[index]) {
			continue;
		}
		bool	res=cons[index]->ping();
		if (result) {
			result=res;
		}
	}
	return result;
}

bool routerconnection::getLastInsertId(uint64_t *id) {

	// run it against the previously used connection,
	// unless there wasn't one
	if (!cur) {
		*id=0;
		return true;
	}
	*id=cur->getLastInsertId();
	if (*id==0) {
		// FIXME: how can we report an error here?
		return false;
	}
	return true;
}

routercursor::routercursor(sqlrserverconnection *conn, uint16_t id) :
						sqlrservercursor(conn,id) {
	routerconn=(routerconnection *)conn;
	nextrow=0;
	con=NULL;
	cur=NULL;
	isbindcur=false;
	curindex=0;
	curs=new sqlrcursor *[routerconn->concount];
	for (uint16_t index=0; index<routerconn->concount; index++) {
		curs[index]=NULL;
		if (!routerconn->cons[index]) {
			continue;
		}
		curs[index]=new sqlrcursor(routerconn->cons[index]);
		curs[index]->setResultSetBufferSize(FETCH_AT_ONCE);
	}
	beginquery=false;

	obv=new outputbindvar[conn->cont->cfg->getMaxBindCount()];
	obcount=0;

	cbv=new cursorbindvar[conn->cont->cfg->getMaxBindCount()];
	cbcount=0;

	createoratemp.compile("(create|CREATE)[ 	\\n\\r]+(global|GLOBAL)[ 	\\n\\r]+(temporary|TEMPORARY)[ 	\\n\\r]+(table|TABLE)[ 	\\n\\r]+");
	preserverows.compile("(on|ON)[ 	\\n\\r]+(commit|COMMIT)[ 	\\n\\r]+(preserve|PRESERVE)[ 	\\n\\r]+(rows|ROWS)");
}

routercursor::~routercursor() {
	for (uint16_t index=0; index<routerconn->concount; index++) {
		delete curs[index];
	}
	delete[] curs;
	delete[] obv;
	delete[] cbv;
}

bool routercursor::prepareQuery(const char *query, uint32_t length) {

	// convert to lowercase and normalize whitespace, for regex matching
	char	*nquery=new char[length+1];
	if (nquery && query) {
		for (uint32_t i=0; i<length; ++i) {
			char	c=query[i];
			if (character::isWhitespace(c)) {
				nquery[i]=' ';
			} else {
				nquery[i]=character::toLowerCase(c);
			}
		}
		nquery[length]='\0';
	}

	// check for a begin query, but not "begin ... end;" query
	beginquery=(routerconn->beginregex.match(nquery) &&
			!routerconn->beginendregex.match(nquery));
	if (beginquery) {
		if (nquery) {
			delete[] nquery;
		}
		return true;
	}

	// reset cur and pointers
	con=NULL;
	if (isbindcur) {
		delete cur;
	}
	cur=NULL;
	isbindcur=false;
	curindex=0;

	// initialize the output bind count
	obcount=0;

	// initialize the cursor bind count
	cbcount=0;

	// FIXME: use the router modules...

	// look through the regular expressions and figure out which
	// connection this query needs to be run through
	uint16_t	conindex=0;
	routenode	*rcn=routerconn->cfg->getRouteList()->getFirst();
	bool	found=false;
	while (rcn && !found) {
		linkedlistnode< regularexpression * >	*ren=
				rcn->getValue()->getRegexList()->getFirst();

		while (ren && !found) {
			if (ren->getValue()->match(nquery)) {
				con=routerconn->cons[conindex];
				routerconn->cur=con;
				cur=curs[conindex];
				curindex=conindex;
				found=true;
			}
			ren=ren->getNext();
		}

		if (!found) {
			rcn=rcn->getNext();
			conindex++;
		}
	}

	// free normalized query
	if (nquery) {
		delete[] nquery;
	}

	// cur could be NULL here either because no connection could be found
	// to run the query, or because the query matched the pattern of
	// a connection which was intentionally set up to filter out the query
	if (!cur) {
		return false;
	}

	// prepare the query using the cursor from whichever
	// connection turned out to be the right one
	cur->prepareQuery(query);

	return true;
}

bool routercursor::supportsNativeBinds(const char *query, uint32_t length) {
	return true;
}

bool routercursor::inputBind(const char *variable, 
				uint16_t variablesize,
				const char *value, 
				uint32_t valuesize,
				int16_t *isnull) {
	cur->inputBind(variable+1,value);
	return true;
}

bool routercursor::inputBind(const char *variable, 
				uint16_t variablesize,
				int64_t *value) {
	cur->inputBind(variable+1,*value);
	return true;
}

bool routercursor::inputBind(const char *variable, 
				uint16_t variablesize,
				double *value,
				uint32_t precision,
				uint32_t scale) {
	cur->inputBind(variable+1,*value,precision,scale);
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
				char *buffer,
				uint16_t buffersize,
				int16_t *isnull) {
	cur->inputBind(variable+1,year,month,day,
			hour,minute,second,microsecond,tz);
	return true;
}

bool routercursor::inputBindBlob(const char *variable, 
					uint16_t variablesize,
					const char *value, 
					uint32_t valuesize,
					int16_t *isnull) {
	cur->inputBindBlob(variable+1,value,valuesize);
	return true;
}

bool routercursor::inputBindClob(const char *variable, 
					uint16_t variablesize,
					const char *value, 
					uint32_t valuesize,
					int16_t *isnull) {
	cur->inputBindClob(variable+1,value,valuesize);
	return true;
}

bool routercursor::outputBind(const char *variable, 
				uint16_t variablesize,
				char *value,
				uint16_t valuesize,
				int16_t *isnull) {
	cur->defineOutputBindString(variable+1,valuesize);
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
	cur->defineOutputBindInteger(variable+1);
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
	cur->defineOutputBindDouble(variable+1);
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
				char *buffer,
				uint16_t buffersize,
				int16_t *isnull) {
	cur->defineOutputBindDouble(variable+1);
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
	obcount++;
	return true;
}


bool routercursor::outputBindBlob(const char *variable, 
					uint16_t variablesize,
					uint16_t index,
					int16_t *isnull) {
	cur->defineOutputBindBlob(variable+1);
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
	cur->defineOutputBindClob(variable+1);
	obv[obcount].variable=variable+1;
	obv[obcount].type=SQLRSERVERBINDVARTYPE_CLOB;
	obv[obcount].isnull=isnull;
	obcount++;
	return true;
}

bool routercursor::outputBindCursor(const char *variable,
					uint16_t variablesize,
					sqlrservercursor *cursor) {
	cur->defineOutputBindCursor(variable+1);
	cbv[cbcount].variable=variable+1;
	cbv[cbcount].cursor=cursor;
	cbcount++;
	return true;
}

bool routercursor::getLobOutputBindLength(uint16_t index, uint64_t *length) {
	*length=cur->getOutputBindLength(obv[index].variable);
	return true;
}

bool routercursor::getLobOutputBindSegment(uint16_t index,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread) {
	const char	*varname=obv[index].variable;
	const char	*var=cur->getOutputBindClob(varname);
	if (!var) {
		var=cur->getOutputBindBlob(varname);
	}
	uint32_t	length=cur->getOutputBindLength(varname);
	if (offset+charstoread>length) {
		charstoread=length-offset;
	}
	bytestring::copy(buffer,var,charstoread);
	*charsread=charstoread;
	return true;
}

bool routercursor::executeQuery(const char *query, uint32_t length) {

	if (beginquery) {
		if (routerconn->anymustbegin) {
			return begin(query,length);
		} else {
			nextrow=0;
			return true;
		}
	}

	if (!cur) {
		if (!prepareQuery(query,length)) {
			return false;
		}
	}

	if (!cur || !cur->executeQuery()) {
		return false;
	}

	checkForTempTable(query,length);

	nextrow=0;

	// populate output bind values
	for (uint16_t outi=0; outi<obcount; outi++) {
		const char	*variable=obv[outi].variable;
		*(obv[outi].isnull)=routerconn->nonnullbindvalue;
		if (obv[outi].type==SQLRSERVERBINDVARTYPE_STRING) {
			const char	*str=cur->getOutputBindString(variable);
			uint32_t	len=cur->getOutputBindLength(variable);
			if (str) {
				charstring::copy(obv[outi].value.stringvalue,
								str,len);
			} else {
				obv[outi].value.stringvalue[0]='\0';
				*(obv[outi].isnull)=routerconn->nullbindvalue;
			} 
		} else if (obv[outi].type==SQLRSERVERBINDVARTYPE_INTEGER) {
			*(obv[outi].value.intvalue)=
					cur->getOutputBindInteger(variable);
		} else if (obv[outi].type==SQLRSERVERBINDVARTYPE_DOUBLE) {
			*(obv[outi].value.doublevalue)=
					cur->getOutputBindDouble(variable);
		} else if (obv[outi].type==SQLRSERVERBINDVARTYPE_DATE) {
			cur->getOutputBindDate(variable,
					obv[outi].value.datevalue.year,
					obv[outi].value.datevalue.month,
					obv[outi].value.datevalue.day,
					obv[outi].value.datevalue.hour,
					obv[outi].value.datevalue.minute,
					obv[outi].value.datevalue.second,
					obv[outi].value.datevalue.microsecond,
					obv[outi].value.datevalue.tz);
		}
	}

	// handle cursor bind values
	for (uint16_t curi=0; curi<cbcount; curi++) {
		routercursor	*rcur=(routercursor *)cbv[curi].cursor;
		rcur->con=con;
		rcur->cur=cur->getOutputBindCursor(cbv[curi].variable);
		if (!rcur->cur) {
			return false;
		}
		rcur->cur->setResultSetBufferSize(FETCH_AT_ONCE);
		rcur->isbindcur=true;
		rcur->nextrow=0;
		if (!rcur->cur->fetchFromBindCursor()) {
			return false;
		}
	}
	return true;
}

void routercursor::checkForTempTable(const char *query, uint32_t length) {

	// for non-oracle db's
	if (charstring::compare(con->identify(),"oracle")) {
		sqlrservercursor::checkForTempTable(query,length);
		return;
	}

	// for oracle db's...

	const char	*ptr=query;
	const char	*endptr=query+length;

	// skip any leading comments
	if (!conn->cont->skipWhitespace(&ptr,endptr) ||
		!conn->cont->skipComment(&ptr,endptr) ||
		!conn->cont->skipWhitespace(&ptr,endptr)) {
		return;
	}

	// look for "create global temporary table "
	if (createoratemp.match(ptr)) {
		ptr=createoratemp.getSubstringEnd(0);
	} else {
		return;
	}

	// get the table name
	stringbuffer	tablename;
	while (ptr && *ptr && *ptr!=' ' &&
		*ptr!='\n' && *ptr!='	' && ptr<endptr) {
		tablename.append(*ptr);
		ptr++;
	}

	// append to list of temp tables
	// check for "on commit preserve rows" otherwise assume
	// "on commit delete rows"
	if (preserverows.match(ptr)) {
		conn->cont->addSessionTempTableForTrunc(tablename.getString());
	}
}

bool routercursor::begin(const char *query, uint32_t length) {

	bool	result=true;
	for (uint16_t index=0; index<routerconn->concount; index++) {

		if (!routerconn->cons[index]) {
			continue;
		}

		// for databases that allow begin's, run the begin query,
		// for others, just set autocommit off
		bool	res=false;
		if (routerconn->beginquery[index]) {
			res=curs[index]->sendQuery(
						routerconn->beginquery[index],
						length);
			if (!res) {
				routerconn->beginQueryFailed(index);
			}
		} else {
			res=routerconn->cons[index]->autoCommitOff();
			if (!res) {
				routerconn->autoCommitOffFailed(index);
			}
		}
		if (result) {
			result=res;
			// if we had an error, set "cur" so
			// we can get the error from it
			if (!res && !cur) {
				cur=curs[index];
				curindex=0;
			}
		}
	}
	// If we're here with no "cur", then all the begin's must have
	// succeeded.  But we need a "cur" so everything else will work.
	// Any of them will do, so use the first one.
	/*if (!cur) {
		cur=curs[0];
		curindex=0;
	}*/
	return result;
}

void routercursor::errorMessage(char *errorbuffer,
					uint32_t errorbufferlength,
					uint32_t *errorlength,
					int64_t *errorcode,
					bool *liveconnection) {
	const char	*errormessage=(cur)?cur->errorMessage():"";
	*errorlength=charstring::length(errormessage);
	charstring::safeCopy(errorbuffer,errorbufferlength,
					errormessage,*errorlength);
	*errorcode=(cur)?cur->errorNumber():0;
	// FIXME: detect downed database or downed relay
	*liveconnection=true;
}

bool routercursor::knowsRowCount() {
	return true;
}

uint64_t routercursor::rowCount() {
	return (cur)?cur->rowCount():0;
}

uint64_t routercursor::affectedRows() {
	return (cur)?cur->affectedRows():0;
}

uint32_t routercursor::colCount() {
	return (cur)?cur->colCount():0;
}

uint16_t routercursor::columnTypeFormat() {
	return (uint16_t)COLUMN_TYPE_NAMES;
}

const char *routercursor::getColumnName(uint32_t col) {
	return (cur)?cur->getColumnName(col):NULL;
}

const char *routercursor::getColumnTypeName(uint32_t col) {
	return (cur)?cur->getColumnType(col):NULL;
}

uint32_t routercursor::getColumnLength(uint32_t col) {
	return (cur)?cur->getColumnLength(col):0;
}

uint32_t routercursor::getColumnPrecision(uint32_t col) {
	return (cur)?cur->getColumnPrecision(col):0;
}

uint32_t routercursor::getColumnScale(uint32_t col) {
	return (cur)?cur->getColumnScale(col):0;
}

uint16_t routercursor::getColumnIsNullable(uint32_t col) {
	return (cur)?cur->getColumnIsNullable(col):0;
}

uint16_t routercursor::getColumnIsPrimaryKey(uint32_t col) {
	return (cur)?cur->getColumnIsPrimaryKey(col):0;
}

uint16_t routercursor::getColumnIsUnique(uint32_t col) {
	return (cur)?cur->getColumnIsUnique(col):0;
}

uint16_t routercursor::getColumnIsPartOfKey(uint32_t col) {
	return (cur)?cur->getColumnIsPartOfKey(col):0;
}

uint16_t routercursor::getColumnIsUnsigned(uint32_t col) {
	return (cur)?cur->getColumnIsUnsigned(col):0;
}

uint16_t routercursor::getColumnIsZeroFilled(uint32_t col) {
	return (cur)?cur->getColumnIsZeroFilled(col):0;
}

uint16_t routercursor::getColumnIsBinary(uint32_t col) {
	return (cur)?cur->getColumnIsBinary(col):0;
}

uint16_t routercursor::getColumnIsAutoIncrement(uint32_t col) {
	return (cur)?cur->getColumnIsAutoIncrement(col):0;
}

bool routercursor::noRowsToReturn() {
	return (((cur)?cur->rowCount():0)==0);
}

bool routercursor::fetchRow() {
	if (!cur) {
		return false;
	}
	if (cur->getField(nextrow,(uint32_t)0)) {
		nextrow++;
		return true;
	}
	return false;
}

void routercursor::getField(uint32_t col,
				const char **field, uint64_t *fieldlength,
				bool *blob, bool *null) {
	const char	*fld=cur->getField(nextrow-1,col);
	uint32_t	len=cur->getFieldLength(nextrow-1,col);
	if (len) {
		*field=fld;
		*fieldlength=len;
	} else {
		*null=true;
	}
}

void routercursor::closeResultSet() {
	if (cur) {
		cur->clearBinds();
	}
	obcount=0;
	cbcount=0;
}

// FIXME: "do something" when these failures occur
void routerconnection::autoCommitOnFailed(uint16_t index) {
}

void routerconnection::autoCommitOffFailed(uint16_t index) {
}

void routerconnection::commitFailed(uint16_t index) {
}

void routerconnection::rollbackFailed(uint16_t index) {
}

void routerconnection::beginQueryFailed(uint16_t index) {
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrserverconnection *new_routerconnection(
						sqlrservercontroller *cont) {
		return new routerconnection(cont);
	}
}
