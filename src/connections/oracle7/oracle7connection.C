// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <oracle7connection.h>
#include <rudiments/charstring.h>
#include <rudiments/environment.h>

#include <config.h>
#include <datatypes.h>

#include <stdio.h>
#include <stdlib.h>

oracle7connection::oracle7connection() : sqlrconnection_svr() {
}

oracle7connection::~oracle7connection() {
}

uint16_t oracle7connection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void oracle7connection::handleConnectString() {
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	sid=connectStringValue("oracle_sid");
	home=connectStringValue("oracle_home");
	nlslang=connectStringValue("nls_lang");
	const char	*autocom=connectStringValue("autocommit");
	setAutoCommitBehavior((autocom &&
		!charstring::compareIgnoringCase(autocom,"yes")));
}

bool oracle7connection::logIn(bool printerrors) {

	// handle ORACLE_HOME
	if (home) {
		if (!environment::setValue("ORACLE_HOME",home)) {
			fprintf(stderr,"Failed to set ORACLE_HOME environment variable.\n");
			return false;
		}
	} else {
		if (!environment::getValue("ORACLE_HOME")) {
			fprintf(stderr,"No ORACLE_HOME environment variable set or specified in connect string.\n");
			return false;
		}
	}

	// handle ORACLE_SID
	if (sid) {
		if (!environment::setValue("ORACLE_SID",sid)) {
			fprintf(stderr,"Failed to set ORACLE_SID environment variable.\n");
			return false;
		}
	} else {
		if (!environment::getValue("ORACLE_SID")) {
			fprintf(stderr,"No ORACLE_SID environment variable set or specified in connect string.\n");
			return false;
		}
	}

	// handle TWO_TASK
	if (sid) {
		if (!environment::setValue("TWO_TASK",sid)) {
			fprintf(stderr,"Failed to set TWO_TASK environment variable.\n");
			return false;
		}
	} else {
		if (!environment::getValue("TWO_TASK")) {
			fprintf(stderr,"No TWO_TASK environment variable set or specified in connect string.\n");
			return false;
		}
	}

	// handle NLS_LANG
	if (nlslang) {
		if (!environment::setValue("NLS_LANG",nlslang)) {
			fprintf(stderr,"Failed to set NLS_LANG environment variable.\n");
			return false;
		}
	}

	// log in
	if (olog(&lda,(ub1 *)hda,
			(text *)getUser(),-1,(text *)getPassword(),-1,
			(text *)0,-1,
			(ub4)OCI_LM_DEF)) {

		// display the error message
		text	message[512];
		sword	n=oerhms(&lda,lda.rc,message,
				(sword)sizeof(message));
		message[n]=(text)NULL;
		fprintf(stderr,"%s\n",message);
		return false;
	}
	return true;
}

sqlrcursor_svr *oracle7connection::initCursor() {
	return (sqlrcursor_svr *)new oracle7cursor((sqlrconnection_svr *)this);
}

void oracle7connection::deleteCursor(sqlrcursor_svr *curs) {
	delete (oracle7cursor *)curs;
}

void oracle7connection::logOut() {
	ologof(&lda);
}

bool oracle7connection::autoCommitOn() {
	return (!ocon(&lda));
}

bool oracle7connection::autoCommitOff() {
	return (!ocof(&lda));
}

bool oracle7connection::commit() {
	return (!ocom(&lda));
}

bool oracle7connection::rollback() {
	return (!orol(&lda));
}

const char *oracle7connection::pingQuery() {
	return "select 1 from dual";
}

const char *oracle7connection::identify() {
	return "oracle7";
}

oracle7cursor::oracle7cursor(sqlrconnection_svr *conn) : sqlrcursor_svr(conn) {
	errormessage=NULL;
	oracle7conn=(oracle7connection *)conn;
	inputbindcount=0;
	for (uint16_t i=0; i<MAXVAR; i++) {
		intbindstring[i]=NULL;
	}
	outbindcount=0;
}

oracle7cursor::~oracle7cursor() {
	delete errormessage;
	for (uint16_t i=0; i<inputbindcount; i++) {
		delete[] intbindstring[i];
	}
}

bool oracle7cursor::openCursor(uint16_t id) {
	return (!oopen(&cda,&oracle7conn->lda,
			(text *)0,-1,-1,(text *)0,-1) &&
			sqlrcursor_svr::openCursor(id));
}

bool oracle7cursor::closeCursor() {
	return !oclose(&cda);
}

