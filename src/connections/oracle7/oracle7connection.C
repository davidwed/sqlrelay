// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <oracle7connection.h>
#include <rudiments/charstring.h>

#include <config.h>
#include <datatypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
	#include <strings.h>
#endif

oracle7connection::oracle7connection() {
	env=new environment();
}

oracle7connection::~oracle7connection() {
	delete env;
}

int oracle7connection::getNumberOfConnectStringVars() {
	return NUM_CONNECT_STRING_VARS;
}

void oracle7connection::handleConnectString() {
	setUser(connectStringValue("user"));
	setPassword(connectStringValue("password"));
	sid=connectStringValue("oracle_sid");
	home=connectStringValue("oracle_home");
	char	*autocom=connectStringValue("autocommit");
	setAutoCommitBehavior((autocom && !strcasecmp(autocom,"yes")));
}

bool oracle7connection::logIn() {

	// handle ORACLE_HOME
	if (home) {
		if (!env->setValue("ORACLE_HOME",home)) {
			fprintf(stderr,"Failed to set ORACLE_HOME environment variable.\n");
			return false;
		}
	} else {
		if (!env->getValue("ORACLE_HOME")) {
			fprintf(stderr,"No ORACLE_HOME environment variable set or specified in connect string.\n");
			return false;
		}
	}

	// handle ORACLE_SID
	if (sid) {
		if (!env->setValue("ORACLE_SID",sid)) {
			fprintf(stderr,"Failed to set ORACLE_SID environment variable.\n");
			return false;
		}
	} else {
		if (!env->getValue("ORACLE_SID")) {
			fprintf(stderr,"No ORACLE_SID environment variable set or specified in connect string.\n");
			return false;
		}
	}

	// handle TWO_TASK
	if (sid) {
		if (!env->setValue("TWO_TASK",sid)) {
			fprintf(stderr,"Failed to set TWO_TASK environment variable.\n");
			return false;
		}
	} else {
		if (!env->getValue("TWO_TASK")) {
			fprintf(stderr,"No TWO_TASK environment variable set or specified in connect string.\n");
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

sqlrcursor *oracle7connection::initCursor() {
	return (sqlrcursor *)new oracle7cursor((sqlrconnection *)this);
}

void oracle7connection::deleteCursor(sqlrcursor *curs) {
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

char *oracle7connection::pingQuery() {
	return "select 1 from dual";
}

char *oracle7connection::identify() {
	return "oracle7";
}

oracle7cursor::oracle7cursor(sqlrconnection *conn) : sqlrcursor(conn) {
	errormessage=NULL;
	oracle7conn=(oracle7connection *)conn;
}

oracle7cursor::~oracle7cursor() {
	delete errormessage;
}

bool oracle7cursor::openCursor(int id) {
	return (!oopen(&cda,&oracle7conn->lda,
			(text *)0,-1,-1,(text *)0,-1) &&
			sqlrcursor::openCursor(id));
}

bool oracle7cursor::closeCursor() {
	return !oclose(&cda);
}

bool oracle7cursor::prepareQuery(const char *query, long length) {
	// parse the query
	return (!oparse(&cda,(text *)query,(sb4)length,
			(sword)PARSE_DEFER,(ub4)PARSE_V7_LNG));
}

bool oracle7cursor::inputBindString(const char *variable, 
						unsigned short variablesize,
						const char *value, 
						unsigned short valuesize,
						short *isnull) {

	// bind the value to the variable
	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!atoi(variable+1)) {
			return false;
		}
		if (obndrn(&cda,(sword)atoi(variable+1),
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
	return true;
}

bool oracle7cursor::inputBindLong(const char *variable, 
						unsigned short variablesize,
						unsigned long *value) {
	
	// bind the value to the variable
	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!atoi(variable+1)) {
			return false;
		}
		if (obndrn(&cda,(sword)atoi(variable+1),
			(ub1 *)value,(sword)sizeof(long),LONG_BIND_TYPE,
			-1,(sb2 *)0,(text *)0,-1,-1)) {
			return false;
		}
	} else {
		if (obndrv(&cda,(text *)variable,(sword)variablesize,
			(ub1 *)value,(sword)sizeof(long),LONG_BIND_TYPE,
			-1,(sb2 *)0,(text *)0,-1,-1)) {
			return false;
		}
	}
	return true;
}

bool oracle7cursor::inputBindDouble(const char *variable, 
						unsigned short variablesize,
						double *value,
						unsigned short precision,
						unsigned short scale) {
	
	// bind the value to the variable
	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!atoi(variable+1)) {
			return false;
		}
		if (obndrn(&cda,(sword)atoi(variable+1),
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
	return true;
}

bool oracle7cursor::outputBindString(const char *variable, 
					unsigned short variablesize,
					char *value, 
					unsigned short valuesize, 
					short *isnull) {

	// bind the value to the variable
	if (charstring::isInteger(variable+1,variablesize-1)) {
		if (!atoi(variable+1)) {
			return false;
		}
		if (obndrn(&cda,(sword)atoi(variable+1),
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
	return true;
}

bool oracle7cursor::executeQuery(const char *query, long length, bool execute) {

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
	return (!oexec(&cda));
}

bool oracle7cursor::queryIsNotSelect() {
	return (cda.ft!=SELECT_QUERY);
}

bool oracle7cursor::queryIsCommitOrRollback() {
	return (cda.ft==COMMIT_QUERY || cda.ft==ROLLBACK_QUERY);
}

char *oracle7cursor::getErrorMessage(bool *liveconnection) {

	// get the message from oracle
	text	message[512];
	sword	n=oerhms(&oracle7conn->lda,
				cda.rc,message,(sword)sizeof(message));

	// check for dead connection
	if (!strncmp((char *)message,"ORA-03114",9) || 
		!strncmp((char *)message,"ORA-03113",9)) {
		*liveconnection=false;
	} else {
		*liveconnection=true;
	}

	// only return an error message if the error wasn't a dead database
	delete errormessage;
	errormessage=new stringbuffer();
	if (*liveconnection) {
		for (int i=0; i<n; i++) {
			errormessage->append((char)message[i]);
		}
	}

	return errormessage->getString();
}

void oracle7cursor::returnRowCounts() {
	// send row counts (actual row count unknown in oracle)
	oracle7conn->sendRowCounts((long)-1,(long)cda.rpc);
}

void oracle7cursor::returnColumnCount() {
	oracle7conn->sendColumnCount(ncols);
}

void oracle7cursor::returnColumnInfo() {

	conn->sendColumnTypeFormat(COLUMN_TYPE_IDS);

	// a useful variables
	int		type;

	// for each column...
	for (int i=0; i<ncols; i++) {

		// set column type
		unsigned short	binary=0;
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
					(short)desc[i].buflen,type,
					(int)desc[i].dbsize,
					(unsigned short)desc[i].precision,
					(unsigned short)desc[i].scale,
					(unsigned short)desc[i].nullok,0,0,
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

			conn->startSendingLong();

			while (retlen==MAX_ITEM_BUFFER_SIZE) {

				oflng(&cda,col+1,
					def_buf[col][row],
					MAX_ITEM_BUFFER_SIZE,1,
					&retlen,offset);

				conn->sendLongSegment((char *)
							def_buf[col][row],
							(long)retlen);

				offset=offset+retlen;
			}

			conn->endSendingLong();

			continue;

		}

		// handle normal datatypes
		conn->sendField((char *)def_buf[col][row],
					(int)def_col_retlen[col][row]);
	}

	// increment the row counter
	row++;
}

void oracle7cursor::cleanUpData(bool freerows, bool freecols,
							bool freebinds) {
	if (freerows) {
		ocan(&cda);
	}
}