bool oracle7cursor::prepareQuery(const char *query, uint32_t length) {
	// parse the query
	return (!oparse(&cda,(text *)query,(sb4)length,
			(sword)PARSE_DEFER,(ub4)PARSE_V7_LNG));
}

bool oracle7cursor::inputBindString(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint16_t valuesize,
						int16_t *isnull) {

	// bind the value to the variable
	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!charstring::toInteger(variable+1)) {
			return false;
		}
		if (obndrn(&cda,(sword)charstring::toInteger(variable+1),
			(ub1 *)value,(sword)valuesize+1,
			NULL_TERMINATED_STRING,
			-1,(sb2 *)isnull,(text *)0,-1,-1)) {
			return false;
		}
	} else {
		if (obndrv(&cda,(text *)variable,(sword)variablesize,
			(ub1 *)value,(sword)valuesize+1,
			NULL_TERMINATED_STRING,
			-1,(sb2 *)isnull,(text *)0,-1,-1)) {
			return false;
		}
	}
	inputbindcount++;
	return true;
}

bool oracle7cursor::inputBindInteger(const char *variable, 
						uint16_t variablesize,
						int64_t *value) {

	intbindstring[inputbindcount]=charstring::parseNumber(*value);
	
	// bind the value to the variable
	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!charstring::toInteger(variable+1)) {
			return false;
		}
		if (obndrn(&cda,(sword)charstring::toInteger(variable+1),
			(ub1 *)intbindstring[inputbindcount],
			(sword)charstring::length(
				intbindstring[inputbindcount])+1,
			NULL_TERMINATED_STRING,
			//(ub1 *)value,(sword)sizeof(int64_t),LONG_BIND_TYPE,
			-1,(sb2 *)0,(text *)0,-1,-1)) {
			return false;
		}
	} else {
		if (obndrv(&cda,(text *)variable,(sword)variablesize,
			(ub1 *)intbindstring[inputbindcount],
			(sword)charstring::length(
				intbindstring[inputbindcount])+1,
			NULL_TERMINATED_STRING,
			//(ub1 *)value,(sword)sizeof(int64_t),LONG_BIND_TYPE,
			-1,(sb2 *)0,(text *)0,-1,-1)) {
			return false;
		}
	}
	inputbindcount++;
	return true;
}

bool oracle7cursor::inputBindDouble(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale) {
	
	// bind the value to the variable
	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!charstring::toInteger(variable+1)) {
			return false;
		}
		if (obndrn(&cda,(sword)charstring::toInteger(variable+1),
			(ub1 *)value,(sword)sizeof(double),DOUBLE_BIND_TYPE,
			(sword)scale,(sb2 *)0,(text *)0,-1,-1)) {
			return false;
		}
	} else {
		if (obndrv(&cda,(text *)variable,(sword)variablesize,
			(ub1 *)value,(sword)sizeof(double),DOUBLE_BIND_TYPE,
			(sword)scale,(sb2 *)0,(text *)0,-1,-1)) {
			return false;
		}
	}
	inputbindcount++;
	return true;
}

bool oracle7cursor::outputBindString(const char *variable, 
					uint16_t variablesize,
					char *value, 
					uint16_t valuesize, 
					int16_t *isnull) {

	outintbindstring[outbindcount]=NULL;

	// bind the value to the variable
	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!charstring::toInteger(variable+1)) {
			return false;
		}
		if (obndrn(&cda,(sword)charstring::toInteger(variable+1),
			(ub1 *)value,(sword)valuesize,
			NULL_TERMINATED_STRING,
			-1,(sb2 *)isnull,(text *)0,-1,-1)) {
			return false;
		}
	} else {
		if (obndrv(&cda,(text *)variable,(sword)variablesize,
			(ub1 *)value,(sword)valuesize,
			NULL_TERMINATED_STRING,
			-1,(sb2 *)isnull,(text *)0,-1,-1)) {
			return false;
		}
	}
	outbindcount++;
	return true;
}

bool oracle7cursor::outputBindInteger(const char *variable, 
					uint16_t variablesize,
					int64_t *value, 
					int16_t *isnull) {

	outintbindstring[outbindcount]=new char[21];
	outintbind[outbindcount]=value;

	// bind the value to the variable
	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!charstring::toInteger(variable+1)) {
			return false;
		}
		if (obndrn(&cda,(sword)charstring::toInteger(variable+1),
			(ub1 *)outintbindstring[outbindcount],(sword)21,
			NULL_TERMINATED_STRING,
			-1,(sb2 *)isnull,(text *)0,-1,-1)) {
			return false;
		}
	} else {
		if (obndrv(&cda,(text *)variable,(sword)variablesize,
			(ub1 *)outintbindstring[outbindcount],(sword)21,
			NULL_TERMINATED_STRING,
			-1,(sb2 *)isnull,(text *)0,-1,-1)) {
			return false;
		}
	}
	outbindcount++;
	return true;
}

bool oracle7cursor::outputBindDouble(const char *variable, 
					uint16_t variablesize,
					double *value, 
					uint32_t *precision,
					uint32_t *scale,
					int16_t *isnull) {

	outintbindstring[outbindcount]=NULL;

	// bind the value to the variable
	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!charstring::toInteger(variable+1)) {
			return false;
		}
		if (obndrn(&cda,(sword)charstring::toInteger(variable+1),
			(ub1 *)value,(sword)sizeof(double),DOUBLE_BIND_TYPE,
			-1,(sb2 *)isnull,(text *)0,-1,-1)) {
			return false;
		}
	} else {
		if (obndrv(&cda,(text *)variable,(sword)variablesize,
			(ub1 *)value,(sword)sizeof(double),DOUBLE_BIND_TYPE,
			-1,(sb2 *)isnull,(text *)0,-1,-1)) {
			return false;
		}
	}
	outbindcount++;
	return true;
}

bool oracle7cursor::executeQuery(const char *query, uint32_t length,
							bool execute) {

	// initialize the column count
	ncols=0;

	// if the query is a select, describe/define it
	if (cda.ft==SELECT_QUERY) {

		// run through the columns...
		for (;;) {

			// init the column name size
			desc[ncols].buflen=MAX_ITEM_BUFFER_SIZE;

			// describe the column
			if (!odescr(&cda,ncols+1,
				&desc[ncols].dbsize, 
				&desc[ncols].dbtype,
				&desc[ncols].buf[0], 
				&desc[ncols].buflen,
				&desc[ncols].dsize, 
				&desc[ncols].precision,
				&desc[ncols].scale, 
				&desc[ncols].nullok)) {

				// define the column
				if (odefin(&cda,ncols+1,
						*def_buf[ncols],
						MAX_ITEM_BUFFER_SIZE,
						NULL_TERMINATED_STRING,
						-1,
						def_indp[ncols],
						(text *)0,
						-1,
						-1,
						def_col_retlen[ncols],
						def_col_retcode[ncols])) {
					return false;
				}

			} else {

				// if we're at the end of the column list...
				if (cda.rc==NOT_IN_LIST) {
					break;
				} else {
					return false;
				}
			}
	
			ncols++;
		}
	}

	// initialize row counters
	row=0;
	maxrow=0;
	totalrows=0;

	// execute the query
	if (oexec(&cda)) {
		return false;
	}

	// convert integer output binds
	for (uint16_t i=0; i<outbindcount; i++) {
		if (outintbindstring[i]) {
			*outintbind[i]=charstring::
					toInteger(outintbindstring[i]);
		}
	}

	return true;
}

bool oracle7cursor::queryIsNotSelect() {
	return (cda.ft!=SELECT_QUERY);
}

bool oracle7cursor::queryIsCommitOrRollback() {
	return (cda.ft==COMMIT_QUERY || cda.ft==ROLLBACK_QUERY);
}

const char *oracle7cursor::errorMessage(bool *liveconnection) {

	// get the message from oracle
	text	message[512];
	sword	n=oerhms(&oracle7conn->lda,
				cda.rc,message,(sword)sizeof(message));

	// check for dead connection
	if (!charstring::compare((char *)message,"ORA-03114",9) || 
		!charstring::compare((char *)message,"ORA-03113",9)) {
		*liveconnection=false;
	} else {
		*liveconnection=true;
	}

	// only return an error message if the error wasn't a dead database
	delete errormessage;
	errormessage=new stringbuffer();
	if (*liveconnection) {
		for (sword i=0; i<n; i++) {
			errormessage->append((char)message[i]);
		}
	}

	return errormessage->getString();
}

bool oracle7cursor::knowsRowCount() {
	return false;
}

uint64_t oracle7cursor::rowCount() {
	return 0;
}

bool oracle7cursor::knowsAffectedRows() {
	return true;
}

uint64_t oracle7cursor::affectedRows() {
	return cda.rpc;
}

uint32_t oracle7cursor::colCount() {
	return ncols;
}

const char * const * oracle7cursor::columnNames() {
	for (sword i=0; i<ncols; i++) {
		columnnames[i]=(char *)desc[i].buf;
	}
	return columnnames;
}

uint16_t oracle7cursor::columnTypeFormat() {
	return (uint16_t)COLUMN_TYPE_IDS;
}

void oracle7cursor::returnColumnInfo() {

	// a useful variables
	uint16_t	type;

	// for each column...
	for (sword i=0; i<ncols; i++) {

		// set column type
		uint16_t	binary=0;
		if (desc[i].dbtype==VARCHAR2_TYPE) {
			type=VARCHAR2_DATATYPE;
		} else if (desc[i].dbtype==NUMBER_TYPE) {
			type=NUMBER_DATATYPE;
		} else if (desc[i].dbtype==LONG_TYPE) {
			type=LONG_DATATYPE;
		} else if (desc[i].dbtype==ROWID_TYPE) {
			type=ROWID_DATATYPE;
		} else if (desc[i].dbtype==DATE_TYPE) {
			type=DATE_DATATYPE;
		} else if (desc[i].dbtype==RAW_TYPE) {
			type=RAW_DATATYPE;
			binary=1;
		} else if (desc[i].dbtype==LONG_RAW_TYPE) {
			type=LONG_RAW_DATATYPE;
			binary=1;
		} else if (desc[i].dbtype==CHAR_TYPE) {
			type=CHAR_DATATYPE;
		} else if (desc[i].dbtype==MLSLABEL_TYPE) {
			type=MLSLABEL_DATATYPE;
		} else {
			type=UNKNOWN_DATATYPE;
		}

		// send the column definition
		conn->sendColumnDefinition((char *)desc[i].buf,
					(uint16_t)desc[i].buflen,
					type,
					(uint32_t)desc[i].dbsize,
					(uint32_t)desc[i].precision,
					(uint32_t)desc[i].scale,
					(uint16_t)desc[i].nullok,0,0,
					0,0,0,binary,0);
	}
}

bool oracle7cursor::noRowsToReturn() {
	return (cda.ft!=SELECT_QUERY);
}

bool oracle7cursor::skipRow() {
	if (fetchRow()) {
		row++;
		return true;
	}
	return false;
}

bool oracle7cursor::fetchRow() {
	if (row==FETCH_AT_ONCE) {
		row=0;
	}
	if (row>0 && row==maxrow) {
		return false;
	}
	if (!row) {
		if (ofen(&cda,FETCH_AT_ONCE) && cda.rpc==totalrows) {
			return false;
		}
		maxrow=cda.rpc-totalrows;
		totalrows=cda.rpc;
	}
	return true;
}

void oracle7cursor::returnRow() {

	for (sword col=0; col<ncols; col++) {

		// handle NULL's
		if (def_indp[col][row]==-1) {
			conn->sendNullField();
			continue;
		}

		// handle long datatypes
		if (desc[col].dbtype==LONG_TYPE ||
			desc[col].dbtype==LONG_RAW_TYPE) {

			ub4	retlen=MAX_ITEM_BUFFER_SIZE;
			sb4	offset=0;

			conn->startSendingLong(def_col_retlen[col][row]);

			while (retlen==MAX_ITEM_BUFFER_SIZE) {

				oflng(&cda,col+1,
					def_buf[col][row],
					MAX_ITEM_BUFFER_SIZE,1,
					&retlen,offset);

				conn->sendLongSegment((char *)
							def_buf[col][row],
							(uint32_t)retlen);

				offset=offset+retlen;
			}

			conn->endSendingLong();

			continue;

		}

		// handle normal datatypes
		conn->sendField((char *)def_buf[col][row],
					(uint32_t)def_col_retlen[col][row]);
	}

	// increment the row counter
	row++;
}

void oracle7cursor::cleanUpData(bool freeresult, bool freebinds) {
	if (freeresult) {
		ocan(&cda);
	}
	if (freebinds) {
		for (uint16_t i=0; i<inputbindcount; i++) {
			delete[] intbindstring[i];
			intbindstring[i]=NULL;
		}
		for (uint16_t i=0; i<outbindcount; i++) {
			delete[] outintbindstring[i];
			outintbindstring[i]=NULL;
			outintbind[i]=NULL;
		}
		inputbindcount=0;
		outbindcount=0;
	}
}
